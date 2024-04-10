#ifndef __ROBOT_INFERENCE_H__
#define __ROBOT_INFERENCE_H__

#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

//using namespace cv;
using namespace cv::dnn;

cv::Mat get_detected_img(Net network, cv::Mat input_image, float score_threshold, cv::Point& robot_point);

#endif