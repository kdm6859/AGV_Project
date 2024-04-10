//Yolo
#include <iostream>
#include <queue>
#include <iterator>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <ctime>
using namespace std;

#include <opencv2/opencv.hpp>

#define CLASS_FILE			"robot.names"
#define CFG_FILE			"yolov4-robot.cfg"
#define WEIGHTS_FILE		"yolov4-robot_final.weights"

using namespace cv;
using namespace cv::dnn;

constexpr float CONFIDENCE_THRESHOLD = 0;
constexpr float NMS_THRESHOLD = 0.4;
constexpr int NUM_CLASSES = 1;

int real_num = 5;		//비교할 숫자
int frame_cnt = 1000;	//테스트할 프레임 수

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
	std::vector<std::string> class_names;
	{
		std::ifstream class_file(CLASS_FILE);
		if (!class_file)
		{
			std::cerr << "failed to open classes.txt\n";
			return 0;
		}

		std::string line;
		while (std::getline(class_file, line))
			class_names.push_back(line);
	}

	int myproms;
	auto net = cv::dnn::readNetFromDarknet(CFG_FILE, WEIGHTS_FILE);
	/*auto net = cv::dnn::readNetFromDarknet("yolov4-robot.cfg", "yolov4-robot_final.weights");*/
	net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
	net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
	/*net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
	net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);*/
	auto output_names = net.getUnconnectedOutLayersNames();

	//cv::VideoCapture source("2.mp4");
	cv::VideoCapture source(1);
	source.set(cv::CAP_PROP_FRAME_WIDTH, 640);
	source.set(cv::CAP_PROP_FRAME_HEIGHT, 360);
	cv::Mat frame;
	cv::Mat blob;
	std::vector<cv::Mat> detections;

	int cnt = 0;		//프레임 횟수
	//int same_cnt = 0;	//비교 숫자와 일치한 횟수
	int num_cnt[10] = { 0 };

	while (1)
	{
		int64 t1 = getTickCount();
		source >> frame;
		if (frame.empty())
		{
			cv::waitKey();
			return -1;
			break;
		}
		
		cv::Rect rect(300, 110, 200, 200);
		cv::circle(frame, cv::Point(300, 110), 3, cv::Scalar(0, 0, 255), -1);

		/*auto total_start = std::chrono::steady_clock::now();*/
		cv::dnn::blobFromImage(frame(rect), blob, 0.00392, cv::Size(416, 416), cv::Scalar(), true, false, CV_32F);
		net.setInput(blob);

		/*auto dnn_start = std::chrono::steady_clock::now();*/
		net.forward(detections, output_names);
		/*auto dnn_end = std::chrono::steady_clock::now();*/
		std::vector<int> indices[NUM_CLASSES];
		std::vector<cv::Rect> boxes[NUM_CLASSES];
		std::vector<float> scores[NUM_CLASSES];

		for (auto& output : detections)
		{
			const auto num_boxes = output.rows;
			for (int i = 0; i < num_boxes; i++)
			{
				auto x = output.at<float>(i, 0) * frame(rect).cols;
				auto y = output.at<float>(i, 1) * frame(rect).rows;
				auto width = output.at<float>(i, 2) * frame(rect).cols;
				auto height = output.at<float>(i, 3) * frame(rect).rows;
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
			cv::dnn::NMSBoxes(boxes[c], scores[c], 0.7, NMS_THRESHOLD, indices[c]);

		for (int c = 0; c < NUM_CLASSES; c++)
		{
			for (size_t i = 0; i < indices[c].size(); ++i)
			{
				const auto color = colors[c % NUM_COLORS];

				auto idx = indices[c][i];
				const auto& rect = boxes[c][idx];
				cv::rectangle(frame(rect), cv::Point(rect.x, rect.y), cv::Point(rect.x + rect.width, rect.y + rect.height), color, 2);
				//cv::circle(frame, cv::Point(rect.x + (rect.width / 2), rect.y + (rect.height / 2)), 3, cv::Scalar(0, 0, 255), -1);
				std::ostringstream label_ss;
				label_ss << class_names[c] << ": " << std::fixed << std::setprecision(2) << scores[c][idx];
				auto label = label_ss.str();
				
				int int_class_names = atoi(class_names[c].c_str());
				/*if (real_num == int_class_names) {
					same_cnt++;
				}*/
				for (int j = 0; j < 10; j++) {
					if (j == int_class_names)
						num_cnt[j]++;
				}

				int baseline;
				auto label_bg_sz = cv::getTextSize(label.c_str(), cv::FONT_HERSHEY_COMPLEX_SMALL, 1, 1, &baseline);
				cv::rectangle(frame(rect), cv::Point(rect.x, rect.y - label_bg_sz.height - baseline - 5), cv::Point(rect.x + label_bg_sz.width - 20, rect.y), color, cv::FILLED);
				cv::putText(frame(rect), label.c_str(), cv::Point(rect.x, rect.y - baseline - 5), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.7, cv::Scalar(0, 0, 0));
			}
		}
		

		/*auto total_end = std::chrono::steady_clock::now();*/

		/*float inference_fps = 1000.0 / std::chrono::duration_cast<std::chrono::milliseconds>(dnn_end - dnn_start).count();
		float total_fps = 1000.0 / std::chrono::duration_cast<std::chrono::milliseconds>(total_end - total_start).count();*/

		/*std::ostringstream stats_ss;
		stats_ss << std::fixed << std::setprecision(2);
		stats_ss << "Inference FPS: " << inference_fps << ", Total FPS: " << total_fps;
		auto stats = stats_ss.str();*/

		/*int baseline;
		auto stats_bg_sz = cv::getTextSize(stats.c_str(), cv::FONT_HERSHEY_COMPLEX_SMALL, 1, 1, &baseline);
		cv::rectangle(frame, cv::Point(0, 0), cv::Point(stats_bg_sz.width, stats_bg_sz.height + 10), cv::Scalar(0, 0, 0), cv::FILLED);
		cv::putText(frame, stats.c_str(), cv::Point(0, stats_bg_sz.height + 5), cv::FONT_HERSHEY_COMPLEX_SMALL, 1, cv::Scalar(255, 255, 255));*/

		cv::imshow("output", frame);
		cv::waitKey(1);
		int64 t2 = getTickCount();
		myproms = (t2 - t1) * 1000 / getTickFrequency();
		std::cout << "time: " << myproms << endl;

		cnt++;
		if (cnt == frame_cnt) {
			break;
		}
	}

	double probability = num_cnt[real_num] / (double)cnt;

	std::cout << real_num << " 일치 횟수 : " << num_cnt[real_num] << ", cnt : " << cnt << std::endl;
	std::cout << real_num << " 확률 : " << probability << std::endl << std::endl;
	for (int i = 0; i < 10; i++) {
		std::cout << i << " 일치 횟수 : " << num_cnt[i] << ", 확률 : " << num_cnt[i] / (double)cnt << std::endl;
	}

	return 0;
}