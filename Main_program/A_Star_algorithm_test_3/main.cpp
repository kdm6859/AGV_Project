#include <iostream>
#include "A_star_algorithm.h"
#include <opencv2/opencv.hpp>
#include <time.h>
//#include <windows.h>

int main() {
	cv::Mat my_map(10, 10, CV_8U, cv::Scalar(0));
	/*cv::line(my_map, cv::Point(1, 1), cv::Point(8, 1), cv::Scalar(55), 1);
	cv::line(my_map, cv::Point(8, 1), cv::Point(8, 6), cv::Scalar(55), 1);
	cv::line(my_map, cv::Point(1, 6), cv::Point(8, 6), cv::Scalar(55), 1);
	cv::line(my_map, cv::Point(5, 6), cv::Point(5, 9), cv::Scalar(55), 1);*/

	cv::line(my_map, cv::Point(1, 1), cv::Point(1, 3), cv::Scalar(55), 1);
	cv::line(my_map, cv::Point(3, 1), cv::Point(3, 3), cv::Scalar(55), 1);
	cv::line(my_map, cv::Point(5, 1), cv::Point(5, 3), cv::Scalar(55), 1);
	cv::line(my_map, cv::Point(7, 1), cv::Point(7, 3), cv::Scalar(55), 1);
	cv::line(my_map, cv::Point(1, 6), cv::Point(1, 8), cv::Scalar(55), 1);
	cv::line(my_map, cv::Point(3, 6), cv::Point(3, 8), cv::Scalar(55), 1);
	cv::line(my_map, cv::Point(5, 6), cv::Point(5, 8), cv::Scalar(55), 1);
	cv::line(my_map, cv::Point(7, 6), cv::Point(7, 8), cv::Scalar(55), 1);

	/*cv::rectangle(my_map, cv::Point(3, 3), cv::Point(4, 7), cv::Scalar(55), -1);
	cv::rectangle(my_map, cv::Point(6, 4), cv::Point(7, 9), cv::Scalar(55), -1);*/

	cv::resize(my_map, my_map, cv::Size(), 20,20, 0);
	/*cv::Mat my_map(100, 100, CV_8U, cv::Scalar(0));
	cv::rectangle(my_map, cv::Point(20, 20), cv::Point(30, 60), cv::Scalar(55), -1);
	cv::rectangle(my_map, cv::Point(70, 40), cv::Point(80, 80), cv::Scalar(55), -1);
	cv::rectangle(my_map, cv::Point(85, 30), cv::Point(90, 99), cv::Scalar(55), -1);*/
	//cv::rectangle(my_map, cv::Point(200, 100), cv::Point(250, 300), cv::Scalar(55), -1);
	//cv::rectangle(my_map, cv::Point(300, 100), cv::Point(400, 500), cv::Scalar(55), -1);
	/*for (int r = 0; r < my_map.rows; r++) {
		for (int c = 0; c < my_map.cols; c++) {
			std::cout << (int)my_map.at<uchar>(r, c);
			if (r == 10 && c == 90)
				my_map.at<uchar>(r, c) = 255;
		}
		std::cout << std::endl;
	}*/
	/*if ((int)my_map.at<uchar>(499, 400) != 0) {
		std::cout << "0아님" << (int)my_map.at<uchar>(499, 400);
	}
	std::cout << my_map.rows;*/
	/*while (1) {
		cv::imshow("my_map", my_map);
		if (cv::waitKey(1) == 27)
			break;
	}*/
	std::cout << "1" << std::endl;
	//A_star_algorithm A_star(my_map, cv::Point(3 * 20, 3 * 20), cv::Point(7 * 20, 8 * 20));
	A_star_algorithm A_star(my_map, cv::Point(1, 1), cv::Point(199, 190));
	//A_star_algorithm A_star(my_map, cv::Point(0, 0), cv::Point(499, 0));
	std::cout << "2" << std::endl;
	clock_t start, end;
	double time_result;
	start = clock();	// 수행 시간 측정 시작
	A_star.Navigate();
	end = clock();		//시간 측정 끝
	time_result = (double)(end - start);
	time_result /= CLOCKS_PER_SEC;
	std::cout << time_result << "초" << std::endl;
	std::cout << "3" << std::endl;
	start = clock();	// 수행 시간 측정 시작
	cv::Mat n_map = A_star.Navigate_map();
	end = clock();		//시간 측정 끝
	time_result = (double)(end - start);
	std::cout << time_result << "ms" << std::endl;
	std::cout << "4" << std::endl;
	//cv::resize(n_map, n_map, cv::Size(),50, 50, 0);
	while (1) {
		cv::imshow("my_map", n_map);
		if (cv::waitKey(1) == 27)
			break;
	}
}

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

//int main() {
//	cv::VideoCapture cap(0);
//
//	cap.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
//	cap.set(cv::CAP_PROP_FRAME_HEIGHT, 720);
//
//	cv::Mat frame;
//	
//	while (1) {
//		cap >> frame;
//		cv::resize(frame, frame, cv::Size(), 0.5, 0.5);
//		//cv::resize(frame, frame, cv::Size(), 10, 10);
//		std::cout << frame.rows << " " << frame.cols << std::endl;
//		cv::imshow("a", frame);
//
//		if (cv::waitKey(1) == 27)
//			break;
//	}
//
//}