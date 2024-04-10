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

#define SERVERPORT 9000		//서버의 port
#define BUFSIZE    512

#define ROBOT_NUM  2

//using namespace cv;
using namespace cv::dnn;



//int main() {
//	cv::Mat my_map(10, 10, CV_8U, cv::Scalar(0));
//	/*cv::line(my_map, cv::Point(1, 1), cv::Point(8, 1), cv::Scalar(55), 1);
//	cv::line(my_map, cv::Point(8, 1), cv::Point(8, 6), cv::Scalar(55), 1);
//	cv::line(my_map, cv::Point(1, 6), cv::Point(8, 6), cv::Scalar(55), 1);
//	cv::line(my_map, cv::Point(5, 6), cv::Point(5, 9), cv::Scalar(55), 1);*/
//
//	cv::line(my_map, cv::Point(1, 0), cv::Point(1, 3), cv::Scalar(55), 1);
//	cv::line(my_map, cv::Point(3, 0), cv::Point(3, 3), cv::Scalar(55), 1);
//	cv::line(my_map, cv::Point(5, 0), cv::Point(5, 3), cv::Scalar(55), 1);
//	cv::line(my_map, cv::Point(7, 0), cv::Point(7, 3), cv::Scalar(55), 1);
//	cv::line(my_map, cv::Point(1, 6), cv::Point(1, 8), cv::Scalar(55), 1);
//	cv::line(my_map, cv::Point(3, 6), cv::Point(3, 8), cv::Scalar(55), 1);
//	cv::line(my_map, cv::Point(5, 6), cv::Point(5, 8), cv::Scalar(55), 1);
//	cv::line(my_map, cv::Point(7, 6), cv::Point(7, 8), cv::Scalar(55), 1);
//
//	/*cv::rectangle(my_map, cv::Point(3, 3), cv::Point(4, 7), cv::Scalar(55), -1);
//	cv::rectangle(my_map, cv::Point(6, 4), cv::Point(7, 9), cv::Scalar(55), -1);*/
//
//	cv::resize(my_map, my_map, cv::Size(), 50, 50, 0);
//	
//	A_star_algorithm A_star(my_map, cv::Point(0, 0), cv::Point(499, 499));
//
//	clock_t start, end;
//	double time_result;
//	start = clock();	// 수행 시간 측정 시작
//	A_star.Navigate();
//	end = clock();		//시간 측정 끝
//	time_result = (double)(end - start);
//	time_result /= CLOCKS_PER_SEC;
//	std::cout << time_result << "초" << std::endl;
//	std::cout << "3" << std::endl;
//	start = clock();	// 수행 시간 측정 시작
//	cv::Mat n_map = A_star.Navigate_map();
//	end = clock();		//시간 측정 끝
//	time_result = (double)(end - start);
//	std::cout << time_result << "ms" << std::endl;
//	std::cout << "4" << std::endl;
//	//cv::resize(n_map, n_map, cv::Size(),50, 50, 0);
//	while (1) {
//		cv::imshow("my_map", n_map);
//		if (cv::waitKey(1) == 27)
//			break;
//	}
//}

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

HANDLE CommandEvent;
std::string input_command = "";
bool command_status = FALSE;

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

		std::string temp(buf);
		input_command += buf;

		if (input_command.find("!") != -1) {
			command_status = TRUE;

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

		std::string temp(buf);
		input_command += buf;

		if (input_command.find("!") != -1) {
			command_status = TRUE;

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

int main() {
	int retval;

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

		////--------------------------------------

		//std::string command_addr = inet_ntoa(clientaddr.sin_addr);
		//int id_num = command_addr[command_addr.length() - 1] - '0';

		//printf("주소 : %s\n", inet_ntoa(clientaddr.sin_addr));
		//printf("id : %d\n", id_num);

		////--------------------------------------

		//스레드 생성
		hThread = CreateThread(NULL, 0, RobotClient, (LPVOID)client_sock, 0, NULL);
		if (hThread == NULL)
			closesocket(client_sock);
		else
			CloseHandle(hThread);
	}

	////로봇 제어 명령을 위한 클라이언트
	//for (int i = 0; i < ROBOT_NUM; i++) {
	//	// accept()
	//	addrlen = sizeof(clientaddr);
	//	client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
	//	if (client_sock == INVALID_SOCKET) {
	//		err_display((char*)"accept()");
	//		return 1;
	//	}

	//	// 접속한 클라이언트 정보 출력
	//	printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
	//		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	//	//스레드 생성
	//	hThread = CreateThread(NULL, 0, CommandClient, (LPVOID)client_sock, 0, NULL);
	//	if (hThread == NULL)
	//		closesocket(client_sock);
	//	else
	//		CloseHandle(hThread);
	//}

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

	//클라이언트 정보 얻기
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR*)&clientaddr, &addrlen);
	

	////데이터 보내기
	//retval = send(client_sock, "hihi", 4, 0);
	//if (retval == SOCKET_ERROR) {
	//	err_display((char*)"send()");
	//	//break;
	//}

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

	/*cap.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, 720);*/
	cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, 360);

	cv::Mat frame;
	cv::Mat hsv_img;
	cv::Mat map, map_img, robot_map(360, 640, CV_8U, cv::Scalar(0));
	cv::Mat path(360, 640, CV_8S, cv::Scalar(-128));
	cv::Mat robot_path(360, 640, CV_8S, cv::Scalar(-128));
	cv::Point robot_point(-1, -1);
	cv::Point robot_head(-1, -1);
	//cv::Point direction_point(-1, -1);
	cv::Rect robot_rect;
	cv::Point goal_point(-1, -1);
	bool robot_move = false;
	bool set_direction = false;
	bool move_start = false;
	char robot_command[20] = { 0 };
	char before_robot_command[20] = { 0 };
	double direction_angle;
	double robot_head_angle;

	//로봇 제어 관련
	const int speed_value = 45;
	const int gain = 12;
	int r_speed_value = 0, l_speed_value = 0;
	int error_value = 0, past_error_value = 10;

	//cv::cuda::GpuMat g_map;
	map_img = cv::imread("map.jpg");
	//path = cv::imread("map.jpg");
	cv::cvtColor(map_img, map_img, cv::COLOR_RGB2GRAY);
	while (cap.isOpened()) {
		clock_t start, end;
		double time_result;
		start = clock();	// 수행 시간 측정 시작

		map = map_img.clone();
		//frame = cv::imread("src_img.jpg");
		cap >> frame;
		if (frame.empty()) { std::cerr << "no frame..." << std::endl; return -1; }
		//cv::resize(frame, frame, cv::Size(), 0.5, 0.5);

		
		frame = get_detected_img(network, frame, 0.8, robot_point, robot_rect);

		if (robot_point == cv::Point(-1, -1))
			std::cout << "로봇 없음" << std::endl;
		else {
			robot_head = robot_find_head(frame, robot_rect);
			if (robot_head != cv::Point(-1, -1))
				cv::circle(frame, robot_head, 4, cv::Scalar(255, 0, 0), -1);
		}

		/*cv::cvtColor(frame, hsv_img, cv::COLOR_BGR2HSV);
		cv::Scalar lower_yellow = cv::Scalar(45, 80, 50);
		cv::Scalar upper_yellow = cv::Scalar(75, 200, 200);
		inRange(hsv_img, lower_yellow, upper_yellow, map);
		cv::imwrite("map.jpg", map);*/
		//cv::Scalar lower_yellow = cv::Scalar(0);
		//cv::Scalar upper_yellow = cv::Scalar(100);
		//cv::threshold(map, map, 125, 255, cv::THRESH_BINARY);

		int px, py;
		if (command_status) {
			int pos = input_command.find(",");
			px = std::stoi(input_command.substr(0, pos));
			py = std::stoi(input_command.substr(pos + 1, (input_command.length() - (pos + 1))));
			goal_point = cv::Point(px, py);
			input_command = "";
			command_status = false;
			SetEvent(CommandEvent);
		}

		if (robot_point != cv::Point(-1, -1)) {
			if (goal_point != cv::Point(-1, -1)) {
				//clock_t start, end;
				//double time_result;
				//start = clock();	// 수행 시간 측정 시작

				//최단거리 측정
				A_star_algorithm A_star(map, robot_point, goal_point);
				A_star.Navigate();
				path = A_star.Navigate_map();
				robot_path = A_star.Navigate_robot_map();
				robot_map = A_star.get_map();
				//direction_point = A_star.get_start_next_p();
				robot_move = true;
				set_direction = true;
				cv::circle(path, goal_point, 10, cv::Scalar(100), -1);
				goal_point = cv::Point(-1, -1);
				direction_angle = getAngle(robot_point, A_star.get_start_next_p());

				//end = clock();		//시간 측정 끝
				//time_result = (double)(end - start);
				//time_result /= CLOCKS_PER_SEC;
				//std::cout << "최단거리 알고리즘 : " << time_result << "초" << std::endl;
			}
			
		}

		//로봇 명령 출력
		if (robot_move) {
			if (set_direction) {
				//double direction_angle = getAngle(robot_point, direction_point);
				robot_head_angle = getAngle(robot_point, robot_head) - direction_angle;
				if (robot_head_angle < 0)
					robot_head_angle = 360 + robot_head_angle;

				if (robot_head_angle < 10 || robot_head_angle>350) {
					set_direction = false;
					//move_start = true;
					strcpy_s(robot_command, "0,0!");
				}
				else if (robot_head_angle < 180)
					//strcpy_s(robot_command, "1054,1054!");
					strcpy_s(robot_command, "30,-30!");
				else
					//strcpy_s(robot_command, "30,30!");
					strcpy_s(robot_command, "-30,30!");
			}
			else {
				std::string robot_command_str = "";
				//로봇 머리위치로 판단
				error_value = (int)robot_path.at<char>(robot_head.y, robot_head.x);
				//로봇 위치로 판단
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

			if (path.at<uchar>(robot_point.y, robot_point.x) == 100) {
				strcpy_s(robot_command, "0,0!");
				robot_move = false;
			}
			std::cout << robot_command << std::endl;

			if (strcmp(before_robot_command, robot_command)) {
				//데이터 보내기
				retval = send(client_sock, robot_command, strlen(robot_command), 0);
				if (retval == SOCKET_ERROR) {
					err_display((char*)"send()");
					break;
				}
				std::cout << "error_value : " << error_value << std::endl;
			}
			
			strcpy_s(before_robot_command, robot_command);
		}

		cv::Mat robot_path2 = robot_path.clone();
		robot_path2 *= 14;
		cv::circle(robot_path2, robot_point, 3, cv::Scalar(127), -1);
		cv::circle(robot_path2, robot_head, 3, cv::Scalar(127), -1);
		cv::circle(robot_map, robot_point, 3, cv::Scalar(127), -1);

		//std::cout << "4" << std::endl;
		cv::imshow("map", map); 
		cv::imshow("robot_map", robot_map);
		cv::imshow("path", path);
		cv::imshow("robot_path", robot_path);
		cv::imshow("robot_path2", robot_path2);
		cv::imshow("frame", frame);
		if (cv::waitKey(1) == 27)
			break;

		end = clock();		//시간 측정 끝
		time_result = (double)(end - start);
		time_result /= CLOCKS_PER_SEC;
		//std::cout << "최단거리 알고리즘 : " << time_result << "초" << std::endl;
	}

	CloseHandle(CommandEvent);

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

//using namespace std;
//using namespace cv;
//void main()
//{
//    Mat src(500, 500, CV_8U, cv::Scalar(0));
//    //circle(src, Point(250, 250), 10, Scalar(255), -1);
//    src.at<uchar>(250, 250) = 255;
//
//
//    vector<vector<Point>> contours;
//    findContours(src, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
//
//    Mat dst = src.clone();
//    //cvtColor(src, dst, COLOR_GRAY2BGR);
//
//    for (int i = 0; i < contours.size(); i++) {
//        Scalar c(255);
//        drawContours(dst, contours, i, c, 3);
//    }
//
//    findContours(dst, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
//    for (int i = 0; i < contours.size(); i++) {
//        Scalar c(100);
//        drawContours(dst, contours, i, c, 1);
//    }
//
//    int cnt = 0;
//    for (int r = 0; r < dst.rows; r++) {
//        for (int c = 0; c < dst.cols; c++) {
//            if (dst.at<uchar>(r, c)==255) {
//                cnt++;
//            }
//        }
//    }
//    cout << cnt << endl;
//    imshow("src", src);
//    imshow("dst", dst);
//
//    waitKey(0);
//    destroyAllWindows();
//}


//#define _WINSOCK_DEPRECATED_NO_WARNINGS
//#pragma comment(lib, "ws2_32")
//#include <winsock2.h>
//#include <stdlib.h>
//#include <stdio.h>
//
//#define SERVERPORT 9000		//서버의 port
//#define BUFSIZE    512
//
//void err_quit(char* msg);
//void err_display(char* msg);
//
//int main(void)
//{
//	int retval;
//
//	// 윈속 초기화
//	WSADATA wsa;
//	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)  //버전 2.2
//	{
//		printf("Winsock init failed\n");//err_quit("WSAStartup()");
//		return 1;
//	}
//
//	// socket()
//	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
//	if (listen_sock == INVALID_SOCKET) err_quit((char*)"socket()");
//
//	// bind()
//	SOCKADDR_IN serveraddr;
//	ZeroMemory(&serveraddr, sizeof(serveraddr));
//	serveraddr.sin_family = AF_INET;
//	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);		//서버ip를 자동으로 설정
//	serveraddr.sin_port = htons(SERVERPORT);			//서버port 설정
//	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
//	if (retval == SOCKET_ERROR) err_quit((char*)"bind()");
//
//	// listen()
//	retval = listen(listen_sock, SOMAXCONN);
//	if (retval == SOCKET_ERROR) err_quit((char*)"listen()");
//
//	// 데이터 통신에 사용할 변수
//	SOCKET client_sock, client_sock2;
//	SOCKADDR_IN clientaddr, clientaddr2;
//	int addrlen;
//	char buf[BUFSIZE + 1], buf2[BUFSIZE + 1];
//
//	while (1) {
//		// accept()
//		addrlen = sizeof(clientaddr);
//		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
//		if (client_sock == INVALID_SOCKET) {
//			err_display((char*)"accept()");
//			break;
//		}
//
//		// 접속한 클라이언트 정보 출력
//		printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
//			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
//
//
//		// 클라이언트와 데이터 통신
//		while (1) {
//			// 데이터 받기
//			retval = recv(client_sock, buf, BUFSIZE, 0);
//			if (retval == SOCKET_ERROR) {
//				err_display((char*)"recv()");
//				break;
//			}
//			else if (retval == 0)
//				break;
//
//			// 받은 데이터 출력
//			buf[retval] = '\0';
//			printf("[TCP/%s:%d] %s\n", inet_ntoa(clientaddr.sin_addr),
//				ntohs(clientaddr.sin_port), buf);
//
//			// 데이터 보내기
//			retval = send(client_sock, "hihi", 4, 0);
//			if (retval == SOCKET_ERROR) {
//				err_display((char*)"send()");
//				break;
//			}
//		}
//
//		// closesocket()
//		closesocket(client_sock);
//		printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
//			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
//	}
//
//	// closesocket()
//	closesocket(listen_sock);
//
//	// 윈속 종료
//	WSACleanup();
//	return 0;
//}
//
//// 소켓 함수 오류 출력 후 종료
//void err_quit(char* msg)
//{
//	LPVOID lpMsgBuf;
//	FormatMessage(
//		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
//		NULL, WSAGetLastError(),
//		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
//		(LPTSTR)&lpMsgBuf, 0, NULL);
//	MessageBox(NULL, (LPCTSTR)lpMsgBuf, (LPCWSTR)msg, MB_ICONERROR);
//	LocalFree(lpMsgBuf);
//	exit(1);
//}
//
//// 소켓 함수 오류 출력
//void err_display(char* msg)
//{
//	LPVOID lpMsgBuf;
//	FormatMessage(
//		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
//		NULL, WSAGetLastError(),
//		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
//		(LPTSTR)&lpMsgBuf, 0, NULL);
//	printf("[%s] %s", msg, (char*)lpMsgBuf);
//	LocalFree(lpMsgBuf);
//}



//#define _WINSOCK_DEPRECATED_NO_WARNINGS
//#pragma comment(lib, "ws2_32")
//#include <winsock2.h>
//#include <stdlib.h>
//#include <stdio.h>
//#include <iostream>
//
//#define SERVERPORT 9000		//서버의 port
//#define BUFSIZE    512
//
//char ch = '0';
//
//void err_quit(char* msg);
//void err_display(char* msg);
//
//DWORD WINAPI ProcessClient(LPVOID arg) {
//	SOCKET client_sock = (SOCKET)arg;
//	int retval;
//	SOCKADDR_IN clientaddr;
//	int addrlen;
//	char buf[BUFSIZE + 1];
//
//	//클라이언트 정보 얻기
//	addrlen = sizeof(clientaddr);
//	getpeername(client_sock, (SOCKADDR*)&clientaddr, &addrlen);
//
//	while (1) {
//		// 데이터 받기
//		retval = recv(client_sock, buf, BUFSIZE, 0);
//		if (retval == SOCKET_ERROR) {
//			err_display((char*)"recv()");
//			break;
//		}
//		else if (retval == 0)
//			break;
//
//		// 받은 데이터 출력
//		buf[retval] = '\0';
//		printf("[TCP/%s:%d] %s\n", inet_ntoa(clientaddr.sin_addr),
//			ntohs(clientaddr.sin_port), buf);
//
//		//ch = buf[0];
//		
//		// 데이터 보내기
//		retval = send(client_sock, buf, retval, 0);
//		if (retval == SOCKET_ERROR) {
//			err_display((char*)"send()");
//			break;
//		}
//	}
//
//	// closesocket()
//	closesocket(client_sock);
//	printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
//		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
//
//	return 0;
//}
//
//int main(void)
//{
//	int retval;
//
//	// 윈속 초기화
//	WSADATA wsa;
//	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)  //버전 2.2
//	{
//		printf("Winsock init failed\n");//err_quit("WSAStartup()");
//		return 1;
//	}
//
//	// socket()
//	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
//	if (listen_sock == INVALID_SOCKET) err_quit((char*)"socket()");
//
//	// bind()
//	SOCKADDR_IN serveraddr;
//	ZeroMemory(&serveraddr, sizeof(serveraddr));
//	serveraddr.sin_family = AF_INET;
//	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);		//서버ip를 자동으로 설정
//	serveraddr.sin_port = htons(SERVERPORT);			//서버port 설정
//	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
//	if (retval == SOCKET_ERROR) err_quit((char*)"bind()");
//
//	// listen()
//	retval = listen(listen_sock, SOMAXCONN);
//	if (retval == SOCKET_ERROR) err_quit((char*)"listen()");
//
//	// 데이터 통신에 사용할 변수
//	SOCKET client_sock;
//	SOCKADDR_IN clientaddr;
//	int addrlen;
//	//char buf[BUFSIZE + 1];
//	HANDLE hThread;
//
//	while (1) {
//		std::cout << ch << std::endl;
//
//		// accept()
//		addrlen = sizeof(clientaddr);
//		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
//		if (client_sock == INVALID_SOCKET) {
//			err_display((char*)"accept()");
//			break;
//		}
//
//		// 접속한 클라이언트 정보 출력
//		printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
//			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
//
//		//스레드 생성
//		hThread = CreateThread(NULL, 0, ProcessClient, (LPVOID)client_sock, 0, NULL);
//		if (hThread == NULL)
//			closesocket(client_sock);
//		else
//			CloseHandle(hThread);
//	}
//
//	// closesocket()
//	closesocket(listen_sock);
//
//	// 윈속 종료
//	WSACleanup();
//	return 0;
//}
//
//// 소켓 함수 오류 출력 후 종료
//void err_quit(char* msg)
//{
//	LPVOID lpMsgBuf;
//	FormatMessage(
//		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
//		NULL, WSAGetLastError(),
//		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
//		(LPTSTR)&lpMsgBuf, 0, NULL);
//	MessageBox(NULL, (LPCTSTR)lpMsgBuf, (LPCWSTR)msg, MB_ICONERROR);
//	LocalFree(lpMsgBuf);
//	exit(1);
//}
//
//// 소켓 함수 오류 출력
//void err_display(char* msg)
//{
//	LPVOID lpMsgBuf;
//	FormatMessage(
//		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
//		NULL, WSAGetLastError(),
//		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
//		(LPTSTR)&lpMsgBuf, 0, NULL);
//	printf("[%s] %s", msg, (char*)lpMsgBuf);
//	LocalFree(lpMsgBuf);
//}



//int main() {
//
//	string model = "robot_detect_model.pb";
//	string config = "robot_detect_config.pbtxt";
//
//	// 모델 네트워크 생성
//	Net network = readNet(model, config);
//	if (network.empty()) { cerr << "Network load failed" << endl; return -1; }
//	network.setPreferableBackend(DNN_BACKEND_CUDA);	//백엔드를 CUDA로 지정함
//	network.setPreferableTarget(DNN_TARGET_CUDA);		//타깃 디바이스를 CUDA로 지정함
//
//	string label[] = { "robot" };
//
//	VideoCapture cap(1);
//	Mat frame;
//
//	cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
//	cap.set(cv::CAP_PROP_FRAME_HEIGHT, 360);
//
//	while (cap.isOpened()) {
//
//		cap >> frame;
//		if (frame.empty()) { cerr << "no frame..." << endl; return -1; }
//
//		Mat inference = get_detected_img(network, frame, 0.8);
//
//		imshow("src", inference);
//
//		if (waitKey(10) == 27) {
//			break;
//		}
//	}
//	return 0;
//}

