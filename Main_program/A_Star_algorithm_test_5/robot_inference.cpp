#include "robot_inference.h"

cv::Mat get_detected_img(Net network, cv::Mat input_image, float score_threshold, cv::Point& robot_point) {

    int width = input_image.cols;
    int height = input_image.rows;
    cv::Scalar meanVal = cv::Scalar(104, 117, 123);
    cv::Size size = cv::Size(300, 300);

    cv::Mat input_blob = blobFromImage(input_image, 1.0, cv::Size(), cv::Scalar(), true);
    network.setInput(input_blob);
    cv::Mat detection = network.forward();

    cv::Mat detection_result(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());

    cv::Scalar green = cv::Scalar(0, 255, 0);

    for (int i = 0; i < detection_result.rows; i++) {

        float score = detection_result.at<float>(i, 2);

        if (score > score_threshold) {

            int left = static_cast<int>(detection_result.at<float>(i, 3) * width);
            int top = static_cast<int>(detection_result.at<float>(i, 4) * height);
            int right = static_cast<int>(detection_result.at<float>(i, 5) * width);
            int bottom = static_cast<int>(detection_result.at<float>(i, 6) * height);

            int x = (left + right) / 2;
            int y = (top + bottom) / 2;

            std::cout << "score : " << score << std::endl;

            robot_point = cv::Point(x, y);
            circle(input_image, robot_point, 4, cv::Scalar(0, 0, 255), -1);
            rectangle(input_image, cv::Point(left, top), cv::Point(right, bottom), green, 2);
        }
    }
    return input_image;
}