#include <iostream>
#include "A_star_algorithm.h"
#include <opencv2/opencv.hpp>

////test
//int main() {
//	HeapNode hn;
//	hn.display();
//
//	MinHeap mh;
//	mh.insert(5);
//	mh.insert(1);
//	mh.insert(4);
//	mh.insert(6);
//	mh.insert(2);
//	mh.insert(3);
//	mh.display();
//
//	double ed = euclidean_distance(3, 3, 0, 0);
//	std::cout << euclidean_distance(3, 3, 0, 0);
//
//
//}

//int main() {
//	cv::Mat my_map(100, 100, CV_8U, cv::Scalar(0));
//	cv::rectangle(my_map, cv::Point(20, 30), cv::Point(30, 40), cv::Scalar(255), -1);
//
//	for (int r = 0; r < my_map.rows; r++) {
//		for (int c = 0; c < my_map.cols; c++) {
//			std::cout << (int)my_map.at<uchar>(r, c);
//		}
//		std::cout << std::endl;
//	}
//
//
//	while (1) {
//		cv::imshow("my_map", my_map);
//		if (cv::waitKey(1) == 27)
//			break;
//	}
//}

int main() {
	/*cv::Mat my_map(100, 100, CV_8U, cv::Scalar(0));
	cv::rectangle(my_map, cv::Point(20, 20), cv::Point(30, 60), cv::Scalar(55), -1);
	cv::rectangle(my_map, cv::Point(70, 40), cv::Point(80, 80), cv::Scalar(55), -1);
	cv::rectangle(my_map, cv::Point(85, 30), cv::Point(90, 99), cv::Scalar(55), -1);*/
	cv::Mat my_map(10, 10, CV_8U, cv::Scalar(0));
	cv::line(my_map, cv::Point(1, 1), cv::Point(8, 1), cv::Scalar(155), 1);
	cv::line(my_map, cv::Point(8, 1), cv::Point(8, 6), cv::Scalar(155), 1);
	cv::line(my_map, cv::Point(1, 6), cv::Point(8, 6), cv::Scalar(155), 1);
	cv::line(my_map, cv::Point(5, 6), cv::Point(5, 9), cv::Scalar(155), 1);
	cv::resize(my_map, my_map, cv::Size(), 2, 2, 0);
	//cv::rectangle(my_map, cv::Point(20, 20), cv::Point(30, 60), cv::Scalar(55), -1);
	//cv::rectangle(my_map, cv::Point(70, 40), cv::Point(80, 80), cv::Scalar(55), -1);
	//cv::rectangle(my_map, cv::Point(85, 30), cv::Point(90, 99), cv::Scalar(55), -1);
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
		std::cout << "0พฦดิ" << (int)my_map.at<uchar>(499, 400);
	}
	std::cout << my_map.rows;*/
	/*while (1) {
		cv::imshow("my_map", my_map);
		if (cv::waitKey(1) == 27)
			break;
	}*/
	std::cout << "1" << std::endl;
	A_star_algorithm A_star(my_map, cv::Point(3*2, 3*2), cv::Point(7*2, 8*2));
	std::cout << "2" << std::endl;
	A_star.Navigate();
	std::cout << "3" << std::endl;
	cv::Mat n_map = A_star.Navigate_map();
	std::cout << "4" << std::endl;
	cv::resize(n_map, n_map, cv::Size(), 25, 25, 0);
	while (1) {
		cv::imshow("my_map", n_map);
		if (cv::waitKey(1) == 27)
			break;
	}
}

//int main() {
//	float a = euclidean_distance(cv::Point(2, 1), cv::Point(99, 99));
//	std::cout << a << std::endl;
//	float b = euclidean_distance(cv::Point(1, 3), cv::Point(99, 99));
//	std::cout << b << std::endl;
//}

//int main() {
//	float a = euclidean_distance(cv::Point(21, 19), cv::Point(99, 99));
//	std::cout << a << std::endl;
//
//	MinHeap mh;
//	mh.insert(cv::Point(0, 0), 5, 0, 0, NULL);
//	mh.insert(cv::Point(1, 0), 1, 0, 0, NULL);
//	mh.insert(cv::Point(2, 0), 2, 0, 0, NULL);
//	mh.insert(cv::Point(3, 0), 3, 0, 0, NULL);
//	mh.insert(cv::Point(4, 0), 3, 0, 0, NULL);
//	mh.insert(cv::Point(5, 0), 5, 0, 0, NULL);
//	mh.insert(cv::Point(6, 0), 6, 0, 0, NULL);
//	mh.insert(cv::Point(7, 0), 0.5, 0, 0, NULL);
//	mh.insert(cv::Point(8, 0), 8, 0, 0, NULL);
//
//	HeapNode hn;
//	hn = mh.remove();
//	hn.display();
//}