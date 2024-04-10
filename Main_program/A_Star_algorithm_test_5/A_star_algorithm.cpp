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

////Navigate_ver3
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
//				//std::cout << "&&&&&&&&&  " << x << "," << y << "  " << (int)map.at<uchar>(x, y) << std::endl;
//				if ((x < 0 || y < 0 || x >= map.cols || y >= map.rows) || (x == current_x && y == current_y))
//					continue;
//				else if (((int)map.at<uchar>(y, x) != 0)) {
//					continue;
//				}
//				/*if ((int)map.at<uchar>(y, x) == 0)
//					map.at<uchar>(y, x) = 255;*/
//				bool ox = false;
//				for (int i = size - 1; i >= 0; i--) {
//					if (close_list[i].getPoint() == cv::Point(x, y)) {
//						ox = true;
//						break;
//					}
//				}
//				if (ox) {
//					//std::cout << "겹침" << std::endl;
//					continue;
//				}
//				if (x != current_x && y != current_y)
//					distance = 1.414;
//				else
//					distance = 1;
//
//				float g_distance = current.get_G_distance() + distance;
//				float h_distance = euclidean_distance(cv::Point(x, y), goal_point);
//
//				open_list.insert(cv::Point(x, y), g_distance + h_distance, g_distance, h_distance, current.getPoint());
//			}
//		}
//		current = open_list.remove();
//		close_list[size++] = current;
//
//		if (current.getPoint() == goal_point)
//			break;
//
//		//std::cout << current.get_parent_point();
//		//std::cout << current.getPoint() << current.getKey() << std::endl;
//		//current.display();
//		//std::cout << current.get_parent_node() << std::endl;
//	}
//	std::cout << "?" << std::endl;
//	cv::Point a, b;
//	//cv::Point path_point;
//	/*path_point = close_list[--size].get_parent_point();
//	a = close_list[size].getPoint();
//	std::cout << "??" << std::endl;*/
//	a = close_list[--size].getPoint();
//	b = close_list[size].get_parent_point();
//	map.at<uchar>(a.y, a.x) = 105;
//	while (1) {
//		/*b = path_node->getPoint();
//		cv::line(map, a, b, cv::Scalar(255), 1);
//		path_node = path_node->get_parent_node();
//		std::cout << a << " " << b << std::endl;
//		if (path_node == NULL)
//			break;
//		a = b;*/
//
//		//cv::line(map, b, a, cv::Scalar(155), 1);
//		map.at<uchar>(b.y, b.x) = 155;
//		//std::cout << a << " " << b << std::endl;
//		if (b == start_point)
//			break;
//		a = b;
//		for (int i = size; i >= 0; i--) {
//			if (close_list[i].getPoint() == a) {
//				b = close_list[i].get_parent_point();
//				break;
//			}
//		}
//		
//	}
//	std::cout << "???" << std::endl;
//}

//Navigate_ver4
void A_star_algorithm::Navigate() {
	cv::Mat robot_map = map.clone();

	for (int r = 0; r < map.rows; r++) {
		for (int c = 0; c < map.cols; c++) {
			if (map.at<uchar>(r, c) != 0) {
				if (map.at<uchar>(r - 1, c - 1) == 0) {
					continue;
				}
				if (map.at<uchar>(r - 1, c) == 0) {
					continue;
				}
				if (map.at<uchar>(r - 1, c + 1) == 0) {
					continue;
				}
				if (map.at<uchar>(r, c - 1) == 0) {
					continue;
				}
				if (map.at<uchar>(r, c + 1) == 0) {
					continue;
				}
				if (map.at<uchar>(r + 1, c - 1) == 0) {
					continue;
				}
				if (map.at<uchar>(r + 1, c) == 0) {
					continue;
				}
				if (map.at<uchar>(r + 1, c + 1) == 0) {
					continue;
				}
				for (int r2 = r - 20; r2 <= r + 20; r2++) {
					for (int c2 = c - 20; c2 <= c + 20; c2++) {
						robot_map.at<uchar>(r2, c2) = 255;
					}
				}
			}
		}
	}

	//시작 노드 닫힌 노드에 삽입
	close_list.insert(close_list.size(), new Node(current_point));
	
	float distance;
	while (1) {
		//현재 노드
		Node* current = close_list.getEntry(close_list.size() - 1);
		int current_x = current->getPoint().x;
		int current_y = current->getPoint().y;

		//현재 노드의 인접 노드 탐색
		for (int x = current_x - 1; x <= current_x + 1; x++) {
			for (int y = current_y - 1; y <= current_y + 1; y++) {
				if ((x < 0 || y < 0 ))
					continue;
				else if(x == current_x && y == current_y)
					continue;
				else if(x >= robot_map.cols || y >= robot_map.rows)
					continue;
				else if (((int)robot_map.at<uchar>(y, x) != 0)) {
					continue;
				}
				else if (close_list.find(cv::Point(x,y)) != NULL) {
					//std::cout << "닫힌 노드" << std::endl;
					continue;
				}
				/*if ((int)map.at<uchar>(y, x) == 0)
					map.at<uchar>(y, x) = 255;*/
				if (x != current_x && y != current_y)
					distance = 1.414;
				else
					distance = 1;

				float g_distance = current->get_G_distance() + distance;
				float h_distance = 10 * euclidean_distance(cv::Point(x, y), goal_point);
				float f_distance = g_distance + h_distance;
				//같은 노드일 경우 f_distance 최소값인 노드로 최신화
				Node* find_node = open_list.find_node(cv::Point(x, y));
				if(find_node == NULL)
					open_list.insert(new Node(cv::Point(x, y), f_distance, g_distance, h_distance, current));
				else if (find_node->get_F_distance() > f_distance) {
					find_node->setNode(f_distance, g_distance, h_distance, current);
				}
			}
		}

		//int close_list_size = close_list.size();
		/*Node* temp = open_list.remove();
		std::cout << temp->getPoint() << std::endl;
		close_list.insert(close_list.size(), temp);*/
		close_list.insert(close_list.size(), open_list.remove());
		/*map2.at<uchar>(close_list.getEntry(close_list.size() - 1)->getPoint().y, close_list.getEntry(close_list.size() - 1)->getPoint().x) = 255;
		cv::imshow("map2", map2);
		cv::waitKey(1);*/
		/*close_list.getEntry(close_list.size() - 1)->display();
		std::cout << "------------------" << std::endl;*/
		if (close_list.getEntry(close_list.size()-1)->getPoint() == goal_point) {
			//std::cout << close_list.getEntry(close_list.size()-1)->getPoint() << std::endl;
			break;
		}
	}
	//close_list.display();
	Node* path_node = close_list.getEntry(close_list.size() - 1);
	cv::Point path_point;
	int value = 10;
	//cv::Mat map1 = res_map.clone();
	while (path_node != NULL) {
		path_point = path_node->getPoint();
		//std::cout << path_point << std::endl;
		res_map.at<uchar>(path_point.y, path_point.x) = value;
		path_node = path_node->get_parent_node();
	}
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(res_map, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
	/*cv::Scalar c(20);
	for (int i = 0; i < contours.size(); i++) {
		drawContours(res_map, contours, i, c, 5);
	}*/
	cv::Scalar c1(100);
	for (int i = 0; i < 3; i++) {
		cv::findContours(res_map, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
		for (int i = 0; i < contours.size(); i++) {
			drawContours(res_map, contours, i, c1, 2);
		}
	}
	cv::Scalar c2(150);
	for (int i = 0; i < 3; i++) {
		cv::findContours(res_map, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
		for (int i = 0; i < contours.size(); i++) {
			drawContours(res_map, contours, i, c2, 2);
		}
	}
	cv::Scalar c3(200);
	for (int i = 0; i < 1; i++) {
		cv::findContours(res_map, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
		for (int i = 0; i < contours.size(); i++) {
			drawContours(res_map, contours, i, c3, 2);
		}
	}
	
	
	
	/*cv::Mat map2 = map1.clone();
	cv::findContours(map2, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
	cv::Scalar c1(100);
	for (int i = 0; i < contours.size(); i++) {
		drawContours(map2, contours, i, c1, 2);
		drawContours(res_map, contours, i, c1, 2);
	}
	cv::Mat map3 = map2.clone();
	cv::findContours(res_map, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
	cv::Scalar c2(200);
	for (int i = 0; i < contours.size(); i++) {
		drawContours(map3, contours, i, c2, 2);
		drawContours(res_map, contours, i, c2, 2);
	}*/
	
	
	/*cv::findContours(res_map, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
	cv::Scalar c2(200);
	for (int i = 0; i < contours.size(); i++) {
		drawContours(res_map, contours, i, c1, 2);
	}*/
}

//euclidean_distance_ver1
float euclidean_distance(cv::Point a, cv::Point b) {
	int x = (a.x - b.x);
	int y = (a.y - b.y);

	return sqrt(x * x + y * y);
}

//float euclidean_distance(cv::Point a, cv::Point b) {
//	int x = (a.x - b.x);
//	if (x < 0)
//		x = -x;
//	int y = (a.y - b.y);
//	if (y < 0)
//		y = -y;
//
//	return x + y;
//}