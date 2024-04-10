#ifndef __ROBOT_INFERENCE_H__
#define __ROBOT_INFERENCE_H__

#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <math.h>
#include "yolo.h"

#define PI 3.141592

using namespace cv::dnn;

cv::Mat get_detected_img(Net network, cv::Mat input_image, float score_threshold,
    cv::Point* robot_point, cv::Rect* robot_rect, int robot_num,
    Net num_network, Net direction_network,
    std::vector<cv::String> num_output_names, std::vector<cv::String> direction_output_names,
    int NUM_CLASSES_NUM, int NUM_CLASSES_DIRECTION,
    float CONFIDENCE_THRESHOLD_NUM, float CONFIDENCE_THRESHOLD_DIRECTION,
    float NMS_THRESHOLD_NUM, float NMS_THRESHOLD_DIRECTION,
    const cv::Scalar* colors, unsigned long long NUM_COLORS,
    std::vector<std::string> num_class_names, std::vector<std::string> direction_class_names,
    cv::Point* direction_point);
int robot_find_ID(cv::Mat frame, cv::Rect robot_rect);
cv::Point robot_find_head(cv::Mat frame, cv::Rect robot_rect);
//double robot_angle(cv::Point robot_head_point, cv::Point start_point, cv::Point direction_point);
double getAngle(cv::Point start, cv::Point end);
//bool robot_rotation_direction(double direction_angle, double robot_head_angle);

#endif