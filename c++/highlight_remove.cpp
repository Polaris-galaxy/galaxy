#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

// 去除高光函数
Mat removeHighlights(const Mat& inputImage, double thresholdValue = 200, double reduceFactor = 0.7) {
    Mat image = inputImage.clone();
    
    // 转换为Lab颜色空间，更好地处理亮度
    Mat labImage;
    cvtColor(image, labImage, COLOR_BGR2Lab);
    
    // 分离Lab通道
    vector<Mat> labChannels(3);
    split(labImage, labChannels);
    
    // L通道代表亮度，我们主要处理这个通道
    Mat luminance = labChannels[0];
    
    // 创建高光区域的掩码
    Mat highlightMask;
    threshold(luminance, highlightMask, thresholdValue, 255, THRESH_BINARY);
    
    // 对掩码进行形态学操作，去除噪声并平滑边缘
    Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));
    morphologyEx(highlightMask, highlightMask, MORPH_CLOSE, kernel);
    morphologyEx(highlightMask, highlightMask, MORPH_OPEN, kernel);
    
    // 对掩码进行高斯模糊，使边缘更平滑
    GaussianBlur(highlightMask, highlightMask, Size(0, 0), 3);
    
    // 降低高光区域的亮度
    for (int y = 0; y < luminance.rows; y++) {
        for (int x = 0; x < luminance.cols; x++) {
            if (highlightMask.at<uchar>(y, x) > 0) {
                // 根据掩码的强度调整亮度减少程度
                double factor = reduceFactor * (highlightMask.at<uchar>(y, x) / 255.0);
                luminance.at<uchar>(y, x) = static_cast<uchar>(luminance.at<uchar>(y, x) * factor);
            }
        }
    }
    
    // 合并通道并转换回BGR
    labChannels[0] = luminance;
    merge(labChannels, labImage);
    cvtColor(labImage, image, COLOR_Lab2BGR);
    
    return image;
}

int main() {
    // 读取图像
    Mat image = imread("input.jpg");
    if (image.empty()) {
        cout << "无法加载图像!" << endl;
        return -1;
    }
    
    // 去除高光
    Mat result = removeHighlights(image, 200, 0.6);
    
    // 显示结果
    imshow("原始图像", image);
    imshow("去除高光后的图像", result);
    
    // 保存结果
    imwrite("output.jpg", result);
    
    waitKey(0);
    return 0;
}
//g++ -ID:\environment-galaxy\opencv\opencv\build\include -LD:\environment-galaxy\opencv\opencv\build\x64\vc16\lib -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs c+.cpp -o c+