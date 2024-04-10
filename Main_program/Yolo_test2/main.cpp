//Yolo
#include <iostream>
#include <queue>
#include <iterator>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include "yolo.h"
using namespace std;
#include <windows.h> 
#include <opencv2/opencv.hpp>

#define CLASS_FILE_NUM			"robot_num_0_4.names"
#define CFG_FILE_NUM			"yolov4-robot_num_0_4.cfg"
#define WEIGHTS_FILE_NUM		"yolov4-robot_num_0_4_final.weights"

#define CLASS_FILE_ROBOT			"robot.names"
#define CFG_FILE_ROBOT				"yolov4-robot.cfg"
#define WEIGHTS_FILE_ROBOT			"yolov4-robot_final.weights"

#define CLASS_FILE_DIRECTION		"robot_direction.names"
#define CFG_FILE_DIRECTION			"yolov4-robot_direction.cfg"
#define WEIGHTS_FILE_DIRECTION		"yolov4-robot_direction_final.weights"

#define ROBOT_NUM 4

using namespace cv;
using namespace cv::dnn;

constexpr float CONFIDENCE_THRESHOLD_NUM = 0;
constexpr float NMS_THRESHOLD_NUM = 0.4;
constexpr int NUM_CLASSES_NUM = 6;

constexpr float CONFIDENCE_THRESHOLD_ROBOT = 0;
constexpr float NMS_THRESHOLD_ROBOT = 0.4;
constexpr int NUM_CLASSES_ROBOT = 1;

constexpr float CONFIDENCE_THRESHOLD_DIRECTION = 0;
constexpr float NMS_THRESHOLD_DIRECTION = 0.4;
constexpr int NUM_CLASSES_DIRECTION = 1;

//int real_num = 5;		//비교할 숫자
//int frame_cnt = 1000;	//테스트할 프레임 수

// colors for bounding boxes
const cv::Scalar colors[] = {
	{0, 255, 255},
	{255, 255, 0},
	{0, 255, 0},
	{255, 0, 0}
};
const auto NUM_COLORS = sizeof(colors) / sizeof(colors[0]);

int main()
{
	std::vector<std::string> num_class_names;
	{
		std::ifstream num_class_file(CLASS_FILE_NUM);
		if (!num_class_file)
		{
			std::cerr << "failed to open classes.txt\n";
			return 0;
		}

		std::string num_line;
		while (std::getline(num_class_file, num_line))
			num_class_names.push_back(num_line);
	}

	std::vector<std::string> robot_class_names;
	{
		std::ifstream robot_class_file(CLASS_FILE_ROBOT);
		if (!robot_class_file)
		{
			std::cerr << "failed to open classes.txt\n";
			return 0;
		}

		std::string robot_line;
		while (std::getline(robot_class_file, robot_line))
			robot_class_names.push_back(robot_line);
	}

	std::vector<std::string> direction_class_names;
	{
		std::ifstream direction_class_file(CLASS_FILE_DIRECTION);
		if (!direction_class_file)
		{
			std::cerr << "failed to open classes.txt\n";
			return 0;
		}

		std::string direction_line;
		while (std::getline(direction_class_file, direction_line))
			direction_class_names.push_back(direction_line);
	}


	auto num_net = cv::dnn::readNetFromDarknet(CFG_FILE_NUM, WEIGHTS_FILE_NUM);
	num_net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
	num_net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
	/*net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
	net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);*/
	auto num_output_names = num_net.getUnconnectedOutLayersNames();

	auto robot_net = cv::dnn::readNetFromDarknet(CFG_FILE_ROBOT, WEIGHTS_FILE_ROBOT);
	robot_net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
	robot_net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
	/*net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
	net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);*/
	auto robot_output_names = robot_net.getUnconnectedOutLayersNames();

	auto direction_net = cv::dnn::readNetFromDarknet(CFG_FILE_DIRECTION, WEIGHTS_FILE_DIRECTION);
	direction_net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
	direction_net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
	/*net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
	net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);*/
	auto direction_output_names = direction_net.getUnconnectedOutLayersNames();

	cv::VideoCapture source(1);
	source.set(cv::CAP_PROP_FRAME_WIDTH, 640);
	source.set(cv::CAP_PROP_FRAME_HEIGHT, 360);
	cv::Mat frame;
	cv::Mat blob;
	std::vector<cv::Mat> detections;
	std::vector<cv::Mat> detections2;
	std::vector<cv::Mat> detections3;

	//int cnt = 0;		//프레임 횟수
	//int same_cnt = 0;	//비교 숫자와 일치한 횟수
	int num_cnt[10] = { 0 };

	cv::Point robot_point[ROBOT_NUM];
	cv::Rect robot_rect[ROBOT_NUM];
	cv::Point direction_point[ROBOT_NUM];

	cv::Rect direction_rect[ROBOT_NUM];
	/*cv::Point detect_point[2];
	cv::Rect detect_rect[2];*/
	

	int robot_num = 1;
	while (1)
	{
		clock_t start, end;
		double time_result;
		start = clock();	// 수행 시간 측정 시작

		source >> frame;
		if (frame.empty())
		{
			cv::waitKey();
			return -1;
			break;
		}
		cv::Mat img = frame.clone();
		get_detected_robot(robot_net, num_net, direction_net, frame, img, 0.7, 
			robot_output_names, num_output_names, direction_output_names,
			NUM_CLASSES_ROBOT, NUM_CLASSES_NUM, NUM_CLASSES_DIRECTION, 
			CONFIDENCE_THRESHOLD_ROBOT, CONFIDENCE_THRESHOLD_NUM, CONFIDENCE_THRESHOLD_DIRECTION,
			NMS_THRESHOLD_ROBOT, NMS_THRESHOLD_NUM, NUM_CLASSES_DIRECTION,
			colors, NUM_COLORS, robot_class_names, num_class_names, direction_class_names,
			robot_point, robot_rect, direction_point, ROBOT_NUM);

		for (int i = 0; i < ROBOT_NUM; i++) {
			cv::rectangle(frame, cv::Point(robot_rect[i].x, robot_rect[i].y), cv::Point(robot_rect[i].x + robot_rect[i].width, robot_rect[i].y + robot_rect[i].height), cv::Scalar(0, 255, 0), 2);
			cv::circle(frame, robot_point[i], 3, cv::Scalar(0, 0, 255), -1);
			//cv::rectangle(frame, cv::Point(robot_rect[0].x, robot_rect[0].y), cv::Point(robot_rect[0].x + robot_rect[0].width, robot_rect[0].y + robot_rect[0].height), colors[0], 2);
			cv::circle(frame, direction_point[i], 3, cv::Scalar(0, 255, 0), -1);
			//cv::rectangle(frame, cv::Point(robot_rect[i].x, robot_rect[i].y - label_bg_sz.height - baseline - 5), cv::Point(rect.x + label_bg_sz.width - 20, rect.y), color, cv::FILLED);
			cv::putText(frame, std::to_string(i), robot_point[i], cv::FONT_HERSHEY_COMPLEX_SMALL, 1, cv::Scalar(0, 255, 255));
		}
		

		//std::cout << robot_point[0] << ", " << direction_point[0] << std::endl;

		//std::cout << num<<"?" << std::endl;

		/*get_detected_img(num_net, frame, img, 0.7, num_output_names,
			NUM_CLASSES_NUM, CONFIDENCE_THRESHOLD_NUM, NMS_THRESHOLD_NUM, 
			colors, NUM_COLORS, num_class_names, robot_point, robot_rect);*/
		/*get_detected_img(net, frame,img, 0.7, output_names,
			NUM_CLASSES, CONFIDENCE_THRESHOLD, NMS_THRESHOLD, 
			colors, NUM_COLORS, class_names, detect_point, detect_rect);*/
		/*get_detected_img(direction_net, frame, img, 0.7, direction_output_names,
			NUM_CLASSES_DIRECTION, CONFIDENCE_THRESHOLD_DIRECTION, NMS_THRESHOLD_DIRECTION,
			colors, NUM_COLORS, direction_class_names, direction_point, direction_rect);*/
		/*get_detected_direction_point(direction_net, frame, img, 0.7, direction_output_names,
			NUM_CLASSES_DIRECTION, CONFIDENCE_THRESHOLD_DIRECTION, NMS_THRESHOLD_DIRECTION,
			colors, NUM_COLORS, direction_class_names, direction_point);*/
		
		cv::imshow("frame", frame);
		cv::imshow("img", img);
		cv::waitKey(1);

		//Sleep(40);

		end = clock();		//시간 측정 끝
		time_result = (double)(end - start);
		time_result /= CLOCKS_PER_SEC;
		std::cout << "main while문 실행시간(1사이클) : " << time_result << "초" << std::endl;
	}

	/*double probability = num_cnt[real_num] / (double)cnt;

	std::cout << real_num << " 일치 횟수 : " << num_cnt[real_num] << ", cnt : " << cnt << std::endl;
	std::cout << real_num << " 확률 : " << probability << std::endl << std::endl;
	for (int i = 0; i < 10; i++) {
		std::cout << i << " 일치 횟수 : " << num_cnt[i] << ", 확률 : " << num_cnt[i] / (double)cnt << std::endl;
	}*/

	return 0;
}