#include <iostream>
#include "A_star_algorithm.h"
#include <opencv2/opencv.hpp>
#include <time.h>
#include <string>
#include "robot_inference.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
//#include <opencv2/cudaimgproc.hpp>	

//#include <windows.h>

#define SERVERPORT 9000		//������ port
#define BUFSIZE    512

#define ROBOT_NUM  2

//using namespace cv;
using namespace cv::dnn;

CRITICAL_SECTION cs;
HANDLE CommandEvent, FindEvent, FindEndEvent;
HANDLE robot_event[ROBOT_NUM];
cv::Mat frame;
std::string input_command = "";
bool command_status = false;
bool find_path = false;
bool move_status[ROBOT_NUM] = { false };
int thread_wake = 0;
int find_end = 0;

cv::Mat map;
cv::Point robot_point[ROBOT_NUM];
cv::Point goal_point(-1, -1);
cv::Point robot_head[ROBOT_NUM];
int result_distance[ROBOT_NUM] = { 0 };

void err_quit(char* msg);
void err_display(char* msg);

DWORD WINAPI CommandClient(LPVOID arg) {
	DWORD wait_event;

	SOCKET client_sock = (SOCKET)arg;
	int retval;
	SOCKADDR_IN clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1];

	//Ŭ���̾�Ʈ ���� ���
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR*)&clientaddr, &addrlen);

	while (1) {
		// ������ �ޱ�
		retval = recv(client_sock, buf, BUFSIZE, 0);
		if (retval == SOCKET_ERROR) {
			err_display((char*)"recv()");
			break;
		}
		else if (retval == 0)
			break;

		// ���� ������ ���
		buf[retval] = '\0';
		printf("[TCP/%s:%d] %s\n", inet_ntoa(clientaddr.sin_addr),
			ntohs(clientaddr.sin_port), buf);

		//std::string temp(buf);
		EnterCriticalSection(&cs);
		input_command += buf;
		LeaveCriticalSection(&cs);

		EnterCriticalSection(&cs);
		std::string temp_input_command = input_command;
		LeaveCriticalSection(&cs);
		if (temp_input_command.find("!") != -1) {
			EnterCriticalSection(&cs);
			command_status = true;
			LeaveCriticalSection(&cs);

			//goal_point ���� �Ϸ� ���
			wait_event = WaitForSingleObject(CommandEvent, INFINITE);
			if (wait_event != WAIT_OBJECT_0) break;
		}
	}

	// closesocket()
	closesocket(client_sock);
	printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	return 0;
}

DWORD WINAPI RobotClient(LPVOID arg) {
	DWORD wait_event;

	SOCKET client_sock = (SOCKET)arg;
	int retval;
	SOCKADDR_IN clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1];

	//Ŭ���̾�Ʈ ���� ���
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR*)&clientaddr, &addrlen);

	std::string command_addr = inet_ntoa(clientaddr.sin_addr);
	int Robot_ID = command_addr[command_addr.length() - 1] - '0';
	
	cv::Mat personal_map;
	cv::Mat path(360, 640, CV_8S, cv::Scalar(-128));
	cv::Mat robot_path(360, 640, CV_8S, cv::Scalar(-128));
	bool personal_find_path = false;
	bool personal_move_status = false;
	bool move_start = true;
	bool robot_move = false;
	bool set_direction = false;
	double direction_angle = 0;
	cv::Point personal_robot_point(-1, -1);
	cv::Point personal_goal_point(-1, -1);
	cv::Point personal_robot_head(-1, -1);
	double robot_head_angle;
	char robot_command[20] = { 0 };
	char before_robot_command[20] = { 0 };
	//�κ� ���� ����
	const int speed_value = 45;
	const int gain = 12;
	int r_speed_value = 0, l_speed_value = 0;
	int error_value = 0, past_error_value = 10;

	////������ ����(Robot_PathCalculation_Client)
	//HANDLE hThread;
	//hThread = CreateThread(NULL, 0, Robot_PathCalculation_Client, (LPVOID)Robot_ID, 0, NULL);
	//if (hThread == NULL)
	//	return 1;
	//else
	//	CloseHandle(hThread);
	cv::Mat robot_map(360, 640, CV_8U, cv::Scalar(0));
	while (1) {
		/*if (robot_point[Robot_ID] != cv::Point(-1, -1)){
			robot_head = robot_find_head(frame, robot_rect[Robot_ID]);
			if (robot_head != cv::Point(-1, -1))
				cv::circle(frame, robot_head, 4, cv::Scalar(255, 0, 0), -1);
		}*/

		/*wait_event = WaitForSingleObject(robot_event[Robot_ID], INFINITE);
		if (wait_event != WAIT_OBJECT_0) break;*/

		EnterCriticalSection(&cs);
		personal_robot_point = robot_point[Robot_ID];
		personal_robot_head = robot_head[Robot_ID];
		LeaveCriticalSection(&cs);

		//��ǥ ��ǥ�� �������� �� ��ǥ��ǥ�� ���� ����� �κ��� ã��
		EnterCriticalSection(&cs);
		personal_find_path = find_path;
		personal_move_status = move_status[Robot_ID];
		LeaveCriticalSection(&cs);
		
		//�κ� ��� ���
		if (personal_find_path && !personal_move_status) {
			EnterCriticalSection(&cs);
			personal_goal_point = goal_point;
			LeaveCriticalSection(&cs);
			std::cout << "Robot_ID : " << Robot_ID << ", personal_goal_point : " << personal_goal_point <<
				", robot_point : " << personal_robot_point << std::endl;
			//std::cout << "robot_point : " << robot_point[Robot_ID] << std::endl;
			
			EnterCriticalSection(&cs);
			personal_map = map;
			LeaveCriticalSection(&cs);
			A_star_algorithm A_star(personal_map, personal_robot_point, personal_goal_point);
			std::cout << "Robot_ID : " << Robot_ID << std::endl;
			A_star.Navigate();

			EnterCriticalSection(&cs);
			result_distance[Robot_ID] = A_star.get_result_distance();
			LeaveCriticalSection(&cs);

			path = A_star.Navigate_map();
			robot_path = A_star.Navigate_robot_map();
			direction_angle = getAngle(personal_robot_point, A_star.get_start_next_p());
			robot_map = A_star.get_map();

			EnterCriticalSection(&cs);
			find_end++;
			LeaveCriticalSection(&cs);
			printf("ID : %d\n", Robot_ID);

			//�ٸ� ������ Ž�� ���� ���
			wait_event = WaitForSingleObject(FindEndEvent, INFINITE);
			if (wait_event != WAIT_OBJECT_0) break;

			ResetEvent(FindEndEvent);

			std::cout << Robot_ID << "�� ������ ���" << std::endl;

			EnterCriticalSection(&cs);
			thread_wake++;
			LeaveCriticalSection(&cs);
			std::cout << Robot_ID<< "�� Ž�� ���� personal_move_status : " << personal_move_status  << std::endl;
		}
		
		//�κ��� �����̴� ����
		/*EnterCriticalSection(&cs);
		personal_move_status = move_status[Robot_ID];
		LeaveCriticalSection(&cs);*/
		//std::cout << Robot_ID << "�� personal_move_status : " << personal_move_status << std::endl;
		if (personal_move_status) {
			if (move_start) {
				std::cout << Robot_ID << "�� �κ� ������ ����" << std::endl;
				robot_move = true;
				set_direction = true;
				cv::circle(path, personal_goal_point, 10, cv::Scalar(100), -1);
				personal_goal_point = cv::Point(-1, -1);
				

				move_start = false;
			}
			
			//�κ� ��� ���
			if (robot_move) {
				if (set_direction) {
					//double direction_angle = getAngle(robot_point, direction_point);
					robot_head_angle = getAngle(personal_robot_point, personal_robot_head) - direction_angle;
					if (robot_head_angle < 0)
						robot_head_angle = 360 + robot_head_angle;

					if (robot_head_angle < 10 || robot_head_angle>350) {
						set_direction = false;
						//move_start = true;
						strcpy_s(robot_command, "0,0!");
					}
					else if (robot_head_angle < 180)
						//strcpy_s(robot_command, "1054,1054!");
						strcpy_s(robot_command, "20,-20!");
					else
						//strcpy_s(robot_command, "30,30!");
						strcpy_s(robot_command, "-20,20!");
				}
				else {
					std::string robot_command_str = "";
					//�κ� �Ӹ���ġ�� �Ǵ�
					error_value = (int)robot_path.at<char>(personal_robot_head.y, personal_robot_head.x);
					//�κ� ��ġ�� �Ǵ�
					//error_value = (int)robot_path.at<char>(robot_point.y, robot_point.x);

					r_speed_value = speed_value - gain * error_value;
					l_speed_value = speed_value + gain * error_value;

					/*l_speed_value = (l_speed_value < 0) ? 1024 - l_speed_value : l_speed_value;
					r_speed_value = (r_speed_value < 0) ? -r_speed_value : r_speed_value + 1024;*/

					robot_command_str += std::to_string(r_speed_value);
					robot_command_str += ",";
					robot_command_str += std::to_string(l_speed_value);
					robot_command_str += "!";

					strcpy_s(robot_command, robot_command_str.c_str());
				}

				//������ ���� Ȯ��
				if (path.at<uchar>(personal_robot_point.y, personal_robot_point.x) == 100) {
					strcpy_s(robot_command, "0,0!");
					robot_move = false;
					EnterCriticalSection(&cs);
					move_status[Robot_ID] = false;
					LeaveCriticalSection(&cs);
					move_start = true;
					std::cout << "Robot_ID : " << Robot_ID << "������ ����" << std::endl;
				}
				//std::cout << robot_command << std::endl;

				if (strcmp(before_robot_command, robot_command)) {
					//������ ������
					retval = send(client_sock, robot_command, strlen(robot_command), 0);
					if (retval == SOCKET_ERROR) {
						err_display((char*)"send()");
						break;
					}
					//std::cout << "error_value : " << error_value << std::endl;
					std::cout << "ID : " << Robot_ID << "������ ���� : " << robot_command << std::endl;
				}

				strcpy_s(before_robot_command, robot_command);
			}
		}
		
		cv::Mat robot_path2 = robot_path.clone();
		robot_path2 *= 14;
		//path *= 3;
		cv::circle(robot_path2, personal_robot_point, 3, cv::Scalar(127), -1);
		cv::circle(robot_path2, personal_robot_head, 3, cv::Scalar(127), -1);

		/*time_t curTime = time(NULL);
		std::cout << Robot_ID << "�� �ð� : " << curTime << std::endl;*/
		
		cv::imshow("robot_map_" + std::to_string(Robot_ID), robot_map);
		//cv::imshow("path_" + std::to_string(Robot_ID), path);
		//cv::imshow("robot_path_" + std::to_string(Robot_ID), robot_path);
		cv::imshow("robot_path2_" + std::to_string(Robot_ID), robot_path2);
		if (cv::waitKey(1) == 27)
			break;
	}

	// closesocket()
	closesocket(client_sock);
	printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	return 0;
}

//DWORD WINAPI Robot_PathCalculation_Client(LPVOID arg) {
//	DWORD wait_event;
//
//	int Robot_ID = (int)arg;
//
//
//	while (1) {
//		//�̺�Ʈ �߻� �� ��� Ž��
//		wait_event = WaitForSingleObject(FindEvent, INFINITE);
//		if (wait_event != WAIT_OBJECT_0) break;
//
//		//�κ� ��� ���
//		if (personal_find_path && !personal_move_status) {
//			EnterCriticalSection(&cs);
//			personal_goal_point = goal_point;
//			LeaveCriticalSection(&cs);
//			std::cout << "Robot_ID : " << Robot_ID << ", personal_goal_point : " << personal_goal_point <<
//				", robot_point : " << personal_robot_point << std::endl;
//			//std::cout << "robot_point : " << robot_point[Robot_ID] << std::endl;
//
//			EnterCriticalSection(&cs);
//			personal_map = map;
//			LeaveCriticalSection(&cs);
//			A_star_algorithm A_star(personal_map, personal_robot_point, personal_goal_point);
//			std::cout << "Robot_ID : " << Robot_ID << std::endl;
//			A_star.Navigate();
//
//			EnterCriticalSection(&cs);
//			result_distance[Robot_ID] = A_star.get_result_distance();
//			LeaveCriticalSection(&cs);
//
//			path = A_star.Navigate_map();
//			robot_path = A_star.Navigate_robot_map();
//			direction_angle = getAngle(personal_robot_point, A_star.get_start_next_p());
//
//			EnterCriticalSection(&cs);
//			find_end++;
//			LeaveCriticalSection(&cs);
//			printf("ID : %d\n", Robot_ID);
//
//			//�ٸ� ������ Ž�� ���� ���
//			wait_event = WaitForSingleObject(FindEvent, INFINITE);
//			if (wait_event != WAIT_OBJECT_0) break;
//
//			std::cout << Robot_ID << "�� ������ ���" << std::endl;
//
//			EnterCriticalSection(&cs);
//			thread_wake++;
//			LeaveCriticalSection(&cs);
//			std::cout << Robot_ID << "�� Ž�� ���� personal_move_status : " << personal_move_status << std::endl;
//		}
//	}
//
//	return 0;
//}

int main() {
	int retval;

	InitializeCriticalSection(&cs);

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)  //���� 2.2
	{
		printf("Winsock init failed\n");//err_quit("WSAStartup()");
		return 1;
	}

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit((char*)"socket()");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);		//����ip�� �ڵ����� ����
	serveraddr.sin_port = htons(SERVERPORT);			//����port ����
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit((char*)"bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit((char*)"listen()");

	// ������ ��ſ� ����� ����
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	//char buf[BUFSIZE + 1];
	HANDLE hThread;

	//CommandClient�� ���� �̺�Ʈ
	CommandEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (CommandEvent == NULL) return 1;
	//������ ����� ���� Ŭ���̾�Ʈ
	for (int i = 0; i < 1; i++) {
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display((char*)"accept()");
			break;
		}

		// ������ Ŭ���̾�Ʈ ���� ���
		printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		//������ ����
		hThread = CreateThread(NULL, 0, CommandClient, (LPVOID)client_sock, 0, NULL);
		if (hThread == NULL)
			closesocket(client_sock);
		else
			CloseHandle(hThread);
	}

	//RobotClient�� ���� �̺�Ʈ
	FindEndEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (FindEndEvent == NULL) return 1;
	////Robot_PathCalculation_Client�� ���� �̺�Ʈ
	//FindEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	//if (FindEndEvent == NULL) return 1;
	//�κ� ���� ����� ���� Ŭ���̾�Ʈ
	for (int i = 0; i < ROBOT_NUM; i++) {
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display((char*)"accept()");
			return 1;
		}

		// ������ Ŭ���̾�Ʈ ���� ���
		printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		//������ ����
		hThread = CreateThread(NULL, 0, RobotClient, (LPVOID)client_sock, 0, NULL);
		if (hThread == NULL)
			closesocket(client_sock);
		else
			CloseHandle(hThread);
	}

	//-----------------------------------------------------------------------
	
	std::string model = "robot_detect_model.pb";
	std::string config = "robot_detect_config.pbtxt";

	// �� ��Ʈ��ũ ����
	Net network = readNet(model, config);
	if (network.empty()) { std::cerr << "Network load failed" << std::endl; return -1; }
	network.setPreferableBackend(DNN_BACKEND_CUDA);	//�鿣�带 CUDA�� ������
	network.setPreferableTarget(DNN_TARGET_CUDA);		//Ÿ�� ����̽��� CUDA�� ������

	std::string label[] = { "robot" };

	cv::VideoCapture cap(1);

	/*cap.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, 720);*/
	cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, 360);

	//cv::Mat frame;
	cv::Mat hsv_img;
	cv::Mat map_img, robot_map(360, 640, CV_8U, cv::Scalar(0)); //map
	cv::Mat path(360, 640, CV_8S, cv::Scalar(-128));
	cv::Mat robot_path(360, 640, CV_8S, cv::Scalar(-128));
	//cv::Point robot_point(-1, -1);
	//EnterCriticalSection(&cs);
	for (int i = 0; i < ROBOT_NUM; i++) {	
		robot_point[i] = cv::Point(-1, -1);	//robot_point �ʱ�ȭ
	}
	for (int i = 0; i < ROBOT_NUM; i++) {
		robot_head[i] = cv::Point(-1, -1);	//robot_head �ʱ�ȭ
	}
	//LeaveCriticalSection(&cs);
	//cv::Point robot_head(-1, -1);
	//cv::Point direction_point(-1, -1);
	//cv::Rect robot_rect;
	//cv::Point goal_point(-1, -1);
	cv::Rect robot_rect[ROBOT_NUM];
	bool robot_move = false;
	bool set_direction = false;
	bool move_start = false;
	int min_robot_id = -1;
	//char robot_command[20] = { 0 };
	//char before_robot_command[20] = { 0 };
	double direction_angle;
	//double robot_head_angle;

	//�κ� ���� ����
	/*const int speed_value = 45;
	const int gain = 12;
	int r_speed_value = 0, l_speed_value = 0;
	int error_value = 0, past_error_value = 10;*/

	//cv::cuda::GpuMat g_map;
	map_img = cv::imread("map.jpg");
	//path = cv::imread("map.jpg");
	cv::cvtColor(map_img, map_img, cv::COLOR_RGB2GRAY);
	while (cap.isOpened()) {
		clock_t start, end;
		double time_result;
		start = clock();	// ���� �ð� ���� ����

		//EnterCriticalSection(&cs);
		map = map_img.clone();
		//LeaveCriticalSection(&cs);
		//frame = cv::imread("src_img.jpg");
		cap >> frame;
		if (frame.empty()) { std::cerr << "no frame..." << std::endl; return -1; }
		//cv::resize(frame, frame, cv::Size(), 0.5, 0.5);

		//�κ� �ν�
		frame = get_detected_img(network, frame, 0.8, robot_point, robot_rect, ROBOT_NUM);

		for (int i = 0; i < ROBOT_NUM; i++) {
			if (robot_point[i] != cv::Point(-1, -1)) {
				robot_head[i] = robot_find_head(frame, robot_rect[i]);
				if (robot_head[i] != cv::Point(-1, -1))
					cv::circle(frame, robot_head[i], 4, cv::Scalar(255, 0, 0), -1);
			}
		}
		

		//��ǥ��ǥ �Է¹���
		int px, py;
		//EnterCriticalSection(&cs);
		bool main_command_status = command_status;
		//LeaveCriticalSection(&cs);
		if (main_command_status) {
			//��ǥ��ǥ �ؼ�
			int pos = input_command.find(",");
			px = std::stoi(input_command.substr(0, pos));
			py = std::stoi(input_command.substr(pos + 1, (input_command.length() - (pos + 1))));
			input_command = "";
			//EnterCriticalSection(&cs);
			goal_point = cv::Point(px, py);
			command_status = false;
			//LeaveCriticalSection(&cs);
			SetEvent(CommandEvent);

			//���� �����ִ� �κ����� ��ǥ������ �Ÿ��� ���(��Ƽ �����忡�� ���)
			//EnterCriticalSection(&cs);
			find_path = true;
			//LeaveCriticalSection(&cs);
			std::cout << "find_path = true;" << std::endl;
		}

		//��Ƽ ������ �Ÿ� ��� ���Ῡ�� Ȯ��
		//EnterCriticalSection(&cs);
		bool main_find_path = find_path;
		//LeaveCriticalSection(&cs);
		if (main_find_path) {
			//EnterCriticalSection(&cs);
			int main_find_end = find_end;
			//LeaveCriticalSection(&cs);
			std::cout << "find_end : " << main_find_end << std::endl;
			int move_status_num = 0;
			for (int i = 0; i < ROBOT_NUM; i++) {
				//EnterCriticalSection(&cs);
				move_status_num += (int)move_status[i];
				//LeaveCriticalSection(&cs);
			}
			if (main_find_end + move_status_num == ROBOT_NUM) {
				//EnterCriticalSection(&cs);
				find_path = false;
				find_end = 0;
				//LeaveCriticalSection(&cs);
				std::cout << "min_robot_id Ž�� ��" << std::endl;
				SetEvent(FindEndEvent);
				while (1) {
					//EnterCriticalSection(&cs);
					if (thread_wake + move_status_num == ROBOT_NUM)  //�����尡 ���� ������� Ȯ��
						break;
					//LeaveCriticalSection(&cs);
				} 
				std::cout << "thread_wake ���" << std::endl;
				//ResetEvent(FindEndEvent);
				//EnterCriticalSection(&cs);
				thread_wake = 0;
				//LeaveCriticalSection(&cs);

				//��ǥ���� �Ÿ��� ���� ª�� �κ� Ȯ��
				//EnterCriticalSection(&cs);
				min_robot_id = heap_minimum_find_index__Exception_zero(result_distance, ROBOT_NUM);
				for (int i = 0; i < ROBOT_NUM; i++) {
					std::cout << i << "�� �Ÿ� : " << result_distance[i] << std::endl;
				}
				for (int i = 0; i < ROBOT_NUM; i++) {
					result_distance[i] = 0;		//��� �� �Ÿ� 0���� �ʱ�ȭ
				}
				//LeaveCriticalSection(&cs);
				std::cout << "min_robot_id : " << min_robot_id << std::endl;
			}
		}

		//��ǥ���� �Ÿ��� ���� ª�� �κ��� ���¸� ���������� �ٲ�
		if (min_robot_id != -1) {
			//EnterCriticalSection(&cs);
			move_status[min_robot_id] = true;
			//LeaveCriticalSection(&cs);
			std::cout << min_robot_id<<"�� �κ� ������ ���·� �ٲ�" << std::endl;
			min_robot_id = -1;
		}

		//if (robot_point != cv::Point(-1, -1)) {
		//	if (goal_point != cv::Point(-1, -1)) {
		//		//clock_t start, end;
		//		//double time_result;
		//		//start = clock();	// ���� �ð� ���� ����
		//
		//		//�ִܰŸ� ����
		//		A_star_algorithm A_star(map, robot_point, goal_point);
		//		A_star.Navigate();
		//		path = A_star.Navigate_map();
		//		robot_path = A_star.Navigate_robot_map();
		//		robot_map = A_star.get_map();
		//		//direction_point = A_star.get_start_next_p();
		//		robot_move = true;
		//		set_direction = true;
		//		cv::circle(path, goal_point, 10, cv::Scalar(100), -1);
		//		goal_point = cv::Point(-1, -1);
		//		direction_angle = getAngle(robot_point, A_star.get_start_next_p());
		//
		//		//end = clock();		//�ð� ���� ��
		//		//time_result = (double)(end - start);
		//		//time_result /= CLOCKS_PER_SEC;
		//		//std::cout << "�ִܰŸ� �˰��� : " << time_result << "��" << std::endl;
		//	}
		//	
		//}

		////�κ� ��� ���
		//if (robot_move) {
		//	if (set_direction) {
		//		//double direction_angle = getAngle(robot_point, direction_point);
		//		robot_head_angle = getAngle(robot_point, robot_head) - direction_angle;
		//		if (robot_head_angle < 0)
		//			robot_head_angle = 360 + robot_head_angle;
		//
		//		if (robot_head_angle < 10 || robot_head_angle>350) {
		//			set_direction = false;
		//			//move_start = true;
		//			strcpy_s(robot_command, "0,0!");
		//		}
		//		else if (robot_head_angle < 180)
		//			//strcpy_s(robot_command, "1054,1054!");
		//			strcpy_s(robot_command, "30,-30!");
		//		else
		//			//strcpy_s(robot_command, "30,30!");
		//			strcpy_s(robot_command, "-30,30!");
		//	}
		//	else {
		//		std::string robot_command_str = "";
		//		//�κ� �Ӹ���ġ�� �Ǵ�
		//		error_value = (int)robot_path.at<char>(robot_head.y, robot_head.x);
		//		//�κ� ��ġ�� �Ǵ�
		//		//error_value = (int)robot_path.at<char>(robot_point.y, robot_point.x);
		//
		//		r_speed_value = speed_value - gain * error_value;
		//		l_speed_value = speed_value + gain * error_value;
		//
		//		/*l_speed_value = (l_speed_value < 0) ? 1024 - l_speed_value : l_speed_value;
		//		r_speed_value = (r_speed_value < 0) ? -r_speed_value : r_speed_value + 1024;*/
		//
		//		robot_command_str += std::to_string(r_speed_value);
		//		robot_command_str += ",";
		//		robot_command_str += std::to_string(l_speed_value);
		//		robot_command_str += "!";
		//
		//		strcpy_s(robot_command, robot_command_str.c_str());
		//	}
		//
		//	if (path.at<uchar>(robot_point.y, robot_point.x) == 100) {
		//		strcpy_s(robot_command, "0,0!");
		//		robot_move = false;
		//	}
		//	std::cout << robot_command << std::endl;
		//
		//	if (strcmp(before_robot_command, robot_command)) {
		//		//������ ������
		//		retval = send(client_sock, robot_command, strlen(robot_command), 0);
		//		if (retval == SOCKET_ERROR) {
		//			err_display((char*)"send()");
		//			break;
		//		}
		//		std::cout << "error_value : " << error_value << std::endl;
		//	}
		//	
		//	strcpy_s(before_robot_command, robot_command);
		//}

		/*cv::Mat robot_path2 = robot_path.clone();
		robot_path2 *= 14;
		cv::circle(robot_path2, robot_point, 3, cv::Scalar(127), -1);
		cv::circle(robot_path2, robot_head, 3, cv::Scalar(127), -1);
		cv::circle(robot_map, robot_point, 3, cv::Scalar(127), -1);*/

		/*cv::imshow("map", map); 
		cv::imshow("robot_map", robot_map);
		cv::imshow("path", path);
		cv::imshow("robot_path", robot_path);*/
		//cv::imshow("robot_path2", robot_path2);
		cv::imshow("frame", frame);
		if (cv::waitKey(1) == 27)
			break;

		end = clock();		//�ð� ���� ��
		time_result = (double)(end - start);
		time_result /= CLOCKS_PER_SEC;
		//std::cout << "main while�� ����ð�(1����Ŭ) : " << time_result << "��" << std::endl;
	}

	CloseHandle(CommandEvent);

	DeleteCriticalSection(&cs);

	return 0;
}

// ���� �Լ� ���� ��� �� ����
void err_quit(char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, (LPCWSTR)msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// ���� �Լ� ���� ���
void err_display(char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

