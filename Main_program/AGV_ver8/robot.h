#ifndef __ROBOT_H__
#define __ROBOT_H__

#include <opencv2/opencv.hpp>
#include <iostream>

class robot
{
	cv::Point robot_point;
	cv::Point robot_head;
	cv::Rect robot_rect;
	cv::Point goal_point;
public:
	robot() {

	}
};

#endif