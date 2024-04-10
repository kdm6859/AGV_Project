#include "robot_inference.h"

cv::Mat get_detected_img(Net network, cv::Mat input_image, float score_threshold, 
    cv::Point* robot_point, cv::Rect* robot_rect, int robot_num,
    Net num_network, Net direction_network,
    std::vector<cv::String> num_output_names, std::vector<cv::String> direction_output_names, 
    int NUM_CLASSES_NUM, int NUM_CLASSES_DIRECTION,
    float CONFIDENCE_THRESHOLD_NUM, float CONFIDENCE_THRESHOLD_DIRECTION,
    float NMS_THRESHOLD_NUM, float NMS_THRESHOLD_DIRECTION,
    const cv::Scalar* colors, unsigned long long NUM_COLORS,
    std::vector<std::string> num_class_names, std::vector<std::string> direction_class_names,
    cv::Point* direction_point) {

    int width = input_image.cols;
    int height = input_image.rows;
    //cv::Scalar meanVal = cv::Scalar(104, 117, 123);
    //cv::Size size = cv::Size(300, 300);

    cv::Mat input_blob = blobFromImage(input_image, 1.0, cv::Size(), cv::Scalar(), true);
    network.setInput(input_blob);
    cv::Mat detection = network.forward();

    cv::Mat detection_result(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());

    cv::Scalar green = cv::Scalar(0, 255, 0);

    int num = 0;
    cv::Rect* robot_temp_rect = new cv::Rect[robot_num];
    for (int i = 0; i < detection_result.rows; i++) {

        float score = detection_result.at<float>(i, 2);

        if (score > score_threshold) {

            int left = static_cast<int>(detection_result.at<float>(i, 3) * width);
            int top = static_cast<int>(detection_result.at<float>(i, 4) * height);
            int right = static_cast<int>(detection_result.at<float>(i, 5) * width);
            int bottom = static_cast<int>(detection_result.at<float>(i, 6) * height);

            int x = (left + right) / 2;
            int y = (top + bottom) / 2;

            //std::cout << "score : " << score << std::endl;

            cv::Rect temp_rect = cv::Rect(cv::Point(left - 12, top - 12), cv::Point(right + 12, bottom + 12));
            //int robot_id = robot_find_ID(input_image, temp_rect);
            robot_temp_rect[num] = temp_rect;
            num++;
            //robot_point[robot_id] = cv::Point(x, y);
            /*circle(input_image, robot_point[robot_id], 4, cv::Scalar(0, 0, 255), -1);
            putText(input_image, std::to_string(robot_id), robot_point[robot_id], 1, 2, cv::Scalar(255, 255, 0), 1, 8);
            rectangle(input_image, robot_rect[robot_id], green, 1);*/
        }
    }

    cv::Mat output_image;
    cv::Point* num_point = new cv::Point[robot_num];
    get_detected_num_point(num_network, input_image, output_image, 0.7, num_output_names,
        NUM_CLASSES_NUM, CONFIDENCE_THRESHOLD_NUM, NMS_THRESHOLD_NUM, colors,
        NUM_COLORS, num_class_names, num_point);
    //std::cout << num_point[0] << std::endl;
    for (int i = 0; i < robot_num; i++) {
        for (int j = 0; j < robot_num; j++) {
            if (num_point[i].inside(robot_temp_rect[j])) {
                robot_rect[i] = robot_temp_rect[j];
                robot_point[i] = cv::Point(robot_rect[i].x + (robot_rect[i].width / 2), robot_rect[i].y + (robot_rect[i].height / 2));
                //std::cout << "¤·¤·" << std::endl;
                break;
            }
        }
    }
    delete[] num_point;

    cv::Point* direction_temp_point = new cv::Point[robot_num];
    get_detected_direction_point(direction_network, input_image, output_image, 0.7, direction_output_names,
        NUM_CLASSES_DIRECTION, CONFIDENCE_THRESHOLD_DIRECTION, NMS_THRESHOLD_DIRECTION,
        colors, NUM_COLORS, direction_class_names, direction_temp_point);
    for (int i = 0; i < robot_num; i++) {
        for (int j = 0; j < robot_num; j++) {
            if (direction_temp_point[j].inside(robot_rect[i])) {
                direction_point[i] = direction_temp_point[j];
                break;
            }
        }
    }

    delete[] direction_temp_point;
    delete[] robot_temp_rect;


    return input_image;
}

int robot_find_ID(cv::Mat frame, cv::Rect robot_rect) {
    if (robot_rect.x < 0)
        robot_rect.x = 0;
    if (robot_rect.y < 0)
        robot_rect.y = 0;
    if (robot_rect.x + robot_rect.width >= frame.cols)
        robot_rect.width = frame.cols - 1 - robot_rect.x;
    if (robot_rect.y + robot_rect.height >= frame.rows)
        robot_rect.height = frame.rows - 1 - robot_rect.y;

    cv::Mat robot_roi = frame(robot_rect).clone();
    cv::cvtColor(robot_roi, robot_roi, cv::COLOR_BGR2HSV);

    //ÆÄ¶û ½ºÆ¼Ä¿
    cv::Scalar lower_blue = cv::Scalar(100, 100, 50);
    cv::Scalar upper_blue = cv::Scalar(120, 240, 130);
    //»¡°­ ½ºÆ¼Ä¿
    cv::Scalar lower_red = cv::Scalar(0, 150, 70);
    cv::Scalar upper_red = cv::Scalar(15, 235, 165);

    cv::Mat blue_mat, red_mat;

    inRange(robot_roi, lower_blue, upper_blue, blue_mat);
    inRange(robot_roi, lower_red, upper_red, red_mat);

    int blue_num = 0, red_num = 0;

    for (int r = 0; r < robot_roi.rows; r++) {
        for (int c = 0; c < robot_roi.cols; c++) {
            if (blue_mat.at<uchar>(r, c) > 0)
                blue_num++;
            if (red_mat.at<uchar>(r, c) > 0)
                red_num++;
        }
    }

    if (blue_num < red_num)
        return 0;
    else
        return 1;
}

cv::Point robot_find_head(cv::Mat frame, cv::Rect robot_rect) {
    if (robot_rect.x < 0)
        robot_rect.x = 0;
    if (robot_rect.y < 0)
        robot_rect.y = 0;
    if (robot_rect.x + robot_rect.width >= frame.cols)
        robot_rect.width = frame.cols - 1 - robot_rect.x;
    if (robot_rect.y + robot_rect.height >= frame.rows)
        robot_rect.height = frame.rows - 1 - robot_rect.y;



    cv::Mat robot_roi = frame(robot_rect).clone();
    cv::cvtColor(robot_roi, robot_roi, cv::COLOR_BGR2HSV);
    //cv::Scalar lower_yellow = cv::Scalar(5, 85, 100); //cv::Scalar(5, 110, 140)
    //cv::Scalar upper_yellow = cv::Scalar(25, 170, 200);
    cv::Scalar lower_yellow = cv::Scalar(13, 100, 100);
    cv::Scalar upper_yellow = cv::Scalar(33, 235, 200);
    inRange(robot_roi, lower_yellow, upper_yellow, robot_roi);

    int nCount = 0;
    int nSumX = 0;
    int nSumY = 0;

    for (int r = 0; r < robot_roi.rows; r++) {
        for (int c = 0; c < robot_roi.cols; c++) {
            if (robot_roi.at<uchar>(r, c) > 0) {
                nSumX += c;
                nSumY += r;
                nCount += 1;
            }
        }
    }
    if (nCount == 0)
        return cv::Point(-1, -1);

    nSumX /= nCount;
    nSumY /= nCount;

    if (nSumX == 0 || nSumY == 0) {
        nSumX = -1;
        nSumX = -1;
    }   

    return cv::Point(nSumX + robot_rect.x, nSumY + robot_rect.y);
}

//double robot_angle(cv::Point robot_head_point, cv::Point start_point, cv::Point direction_point) {
//    double a, b, c;
//    double angle, temp;
//
//    a = sqrt(pow(robot_head_point.x - direction_point.x, 2) + pow(robot_head_point.y - direction_point.y, 2));
//    b = sqrt(pow(robot_head_point.x - start_point.x, 2) + pow(robot_head_point.y - start_point.y, 2));
//    c = sqrt(pow(start_point.x - direction_point.x, 2) + pow(start_point.y - direction_point.y, 2));
//
//    temp = (pow(b, 2) + pow(c, 2) - pow(a, 2)) / (2 * b * c);
//
//    angle = acos(temp);
//    angle = angle * (180 / PI);
//
//    return angle;
//}

double getAngle(cv::Point start, cv::Point end) {
    double dy = end.y - start.y;
    double dx = end.x - start.x;
    double angle = atan(dy / dx) * (180.0 / PI);
    if (dx < 0.0) {
        angle += 180.0;
    }
    else {
        if (dy < 0.0) angle += 360.0;
    }

    return angle;
}

//bool robot_rotation_direction(double direction_angle, double robot_head_angle) {
//    //double direction_angle = getAngle(start_point, direction_point);
//    //double robot_head_angle = getAngle(start_point, robot_head_point);
//
//    if (robot_head_angle > 180 + direction_angle || (robot_head_angle >= 0 && robot_head_angle <= 30))
//        return false;
//    else
//        return true;
//}