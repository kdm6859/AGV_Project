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
//	start = clock();	// ���� �ð� ���� ����
//	A_star.Navigate();
//	end = clock();		//�ð� ���� ��
//	time_result = (double)(end - start);
//	time_result /= CLOCKS_PER_SEC;
//	std::cout << time_result << "��" << std::endl;
//	std::cout << "3" << std::endl;
//	start = clock();	// ���� �ð� ���� ����
//	cv::Mat n_map = A_star.Navigate_map();
//	end = clock();		//�ð� ���� ��
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
//	//start = clock(); // ���� �ð� ���� ����
//	a.insert(a.size(), new Node(cv::Point(0, 0), 0, 0, 0));
//	a.insert(a.size(), new Node(cv::Point(1, 1), 1, 1, 1));
//	a.insert(a.size(), new Node(cv::Point(2, 2), 2, 2, 2));
//
//	std::cout << a.getEntry(a.size()-1)->get_F_distance() << std::endl;
//	//Sleep(1000);
//	//end = clock(); //�ð� ���� ��
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

		std::string temp(buf);
		input_command += buf;

		if (input_command.find("!") != -1) {
			command_status = TRUE;

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

		std::string temp(buf);
		input_command += buf;

		if (input_command.find("!") != -1) {
			command_status = TRUE;

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

int main() {
	int retval;

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

	CommandEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	if (CommandEvent == NULL) return 1;

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

	////������ ������
	//retval = send(client_sock, "hihi", 4, 0);
	//if (retval == SOCKET_ERROR) {
	//	err_display((char*)"send()");
	//	//break;
	//}

	//-----------------------------------------------------------------------
	
	/*std::string model = "robot_detect_model.pb";
	std::string config = "robot_detect_config.pbtxt";*/
	std::string model = "yolov4-robot_4000.weights";
	std::string config = "yolov4-robot.cfg";

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

	cv::Mat frame;
	cv::Mat hsv_img;
	cv::Mat map, map_img;
	cv::Mat path;
	cv::Point robot_point(-1, -1);
	cv::Point goal_point(-1, -1);
	//cv::cuda::GpuMat g_map;
	map_img = cv::imread("map.jpg");
	path = cv::imread("map.jpg");
	cv::cvtColor(map_img, map_img, cv::COLOR_RGB2GRAY);
	while (cap.isOpened()) {
		map = map_img.clone();
		cap >> frame;
		if (frame.empty()) { std::cerr << "no frame..." << std::endl; return -1; }
		//cv::resize(frame, frame, cv::Size(), 0.5, 0.5);

		
		frame = get_detected_img(network, frame, 0.8, robot_point);//?
		
		if (robot_point == cv::Point(-1, -1))
			std::cout << "�κ� ����" << std::endl;
		else
			std::cout << robot_point << std::endl;

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
			command_status = FALSE;
			SetEvent(CommandEvent);
		}

		if (robot_point != cv::Point(-1, -1)) {
			if (goal_point != cv::Point(-1, -1)) {
				clock_t start, end;
				double time_result;
				start = clock();	// ���� �ð� ���� ����

				A_star_algorithm A_star(map, robot_point, goal_point);
				A_star.Navigate();
				path = A_star.Navigate_map();

				end = clock();		//�ð� ���� ��
				time_result = (double)(end - start);
				time_result /= CLOCKS_PER_SEC;
				std::cout << "�ִܰŸ� �˰��� : " << time_result << "��" << std::endl;
			}
		}

		//std::cout << "4" << std::endl;
		cv::imshow("map", map);
		cv::imshow("path", path);
		cv::imshow("frame", frame);
		if (cv::waitKey(1) == 27)
			break;
	}

	CloseHandle(CommandEvent);

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
//#define SERVERPORT 9000		//������ port
//#define BUFSIZE    512
//
//void err_quit(char* msg);
//void err_display(char* msg);
//
//int main(void)
//{
//	int retval;
//
//	// ���� �ʱ�ȭ
//	WSADATA wsa;
//	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)  //���� 2.2
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
//	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);		//����ip�� �ڵ����� ����
//	serveraddr.sin_port = htons(SERVERPORT);			//����port ����
//	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
//	if (retval == SOCKET_ERROR) err_quit((char*)"bind()");
//
//	// listen()
//	retval = listen(listen_sock, SOMAXCONN);
//	if (retval == SOCKET_ERROR) err_quit((char*)"listen()");
//
//	// ������ ��ſ� ����� ����
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
//		// ������ Ŭ���̾�Ʈ ���� ���
//		printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
//			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
//
//
//		// Ŭ���̾�Ʈ�� ������ ���
//		while (1) {
//			// ������ �ޱ�
//			retval = recv(client_sock, buf, BUFSIZE, 0);
//			if (retval == SOCKET_ERROR) {
//				err_display((char*)"recv()");
//				break;
//			}
//			else if (retval == 0)
//				break;
//
//			// ���� ������ ���
//			buf[retval] = '\0';
//			printf("[TCP/%s:%d] %s\n", inet_ntoa(clientaddr.sin_addr),
//				ntohs(clientaddr.sin_port), buf);
//
//			// ������ ������
//			retval = send(client_sock, "hihi", 4, 0);
//			if (retval == SOCKET_ERROR) {
//				err_display((char*)"send()");
//				break;
//			}
//		}
//
//		// closesocket()
//		closesocket(client_sock);
//		printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
//			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
//	}
//
//	// closesocket()
//	closesocket(listen_sock);
//
//	// ���� ����
//	WSACleanup();
//	return 0;
//}
//
//// ���� �Լ� ���� ��� �� ����
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
//// ���� �Լ� ���� ���
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
//#define SERVERPORT 9000		//������ port
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
//	//Ŭ���̾�Ʈ ���� ���
//	addrlen = sizeof(clientaddr);
//	getpeername(client_sock, (SOCKADDR*)&clientaddr, &addrlen);
//
//	while (1) {
//		// ������ �ޱ�
//		retval = recv(client_sock, buf, BUFSIZE, 0);
//		if (retval == SOCKET_ERROR) {
//			err_display((char*)"recv()");
//			break;
//		}
//		else if (retval == 0)
//			break;
//
//		// ���� ������ ���
//		buf[retval] = '\0';
//		printf("[TCP/%s:%d] %s\n", inet_ntoa(clientaddr.sin_addr),
//			ntohs(clientaddr.sin_port), buf);
//
//		//ch = buf[0];
//		
//		// ������ ������
//		retval = send(client_sock, buf, retval, 0);
//		if (retval == SOCKET_ERROR) {
//			err_display((char*)"send()");
//			break;
//		}
//	}
//
//	// closesocket()
//	closesocket(client_sock);
//	printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
//		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
//
//	return 0;
//}
//
//int main(void)
//{
//	int retval;
//
//	// ���� �ʱ�ȭ
//	WSADATA wsa;
//	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)  //���� 2.2
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
//	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);		//����ip�� �ڵ����� ����
//	serveraddr.sin_port = htons(SERVERPORT);			//����port ����
//	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
//	if (retval == SOCKET_ERROR) err_quit((char*)"bind()");
//
//	// listen()
//	retval = listen(listen_sock, SOMAXCONN);
//	if (retval == SOCKET_ERROR) err_quit((char*)"listen()");
//
//	// ������ ��ſ� ����� ����
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
//		// ������ Ŭ���̾�Ʈ ���� ���
//		printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
//			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
//
//		//������ ����
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
//	// ���� ����
//	WSACleanup();
//	return 0;
//}
//
//// ���� �Լ� ���� ��� �� ����
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
//// ���� �Լ� ���� ���
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
//	// �� ��Ʈ��ũ ����
//	Net network = readNet(model, config);
//	if (network.empty()) { cerr << "Network load failed" << endl; return -1; }
//	network.setPreferableBackend(DNN_BACKEND_CUDA);	//�鿣�带 CUDA�� ������
//	network.setPreferableTarget(DNN_TARGET_CUDA);		//Ÿ�� ����̽��� CUDA�� ������
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

