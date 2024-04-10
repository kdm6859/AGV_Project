#ifndef __A_STAR_ALGORITHM_H__
#define __A_STAR_ALGORITHM_H__

#include "LinkedList.h"
#include "MinHeap.h"
#include <cmath>
#include <opencv2/opencv.hpp>

class A_star_algorithm
{
	cv::Point start_point, goal_point, current_point;
	cv::Mat map, res_map;
	MinHeap open_list;
	LinkedList close_list;
	int size;
public:
	A_star_algorithm(cv::Mat map, cv::Point start_point, cv::Point goal_point) :
		size(0), map(map), start_point(start_point), goal_point(goal_point), current_point(start_point) {
		res_map = cv::Mat(map.rows, map.cols, CV_8U, cv::Scalar(0));
	}
	//void put_open_list();
	//void put_close_list();
	void Navigate();
	cv::Mat Navigate_map() { return res_map; }
};

float euclidean_distance(cv::Point a, cv::Point b);

#endif