#include <iostream>
#include <opencv2/opencv.hpp>
#include <time.h>
using namespace std;
using namespace cv;

#define ROBOT_NUM 9
#define START_PAGE 600
#define TOTAL_PAGE 699

int main() {
    cv::VideoCapture cap(1);
    /*cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 360);*/
    cv::Mat frame;
    string file_name;
    int num = START_PAGE;
    int cnt = 0;

    clock_t start, end;
    double time_result;
    start = clock();	// 수행 시간 측정 시작
    
    while (1) {
        cap >> frame;

        if (cnt == 10) {
            //file_name = format("num_%d/%d_%d.jpg", ROBOT_NUM, ROBOT_NUM, num);
            //file_name = format("num_%d_%d/%d_%d__%d.jpg", ROBOT_NUM, ROBOT_NUM + 1, ROBOT_NUM, ROBOT_NUM + 1, num);
            file_name = format("robot_direction/robot_direction_%d.jpg", num);
            cv::imwrite(file_name, frame);

            if (num == TOTAL_PAGE)
                break;

            num++;
            cnt = -1;
        }

        imshow("frame", frame);
        cv::waitKey(1);

        cnt++;
    }

    end = clock();		//시간 측정 끝
    time_result = (double)(end - start);
    time_result /= CLOCKS_PER_SEC;
    std::cout << "총 걸린 시간 : " << time_result << "초" << std::endl;

}