#include <iostream>
#include "A_star_algorithm.h"
#include <opencv2/opencv.hpp>
#include <time.h>
//#include <opencv2/cudaimgproc.hpp>

//#include <windows.h>

//int main() {
//	cv::Mat my_map(10, 10, CV_8U, cv::Scalar(0));
//	/*cv::line(my_map, cv::Point(1, 1), cv::Point(8, 1), cv::Scalar(55), 1);
//	cv::line(my_map, cv::Point(8, 1), cv::Point(8, 6), cv::Scalar(55), 1);
//	cv::line(my_map, cv::Point(1, 6), cv::Point(8, 6), cv::Scalar(55), 1);
//	cv::line(my_map, cv::Point(5, 6), cv::Point(5, 9), cv::Scalar(55), 1);*/
//
//	cv::line(my_map, cv::Point(1, 0), cv::Point(1, 3), cv::Scalar(55), 1);
//	cv::line(my_map, cv::Point(3, 0), cv::Point(3, 3), cv::Scalar(55), 1);
//	cv::line(my_map, cv::Point(5, 0), cv::Point(5, 3), cv::Scalar(55), 1);
//	cv::line(my_map, cv::Point(7, 0), cv::Point(7, 3), cv::Scalar(55), 1);
//	cv::line(my_map, cv::Point(1, 6), cv::Point(1, 8), cv::Scalar(55), 1);
//	cv::line(my_map, cv::Point(3, 6), cv::Point(3, 8), cv::Scalar(55), 1);
//	cv::line(my_map, cv::Point(5, 6), cv::Point(5, 8), cv::Scalar(55), 1);
//	cv::line(my_map, cv::Point(7, 6), cv::Point(7, 8), cv::Scalar(55), 1);
//
//	/*cv::rectangle(my_map, cv::Point(3, 3), cv::Point(4, 7), cv::Scalar(55), -1);
//	cv::rectangle(my_map, cv::Point(6, 4), cv::Point(7, 9), cv::Scalar(55), -1);*/
//
//	cv::resize(my_map, my_map, cv::Size(), 50, 50, 0);
//	
//	A_star_algorithm A_star(my_map, cv::Point(0, 0), cv::Point(499, 499));
//
//	clock_t start, end;
//	double time_result;
//	start = clock();	// 수행 시간 측정 시작
//	A_star.Navigate();
//	end = clock();		//시간 측정 끝
//	time_result = (double)(end - start);
//	time_result /= CLOCKS_PER_SEC;
//	std::cout << time_result << "초" << std::endl;
//	std::cout << "3" << std::endl;
//	start = clock();	// 수행 시간 측정 시작
//	cv::Mat n_map = A_star.Navigate_map();
//	end = clock();		//시간 측정 끝
//	time_result = (double)(end - start);
//	std::cout << time_result << "ms" << std::endl;
//	std::cout << "4" << std::endl;
//	//cv::resize(n_map, n_map, cv::Size(),50, 50, 0);
//	while (1) {
//		cv::imshow("my_map", n_map);
//		if (cv::waitKey(1) == 27)
//			break;
//	}
//}

//int main() {
//	LinkedList a;
//	//clock_t start, end; 
//	//double result;
//	//start = clock(); // 수행 시간 측정 시작
//	a.insert(a.size(), new Node(cv::Point(0, 0), 0, 0, 0));
//	a.insert(a.size(), new Node(cv::Point(1, 1), 1, 1, 1));
//	a.insert(a.size(), new Node(cv::Point(2, 2), 2, 2, 2));
//
//	std::cout << a.getEntry(a.size()-1)->get_F_distance() << std::endl;
//	//Sleep(1000);
//	//end = clock(); //시간 측정 끝
//	//result = (double)(end - start);
//	//std::cout << "result : " << result << " microseconds" << std::endl;
//	//std::cout << a.getEntry(a.size()-1)->get_F_distance() << std::endl;
//	/*for (int i = 3; i <= 5; i++) {
//		a.insert(a.size(), new Node(cv::Point(i, i), i, i, i));
//	}
//
//	for (int i = 0; i < a.size(); i++) {
//		std::cout << a.getEntry(i)->get_F_distance() << std::endl;
//	}*/
//	/*std::cout << a.getEntry(0)->get_F_distance() << std::endl;
//	std::cout << a.getEntry(1)->get_F_distance() << std::endl;
//	std::cout << a.getEntry(2)->get_F_distance() << std::endl;*/
//	//std::cout << a.remove(2)->get_F_distance() << std::endl;
//	//std::cout << a.find_pos(cv::Point(0,0)) << std::endl;
//
//}

int main() {
	cv::VideoCapture cap(1);

	/*cap.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, 720);*/
	cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, 360);

	cv::Mat frame;
	cv::Mat hsv_img;
	cv::Mat map;
	//cv::cuda::GpuMat g_map;
	while (1) {
		cap >> frame;
		//cv::resize(frame, frame, cv::Size(), 0.5, 0.5);
		cv::cvtColor(frame, hsv_img, cv::COLOR_BGR2HSV);

		cv::Scalar lower_yellow = cv::Scalar(45, 80, 50);
		cv::Scalar upper_yellow = cv::Scalar(75, 200, 200);
		inRange(hsv_img, lower_yellow, upper_yellow, map);

		clock_t start, end;
		double time_result;
		start = clock();	// 수행 시간 측정 시작

		A_star_algorithm A_star(map, cv::Point(0, 0), cv::Point(639, 300));
		A_star.Navigate();
		map = A_star.Navigate_map();

		end = clock();		//시간 측정 끝
		time_result = (double)(end - start);
		time_result /= CLOCKS_PER_SEC;
		std::cout << "최단거리 알고리즘 : " << time_result << "초" << std::endl;

		//std::cout << "4" << std::endl;
		cv::imshow("map", map);
		cv::imshow("frame", frame);
		if (cv::waitKey(1) == 27)
			break;

	}

}