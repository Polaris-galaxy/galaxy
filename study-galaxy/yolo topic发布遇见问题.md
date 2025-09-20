# 遇到问题及其解决

## 问题1：

[ERROR] [1747391614.645683155]: image_publisher requires filename. Typical command-line usage:
	$ rosrun image_publisher image_publisher <filename>
[video_publisher-1] process has died [pid 19719, exit code 1, cmd /opt/ros/noetic/lib/image_publisher/image_publisher __name:=video_publisher __log:=/home/polaris/.ros/log/583fe1fa-323f-11f0-a151-9beea6d606e3/video_publisher-1.log].
log file: /home/polaris/.ros/log/583fe1fa-323f-11f0-a151-9beea6d606e3/video_publisher-1*.log

问题原因：image_publisher 找不到文件位置或者没有对应文件。

问题解决：

情况一(launch文件错误)：修改launch文件，必要时相对路径可以修改为绝对路径(注意使用“rospkg"定位文件相对路径时需要使用source devel/setup.bash)。

情况二(py或者cpp源码文件错误)：修改源码使其路径正确，正确引用文件。

## 问题二：

[ERROR] [1747391618.142522]: Initialization failed: cannot instantiate 'WindowsPath' on your system. Cache may be out of date, try `force_reload=True` or see https://docs.ultralytics.com/yolov5/tutorials/pytorch_hub_model_loading for help.

问题原因：windows文件残留导致linux无法识别。

问题解决：

若文件格式为.pt 可以使用脚本将文件转化为.onnx文件。

### 问题三：

[yolo_detector-2] process has died [pid 19720, exit code 1, cmd /home/polaris/catkin_ws/devel/lib/yolo_ros/yolo_detector.py /camera/image_raw:=/video_publisher/image_raw __name:=yolo_detector __log:=/home/polaris/.ros/log/583fe1fa-323f-11f0-a151-9beea6d606e3/yolo_detector-2.log].
log file: /home/polaris/.ros/log/583fe1fa-323f-11f0-a151-9beea6d606e3/yolo_detector-2*.log

问题原因：节点初始化有其他报错导致节点崩溃。

问题解决：情况一：解决其他问题自己会解决。

​                   情况二：（ubuntu中cuda，cnn等不兼容等问题）

​                                *第一步，查看日志文件，用cat命令输出日志内容，这能直接看到Python的报错信息，比如具体的异常堆栈跟踪。根据错误信息，可以更快定                            		               位问题。*

​                               *第二步，如果日志中没有足够信息，建议用户手动运行节点，使用rosrun命令，这样可以在终端直接看到输出，可能更容易发现错误。*

​                              *第三步，检查依赖项，确保所有Python库已安装，特别是PyTorch和OpenCV。如果用户使用了虚拟环境，可能需要确认是否在正确的环境中运			      行。*

​                             *第四步，检查模型文件路径是否正确，是否有读取权限。可能需要绝对路径，或者使用ROS的$(find pkg)来指定路径。*

​                            *第五步，检查GPU支持，如果代码中启用了GPU，但环境不支持，可能需要修改代码回退到CPU模式，或者安装相应的CUDA驱动和库。*

​                           *第六步，测试代码的输入，比如是否能够正确接收图像消息，处理图像时是否会出现异常。可以添加调试语句，或者用RViz查看话题是否有数据。*



### 问题四：

windows下遇见可能存在包括但不限于python，cuda等版本过高过低等问题。

解决方法（ubuntu也可以使用conda）：使用conda创建合适版本的虚拟环境。

ubuntu安装conda

```
wget https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh

bash Miniconda3-latest-Linux-x86_64.sh
```

### 问题五：

问题：catkin_make时提示opencv头文件找不到

#### **解决步骤**

##### 1. **确认 OpenCV 安装完整性**

- 检查 OpenCV 开发库是否安装：

  ```
  sudo apt install libopencv-dev  # 安装完整的 OpenCV 开发库
  ```

- 验证头文件路径是否存在：

  ```
  # 查找 opencv.hpp 的路径（通常位于 /usr/include/opencv4/ 或 /usr/local/include/opencv4/）
  sudo find / -name "opencv.hpp"
  ```

##### 2. **修改 `CMakeLists.txt` 配置**

- 在你的 ROS 包（`yolo_topic`）的 `CMakeLists.txt` 中，添加以下内容：

  ```
  # 添加 OpenCV 依赖
  find_package(OpenCV REQUIRED)  # 必须放在 add_executable 之前
  
  # 包含 OpenCV 头文件路径
  include_directories(
    ${OpenCV_INCLUDE_DIRS}
  )
  
  # 生成可执行文件
  add_executable(yolo_topic_node 
    src/yolo_topic_node.cpp
  )
  
  # 链接 OpenCV 库
  target_link_libraries(yolo_topic_node 
    ${catkin_LIBRARIES}
    ${OpenCV_LIBS}  # 添加这一行
  )
  ```

#### **其他可能问题**

1. ##### **环境变量未生效**

   - 如果手动编译了 OpenCV，确保路径已添加到环境变量：

     ```
     export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH
     export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
     ```

2. ##### **头文件路径不匹配**

   - 如果头文件路径为 `/usr/include/opencv4/opencv2/opencv.hpp`，修改代码中的包含语句：#include <opencv2/opencv.hpp>         // 原代码
     #include <opencv4/opencv2/opencv.hpp> // 如果路径中包含 opencv4 层级

3. ##### **动态库链接问题**

   - 编译通过但运行时报错，执行以下命令更新库缓存：

     ```
     sudo ldconfig
     ```

------

##### **验证 OpenCV 安装**

```
# 检查 OpenCV 版本
pkg-config --modversion opencv4

# 检查头文件路径
echo | g++ -E -v -  # 在输出中查找 OpenCV 相关路径
```

### 问题六：

AttributeError: module 'em' has no attribute 'RAW_OPT'

##### **步骤 1：检查并安装兼容版本的Empy**

该错误通常由Empy（Python模板库）版本不兼容引起。ROS的某些版本需要较旧的Empy（如3.x），而新版本可能移除了`RAW_OPT`属性。

1. **查看当前安装的Empy版本**：

   ```
   pip show empy  # 如果通过pip安装
   # 或
   apt list python3-empy  # 如果通过apt安装
   ```

2. **降级Empy到3.3.4**（推荐兼容版本）：

   ```
   pip install empy==3.3.4
   ```

   如果系统同时存在Python2和Python3环境，请使用`pip3`：

   ```
   pip3 install empy==3.3.4
   ```

##### **步骤 2：重新安装ROS消息生成工具**

确保ROS的消息生成包（如`genmsg`, `gencpp`）已正确安装：

```
sudo apt-get install --reinstall ros-<你的ROS版本>-genmsg ros-<你的ROS版本>-gencpp
```

将`<你的ROS版本>`替换为实际版本，如`noetic`（ROS1）或`humble`（ROS2）。

##### **步骤 3：清理并重新编译工作空间**

1. **清理旧的编译文件**：

   ```
   cd ~/yolov5_ros  # 进入你的工作空间根目录
   rm -rf devel build install  # 删除生成的文件
   ```

2. **重新编译**：

   ```
   catkin_make
   ```

##### **步骤 4（可选）：检查环境变量**

确保没有冲突的Python环境变量。在编译前激活ROS环境：

```
source /opt/ros/<你的ROS版本>/setup.bash
```

#### **验证解决**

完成上述步骤后，重新运行编译命令。如果问题依旧，请检查：

- **Empy版本是否正确**：确认`pip list`中Empy版本是否为3.3.4。
- **模板文件适配**：如果问题持续，可能需要手动修改ROS消息模板文件中的`em.RAW_OPT`引用，但这通常需要深入代码分析。

#### **常见问题**

- **权限问题**：如果使用`pip`安装时出现权限错误，尝试添加`--user`选项或使用虚拟环境。
- **多版本冲突**：确保系统中没有多个版本的Empy冲突，必要时使用虚拟环境隔离。

通过以上步骤，应能解决因Empy版本导致的`AttributeError`问题，顺利完成ROS消息生成和编译。
