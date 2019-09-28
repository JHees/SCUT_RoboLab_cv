
#include "Functions.h"
#include <iostream>
#include <opencv.hpp>
#include <boost/progress.hpp>
#include <vector>
#define is_Red(i) ((i>156)||(i<10))
#define is_Blue(i) (i>=90&&i<124)

//#define TARGET(i) is_Red(i)
#ifndef TARGET(i)
    #define TARGET(i) is_Blue(i)
#endif

constexpr auto _VEDIO = "../resource/sfB2.avi";
int main()
{
    cv::VideoCapture cap(_VEDIO);

    int shold_median = 5;
    int shold = 16;
    cv::createTrackbar("shold: ", "R",&shold,20);
    while (1)
    {
        std::cout << "\n------------------\n";
        boost::timer g_time; 
        cv::Mat frame;
		cap >> frame;
        cv::Mat reduce;
        cv::Mat HSV;
		std::vector<cv::Mat> channels;
		cv::Mat Hue_unB_sholded, Value_unB_sholded;
		cv::Mat R;
		std::vector<std::vector<cv::Point>> contours,contours_Tar;
		std::vector<cv::Vec4i> hierarchy;
		
		#ifdef Debug
		std::cout << "Create: " << g_time.elapsed() << std::endl;
		#endif
        if (frame.empty())
        {
            cap.release();
            cap.open(_VEDIO);
            continue;
        }
        
        {
			#ifdef Debug
			boost::progress_timer t;
			#endif
            colorReduce(frame, reduce, 16);
            cvtColor(reduce, HSV, cv::COLOR_BGR2HSV);
			#ifdef Debug
            std::cout << "colorReduce: ";
			#endif
		}
		
		{
			#ifdef Debug
			boost::progress_timer t;
			#endif
			split(HSV, channels);
			myShold(channels[0], Hue_unB_sholded, [](int i)->uchar {return TARGET(i) ? 255 : 0; });
			myShold(channels[2], Value_unB_sholded, [](int i)->uchar {return i >= 88 ? 255 : 0; });
			R = (Hue_unB_sholded& Value_unB_sholded);
			cv::morphologyEx(R, R, cv::MORPH_CLOSE, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(7, 7), cv::Point(-1, -1)));
			cv::morphologyEx(R, R, cv::MORPH_OPEN, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3), cv::Point(-1, -1)));
			cv::threshold(R, R, 1, 255, cv::THRESH_BINARY);
			cv::morphologyEx(R, R, cv::MORPH_DILATE, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(7, 7), cv::Point(-1, -1)));
			cv::medianBlur(R, R, 9);
			cv::morphologyEx(R, R, cv::MORPH_ERODE, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5), cv::Point(-1, -1)));
			std::cout << "morphology: ";
		}
		{
			#ifdef Debug
			boost::progress_timer t;
			#endif
			cv::findContours(R, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_TC89_KCOS);
			for (std::vector<cv::Vec4i>::iterator i = hierarchy.begin(); i != hierarchy.end(); ++i)
			{
				if (i->val[0] == -1 && i->val[1] == -1 && i->val[2] == -1 && i->val[3] != -1)
				{
					contours_Tar.push_back(contours[std::distance(hierarchy.begin(), i)]);
				}
			}
			#ifdef Debug
			std::cout << "findContours: ";
			#endif
		}
		
		if (contours_Tar.size() != 1)
		{
			std::cout << "error Targer." << std::endl;
			continue;
		}
			cv::Point2d center(0, 0);
			cv::RotatedRect rRect = cv::minAreaRect(contours_Tar[0]);
			cv::Point2f vertices[4];
		{
			rRect.points(vertices);
			for (int i = 0; i < 4; i++)
			{
				center.x += vertices[i].x / 4;
				center.y += vertices[i].y / 4;
				line(frame, vertices[i], vertices[(i + 1) % 4], cv::Scalar(179,245, 222 ), 2);
			}
			cv::circle(frame, center, 2, cv::Scalar(79, 245, 222), 2);
		}
		//imshow("Reduce", reduce);
		//imshow("CON", con);
		//imshow("R", R);
        //imshow("R_Ex", R_Ex);
		imshow("op", frame);
		std::cout << "Center: " << center.x << "," << center.y << std::endl << std::endl;
		std::cout << "global time: " << g_time.elapsed() << std::endl;;
        cv::waitKey((30-1000*g_time.elapsed())>0? (30 - 1000*g_time.elapsed()):1);
    }
}

