#ifndef __YOLO_H__
#define __YOLO_H__

#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

using namespace cv::dnn;

void get_detected_img1(Net network, cv::Mat input_image, cv::Mat& output_image, float score_threshold,
    std::vector<cv::String> output_names, int NUM_CLASSES, float CONFIDENCE_THRESHOLD,
    float NMS_THRESHOLD, const cv::Scalar* colors, unsigned long long NUM_COLORS,
    std::vector<std::string> class_names, cv::Point* detect_point, cv::Rect* detect_rect);

void get_detected_num_point(Net network, cv::Mat input_image, cv::Mat& output_image, float score_threshold,
    std::vector<cv::String> output_names, int NUM_CLASSES, float CONFIDENCE_THRESHOLD,
    float NMS_THRESHOLD, const cv::Scalar* colors, unsigned long long NUM_COLORS,
    std::vector<std::string> class_names, cv::Point* point);

void get_detected_direction_point(Net network, cv::Mat input_image, cv::Mat& output_image, float score_threshold,
    std::vector<cv::String> output_names, int NUM_CLASSES, float CONFIDENCE_THRESHOLD,
    float NMS_THRESHOLD, const cv::Scalar* colors, unsigned long long NUM_COLORS,
    std::vector<std::string> class_names, cv::Point* point);

void get_detected_robot(Net robot_network, Net num_network, Net direction_network,
    cv::Mat input_image, cv::Mat& output_image, float score_threshold,
    std::vector<cv::String> robot_output_names, std::vector<cv::String> num_output_names,
    std::vector<cv::String> direction_output_names, int NUM_CLASSES_ROBOT,
    int NUM_CLASSES_NUM, int NUM_CLASSES_DIRECTION, float CONFIDENCE_THRESHOLD_ROBOT,
    float CONFIDENCE_THRESHOLD_NUM, float CONFIDENCE_THRESHOLD_DIRECTION,
    float NMS_THRESHOLD_ROBOT, float NMS_THRESHOLD_NUM, float NMS_THRESHOLD_DIRECTION,
    const cv::Scalar* colors, unsigned long long NUM_COLORS,
    std::vector<std::string> robot_class_names, std::vector<std::string> num_class_names,
    std::vector<std::string> direction_class_names,
    cv::Point* robot_point, cv::Rect* robot_rect, cv::Point* direction_point, int robot_num);

#endif