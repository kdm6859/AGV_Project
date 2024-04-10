#include "A_star_algorithm.h"

//void put_open_list() {
//
//}
//
//void put_close_list() {
//
//}

////Navigate_ver1
//void A_star_algorithm::Navigate() {
//	HeapNode current(0, 0, 0, current_point);
//	close_list[size++] = current;
//	std::cout << "@" << std::endl;
//	float distance;
//	while (1) {
//		int current_x = current.getPoint().x;
//		int current_y = current.getPoint().y;
//		for (int x = current_x - 1; x <= current_x + 1; x++) {
//			for (int y = current_y - 1; y <= current_y + 1; y++) {
//				if ((x < 0 || y < 0) || (map.at<uchar>(x, y) != 0) || (x == current_x && y == current_y))
//					continue;
//				if (x != current_x && y != current_y)
//					distance = 1.414;
//				else
//					distance = 1;
//
//				float g_distance = current.get_G_distance() + distance;
//				float h_distance = euclidean_distance(cv::Point(x, y), goal_point);
//				
//				open_list.insert(cv::Point(x, y), g_distance + h_distance, g_distance, h_distance, &current);
//			}
//		}
//		current = open_list.remove();
//		close_list[size++] = current;
//
//		if (current.getPoint() == goal_point)
//			break;
//
//		std::cout << current.get_parent_node()->getPoint();
//		std::cout << current.getPoint() << current.getKey() << std::endl;
//		//current.display();
//		//std::cout << current.get_parent_node() << std::endl;
//	}
//	std::cout << "?" << std::endl;
//	cv::Point a, b;
//	HeapNode* path_node;
//	path_node = close_list[--size].get_parent_node();
//	a = close_list[size].getPoint();
//	std::cout << "??" << std::endl;
//	while (1) {
//		b = path_node->getPoint();
//		cv::line(map, a, b, cv::Scalar(255), 1);
//		path_node = path_node->get_parent_node();
//		std::cout << a << " " << b << std::endl;
//		if (path_node == NULL)
//			break;
//		a = b;
//	}
//	std::cout << "???" << std::endl;
//}

//Navigate_ver3
void A_star_algorithm::Navigate() {
	HeapNode current(0, 0, 0, current_point);
	close_list[size++] = current;
	std::cout << "@" << std::endl;
	float distance;
	while (1) {
		int current_x = current.getPoint().x;
		int current_y = current.getPoint().y;
		for (int x = current_x - 1; x <= current_x + 1; x++) {
			for (int y = current_y - 1; y <= current_y + 1; y++) {
				//std::cout << "&&&&&&&&&  " << x << "," << y << "  " << (int)map.at<uchar>(x, y) << std::endl;
				if ((x < 0 || y < 0 || x >= map.cols || y >= map.rows) || (x == current_x && y == current_y))
					continue;
				else if (((int)map.at<uchar>(y, x) != 0)) {
					continue;
				}
				/*if ((int)map.at<uchar>(y, x) == 0)
					map.at<uchar>(y, x) = 255;*/
				bool ox = false;
				for (int i = size - 1; i >= 0; i--) {
					if (close_list[i].getPoint() == cv::Point(x, y)) {
						ox = true;
						break;
					}
				}
				if (ox) {
					//std::cout << "°ãÄ§" << std::endl;
					continue;
				}
				if (x != current_x && y != current_y)
					distance = 1.414;
				else
					distance = 1;

				float g_distance = current.get_G_distance() + distance;
				float h_distance = euclidean_distance(cv::Point(x, y), goal_point);

				open_list.insert(cv::Point(x, y), g_distance + h_distance, g_distance, h_distance, current.getPoint());
			}
		}
		current = open_list.remove();
		close_list[size++] = current;

		if (current.getPoint() == goal_point)
			break;

		//std::cout << current.get_parent_point();
		//std::cout << current.getPoint() << current.getKey() << std::endl;
		//current.display();
		//std::cout << current.get_parent_node() << std::endl;
	}
	std::cout << "?" << std::endl;
	cv::Point a, b;
	//cv::Point path_point;
	/*path_point = close_list[--size].get_parent_point();
	a = close_list[size].getPoint();
	std::cout << "??" << std::endl;*/
	a = close_list[--size].getPoint();
	b = close_list[size].get_parent_point();
	map.at<uchar>(a.y, a.x) = 105;
	while (1) {
		/*b = path_node->getPoint();
		cv::line(map, a, b, cv::Scalar(255), 1);
		path_node = path_node->get_parent_node();
		std::cout << a << " " << b << std::endl;
		if (path_node == NULL)
			break;
		a = b;*/

		//cv::line(map, b, a, cv::Scalar(155), 1);
		map.at<uchar>(b.y, b.x) = 105;
		//std::cout << a << " " << b << std::endl
		//std::cout << b << std::endl;
		if (b == start_point)
			break;
		a = b;
		for (int i = size; i >= 0; i--) {
			if (close_list[i].getPoint() == a) {
				b = close_list[i].get_parent_point();
				break;
			}
		}
		
	}
	std::cout << "???" << std::endl;
}

////euclidean_distance_ver1
//float euclidean_distance(cv::Point a, cv::Point b) {
//	int x = (a.x - b.x);
//	int y = (a.y - b.y);
//
//	return sqrt(x * x + y * y);
//}

float euclidean_distance(cv::Point a, cv::Point b) {
	int x = (a.x - b.x);
	if (x < 0)
		x = -x;
	int y = (a.y - b.y);
	if (y < 0)
		y = -y;

	return x + y;
}