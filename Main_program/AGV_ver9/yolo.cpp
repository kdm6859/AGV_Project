#include "yolo.h"

//cv::Mat get_detected_img(Net network, cv::Mat input_image, float score_threshold, cv::Point* robot_point, cv::Rect* robot_rect, int robot_num) {
//
//    int width = input_image.cols;
//    int height = input_image.rows;
//    //cv::Scalar meanVal = cv::Scalar(104, 117, 123);
//    //cv::Size size = cv::Size(300, 300);
//
//    cv::Mat input_blob = blobFromImage(input_image, 1.0, cv::Size(), cv::Scalar(), true);
//    network.setInput(input_blob);
//    cv::Mat detection = network.forward();
//
//    cv::Mat detection_result(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());
//
//    cv::Scalar green = cv::Scalar(0, 255, 0);
//
//    for (int i = 0; i < detection_result.rows; i++) {
//
//        float score = detection_result.at<float>(i, 2);
//
//        if (score > score_threshold) {
//
//            int left = static_cast<int>(detection_result.at<float>(i, 3) * width);
//            int top = static_cast<int>(detection_result.at<float>(i, 4) * height);
//            int right = static_cast<int>(detection_result.at<float>(i, 5) * width);
//            int bottom = static_cast<int>(detection_result.at<float>(i, 6) * height);
//
//            int x = (left + right) / 2;
//            int y = (top + bottom) / 2;
//
//            //std::cout << "score : " << score << std::endl;
//
//            cv::Rect temp_rect = cv::Rect(cv::Point(left - 12, top - 12), cv::Point(right + 12, bottom + 12));
//            int robot_id = robot_find_ID(input_image, temp_rect);
//            robot_rect[robot_id] = temp_rect;
//            robot_point[robot_id] = cv::Point(x, y);
//            circle(input_image, robot_point[robot_id], 4, cv::Scalar(0, 0, 255), -1);
//            putText(input_image, std::to_string(robot_id), robot_point[robot_id], 1, 2, cv::Scalar(255, 255, 0), 1, 8);
//            rectangle(input_image, robot_rect[robot_id], green, 1);
//        }
//    }
//    return input_image;
//}

void get_detected_robot(Net network, cv::Mat input_image, cv::Mat& output_image, float score_threshold, 
    std::vector<cv::String> output_names, int NUM_CLASSES, float CONFIDENCE_THRESHOLD, 
    float NMS_THRESHOLD, const cv::Scalar *colors, unsigned long long NUM_COLORS, 
    std::vector<std::string> class_names, cv::Point* detect_point, cv::Rect* detect_rect) {

    cv::Mat input_img = input_image.clone();

    cv::Mat input_blob = blobFromImage(input_img, 0.00392, cv::Size(416, 416), cv::Scalar(), true, false, CV_32F);
    network.setInput(input_blob);
    std::vector<cv::Mat> detections;
    network.forward(detections, output_names);

    std::vector<int>* indices = new std::vector<int>[NUM_CLASSES];
    std::vector<cv::Rect>* boxes = new std::vector<cv::Rect>[NUM_CLASSES];
    std::vector<float>* scores = new std::vector<float>[NUM_CLASSES];

    for (auto& output : detections)
    {
        const auto num_boxes = output.rows;
        for (int i = 0; i < num_boxes; i++)
        {
            auto x = output.at<float>(i, 0) * input_img.cols;
            auto y = output.at<float>(i, 1) * input_img.rows;
            auto width = output.at<float>(i, 2) * input_img.cols;
            auto height = output.at<float>(i, 3) * input_img.rows;
            cv::Rect rect(x - width / 2, y - height / 2, width, height);

            for (int c = 0; c < NUM_CLASSES; c++)
            {
                auto confidence = *output.ptr<float>(i, 5 + c);
                if (confidence >= CONFIDENCE_THRESHOLD)
                {
                    boxes[c].push_back(rect);
                    scores[c].push_back(confidence);
                }
            }
        }
    }

    for (int c = 0; c < NUM_CLASSES; c++)
        cv::dnn::NMSBoxes(boxes[c], scores[c], score_threshold, NMS_THRESHOLD, indices[c]);

    //int num = 0;
    for (int c = 0; c < NUM_CLASSES; c++)
    {
        for (size_t i = 0; i < indices[c].size(); ++i)
        {
            const auto color = colors[c % NUM_COLORS];

            auto idx = indices[c][i];
            const auto& rect = boxes[c][idx];
            
            cv::Rect temp_rect = cv::Rect(rect.x - 3, rect.y - 3, rect.width + 6, rect.height + 6);
            int robot_id = robot_find_ID(input_image, temp_rect);
            detect_rect[robot_id] = temp_rect;
            detect_point[robot_id] = cv::Point(rect.x + (rect.width / 2), rect.y + (rect.height / 2));
            cv::rectangle(output_image, cv::Point(detect_rect[robot_id].x, detect_rect[robot_id].y), cv::Point(detect_rect[robot_id].x + detect_rect[robot_id].width, detect_rect[robot_id].y + detect_rect[robot_id].height), color, 1);
            cv::circle(output_image, cv::Point(detect_rect[robot_id].x + (detect_rect[robot_id].width / 2), detect_rect[robot_id].y + (detect_rect[robot_id].height / 2)), 3, cv::Scalar(0, 0, 255), -1);
            //num++;

            std::ostringstream label_ss;
            label_ss << class_names[c] << ": " << std::fixed << std::setprecision(2) << scores[c][idx];
            auto label = label_ss.str();

            int baseline;
            auto label_bg_sz = cv::getTextSize(label.c_str(), cv::FONT_HERSHEY_COMPLEX_SMALL, 1, 1, &baseline);
            putText(output_image, std::to_string(robot_id), detect_point[robot_id], 1, 2, cv::Scalar(255, 255, 0), 1, 8);
            /*cv::rectangle(output_image, cv::Point(rect.x, rect.y - label_bg_sz.height - baseline - 5), cv::Point(rect.x + label_bg_sz.width - 20, rect.y), color, cv::FILLED);
            cv::putText(output_image, label.c_str(), cv::Point(rect.x, rect.y - baseline - 5), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.7, cv::Scalar(0, 0, 0));*/
        }
    }

    delete[] indices;
    delete[] boxes;
    delete[] scores;
}

//void get_detected_num_point(Net network, cv::Mat input_image, cv::Mat& output_image, float score_threshold,
//    std::vector<cv::String> output_names, int NUM_CLASSES, float CONFIDENCE_THRESHOLD,
//    float NMS_THRESHOLD, const cv::Scalar* colors, unsigned long long NUM_COLORS,
//    std::vector<std::string> class_names, cv::Point* point) {
//
//    cv::Mat input_img = input_image.clone();
//
//    cv::Mat input_blob = blobFromImage(input_img, 0.00392, cv::Size(416, 416), cv::Scalar(), true, false, CV_32F);
//    network.setInput(input_blob);
//    std::vector<cv::Mat> detections;
//    network.forward(detections, output_names);
//
//    std::vector<int>* indices = new std::vector<int>[NUM_CLASSES];
//    std::vector<cv::Rect>* boxes = new std::vector<cv::Rect>[NUM_CLASSES];
//    std::vector<float>* scores = new std::vector<float>[NUM_CLASSES];
//
//    for (auto& output : detections)
//    {
//        const auto num_boxes = output.rows;
//        for (int i = 0; i < num_boxes; i++)
//        {
//            auto x = output.at<float>(i, 0) * input_img.cols;
//            auto y = output.at<float>(i, 1) * input_img.rows;
//            auto width = output.at<float>(i, 2) * input_img.cols;
//            auto height = output.at<float>(i, 3) * input_img.rows;
//            cv::Rect rect(x - width / 2, y - height / 2, width, height);
//
//            for (int c = 0; c < NUM_CLASSES; c++)
//            {
//                auto confidence = *output.ptr<float>(i, 5 + c);
//                if (confidence >= CONFIDENCE_THRESHOLD)
//                {
//                    boxes[c].push_back(rect);
//                    scores[c].push_back(confidence);
//                }
//            }
//        }
//    }
//
//    for (int c = 0; c < NUM_CLASSES; c++)
//        cv::dnn::NMSBoxes(boxes[c], scores[c], score_threshold, NMS_THRESHOLD, indices[c]);
//
//    /*std::cout << "num start" << std::endl;
//    cv::imshow("test", input_img);
//    cv::waitKey(1);*/
//
//    int num;
//    for (int c = 0; c < NUM_CLASSES; c++)
//    {
//        for (size_t i = 0; i < indices[c].size(); ++i)
//        {
//            const auto color = colors[c % NUM_COLORS];
//
//            auto idx = indices[c][i];
//            const auto& rect = boxes[c][idx];
//            cv::rectangle(output_image, cv::Point(rect.x, rect.y), cv::Point(rect.x + rect.width, rect.y + rect.height), color, 2);
//            cv::circle(output_image, cv::Point(rect.x + (rect.width / 2), rect.y + (rect.height / 2)), 3, cv::Scalar(0, 0, 255), -1);
//
//            std::ostringstream label_ss;
//            label_ss << class_names[c] << ": " << std::fixed << std::setprecision(2) << scores[c][idx];
//            auto label = label_ss.str();
//
//            num = atoi(class_names[c].c_str());
//
//            point[num] = cv::Point(rect.x + (rect.width / 2), rect.y + (rect.height / 2));
//            //cv::circle(output_image, point[num], 3, cv::Scalar(0, 0, 255), -1);
//
//            int baseline;
//            auto label_bg_sz = cv::getTextSize(label.c_str(), cv::FONT_HERSHEY_COMPLEX_SMALL, 1, 1, &baseline);
//            //cv::rectangle(output_image, cv::Point(rect.x, rect.y - label_bg_sz.height - baseline - 5), cv::Point(rect.x + label_bg_sz.width - 20, rect.y), color, cv::FILLED);
//            //cv::putText(output_image, label.c_str(), cv::Point(rect.x, rect.y - baseline - 5), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.7, cv::Scalar(0, 0, 0));
//        }
//    }
//
//    delete[] indices;
//    delete[] boxes;
//    delete[] scores;
//}
//
//void get_detected_direction_point(Net network, cv::Mat input_image, cv::Mat& output_image, float score_threshold,
//    std::vector<cv::String> output_names, int NUM_CLASSES, float CONFIDENCE_THRESHOLD,
//    float NMS_THRESHOLD, const cv::Scalar* colors, unsigned long long NUM_COLORS,
//    std::vector<std::string> class_names, cv::Point* point) {
//
//    cv::Mat input_img = input_image.clone();
//
//    cv::Mat input_blob = blobFromImage(input_img, 0.00392, cv::Size(416, 416), cv::Scalar(), true, false, CV_32F);
//    network.setInput(input_blob);
//    std::vector<cv::Mat> detections;
//    network.forward(detections, output_names);
//
//    std::vector<int>* indices = new std::vector<int>[NUM_CLASSES];
//    std::vector<cv::Rect>* boxes = new std::vector<cv::Rect>[NUM_CLASSES];
//    std::vector<float>* scores = new std::vector<float>[NUM_CLASSES];
//
//    for (auto& output : detections)
//    {
//        const auto num_boxes = output.rows;
//        for (int i = 0; i < num_boxes; i++)
//        {
//            auto x = output.at<float>(i, 0) * input_img.cols;
//            auto y = output.at<float>(i, 1) * input_img.rows;
//            auto width = output.at<float>(i, 2) * input_img.cols;
//            auto height = output.at<float>(i, 3) * input_img.rows;
//            cv::Rect rect(x - width / 2, y - height / 2, width, height);
//
//            for (int c = 0; c < NUM_CLASSES; c++)
//            {
//                auto confidence = *output.ptr<float>(i, 5 + c);
//                if (confidence >= CONFIDENCE_THRESHOLD)
//                {
//                    boxes[c].push_back(rect);
//                    scores[c].push_back(confidence);
//                }
//            }
//        }
//    }
//
//    for (int c = 0; c < NUM_CLASSES; c++)
//        cv::dnn::NMSBoxes(boxes[c], scores[c], score_threshold, NMS_THRESHOLD, indices[c]);
//
//    /*std::cout << "num start" << std::endl;
//    cv::imshow("test", input_img);
//    cv::waitKey(1);*/
//
//    int num = 0;
//    for (int c = 0; c < NUM_CLASSES; c++)
//    {
//        for (size_t i = 0; i < indices[c].size(); ++i)
//        {
//            const auto color = colors[c % NUM_COLORS];
//
//            auto idx = indices[c][i];
//            const auto& rect = boxes[c][idx];
//            cv::rectangle(output_image, cv::Point(rect.x, rect.y), cv::Point(rect.x + rect.width, rect.y + rect.height), color, 2);
//            cv::circle(output_image, cv::Point(rect.x + (rect.width / 2), rect.y + (rect.height / 2)), 3, cv::Scalar(0, 0, 255), -1);
//
//            std::ostringstream label_ss;
//            label_ss << class_names[c] << ": " << std::fixed << std::setprecision(2) << scores[c][idx];
//            auto label = label_ss.str();
//
//            //num = atoi(class_names[c].c_str());
//
//            point[num] = cv::Point(rect.x + (rect.width / 2), rect.y + (rect.height / 2));
//            num++;
//            //cv::circle(output_image, point[num], 3, cv::Scalar(0, 0, 255), -1);
//
//            int baseline;
//            auto label_bg_sz = cv::getTextSize(label.c_str(), cv::FONT_HERSHEY_COMPLEX_SMALL, 1, 1, &baseline);
//            //cv::rectangle(output_image, cv::Point(rect.x, rect.y - label_bg_sz.height - baseline - 5), cv::Point(rect.x + label_bg_sz.width - 20, rect.y), color, cv::FILLED);
//            //cv::putText(output_image, label.c_str(), cv::Point(rect.x, rect.y - baseline - 5), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.7, cv::Scalar(0, 0, 0));
//        }
//    }
//
//    delete[] indices;
//    delete[] boxes;
//    delete[] scores;
//}
//
//void get_detected_robot(Net robot_network, Net num_network, Net direction_network, 
//    cv::Mat input_image, cv::Mat& output_image, float score_threshold,
//    std::vector<cv::String> robot_output_names, std::vector<cv::String> num_output_names,
//    std::vector<cv::String> direction_output_names, int NUM_CLASSES_ROBOT, 
//    int NUM_CLASSES_NUM, int NUM_CLASSES_DIRECTION, float CONFIDENCE_THRESHOLD_ROBOT,
//    float CONFIDENCE_THRESHOLD_NUM, float CONFIDENCE_THRESHOLD_DIRECTION, 
//    float NMS_THRESHOLD_ROBOT, float NMS_THRESHOLD_NUM, float NMS_THRESHOLD_DIRECTION,
//    const cv::Scalar* colors, unsigned long long NUM_COLORS,
//    std::vector<std::string> robot_class_names, std::vector<std::string> num_class_names,
//    std::vector<std::string> direction_class_names,
//    cv::Point* robot_point, cv::Rect* robot_rect, cv::Point* direction_point, int robot_num) {
//
//    cv::Mat input_img = input_image.clone();
//
//    cv::Mat input_blob = blobFromImage(input_img, 0.00392, cv::Size(416, 416), cv::Scalar(), true, false, CV_32F);
//    robot_network.setInput(input_blob);
//    std::vector<cv::Mat> detections;
//    robot_network.forward(detections, robot_output_names);
//
//    std::vector<int>* indices = new std::vector<int>[NUM_CLASSES_ROBOT];
//    std::vector<cv::Rect>* boxes = new std::vector<cv::Rect>[NUM_CLASSES_ROBOT];
//    std::vector<float>* scores = new std::vector<float>[NUM_CLASSES_ROBOT];
//
//    for (auto& output : detections)
//    {
//        const auto num_boxes = output.rows;
//        for (int i = 0; i < num_boxes; i++)
//        {
//            auto x = output.at<float>(i, 0) * input_img.cols;
//            auto y = output.at<float>(i, 1) * input_img.rows;
//            auto width = output.at<float>(i, 2) * input_img.cols;
//            auto height = output.at<float>(i, 3) * input_img.rows;
//            cv::Rect rect(x - width / 2, y - height / 2, width, height);
//
//            for (int c = 0; c < NUM_CLASSES_ROBOT; c++)
//            {
//                auto confidence = *output.ptr<float>(i, 5 + c);
//                if (confidence >= CONFIDENCE_THRESHOLD_ROBOT)
//                {
//                    boxes[c].push_back(rect);
//                    scores[c].push_back(confidence);
//                }
//            }
//        }
//    }
//
//    for (int c = 0; c < NUM_CLASSES_ROBOT; c++)
//        cv::dnn::NMSBoxes(boxes[c], scores[c], score_threshold, NMS_THRESHOLD_ROBOT, indices[c]);
//
//    int num=0;
//    cv::Rect* robot_temp_rect = new cv::Rect[robot_num];
//    for (int c = 0; c < NUM_CLASSES_ROBOT; c++)
//    {
//        for (size_t i = 0; i < indices[c].size(); ++i)
//        {
//            const auto color = colors[c % NUM_COLORS];
//
//            auto idx = indices[c][i];
//            const auto& rect = boxes[c][idx];
//            //cv::rectangle(output_image, cv::Point(rect.x, rect.y), cv::Point(rect.x + rect.width, rect.y + rect.height), color, 2);
//            //cv::circle(output_image, cv::Point(rect.x + (rect.width / 2), rect.y + (rect.height / 2)), 3, cv::Scalar(255, 0, 0), -1);
//            
//            /*std::cout << "num start" << std::endl;
//            cv::imshow("test", input_img(rect));
//            cv::waitKey(1);*/
//
//            /*get_detected_num(num_network, input_img(rect), output_image, 0.7, num_output_names,
//                NUM_CLASSES_NUM, CONFIDENCE_THRESHOLD_NUM, NMS_THRESHOLD_NUM, colors,
//                NUM_COLORS, num_class_names, num);*/
//            //std::cout << "num end" << std::endl;
//
//            //robot_temp_rect[num] = rect;
//            robot_temp_rect[num] = cv::Rect(rect.x - 5, rect.y - 5, rect.width + 10, rect.height + 10);
//            //detect_point[num] = cv::Point(rect.x + (rect.width / 2), rect.y + (rect.height / 2));
//            num++;
//
//            std::ostringstream label_ss;
//            label_ss << robot_class_names[c] << ": " << std::fixed << std::setprecision(2) << scores[c][idx];
//            auto label = label_ss.str();
//
//            int baseline;
//            auto label_bg_sz = cv::getTextSize(label.c_str(), cv::FONT_HERSHEY_COMPLEX_SMALL, 1, 1, &baseline);
//            //cv::rectangle(output_image, cv::Point(rect.x, rect.y - label_bg_sz.height - baseline - 5), cv::Point(rect.x + label_bg_sz.width - 20, rect.y), color, cv::FILLED);
//            //cv::putText(output_image, std::to_string(num) + label.c_str(), cv::Point(rect.x, rect.y - baseline - 5), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.7, cv::Scalar(0, 0, 0));
//        }
//    }
//
//    delete[] indices;
//    delete[] boxes;
//    delete[] scores;
//
//    cv::Point* num_point = new cv::Point[robot_num];
//    get_detected_num_point(num_network, input_img, output_image, 0.7, num_output_names,
//        NUM_CLASSES_NUM, CONFIDENCE_THRESHOLD_NUM, NMS_THRESHOLD_NUM, colors,
//        NUM_COLORS, num_class_names, num_point);
//    //std::cout << num_point[0] << std::endl;
//    for (int i = 0; i < robot_num; i++) {
//        for (int j = 0; j < robot_num; j++) {
//            if (num_point[i].inside(robot_temp_rect[j])) {
//                robot_rect[i] = robot_temp_rect[j];
//                robot_point[i] = cv::Point(robot_rect[i].x + (robot_rect[i].width / 2), robot_rect[i].y + (robot_rect[i].height / 2));
//                //std::cout << "¤·¤·" << std::endl;
//                break;
//            }
//        }
//    }
//    //std::cout << "2" << robot_rect[0] << std::endl;
//    delete[] num_point;
//
//    cv::Point* direction_temp_point = new cv::Point[robot_num];
//    get_detected_direction_point(direction_network, input_img, output_image, 0.7, direction_output_names,
//        NUM_CLASSES_DIRECTION, CONFIDENCE_THRESHOLD_DIRECTION, NMS_THRESHOLD_DIRECTION, 
//        colors, NUM_COLORS, direction_class_names, direction_temp_point);
//    for (int i = 0; i < robot_num; i++) {
//        for (int j = 0; j < robot_num; j++) {
//            if (direction_temp_point[j].inside(robot_rect[i])) {
//                direction_point[i] = direction_temp_point[j];
//                break;
//            }
//        }
//    }
//    
//    delete[] direction_temp_point;
//    delete[] robot_temp_rect;
//}