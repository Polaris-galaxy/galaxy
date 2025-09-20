### **1. 整体架构**

需要实现的核心流程：

```
摄像头图像 → ROS订阅 → YOLO推理 → 检测结果解析 → ROS消息封装 → 话题发布
```

------

### **2. 自定义消息设计**

YOLO 的输出包含多个检测框信息（类别、置信度、坐标），ROS 标准消息类型无法直接表达这种结构化数据。

#### **消息定义**

创建两个 `.msg` 文件：

1. **BoundingBox.msg**（单个检测框信息）

```
string Class       # 类别名称（如 "person"）
float64 probability  # 置信度（0~1）
int64 xmin         # 左上角x坐标
int64 ymin         # 左上角y坐标
int64 xmax         # 右下角x坐标
int64 ymax         # 右下角y坐标
```

1. **BoundingBoxes.msg**（所有检测框的集合）

```
std_msgs/Header header     # 消息头（时间戳、坐标系）
BoundingBox[] bounding_boxes # 检测框数组
```

#### **关键点说明**

- `header` 用于保持时间同步（与其他传感器数据对齐）
- `xmin/ymin/xmax/ymax` 使用图像坐标系（左上角为原点）

------

### **3. ROS Package 配置**

#### **修改 package.xml**

添加消息生成依赖：

```
<build_depend>message_generation</build_depend>
<exec_depend>message_runtime</exec_depend>
```

#### 修改 CMakeLists.txt

```
find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
  sensor_msgs
  message_generation
)

add_message_files(
  FILES
  BoundingBox.msg
  BoundingBoxes.msg
)

generate_messages(
  DEPENDENCIES
  std_msgs
)

catkin_package(
  CATKIN_DEPENDS 
  message_runtime 
  roscpp 
  std_msgs 
  sensor_msgs
)
```

------

### **4. C++ 节点代码详解**

#### 核心类结构

```
class YoloDetector {
private:
    ros::NodeHandle nh_;
    ros::Subscriber image_sub_;
    ros::Publisher bboxes_pub_;
    cv::dnn::Net net_;
    float confThreshold_ = 0.5;  // 置信度阈值
    float nmsThreshold_ = 0.4;   // NMS阈值
    std::vector<std::string> classes_; // 类别标签
};
```

------

#### **关键函数**

##### 1. 构造函数：初始化YOLO模型

```
YoloDetector() : nh_("~") {
    // 加载模型参数
    std::string weights_path, cfg_path;
    nh_.param<std::string>("weights_path", weights_path, "yolov4.weights");
    nh_.param<std::string>("cfg_path", cfg_path, "yolov4.cfg");
    
    // 加载模型
    net_ = cv::dnn::readNetFromDarknet(cfg_path, weights_path);
    net_.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
    net_.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);

    // 加载类别标签（coco.names）
    std::ifstream class_file("coco.names");
    std::string line;
    while (std::getline(class_file, line)) {
        classes_.push_back(line);
    }

    // 初始化ROS通信
    image_sub_ = nh_.subscribe("/camera/image_raw", 1, &YoloDetector::imageCallback, this);
    bboxes_pub_ = nh_.advertise<your_package::BoundingBoxes>("detections", 10);
}
```

##### **2. 图像回调函数**

```
void imageCallback(const sensor_msgs::ImageConstPtr& msg) {
    // 转换ROS图像到OpenCV格式
    cv_bridge::CvImagePtr cv_ptr;
    try {
        cv_ptr = cv_bridge::toCvCopy(msg, "bgr8");
    } catch (cv_bridge::Exception& e) {
        ROS_ERROR("cv_bridge exception: %s", e.what());
        return;
    }
    
    // YOLO推理
    cv::Mat blob = cv::dnn::blobFromImage(
        cv_ptr->image, 
        1/255.0,         // 归一化系数
        cv::Size(416, 416), // 输入尺寸（需与cfg文件一致）
        cv::Scalar(0,0,0), 
        true              // 交换RB通道（YOLO需要）
    );
    
    net_.setInput(blob);
    std::vector<cv::Mat> outputs;
    net_.forward(outputs, getOutputsNames());
    
    // 解析检测结果
    your_package::BoundingBoxes bboxes_msg;
    processDetections(outputs, cv_ptr->image.size(), bboxes_msg);
    
    // 发布消息
    bboxes_msg.header = msg->header; // 继承原图header
    bboxes_pub_.publish(bboxes_msg);
}
```

##### 3. 解析YOLO输出

```
void processDetections(const std::vector<cv::Mat>& outputs, 
                      const cv::Size& frame_size,
                      your_package::BoundingBoxes& msg) 
{
    std::vector<int> classIds;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;

    for (const auto& output : outputs) {
        float* data = (float*)output.data;
        for (int i = 0; i < output.rows; ++i, data += output.cols) {
            cv::Mat scores = output.row(i).colRange(5, output.cols);
            cv::Point classIdPoint;
            double confidence;
            cv::minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);
            
            if (confidence > confThreshold_) {
                // 计算实际坐标
                int centerX = (int)(data[0] * frame_size.width);
                int centerY = (int)(data[1] * frame_size.height);
                int width = (int)(data[2] * frame_size.width);
                int height = (int)(data[3] * frame_size.height);
                int left = centerX - width / 2;
                int top = centerY - height / 2;

                // 保存结果
                classIds.push_back(classIdPoint.x);
                confidences.push_back(confidence);
                boxes.emplace_back(left, top, width, height);
            }
        }
    }

    // 应用非极大值抑制（NMS）
    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confidences, confThreshold_, nmsThreshold_, indices);

    // 填充ROS消息
    for (int idx : indices) {
        your_package::BoundingBox bbox;
        bbox.Class = classes_[classIds[idx]];
        bbox.probability = confidences[idx];
        bbox.xmin = boxes[idx].x;
        bbox.ymin = boxes[idx].y;
        bbox.xmax = boxes[idx].x + boxes[idx].width;
        bbox.ymax = boxes[idx].y + boxes[idx].height;
        msg.bounding_boxes.push_back(bbox);
    }
}
```

------

### **5. 关键细节**

#### **YOLO输出解析原理**

- YOLO 输出层的每个检测框包含：
  - 前4个值：`(center_x, center_y, width, height)`（归一化到0~1）
  - 第5个值：置信度
  - 后续值：各类别概率

#### 坐标转换逻辑

```
原始图像尺寸 → 根据归一化值计算实际坐标：
left = center_x * image_width - width/2
top = center_y * image_height - height/2
```

#### **NMS的作用**

- 过滤重叠的检测框，保留置信度最高的
- OpenCV 的 `NMSBoxes` 函数自动完成此过程

------

