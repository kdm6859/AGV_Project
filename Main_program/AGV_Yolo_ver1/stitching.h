#ifndef __STITCHING__
#define __STITCHING__


#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <tuple>


class My_Stitching {

public:
    cv::Mat homography;


    std::tuple<std::vector<cv::KeyPoint>, cv::Mat> detect_and_describe(cv::Mat image);

    cv::Mat match_keypoints(cv::Mat imageA, cv::Mat imageB, std::vector<cv::KeyPoint> keypointA, std::vector<cv::KeyPoint> keypointB,
        cv::Mat featuresA, cv::Mat featuresB, float ratio, double repojThresh);

    cv::Mat stitch(cv::Mat imageA, cv::Mat imageB, std::string mode, float ratio = 0.75, double repojThresh = 4.0);
};

#endif
