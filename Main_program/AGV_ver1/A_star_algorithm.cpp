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

	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(map, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
	cv::Scalar robot_map_c(255);
	for (int i = 0; i < contours.size(); i++) {
		drawContours(robot_map, contours, i, robot_map_c, 54);
		drawContours(map, contours, i, robot_map_c, 54);
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
	int value = 255;
	//cv::Mat map1 = res_map.clone();
	std::cout << "??" << std::endl;
	while (path_node != NULL) {
		path_point = path_node->getPoint();
		std::cout << path_point << std::endl;
		res_map.at<uchar>(path_point.y, path_point.x) = value;
		path_node = path_node->get_parent_node();
	}
	std::cout << "!!" << std::endl;
	cv::findContours(res_map, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
	cv::Scalar c(1);
	std::cout << "@@" << std::endl;
	for (int i = 0; i < contours.size(); i++) {
		drawContours(res_map, contours, i, c, 7);
	}

	cv::Scalar c1(1);
	for (int j = 0; j < 2; j++) {
		cv::findContours(res_map, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
		for (int i = 0; i < contours.size(); i++) {
			drawContours(res_map, contours, i, c1, 2);
		}
	}
	cv::Scalar c2(2);
	for (int j = 0; j < 2; j++) {
		cv::findContours(res_map, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
		for (int i = 0; i < contours.size(); i++) {
			drawContours(res_map, contours, i, c2, 2);
		}
	}
	cv::Scalar c3(3);
	for (int j = 0; j < 2; j++) {
		cv::findContours(res_map, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
		for (int i = 0; i < contours.size(); i++) {
			drawContours(res_map, contours, i, c3, 2);
		}
	}
	cv::Scalar c4(4);
	for (int j = 0; j < 2; j++) {
		cv::findContours(res_map, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
		for (int i = 0; i < contours.size(); i++) {
			drawContours(res_map, contours, i, c4, 2);
		}
	}
	cv::Scalar c5(5);
	for (int j = 0; j < 2; j++) {
		cv::findContours(res_map, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
		for (int i = 0; i < contours.size(); i++) {
			drawContours(res_map, contours, i, c5, 2);
		}
	}
	cv::Scalar c6(6);
	for (int j = 0; j < 2; j++) {
		cv::findContours(res_map, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
		for (int i = 0; i < contours.size(); i++) {
			drawContours(res_map, contours, i, c6, 2);
		}
	}
	cv::Scalar c7(7);
	for (int j = 0; j < 2; j++) {
		cv::findContours(res_map, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
		for (int i = 0; i < contours.size(); i++) {
			drawContours(res_map, contours, i, c7, 2);
		}
	}
	cv::Scalar c8(8);
	for (int j = 0; j < 2; j++) {
		cv::findContours(res_map, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
		for (int i = 0; i < contours.size(); i++) {
			drawContours(res_map, contours, i, c8, 2);
		}
	}
	cv::Scalar c9(9);
	for (int j = 0; j < 15; j++) {
		cv::findContours(res_map, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
		for (int i = 0; i < contours.size(); i++) {
			drawContours(res_map, contours, i, c9, 2);
		}
	}
	std::cout << "@?" << std::endl;
	//cv::Point start_next_p;
	path_node = close_list.getEntry(close_list.size() - 1);
	while (path_node != NULL) {
		path_point = path_node->getPoint();
		//std::cout << path_point << std::endl;
		//res_map.at<uchar>(path_point.y, path_point.x) = 0;
		if (path_node->get_parent_node()->getPoint() == start_point) {
			std::cout << path_node->get_parent_node()->getPoint() << std::endl;
			std::cout << path_node->getPoint() << std::endl;

			start_next_p = path_node->getPoint();
			break;
		}
		path_node = path_node->get_parent_node(); 
		cv::Point n_p = path_node->getPoint();
		cv::line(res_map, path_point, n_p, cv::Scalar(0), 1, cv::LINE_4);
	}
	//res_map.at<uchar>(start_point.y, start_point.x) = 0;
	cv::line(res_map, path_point, start_point, cv::Scalar(0), 1, cv::LINE_4);

	//std::cout << "11" << std::endl;
	//std::cout << before_start_p << std::endl;

	path_node = close_list.getEntry(close_list.size() - 1);
	cv::Point before_goal_p = path_node->get_parent_node()->getPoint();

	cv::Point divide_s_p;
	cv::Point before_or_next_p = start_next_p;
	cv::Point s_g_p = start_point;
	int direction = 0;
	for (int i = 0; i < 2; i++) {
		if (before_or_next_p.x == s_g_p.x) {
			if (before_or_next_p.y < s_g_p.y) {
				direction = 7;
			}
			else {
				direction = 2;
			}
		}
		else if (before_or_next_p.y == s_g_p.y) {
			if (before_or_next_p.x < s_g_p.x) {
				direction = 5;
			}
			else {
				direction = 4;
			}
		}
		else if (before_or_next_p.x < s_g_p.x) {
			if (before_or_next_p.y < s_g_p.y) {
				direction = 8;
			}
			else {
				direction = 3;
			}
		}
		else {
			if (before_or_next_p.y < s_g_p.y) {
				direction = 6;
			}
			else {
				direction = 1;
			}
		}
		divide_s_p = s_g_p;

		//std::cout << "22" << std::endl;
		std::cout << direction << std::endl;
		int dx = divide_s_p.x, dy = divide_s_p.y;
		switch (direction)
		{
		case 1:
			while (1) {
				//res_map.at<uchar>(dy--, dx--) = 0;
				dy--;
				dx--;
				if (dx < 0 || dy < 0)
					break;
				if (res_map.at<uchar>(dy, dx) == 0)
					break;
			}
			break;
		case 2:
			while (1) {
				//res_map.at<uchar>(dy--, dx) = 0;
				dy--;
				if (dy < 0)
					break;
				if (res_map.at<uchar>(dy, dx) == 0)
					break;
			}
			break;
		case 3:
			while (1) {
				//res_map.at<uchar>(dy--, dx++) = 0;
				dy--;
				dx++;
				if (dx >= res_map.cols || dy < 0)
					break;
				if (res_map.at<uchar>(dy, dx) == 0)
					break;
			}
			break;
		case 4:
			while (1) {
				//res_map.at<uchar>(dy, dx--) = 0;
				dx--;
				if (dx < 0)
					break;
				if (res_map.at<uchar>(dy, dx) == 0)
					break;
			}
			break;
		case 5:
			while (1) {
				//res_map.at<uchar>(dy, dx++) = 0;
				dx++;
				if (dx >= res_map.cols)
					break;
				if (res_map.at<uchar>(dy, dx) == 0)
					break;
			}
			break;
		case 6:
			while (1) {
				//res_map.at<uchar>(dy++, dx--) = 0;
				dy++;
				dx--;
				if (dx < 0 || dy >= res_map.rows)
					break;
				if (res_map.at<uchar>(dy, dx) == 0)
					break;
			}
			break;
		case 7:
			while (1) {
				//res_map.at<uchar>(dy++, dx) = 0;
				dy++;
				if (dy >= res_map.rows)
					break;
				if (res_map.at<uchar>(dy, dx) == 0)
					break;
			}
			break;
		case 8:
			while (1) {
				//res_map.at<uchar>(dy++, dx++) = 0;
				dy++;
				dx++;
				if (dx >= res_map.cols || dy >= res_map.rows)
					break;
				if (res_map.at<uchar>(dy, dx) == 0)
					break;
			}
			break;
		default:
			break;
		}
		cv::line(res_map, cv::Point(dx, dy), s_g_p, cv::Scalar(0), 1, cv::LINE_4);

		before_or_next_p = before_goal_p;
		s_g_p = goal_point;
	}
	
	cv::findContours(res_map, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
	cv::Scalar cc(255);
	
	cv::Mat contour_map(res_map.size(), res_map.type());
	drawContours(contour_map, contours, -1, cc, 1, cv::LINE_4);

	if (start_next_p.x == start_point.x) {
		if (start_next_p.y < start_point.y) {
			direction = 4;
		}
		else {
			direction = 5;
		}
	}
	else if (start_next_p.y == start_point.y) {
		if (start_next_p.x < start_point.x) {
			direction = 7;
		}
		else {
			direction = 2;
		}
	}
	else if (start_next_p.x < start_point.x) {
		if (start_next_p.y < start_point.y) {
			direction = 6;
		}
		else {
			direction = 8;
		}
	}
	else {
		if (start_next_p.y < start_point.y) {
			direction = 1;
		}
		else {
			direction = 3;
		}
	}
	int dx = start_point.x, dy = start_point.y;
	std::cout << direction << std::endl;
	switch (direction)
	{
	case 1:
		while (1) {
			//res_map.at<uchar>(dy--, dx--) = 0;
			dy--;
			dx--;
			/*if (dx < 0 || dy < 0)
				break;*/
			if (contour_map.at<uchar>(dy, dx) == 255)
				break;
		}
		break;
	case 2:
		while (1) {
			//res_map.at<uchar>(dy--, dx) = 0;
			dy--;
			/*if (dy < 0)
				break;*/
			if (contour_map.at<uchar>(dy, dx) == 255)
				break;
		}
		break;
	case 3:
		while (1) {
			//res_map.at<uchar>(dy--, dx++) = 0;
			dy--;
			dx++;
			/*if (dx >= contour_map.cols || dy < 0)
				break;*/
			if (contour_map.at<uchar>(dy, dx) == 255)
				break;
		}
		break;
	case 4:
		while (1) {
			//res_map.at<uchar>(dy, dx--) = 0;
			dx--;
			/*if (dx < 0)
				break;*/
			if (contour_map.at<uchar>(dy, dx) == 255)
				break;
		}
		break;
	case 5:
		while (1) {
			//res_map.at<uchar>(dy, dx++) = 0;
			dx++;
			/*if (dx >= contour_map.cols)
				break;*/
			if (contour_map.at<uchar>(dy, dx) == 255)
				break;
		}
		break;
	case 6:
		while (1) {
			//res_map.at<uchar>(dy++, dx--) = 0;
			dy++;
			dx--;
			/*if (dx < 0 || dy >= res_map.rows)
				break;*/
			if (contour_map.at<uchar>(dy, dx) == 255)
				break;
		}
		break;
	case 7:
		while (1) {
			//res_map.at<uchar>(dy++, dx) = 0;
			dy++;
			/*if (dy >= res_map.rows)
				break;*/
			if (contour_map.at<uchar>(dy, dx) == 255)
				break;
		}
		break;
	case 8:
		while (1) {
			//res_map.at<uchar>(dy++, dx++) = 0;
			dy++;
			dx++;
			/*if (dx >= res_map.cols || dy >= res_map.rows)
				break;*/
			if (contour_map.at<uchar>(dy, dx) == 255)
				break;
		}
		break;
	default:
		break;
	}
	cv::Point d_point(dx, dy);
	std::cout << "d_point : " << d_point << std::endl;
	/*for (int i = 0; i < contours.size(); i++) {
		drawContours(res_map, contours, i, cc, 1);
	}*/
	//drawContours(res_map, contours, -1, cc, 1);
	int left_num = -1;
	int right_num = -1;
	for (int i = 0; i < contours.size(); i++) {
		for (int j = 0; j < contours[i].size(); j++) {
			if (contours[i][j] == d_point) {
				left_num = i;
				break;
			}
			//std::cout << "[" << i << "]" << "[" << j << "]" << std::endl;
		}
	}
	if (left_num == 0)
		right_num = 1;
	else
		right_num = 0;

	std::cout << left_num << std::endl;
	
	cv::Mat left_object(res_map.size(), res_map.type(), cv::Scalar(0));
	if (left_num != -1)
		drawContours(left_object, contours, left_num, cc, -1);
	else {
		std::cout << "오류" << std::endl;
		return;
	}

	cv::Mat right_object(res_map.size(), res_map.type(), cv::Scalar(0));
	if (right_num != -1)
		drawContours(right_object, contours, right_num, cc, -1);
	else {
		std::cout << "오류" << std::endl;//오류 뜨는지 안뜨는지 확인
		return;
	}

	cv::bitwise_and(res_map, res_map, left_object, left_object);
	cv::bitwise_and(res_map, res_map, right_object, right_object);

	left_object.convertTo(left_object, CV_8S);
	right_object.convertTo(right_object, CV_8S);

	//left_object = -left_object;
	right_object = -right_object;

	robot_res_map = left_object + right_object;

	while (1) {
		path_point = path_node->getPoint();
		//res_map.at<uchar>(path_point.y, path_point.x) = 0;
		path_node = path_node->get_parent_node();
		if (path_node == NULL)
			break;
		cv::Point n_p = path_node->getPoint();
		cv::line(robot_res_map, path_point, n_p, cv::Scalar(0), 2, cv::LINE_4);
	}





	//robot_res_map = right_object;

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