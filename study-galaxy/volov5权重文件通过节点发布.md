*为了将 YOLOv5 的检测结果通过 C++ 编写的 ROS 节点发布，需完成以下步骤。本方案基于 LibTorch（PyTorch C++接口）实现模型推理，并集成到ROS中。*

------

### **1. 环境准备**

#### 1.1 安装依赖

- **ROS Noetic**（或其他版本，需支持C++）

- **OpenCV**（用于图像处理）

- **LibTorch**（PyTorch的C++库）

  - 从 [PyTorch官网](https://pytorch.org/) 下载对应CUDA版本的LibTorch。

  - 解压后设置环境变量：

    ```
export LIBTORCH_HOME=/path/to/libtorch
    export LD_LIBRARY_PATH=$LIBTORCH_HOME/lib:$LD_LIBRARY_PATH
```

#### 1.2 转换YOLOv5模型为TorchScript

YOLOv5的 `.pt` 模型需转换为TorchScript格式（`.torchscript.pt`）：

```
git clone https://github.com/ultralytics/yolov5
cd yolov5
pip install -r requirements.txt
python export.py --weights yolov5s.pt --include torchscript
```

生成 `yolov5s.torchscript.pt`，后续在C++中加载此文件。

------

### **2. 创建ROS包和自定义消息**

#### 2.1 创建ROS包

```
catkin_create_pkg yolo_ros_cpp roscpp std_msgs sensor_msgs cv_bridge
```

#### 2.2 定义自定义消息

- 在 `msg/BoundingBox.msg` 中定义单条检测结果：

  ```
  string label
  float32 probability
  int32 xmin
  int32 ymin
  int32 xmax
  int32 ymax
  ```
  
- 在 `msg/BoundingBoxes.msg` 中定义检测结果列表：

  ```
Header header
  BoundingBox[] boxes
```

#### 2.3 配置 `CMakeLists.txt` 和 `package.xml`

- **package.xml**：添加消息生成依赖：

  ```
  <build_depend>message_generation</build_depend>
  <exec_depend>message_runtime</exec_depend>
  ```
  
- **CMakeLists.txt**：启用消息生成和链接库：

  ```
  find_package(catkin REQUIRED COMPONENTS
    roscpp
    std_msgs
    sensor_msgs
    cv_bridge
    message_generation
  )
  add_message_files(FILES BoundingBox.msg BoundingBoxes.msg)
  generate_messages(DEPENDENCIES std_msgs)
  ```

------

### **3. C++节点实现**

#### 3.1 代码结构 (`src/yolo_detector.cpp`)

```
#include <ros/ros.h>
#include <sensor_msgs/Image.h>
#include <cv_bridge/cv_bridge.h>
#include <yolo_ros_cpp/BoundingBoxes.h>
#include <yolo_ros_cpp/BoundingBox.h>
#include <opencv2/opencv.hpp>
#include <torch/script.h>

class YOLOv5Detector {
public:
    YOLOv5Detector() : nh_("~") {
        // 加载模型
        try {
            module_ = torch::jit::load("/path/to/yolov5s.torchscript.pt");
            module_.to(torch::kCUDA); // 使用GPU
        } catch (const c10::Error& e) {
            ROS_ERROR("模型加载失败: %s", e.what());
            exit(1);
        }

        // 订阅图像话题（假设话题名为/camera/image_raw）
        image_sub_ = nh_.subscribe("/camera/image_raw", 1, &YOLOv5Detector::imageCallback, this);
        
        // 发布检测结果和可视化图像
        bbox_pub_ = nh_.advertise<yolo_ros_cpp::BoundingBoxes>("/yolo/bounding_boxes", 10);
        image_pub_ = nh_.advertise<sensor_msgs::Image>("/yolo/detected_image", 10);
    }

private:
    // 图像回调函数
    void imageCallback(const sensor_msgs::ImageConstPtr& msg) {
        try {
            // 将ROS图像转换为OpenCV格式
            cv_bridge::CvImagePtr cv_ptr = cv_bridge::toCvCopy(msg, "bgr8");
            cv::Mat image = cv_ptr->image;

            // 预处理：调整尺寸、归一化、转换为Tensor
            cv::Mat resized;
            cv::resize(image, resized, cv::Size(640, 640)); // YOLOv5输入尺寸
            torch::Tensor input_tensor = torch::from_blob(
                resized.data, {640, 640, 3}, torch::kByte);
            input_tensor = input_tensor.permute({2, 0, 1}) // HWC -> CHW
                              .unsqueeze(0)                // 添加batch维度
                              .to(torch::kFloat)
                              .div(255)                    // 归一化到[0,1]
                              .to(torch::kCUDA);            // 移动到GPU

            // 模型推理
            torch::Tensor output = module_.forward({input_tensor}).toTensor();

            // 后处理：解析检测结果
            std::vector<Detection> detections = postprocess(output, image.size());

            // 发布检测结果和可视化图像
            publishDetections(detections, msg->header);
            publishImage(image, detections, msg->header);
        } catch (cv_bridge::Exception& e) {
            ROS_ERROR("图像转换失败: %s", e.what());
        }
    }

    // 后处理逻辑（非极大值抑制、阈值过滤等）
    struct Detection { int x1, y1, x2, y2; float conf; int class_id; };

    std::vector<Detection> postprocess(torch::Tensor& output, const cv::Size& orig_size) {
        std::vector<Detection> detections;
        auto dets = output[0]; // 假设输出形状为 [1, num_det, 6]

        for (int i = 0; i < dets.size(0); ++i) {
            float conf = dets[i][4].item<float>();
            if (conf < 0.5) continue; // 置信度阈值

            // 坐标还原到原始图像尺寸
            int x1 = dets[i][0].item<float>() * orig_size.width / 640;
            int y1 = dets[i][1].item<float>() * orig_size.height / 640;
            int x2 = dets[i][2].item<float>() * orig_size.width / 640;
            int y2 = dets[i][3].item<float>() * orig_size.height / 640;

            detections.push_back({
                x1, y1, x2, y2,
                conf,
                static_cast<int>(dets[i][5].item<float>())
            });
        }

        // 可选：非极大值抑制（NMS）
        return detections;
    }

    // 发布检测结果
    void publishDetections(const std::vector<Detection>& dets, const std_msgs::Header& header) {
        yolo_ros_cpp::BoundingBoxes msg;
        msg.header = header;

        for (const auto& d : dets) {
            yolo_ros_cpp::BoundingBox bbox;
            bbox.label = class_names_[d.class_id]; // 类别名称数组需自行定义
            bbox.probability = d.conf;
            bbox.xmin = d.x1;
            bbox.ymin = d.y1;
            bbox.xmax = d.x2;
            bbox.ymax = d.y2;
            msg.boxes.push_back(bbox);
        }

        bbox_pub_.publish(msg);
    }

    // 发布带检测框的可视化图像
    void publishImage(cv::Mat& image, const std::vector<Detection>& dets, const std_msgs::Header& header) {
        for (const auto& d : dets) {
            cv::rectangle(image, cv::Point(d.x1, d.y1), cv::Point(d.x2, d.y2), cv::Scalar(0, 255, 0), 2);
            cv::putText(image, class_names_[d.class_id], cv::Point(d.x1, d.y1 - 5),
                        cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 1);
        }

        cv_bridge::CvImage out_msg;
        out_msg.header = header;
        out_msg.encoding = "bgr8";
        out_msg.image = image;
        image_pub_.publish(out_msg.toImageMsg());
    }

    ros::NodeHandle nh_;
    ros::Subscriber image_sub_;
    ros::Publisher bbox_pub_, image_pub_;
    torch::jit::script::Module module_;
    std::vector<std::string> class_names_ = {"person", "car", "truck", ...}; // 替换为实际类别
};

int main(int argc, char** argv) {
    ros::init(argc, argv, "yolo_detector");
    YOLOv5Detector detector;
    ros::spin();
    return 0;
}
```

#### 3.2 关键代码解释

1. **模型加载**：使用 `torch::jit::load()` 加载TorchScript模型，并移动到GPU。
2. **图像预处理**：
   - 调整输入图像尺寸为640x640（YOLOv5默认输入）。
   - 转换为Tensor，并调整维度顺序为 `CHW`，归一化到 `[0, 1]`。
3. **模型推理**：调用 `module_.forward()` 执行推理。
4. **后处理**：
   - 解析模型输出的检测框坐标、置信度和类别。
   - 将坐标还原到原始图像尺寸。
   - 应用置信度阈值过滤和NMS（示例中未实现，需补充）。
5. **发布结果**：
   - 将检测结果封装为 `BoundingBoxes` 消息。
   - 在图像上绘制检测框并发布可视化结果。

------

### **4. 编译与运行**

#### 4.1 配置 `CMakeLists.txt`

添加LibTorch和OpenCV依赖：

```
find_package(OpenCV REQUIRED)
find_package(Torch REQUIRED)

add_executable(yolo_detector src/yolo_detector.cpp)
target_link_libraries(yolo_detector
  ${catkin_LIBRARIES}
  ${OpenCV_LIBRARIES}
  ${TORCH_LIBRARIES}
)
```

#### 4.2 编译并运行

```
catkin_make
source devel/setup.bash
rosrun yolo_ros_cpp yolo_detector
```

------

### **5. 订阅检测结果**

其他节点可订阅 `/yolo/bounding_boxes` 获取检测结果。示例订阅者：

```
#include <ros/ros.h>
#include <yolo_ros_cpp/BoundingBoxes.h>

void callback(const yolo_ros_cpp::BoundingBoxes::ConstPtr& msg) {
    for (const auto& box : msg->boxes) {
        ROS_INFO("检测到 %s (置信度: %.2f) 位置: [%d, %d, %d, %d]",
                 box.label.c_str(), box.probability,
                 box.xmin, box.ymin, box.xmax, box.ymax);
    }
}

int main(int argc, char** argv) {
    ros::init(argc, argv, "detection_subscriber");
    ros::NodeHandle nh;
    ros::Subscriber sub = nh.subscribe("/yolo/bounding_boxes", 10, callback);
    ros::spin();
    return 0;
}
```

------

### **6. 常见问题与优化**

#### 6.1 模型输出解析

YOLOv5的输出格式可能因版本不同而变化。需确保后处理逻辑与模型输出一致。通常输出为 `[batch, num_det, 6]`，其中最后一维包含 `[x1, y1, x2, y2, conf, class_id]`。

#### 6.2 性能优化

- **GPU加速**：确保LibTorch为CUDA版本，模型和输入Tensor均位于GPU。
- **半精度推理**：将Tensor转换为 `torch::kHalf` 减少内存占用。
- **异步推理**：使用多线程分离图像采集和推理过程。

#### 6.3 非极大值抑制（NMS）

若模型未内置NMS，需手动实现：

```
#include <torchvision/vision.h> // 需要安装torchvision C++库

std::vector<Detection> applyNMS(const std::vector<Detection>& dets, float iou_threshold) {
    torch::Tensor boxes = torch::zeros({dets.size(), 4});
    torch::Tensor scores = torch::zeros({dets.size()});
    for (size_t i = 0; i < dets.size(); ++i) {
        boxes[i][0] = dets[i].x1;
        boxes[i][1] = dets[i].y1;
        boxes[i][2] = dets[i].x2;
        boxes[i][3] = dets[i].y2;
        scores[i] = dets[i].conf;
    }
    auto keepers = vision::ops::nms(boxes, scores, iou_threshold);
    std::vector<Detection> filtered;
    for (auto idx : keepers) {
        filtered.push_back(dets[idx]);
    }
    return filtered;
}
```

------

通过以上步骤，你可以将YOLOv5的检测结果通过ROS话题发布，供其他节点订阅使用。