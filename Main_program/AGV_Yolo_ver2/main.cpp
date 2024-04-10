#include <iostream>
#include "A_star_algorithm.h"
#include "stitching.h"
#include <opencv2/opencv.hpp>
#include <time.h>
#include <string>
#include "robot_inference.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <queue>
#include <iterator>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <chrono>
//#include <ctime>
//#include <opencv2/cudaimgproc.hpp>	

//#include <windows.h>

/*YOLO FILE*/
#define CLASS_FILE			"robot_num.names"
#define CFG_FILE			"yolov4-robot_num.cfg"
#define WEIGHTS_FILE		"yolov4-robot_num_final.weights"


#define SERVERPORT 9000		//서버의 port
#define BUFSIZE    512

#define ROBOT_NUM  2

//using namespace cv;
using namespace cv::dnn;

constexpr float CONFIDENCE_THRESHOLD = 0;
constexpr float NMS_THRESHOLD = 0.4;
constexpr int NUM_CLASSES = 10;

// colors for bounding boxes
const cv::Scalar colors[] = {
	{0, 255, 255},
	{255, 255, 0},
	{0, 255, 0},
	{255, 0, 0}
};
const auto NUM_COLORS = sizeof(colors) / sizeof(colors[0]);

void CallbackFunc(int event, int x, int y, int flags, void* userdata) {
	if (event == cv::EVENT_LBUTTONDOWN) {
		std::cout << "x : " << x << " y : " << y << std::endl;
	}
}

DWORD WINAPI CommandClient(LPVOID arg);
DWORD WINAPI RobotClient(LPVOID arg);
DWORD WINAPI Robot_PathCalculation_Client(LPVOID arg);

CRITICAL_SECTION cs_input_command, cs_command_status, cs_robot_point[ROBOT_NUM]
, cs_robot_head[ROBOT_NUM], cs_find_path, cs_move_status[ROBOT_NUM]
, cs_goal_point, cs_map, cs_result_distance[ROBOT_NUM], cs_find_end[ROBOT_NUM]
, cs_thread_wake, cs_clash_imminent_status[ROBOT_NUM], cs_clash_imminent_find_end[ROBOT_NUM]
, cs_clash_min_id[ROBOT_NUM], cs_not_clash_min_id[ROBOT_NUM], cs_clash_min_id_map
, cs_not_clash_min_id_map[ROBOT_NUM];
HANDLE CommandEvent, FindEvent[ROBOT_NUM], FindEndEvent;
HANDLE robot_event[ROBOT_NUM];
std::string input_command = "";
bool command_status = false;
bool find_path[ROBOT_NUM] = { false };
bool move_status[ROBOT_NUM] = { false };
bool clash_imminent_status[ROBOT_NUM] = { false };
int thread_wake = 0;
bool find_end[ROBOT_NUM] = { false };
bool clash_imminent_find_end[ROBOT_NUM] = { false };
bool clash_min_id[ROBOT_NUM] = { false };
bool not_clash_min_id[ROBOT_NUM] = { false };

cv::Mat map;
cv::Mat clash_min_id_map;
cv::Mat not_clash_min_id_map[ROBOT_NUM];
cv::Point robot_point[ROBOT_NUM];
cv::Point goal_point(-1, -1);
cv::Point robot_head[ROBOT_NUM];
int result_distance[ROBOT_NUM] = { 0 };

cv::Mat path[ROBOT_NUM];
cv::Mat robot_path[ROBOT_NUM];
cv::Mat pure_path[ROBOT_NUM];
double direction_angle[ROBOT_NUM] = { 0 };
cv::Mat robot_map[ROBOT_NUM];

void err_quit(char* msg);
void err_display(char* msg);

DWORD WINAPI CommandClient(LPVOID arg) {
	DWORD wait_event;

	SOCKET client_sock = (SOCKET)arg;
	int retval;
	SOCKADDR_IN clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1];

	//클라이언트 정보 얻기
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR*)&clientaddr, &addrlen);

	while (1) {
		// 데이터 받기
		retval = recv(client_sock, buf, BUFSIZE, 0);
		if (retval == SOCKET_ERROR) {
			err_display((char*)"recv()");
			break;
		}
		else if (retval == 0)
			break;

		// 받은 데이터 출력
		buf[retval] = '\0';
		printf("[TCP/%s:%d] %s\n", inet_ntoa(clientaddr.sin_addr),
			ntohs(clientaddr.sin_port), buf);

		//std::string temp(buf);
		EnterCriticalSection(&cs_input_command);
		input_command += buf;
		LeaveCriticalSection(&cs_input_command);

		EnterCriticalSection(&cs_input_command);
		std::string temp_input_command = input_command;
		LeaveCriticalSection(&cs_input_command);
		if (temp_input_command.find("!") != -1) {
			EnterCriticalSection(&cs_command_status);
			command_status = true;
			LeaveCriticalSection(&cs_command_status);

			//goal_point 저장 완료 대기
			wait_event = WaitForSingleObject(CommandEvent, INFINITE);
			if (wait_event != WAIT_OBJECT_0) break;
		}
	}

	// closesocket()
	closesocket(client_sock);
	printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	return 0;
}
int r_num = 0;
DWORD WINAPI RobotClient(LPVOID arg) {
	DWORD wait_event;
	
	SOCKET client_sock = (SOCKET)arg;
	int retval;
	SOCKADDR_IN clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1];

	//클라이언트 정보 얻기
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR*)&clientaddr, &addrlen);

	std::string command_addr = inet_ntoa(clientaddr.sin_addr);
	//int Robot_ID = command_addr[command_addr.length() - 1] - '0';
	int Robot_ID = r_num++;
	
	cv::Mat personal_map;
	//bool personal_find_path = false;
	bool personal_move_status = false;
	bool move_start = true;
	bool robot_move = false;
	bool set_direction = false;
	//double direction_angle = 0;
	cv::Point personal_robot_point(-1, -1);
	cv::Point personal_goal_point(-1, -1);
	cv::Point personal_robot_head(-1, -1);
	double robot_head_angle;
	char robot_command[20] = { 0 };
	char before_robot_command[20] = { 0 };
	//로봇 제어 관련
	const int speed_value = 45;
	const int gain = 12;
	int r_speed_value = 0, l_speed_value = 0;
	int error_value = 0, past_error_value = 10;

	path[Robot_ID] = cv::Mat(360, 640, CV_8S, cv::Scalar(-128));
	robot_path[Robot_ID] = cv::Mat(360, 640, CV_8S, cv::Scalar(-128));
	pure_path[Robot_ID] = cv::Mat(360, 640, CV_8U, cv::Scalar(0));

	//스레드 생성(Robot_PathCalculation_Client)
	HANDLE hThread;
	hThread = CreateThread(NULL, 0, Robot_PathCalculation_Client, (LPVOID)Robot_ID, 0, NULL);
	if (hThread == NULL)
		return 1;
	else
		CloseHandle(hThread);

	robot_map[Robot_ID] = cv::Mat(360, 640, CV_8U, cv::Scalar(0));
	while (1) {
		EnterCriticalSection(&cs_robot_point[Robot_ID]);
		personal_robot_point = robot_point[Robot_ID];
		LeaveCriticalSection(&cs_robot_point[Robot_ID]);
		EnterCriticalSection(&cs_robot_head[Robot_ID]);
		personal_robot_head = robot_head[Robot_ID];
		LeaveCriticalSection(&cs_robot_head[Robot_ID]);

		//목표 좌표가 정해졌을 때 목표좌표에 가장 가까운 로봇을 찾음
		EnterCriticalSection(&cs_move_status[Robot_ID]);
		personal_move_status = move_status[Robot_ID];
		LeaveCriticalSection(&cs_move_status[Robot_ID]);
		
		if (personal_move_status) {
			if (move_start) {
				std::cout << Robot_ID << "번 로봇 움직임 시작" << std::endl;
				robot_move = true;
				set_direction = true;
				EnterCriticalSection(&cs_goal_point);
				personal_goal_point = goal_point;
				LeaveCriticalSection(&cs_goal_point);
				cv::circle(path[Robot_ID], personal_goal_point, 13, cv::Scalar(100), -1);
				personal_goal_point = cv::Point(-1, -1);
				

				move_start = false;
			}
			
			//로봇 명령 출력
			if (robot_move) {
				if (set_direction) {
					robot_head_angle = getAngle(personal_robot_point, personal_robot_head) - direction_angle[Robot_ID];
					if (robot_head_angle < 0)
						robot_head_angle = 360 + robot_head_angle;

					if (robot_head_angle < 10 || robot_head_angle>350) {
						set_direction = false;
						//move_start = true;
						strcpy_s(robot_command, "0,0!");
					}
					else if (robot_head_angle < 180)
						strcpy_s(robot_command, "20,-20!");
					else
						strcpy_s(robot_command, "-20,20!");
				}
				else {
					std::string robot_command_str = "";
					//로봇 머리위치로 판단
					error_value = (int)robot_path[Robot_ID].at<char>(personal_robot_head.y, personal_robot_head.x);
					//로봇 위치로 판단
					//error_value = (int)robot_path.at<char>(robot_point.y, robot_point.x);

					r_speed_value = speed_value - gain * error_value;
					l_speed_value = speed_value + gain * error_value;

					robot_command_str += std::to_string(r_speed_value);
					robot_command_str += ",";
					robot_command_str += std::to_string(l_speed_value);
					robot_command_str += "!";

					strcpy_s(robot_command, robot_command_str.c_str());
				}

				//목적지 도착 확인
				if (path[Robot_ID].at<uchar>(personal_robot_point.y, personal_robot_point.x) == 100) {
					strcpy_s(robot_command, "0,0!");
					robot_move = false;
					EnterCriticalSection(&cs_move_status[Robot_ID]);
					move_status[Robot_ID] = false;
					LeaveCriticalSection(&cs_move_status[Robot_ID]);
					move_start = true;
					pure_path[Robot_ID] = cv::Mat(map.rows, map.cols, CV_8U, cv::Scalar(0));
					std::cout << "Robot_ID : " << Robot_ID << "목적지 도착" << std::endl;
				}

				/*EnterCriticalSection(&cs_clash_imminent_status[Robot_ID]);
				bool personal_clash_imminent_status = clash_imminent_status[Robot_ID];
				LeaveCriticalSection(&cs_clash_imminent_status[Robot_ID]);*/

				////로봇 충돌 시
				//if (personal_clash_imminent_status) {
				//	strcpy_s(robot_command, "0,0!");
				//	SetEvent(FindEvent[Robot_ID]);
				//}

				if (strcmp(before_robot_command, robot_command)) {
					//데이터 보내기
					retval = send(client_sock, robot_command, strlen(robot_command), 0);
					if (retval == SOCKET_ERROR) {
						err_display((char*)"send()");
						break;
					}
					std::cout << "ID : " << Robot_ID << "데이터 보냄 : " << robot_command << std::endl;
				}

				strcpy_s(before_robot_command, robot_command);
			}
		}
		
		cv::Mat robot_path2 = robot_path[Robot_ID].clone();
		robot_path2 *= 14;

		cv::circle(robot_path2, personal_robot_point, 28, cv::Scalar(60), -1);
		cv::circle(robot_path2, personal_robot_head, 3, cv::Scalar(127), -1);
		
		//cv::imshow("robot_map_" + std::to_string(Robot_ID), robot_map[Robot_ID]);
		//cv::imshow("path_" + std::to_string(Robot_ID), path[Robot_ID]);
		//cv::imshow("robot_path_" + std::to_string(Robot_ID), robot_path);
		cv::imshow("로봇 " + std::to_string(Robot_ID), robot_path2);
		if (cv::waitKey(1) == 27)
			break;
	}

	// closesocket()
	closesocket(client_sock);
	printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	return 0;
}

DWORD WINAPI Robot_PathCalculation_Client(LPVOID arg) {
	DWORD wait_event;

	int Robot_ID = (int)arg;

	bool personal_find_path = false;
	bool personal_move_status = false;

	cv::Point personal_robot_point(-1, -1);
	cv::Point personal_goal_point(-1, -1);
	cv::Point personal_robot_head(-1, -1);

	while (1) {
		//이벤트 발생 시 경로 탐색
		wait_event = WaitForSingleObject(FindEvent[Robot_ID], INFINITE);
		if (wait_event != WAIT_OBJECT_0) break;

		ResetEvent(FindEvent[Robot_ID]);

		cv::Mat personal_map;

		EnterCriticalSection(&cs_robot_point[Robot_ID]);
		personal_robot_point = robot_point[Robot_ID];
		LeaveCriticalSection(&cs_robot_point[Robot_ID]);

		//목표 좌표가 정해졌을 때 목표좌표에 가장 가까운 로봇을 찾음
		EnterCriticalSection(&cs_find_path);
		personal_find_path = find_path[Robot_ID];
		LeaveCriticalSection(&cs_find_path);
		EnterCriticalSection(&cs_move_status[Robot_ID]);
		personal_move_status = move_status[Robot_ID];
		LeaveCriticalSection(&cs_move_status[Robot_ID]);

		//로봇 경로 계산
		EnterCriticalSection(&cs_goal_point);
		personal_goal_point = goal_point;
		LeaveCriticalSection(&cs_goal_point);
		/*EnterCriticalSection(&cs_clash_imminent_status[Robot_ID]);
		bool personal_clash_imminent_status = clash_imminent_status[Robot_ID];
		LeaveCriticalSection(&cs_clash_imminent_status[Robot_ID]);
		if (!personal_clash_imminent_status) {
			EnterCriticalSection(&cs_goal_point);
			personal_goal_point = goal_point;
			LeaveCriticalSection(&cs_goal_point);
		}*/
		std::cout << "Robot_ID : " << Robot_ID << ", personal_goal_point : " << personal_goal_point <<
			", robot_point : " << personal_robot_point << std::endl;
		
		EnterCriticalSection(&cs_map);
		personal_map = map.clone();
		LeaveCriticalSection(&cs_map);

		///*EnterCriticalSection(&cs_clash_min_id[Robot_ID]);
		//bool personal_clash_min_id = clash_min_id[Robot_ID];
		//LeaveCriticalSection(&cs_clash_min_id[Robot_ID]);*/
		//EnterCriticalSection(&cs_not_clash_min_id[Robot_ID]);
		//bool personal_not_clash_min_id = not_clash_min_id[Robot_ID];
		//LeaveCriticalSection(&cs_not_clash_min_id[Robot_ID]);
		///*if (personal_clash_min_id) {
		//	EnterCriticalSection(&cs_clash_min_id_map);
		//	personal_map = clash_min_id_map.clone();
		//	LeaveCriticalSection(&cs_clash_min_id_map);
		//}*/
		//if (personal_not_clash_min_id) {
		//	EnterCriticalSection(&cs_not_clash_min_id_map[Robot_ID]);
		//	personal_map = not_clash_min_id_map[Robot_ID].clone();
		//	LeaveCriticalSection(&cs_not_clash_min_id_map[Robot_ID]);
		//}
		//else {
		//	EnterCriticalSection(&cs_map);
		//	personal_map = map.clone();
		//	LeaveCriticalSection(&cs_map);
		//}
		A_star_algorithm A_star(personal_map, personal_robot_point, personal_goal_point);
		std::cout << "Robot_ID : " << Robot_ID << std::endl;
		A_star.Navigate();

		EnterCriticalSection(&cs_result_distance[Robot_ID]);
		result_distance[Robot_ID] = A_star.get_result_distance();
		LeaveCriticalSection(&cs_result_distance[Robot_ID]);

		path[Robot_ID] = A_star.Navigate_map();
		robot_path[Robot_ID] = A_star.Navigate_robot_map();
		direction_angle[Robot_ID] = getAngle(personal_robot_point, A_star.get_start_next_p());
		robot_map[Robot_ID] = A_star.get_map();
		pure_path[Robot_ID] = A_star.get_pure_path();

		EnterCriticalSection(&cs_find_end[Robot_ID]);
		find_end[Robot_ID] = true;
		LeaveCriticalSection(&cs_find_end[Robot_ID]);

		/*if (!clash_imminent_status[Robot_ID]) {
			EnterCriticalSection(&cs_find_end[Robot_ID]);
			find_end[Robot_ID] = true;
			LeaveCriticalSection(&cs_find_end[Robot_ID]);
		}
		else {
			EnterCriticalSection(&cs_clash_imminent_find_end[Robot_ID]);
			clash_imminent_find_end[Robot_ID] = true;
			LeaveCriticalSection(&cs_clash_imminent_find_end[Robot_ID]);
		}*/
		
		printf("ID : %d\n", Robot_ID);

		std::cout << Robot_ID << "번 탐색 종료 personal_move_status : " << personal_move_status << std::endl;

		////로봇 경로 계산
		//if (personal_find_path && !personal_move_status) {
		//	EnterCriticalSection(&cs_goal_point);
		//	personal_goal_point = goal_point;
		//	LeaveCriticalSection(&cs_goal_point);
		//	std::cout << "Robot_ID : " << Robot_ID << ", personal_goal_point : " << personal_goal_point <<
		//		", robot_point : " << personal_robot_point << std::endl;
		//
		//	EnterCriticalSection(&cs_map);
		//	personal_map = map.clone();
		//	LeaveCriticalSection(&cs_map);
		//	A_star_algorithm A_star(personal_map, personal_robot_point, personal_goal_point);
		//	std::cout << "Robot_ID : " << Robot_ID << std::endl;
		//	A_star.Navigate();
		//
		//	EnterCriticalSection(&cs_result_distance[Robot_ID]);
		//	result_distance[Robot_ID] = A_star.get_result_distance();
		//	LeaveCriticalSection(&cs_result_distance[Robot_ID]);
		//
		//	path[Robot_ID] = A_star.Navigate_map();
		//	robot_path[Robot_ID] = A_star.Navigate_robot_map();
		//	direction_angle[Robot_ID] = getAngle(personal_robot_point, A_star.get_start_next_p());
		//	robot_map[Robot_ID] = A_star.get_map();
		//
		//	EnterCriticalSection(&cs_find_end[Robot_ID]);
		//	find_end[Robot_ID] = true;
		//	LeaveCriticalSection(&cs_find_end[Robot_ID]);
		//	printf("ID : %d\n", Robot_ID);
		//
		//	////다른 쓰레드 탐색 종료 대기
		//	//wait_event = WaitForSingleObject(FindEndEvent, INFINITE);
		//	//if (wait_event != WAIT_OBJECT_0) break;
		//
		//	//ResetEvent(FindEndEvent);
		//
		//	/*std::cout << Robot_ID << "번 쓰레드 깨어남" << std::endl;
		//
		//	EnterCriticalSection(&cs_thread_wake);
		//	thread_wake++;
		//	LeaveCriticalSection(&cs_thread_wake);*/
		//	std::cout << Robot_ID << "번 탐색 종료 personal_move_status : " << personal_move_status << std::endl;
		//}
	}

	return 0;
}

int main() {
	int retval;
	
	InitializeCriticalSection(&cs_input_command);
	InitializeCriticalSection(&cs_command_status);
	for (int i = 0; i < ROBOT_NUM; i++) {
		InitializeCriticalSection(&cs_robot_point[i]);
	}
	for (int i = 0; i < ROBOT_NUM; i++) {
		InitializeCriticalSection(&cs_robot_head[i]);
	}
	InitializeCriticalSection(&cs_find_path);
	for (int i = 0; i < ROBOT_NUM; i++) {
		InitializeCriticalSection(&cs_move_status[i]);
	}
	InitializeCriticalSection(&cs_goal_point);
	InitializeCriticalSection(&cs_map);
	for (int i = 0; i < ROBOT_NUM; i++) {
		InitializeCriticalSection(&cs_result_distance[i]);
	}
	for (int i = 0; i < ROBOT_NUM; i++) {
		InitializeCriticalSection(&cs_find_end[i]);
	}
	InitializeCriticalSection(&cs_thread_wake);
	for (int i = 0; i < ROBOT_NUM; i++) {
		InitializeCriticalSection(&cs_clash_imminent_status[i]);
	}
	for (int i = 0; i < ROBOT_NUM; i++) {
		InitializeCriticalSection(&cs_clash_imminent_find_end[i]);
	}
	for (int i = 0; i < ROBOT_NUM; i++) {
		InitializeCriticalSection(&cs_clash_min_id[i]);
	}
	for (int i = 0; i < ROBOT_NUM; i++) {
		InitializeCriticalSection(&cs_not_clash_min_id[i]);
	}
	InitializeCriticalSection(&cs_clash_min_id_map);
	for (int i = 0; i < ROBOT_NUM; i++) {
		InitializeCriticalSection(&cs_not_clash_min_id_map[i]);
	}

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)  //버전 2.2
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
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);		//서버ip를 자동으로 설정
	serveraddr.sin_port = htons(SERVERPORT);			//서버port 설정
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit((char*)"bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit((char*)"listen()");

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	//char buf[BUFSIZE + 1];
	HANDLE hThread;

	//CommandClient를 위한 이벤트
	CommandEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (CommandEvent == NULL) return 1;
	//목적지 명령을 위한 클라이언트
	for (int i = 0; i < 1; i++) {
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display((char*)"accept()");
			break;
		}

		// 접속한 클라이언트 정보 출력
		printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		//스레드 생성
		hThread = CreateThread(NULL, 0, CommandClient, (LPVOID)client_sock, 0, NULL);
		if (hThread == NULL)
			closesocket(client_sock);
		else
			CloseHandle(hThread);
	}

	//RobotClient를 위한 이벤트
	FindEndEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (FindEndEvent == NULL) return 1;
	////Robot_PathCalculation_Client를 위한 이벤트
	for (int i = 0; i < ROBOT_NUM; i++) {
		FindEvent[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
	}
	//if (FindEndEvent == NULL) return 1;
	//로봇 제어 명령을 위한 클라이언트
	for (int i = 0; i < ROBOT_NUM; i++) {
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display((char*)"accept()");
			return 1;
		}

		// 접속한 클라이언트 정보 출력
		printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		//스레드 생성
		hThread = CreateThread(NULL, 0, RobotClient, (LPVOID)client_sock, 0, NULL);
		if (hThread == NULL)
			closesocket(client_sock);
		else
			CloseHandle(hThread);
	}

	//-----------------------------------------------------------------------

	std::vector<std::string> class_names;
	{
		std::ifstream class_file(CLASS_FILE);
		if (!class_file)
		{
			std::cerr << "failed to open classes.txt\n";
			return 0;
		}

		std::string line;
		while (std::getline(class_file, line))
			class_names.push_back(line);
	}

	auto darknet = cv::dnn::readNetFromDarknet(CFG_FILE, WEIGHTS_FILE);
	/*auto net = cv::dnn::readNetFromDarknet("yolov4-robot.cfg", "yolov4-robot_final.weights");*/
	darknet.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
	darknet.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
	/*net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
	net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);*/
	auto output_names = darknet.getUnconnectedOutLayersNames();

	cv::Mat blob;
	std::vector<cv::Mat> detections;

	//-----------------------------------------------------------------------
	
	std::string model = "robot_detect_model.pb";
	std::string config = "robot_detect_config.pbtxt";

	// 모델 네트워크 생성
	Net network = readNet(model, config);
	if (network.empty()) { std::cerr << "Network load failed" << std::endl; return -1; }
	network.setPreferableBackend(DNN_BACKEND_CUDA);	//백엔드를 CUDA로 지정함
	network.setPreferableTarget(DNN_TARGET_CUDA);		//타깃 디바이스를 CUDA로 지정함

	std::string label[] = { "robot" };

	cv::VideoCapture cap(1);
	/*cv::VideoCapture cap0(1);
	cv::VideoCapture cap1(2);*/

	if (!(cap.isOpened())) {
		std::cerr << "Camera open failed !!!" << std::endl;
		return -1;
	}
	/*if (!(cap0.isOpened()) || !(cap1.isOpened())) {
		std::cerr << "Camera open failed !!!" << std::endl;
		return -1;
	}*/

	/*cap.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, 720);*/
	cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, 360);
	/*cap0.set(cv::CAP_PROP_FRAME_WIDTH, 640);
	cap0.set(cv::CAP_PROP_FRAME_HEIGHT, 360);
	cap1.set(cv::CAP_PROP_FRAME_WIDTH, 640);
	cap1.set(cv::CAP_PROP_FRAME_HEIGHT, 360);*/

	cv::Mat frame;
	//cv::Mat frame0, frame1;
	cv::Mat stitch_up;
	My_Stitching stitcher0;

	cv::Mat hsv_img;
	cv::Rect my_roi(0, 0, 640, 360);
	cv::Mat map_roi;

	cv::Mat robot_map(360, 640, CV_8U, cv::Scalar(0)); //map
	cv::Mat path(360, 640, CV_8S, cv::Scalar(-128));
	
	//cv::Mat robot_path(360, 640, CV_8S, cv::Scalar(-128));
	//cv::Mat Robot_Clash_Prevention_map;
	for (int i = 0; i < ROBOT_NUM; i++) {	
		robot_point[i] = cv::Point(-1, -1);	//robot_point 초기화
	}
	for (int i = 0; i < ROBOT_NUM; i++) {
		robot_head[i] = cv::Point(-1, -1);	//robot_head 초기화
	}
	cv::Rect robot_rect[ROBOT_NUM];
	bool robot_move = false;
	bool set_direction = false;
	bool move_start = false;
	bool main_find_path = false;
	int min_robot_id = -1;
	double direction_angle;

	/*cv::Mat map_img;
	map_img = cv::imread("map.jpg");
	cv::cvtColor(map_img, map_img, cv::COLOR_RGB2GRAY);
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(map_img, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
	cv::Scalar robot_map_c(255);
	cv::Mat map2 = map_img.clone();
	for (int i = 0; i < contours.size(); i++) {
		drawContours(map_img, contours, i, robot_map_c, 54);
	}
	map = map_img.clone();*/

	bool main_find_path_num[ROBOT_NUM] = { false };
	int main_find_path_num_cnt = 0;
	//bool main_clash_imminent_status = false;

	std::cout << "start" << std::endl;
	while (1) {
		clock_t start, end;
		double time_result;
		start = clock();	// 수행 시간 측정 시작

		cap >> frame;
		/*cap0 >> frame0;
		if (frame0.empty()) { std::cerr << "frame0 not load ..." << std::endl; break; }
		cap1 >> frame1;
		if (frame1.empty()) { std::cerr << "frame1 not load ..." << std::endl; break; }*/


		//로봇 인식
		stitch_up = get_detected_img(network, frame, 0.8, robot_point, robot_rect, ROBOT_NUM);

		for (int i = 0; i < ROBOT_NUM; i++) {
			if (robot_point[i] != cv::Point(-1, -1)) {
				robot_head[i] = robot_find_head(stitch_up, robot_rect[i]);
				if (robot_head[i] != cv::Point(-1, -1))
					cv::circle(stitch_up, robot_head[i], 4, cv::Scalar(255, 0, 0), -1);
			}
		}

		//-----------------------------------------------------------------------

		/*auto total_start = std::chrono::steady_clock::now();*/
		cv::dnn::blobFromImage(frame, blob, 0.00392, cv::Size(416, 416), cv::Scalar(), true, false, CV_32F);
		darknet.setInput(blob);

		/*auto dnn_start = std::chrono::steady_clock::now();*/
		darknet.forward(detections, output_names);
		/*auto dnn_end = std::chrono::steady_clock::now();*/
		std::vector<int> indices[NUM_CLASSES];
		std::vector<cv::Rect> boxes[NUM_CLASSES];
		std::vector<float> scores[NUM_CLASSES];
		std::cout << "1" << std::endl;
		for (auto& output : detections)
		{
			const auto num_boxes = output.rows;
			for (int i = 0; i < num_boxes; i++)
			{
				auto x = output.at<float>(i, 0) * frame.cols;
				auto y = output.at<float>(i, 1) * frame.rows;
				auto width = output.at<float>(i, 2) * frame.cols;
				auto height = output.at<float>(i, 3) * frame.rows;
				cv::Rect rect(x - width / 2, y - height / 2, width, height);

				for (int c = 0; c < NUM_CLASSES; c++)
				{
					auto confidence = *output.ptr<float>(i, 5 + c);
					if (confidence >= CONFIDENCE_THRESHOLD)
					{
						boxes[c].push_back(rect);
						scores[c].push_back(confidence);
					}
				}
			}
		}

		for (int c = 0; c < NUM_CLASSES; c++)
			cv::dnn::NMSBoxes(boxes[c], scores[c], 0.7, NMS_THRESHOLD, indices[c]);
		for (int c = 0; c < NUM_CLASSES; c++)
		{
			for (size_t i = 0; i < indices[c].size(); ++i)
			{
				const auto color = colors[c % NUM_COLORS];

				auto idx = indices[c][i];
				const auto& rect = boxes[c][idx];
				cv::rectangle(stitch_up, cv::Point(rect.x, rect.y), cv::Point(rect.x + rect.width, rect.y + rect.height), color, 3);

				std::ostringstream label_ss;
				label_ss << class_names[c] << ": " << std::fixed << std::setprecision(2) << scores[c][idx];
				auto label = label_ss.str();

				int baseline;
				auto label_bg_sz = cv::getTextSize(label.c_str(), cv::FONT_HERSHEY_COMPLEX_SMALL, 1, 1, &baseline);
				cv::rectangle(stitch_up, cv::Point(rect.x, rect.y - label_bg_sz.height - baseline - 5), cv::Point(rect.x + label_bg_sz.width - 20, rect.y), color, cv::FILLED);
				cv::putText(stitch_up, label.c_str(), cv::Point(rect.x, rect.y - baseline - 5), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.7, cv::Scalar(0, 0, 0));
			}
		}

		//-----------------------------------------------------------------------

		cv::cvtColor(stitch_up, hsv_img, cv::COLOR_BGR2HSV);
		cv::Scalar lower_map = cv::Scalar(0, 0, 20);
		cv::Scalar upper_map = cv::Scalar(180, 100, 120);
		cv::inRange(hsv_img, lower_map, upper_map, map);
		map = ~map;
		for (int i = 0; i < ROBOT_NUM; i++) {
			//map(robot_rect[i]) = 0;
			cv::rectangle(map, robot_rect[i], cv::Scalar(0), -1);
		}
		std::vector<std::vector<cv::Point>> contours;
		std::vector<cv::Vec4i> hierarchy;
		cv::findContours(map, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
		//cv::Scalar robot_map_c(0);
		int min_area = 600;
		for (int i = 0; i < contours.size(); i++) {
			int area = contourArea(contours[i]);

			if (area < min_area) {
				drawContours(map, contours, i, 0, -1);
			}
		}
		cv::Mat map2 = map.clone();
		cv::findContours(map, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
		//cv::Scalar robot_map_c(255);
		for (int i = 0; i < contours.size(); i++) {
			drawContours(map, contours, i, 255, 70);
		}


		//목표좌표 입력받음
		int px, py;
		bool main_command_status = command_status;
		if (main_command_status) {
			//목표좌표 해석
			int pos = input_command.find(",");
			px = std::stoi(input_command.substr(0, pos));
			py = std::stoi(input_command.substr(pos + 1, (input_command.length() - (pos + 1))));
			input_command = "";
			goal_point = cv::Point(px, py);
			command_status = false;
			SetEvent(CommandEvent);

			//현재 쉬고있는 로봇들의 목표까지의 거리를 계산(멀티 쓰레드에서 계산)
			for (int i = 0; i < ROBOT_NUM; i++) {
				if (!move_status[i]) {
					SetEvent(FindEvent[i]);
					main_find_path_num[i] = true;
					main_find_path_num_cnt++;
				}
			}
			main_find_path = true;
			/*for (int i = 0; i < ROBOT_NUM; i++) {
				find_path[i] = true;
			}*/
			
			std::cout << "find_path = true;" << std::endl;
		}

		//멀티 쓰레드 거리 계산 종료여부 확인
		//bool main_find_path = find_path;
		if (main_find_path) {
			int find_path_end_cnt = 0;
			for (int i = 0; i < ROBOT_NUM; i++) {
				if (main_find_path_num[i]) {
					if (find_end[i]) {
						find_path_end_cnt++;
					}
				}
			}
			if (find_path_end_cnt == main_find_path_num_cnt) {
				std::cout << "min_robot_id 탐색 끝" << std::endl;
				//목표까지 거리가 가장 짧은 로봇 확인
				int* main_result_distance = new int[main_find_path_num_cnt];
				int* main_find_path_robot_id = new int[main_find_path_num_cnt];
				int index = 0;
				for (int i = 0; i < ROBOT_NUM; i++) {
					if (main_find_path_num[i]) {
						main_result_distance[index] = result_distance[i];
						main_find_path_robot_id[index++] = i;
					}
				}

				min_robot_id = heap_minimum_find_index__Exception_zero(main_result_distance, main_find_path_num_cnt);
				for (int i = 0; i < main_find_path_num_cnt; i++) {
					if (min_robot_id == i) {
						min_robot_id = main_find_path_robot_id[i];
						break;
					}
				}

				for (int i = 0; i < main_find_path_num_cnt; i++) {
					std::cout << main_find_path_robot_id[i] << "번 거리 : " 
						<< main_result_distance[i] << std::endl;
				}
				for (int i = 0; i < main_find_path_num_cnt; i++) {
					result_distance[main_find_path_robot_id[i]] = 0;		//계산 후 거리 0으로 초기화
				}
				std::cout << "min_robot_id : " << min_robot_id << std::endl;

				for (int i = 0; i < main_find_path_num_cnt; i++) {
					find_end[main_find_path_robot_id[i]] = false;
				}
				for (int i = 0; i < main_find_path_num_cnt; i++) {
					main_find_path_num[main_find_path_robot_id[i]] = false;
				}
				main_find_path_num_cnt = 0;
				main_find_path = false;
			}
		}

		//목표까지 거리가 가장 짧은 로봇의 상태를 움직임으로 바꿈
		if (min_robot_id != -1) {
			move_status[min_robot_id] = true;
			std::cout << min_robot_id<<"번 로봇 움직임 상태로 바꿈" << std::endl;
			min_robot_id = -1;
		}
		
		cv::Mat status_frame(360, 640, CV_8U, cv::Scalar(0));
		status_frame += map2;
		for (int i = 0; i < ROBOT_NUM; i++) {
			if (move_status[i])
				status_frame += pure_path[i].clone();
			cv::circle(status_frame, robot_point[i], 28, cv::Scalar(150), -1);
			cv::circle(status_frame, robot_head[i], 3, cv::Scalar(200), -1);
		}
		
		//cv::imshow("map", map); 
		//cv::imshow("robot_map", robot_map);
		//cv::imshow("path", path);
		//cv::imshow("robot_path", robot_path);
		//cv::imshow("robot_path2", robot_path2);
		//cv::imshow("Robot_Clash_Prevention_map", Robot_Clash_Prevention_map);
		//cv::imshow("frame2", frame2);
		cv::imshow("frame", frame);
		cv::imshow("stitch_up", stitch_up);
		cv::imshow("status_frame", status_frame);
		cv::imshow("map", map);

		cv::setMouseCallback("map", CallbackFunc, NULL);

		int key = cv::waitKey(1);
		if (key == 'q') break;
		else if (key == 's') {
			stitcher0.homography = cv::Mat();
		}
		/*if (cv::waitKey(1) == 27)
			break;*/

		end = clock();		//시간 측정 끝
		time_result = (double)(end - start);
		time_result /= CLOCKS_PER_SEC;
		//std::cout << "main while문 실행시간(1사이클) : " << time_result << "초" << std::endl;
	}

	CloseHandle(CommandEvent);

	DeleteCriticalSection(&cs_input_command);
	DeleteCriticalSection(&cs_command_status);
	for (int i = 0; i < ROBOT_NUM; i++) {
		DeleteCriticalSection(&cs_robot_point[i]);
	}
	for (int i = 0; i < ROBOT_NUM; i++) {
		DeleteCriticalSection(&cs_robot_head[i]);
	}
	DeleteCriticalSection(&cs_find_path);
	for (int i = 0; i < ROBOT_NUM; i++) {
		DeleteCriticalSection(&cs_move_status[i]);
	}
	DeleteCriticalSection(&cs_goal_point);
	DeleteCriticalSection(&cs_map);
	for (int i = 0; i < ROBOT_NUM; i++) {
		DeleteCriticalSection(&cs_result_distance[i]);
	}
	for (int i = 0; i < ROBOT_NUM; i++) {
		DeleteCriticalSection(&cs_find_end[i]);
	}
	DeleteCriticalSection(&cs_thread_wake);
	for (int i = 0; i < ROBOT_NUM; i++) {
		DeleteCriticalSection(&cs_clash_imminent_status[i]);
	}
	for (int i = 0; i < ROBOT_NUM; i++) {
		DeleteCriticalSection(&cs_clash_imminent_find_end[i]);
	}
	for (int i = 0; i < ROBOT_NUM; i++) {
		DeleteCriticalSection(&cs_clash_min_id[i]);
	}
	for (int i = 0; i < ROBOT_NUM; i++) {
		DeleteCriticalSection(&cs_not_clash_min_id[i]);
	}
	DeleteCriticalSection(&cs_clash_min_id_map);
	for (int i = 0; i < ROBOT_NUM; i++) {
		DeleteCriticalSection(&cs_not_clash_min_id_map[i]);
	}
	
	return 0;
}

// 소켓 함수 오류 출력 후 종료
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

// 소켓 함수 오류 출력
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

