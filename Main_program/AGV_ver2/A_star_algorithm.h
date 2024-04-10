#ifndef __A_STAR_ALGORITHM_H__
#define __A_STAR_ALGORITHM_H__

#include "LinkedList.h"
#include "MinHeap.h"
#include <cmath>
#include <opencv2/opencv.hpp>

class A_star_algorithm
{
	cv::Point start_point, goal_point, current_point, start_next_p;
	cv::Mat map, res_map, robot_res_map; //redmap, res_map2;
	MinHeap_Node open_list;
	LinkedList close_list;
	int size;
	int result_distance;
public:
	A_star_algorithm() {
		size = 0;
		result_distance = 0;
		res_map = cv::Mat(map.rows, map.cols, CV_8U, cv::Scalar(0));
		//res_map2 = cv::Mat(map.rows, map.cols, CV_8S, cv::Scalar(0));
		robot_res_map = cv::Mat(map.rows, map.cols, CV_8S, cv::Scalar(0));
		start_next_p = cv::Point(-1, -1);
	}
	A_star_algorithm(cv::Mat map, cv::Point start_point, cv::Point goal_point) :
		size(0), result_distance(0), map(map), start_point(start_point), goal_point(goal_point), current_point(start_point) {
		res_map = cv::Mat(map.rows, map.cols, CV_8U, cv::Scalar(0));
		//res_map2 = cv::Mat(map.rows, map.cols, CV_8S, cv::Scalar(0));
		robot_res_map = cv::Mat(map.rows, map.cols, CV_8S, cv::Scalar(0));
		//redmap = cv::Mat(map.rows, map.cols, CV_8UC3, cv::Scalar(0));
		start_next_p = cv::Point(-1, -1);
	}
	void set_initialization(cv::Mat map, cv::Point start_point, cv::Point goal_point) {
		size = 0;
		result_distance = 0;
		this->map = map;
		this->start_point = start_point;
		this->goal_point = goal_point;
		this->current_point = start_point;
		res_map = cv::Mat(map.rows, map.cols, CV_8U, cv::Scalar(0));
		//res_map2 = cv::Mat(map.rows, map.cols, CV_8S, cv::Scalar(0));
		robot_res_map = cv::Mat(map.rows, map.cols, CV_8S, cv::Scalar(0));
		start_next_p = cv::Point(-1, -1);
	}
	//void put_open_list();
	//void put_close_list();
	void Navigate();
	cv::Mat Navigate_map() { return res_map; }
	cv::Mat get_map() { return map; }
	//cv::Mat Navigate_map2() { return res_map2; }
	cv::Mat Navigate_robot_map() { return robot_res_map; }
	cv::Point get_start_next_p() { return start_next_p; }
	int get_result_distance() { return result_distance; }
};

float euclidean_distance(cv::Point a, cv::Point b);


#endif