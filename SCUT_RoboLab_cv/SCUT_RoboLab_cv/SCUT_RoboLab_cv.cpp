
#include "pch.h"
#include "Functions.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <boost/progress.hpp>
#include <vector>
#define is_Red(i) (i>156||i<10)
#define is_Blue(i) (i>=90&&i<124)

//#define TARGET(i) is_Red(i)
#ifndef TARGET(i)
    #define TARGET(i) is_Blue(i)
#endif

constexpr auto _VEDIO = "sfB2.avi";
int main()
{
    cv::VideoCapture cap(_VEDIO);
    cv::namedWindow("Frame");
    cv::namedWindow("Reduce");

    cv::namedWindow("R");


    int shold_median = 5;
    int shold = 16;
    cv::createTrackbar("shold: ", "R",&shold,20);
    while (1)
    {
        std::cout << "\n------------------\n";
        boost::progress_timer g_time; 
        cv::Mat frame;
        cv::Mat reduce;
        cv::Mat HSV;
        cv::Mat HSV_median;
        cap >> frame;
        if (frame.empty())
        {
            cap.release();
            cap.open(_VEDIO);
            continue;
        }
        
        {
            boost::progress_timer t;
            colorReduce(frame, reduce, 16);
            cvtColor(reduce, HSV, cv::COLOR_BGR2HSV);
            cv::medianBlur(HSV, HSV_median, shold_median);
            //imshow("Blur", HSV_median);
            std::cout << "colorReduce and Blur: ";
        }

        std::vector<cv::Mat> channels_unB,channels;
        split(HSV, channels_unB);
        split(HSV_median, channels);
        cv::Mat Hue, Value;
        cv::Mat Hue_unB_sholded, Value_unB_sholded;



        myShold(channels_unB[0], Hue_unB_sholded, [](int i)->uchar {return TARGET(i) ? 255 : 0; });
        myShold(channels_unB[2], Value_unB_sholded, [](int i)->uchar {return i >= 88 ? 255 : 0; });
        cv::Mat R = (Hue_unB_sholded  & channels[0]) & (channels[2] & Value_unB_sholded);
		cv::morphologyEx(R, R, cv::MORPH_CLOSE, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(7, 7), cv::Point(-1, -1)));

        cv::Mat R_Ex,R_Sk;
        cv::morphologyEx(R, R_Ex, cv::MORPH_DILATE, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(7,7), cv::Point(-1, -1)));
        cv::medianBlur(R_Ex, R_Ex, 9);
        cv::morphologyEx(R_Ex, R_Ex, cv::MORPH_ERODE, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(7, 7), cv::Point(-1, -1)));
	
		mySkeleton(R_Ex, R_Sk);
		imshow("Reduce", reduce);
		
		imshow("R", R);
        imshow("R_Ex", R_Ex);
		imshow("R_Sk", R_Sk);
      
         
        std::cout << "global time: ";
        cv::waitKey((30-g_time.elapsed())>0? (30 - g_time.elapsed()):0);
    }
}

