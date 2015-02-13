/*	CS440 P1 - Nicholas Papadopoulos
 
    -myFrameDifferencing function taken from Lab2.
 */

#include "stdafx.h"
//opencv libraries
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
//C++ standard libraries
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

//Function that does frame differencing between the current frame and the previous frame
void myFrameDifferencing(Mat& prev, Mat& curr, Mat& dst) {
    //For more information on operation with arrays: http://docs.opencv.org/modules/core/doc/operations_on_arrays.html
    //For more information on how to use background subtraction methods: http://docs.opencv.org/trunk/doc/tutorials/video/background_subtraction/background_subtraction.html
    absdiff(prev, curr, dst);
    Mat gs = dst.clone();
    cvtColor(dst, gs, CV_BGR2GRAY);
    dst = gs > 50;
    Vec3b intensity = dst.at<Vec3b>(100,100);
}

int main()
{
    
    //----------------
    //a) Reading a stream of images from a webcamera, and displaying the video
    //----------------
    // For more information on reading and writing video: http://docs.opencv.org/modules/highgui/doc/reading_and_writing_images_and_video.html
    // open the video camera no. 0
    VideoCapture cap(0);
    
    // if not successful, exit program
    if (!cap.isOpened())
    {
        cout << "Cannot open the video cam" << endl;
        return -1;
    }
    
    //create a window called "MyVideoFrame0"
    Mat frame0;
    
    // read a new frame from video
    bool bSuccess0 = cap.read(frame0);
    
    //if not successful, break loop
    if (!bSuccess0)
    {
        cout << "Cannot read a frame from video stream" << endl;
    }
    
    //create a window called MyVideoFore
    cvNamedWindow("MyVideoFore",0);
    cvResizeWindow("MyVideoFore",200,200);
    
    while (1)
    {
        // read a new frame from video
        Mat frame;
        bool bSuccess = cap.read(frame);
        
        //if not successful, break loop
        if (!bSuccess)
        {
            cout << "Cannot read a frame from video stream" << endl;
            break;
        }
        
        // destination frame
        Mat frameDest;
        frameDest = Mat::zeros(frame.rows, frame.cols, CV_8UC1); //Returns a zero array of same size as src mat, and of type CV_8UC1
        
        myFrameDifferencing(frame0, frame, frameDest);
        
//        Mat edit = frameDest.clone();
        
        std::map<int,int> maxChanges;
        int changeCount = 0;
        int prevColor = 0;
        int widthCount = 0;
        for (int i = 0; i < frameDest.rows; i++) {
            changeCount = 0;
            prevColor = 0;
            widthCount = 0;
            for (int j = 0; j < frameDest.cols; j++) {
                if (j == 20) {
                    frameDest.at<uchar>(i,j) = 125;
                }
                int gray_intensity = frameDest.at<uchar>(i,j);
                if (gray_intensity == 255){
                    if (prevColor == 0) {
                        widthCount = 1;
                    } else {
                        widthCount++;
//                        frameDest.at<uchar>(i,j) = 0;
                    }
                    prevColor = 255;
                } else if (gray_intensity == 0) {
                    if (prevColor == 255) {
                        if (widthCount > 10 && widthCount < 75) {
                            changeCount++;
//                            line(edit, Point(j,i), Point(j-75,i), CV_RGB(100.0, 100.0, 100.0), 5);
                            //                        frameDest.at<uchar>(i,j) = 255;
                        }
                    }
                    prevColor = 0;
                }
            }
            
            if (changeCount != 0) {
                if (maxChanges.count(changeCount) > 0) {
                    maxChanges[changeCount]++;
                } else {
                    maxChanges[changeCount] = 1;
                }
            }
        }
        
        int maxVal = 0;
        int maxKey = -1;
        for (std::map<int,int>::iterator it=maxChanges.begin(); it!=maxChanges.end(); ++it) {
//            cout << it->first << "\n";
            if (it->second > maxVal) {
                maxVal = it->second;
                maxKey = it->first;
            }
        }
//        cout << maxKey << "\n";
        
        std::string max = std::to_string(maxKey);
        putText(frameDest, max, Point(10,100), FONT_HERSHEY_PLAIN, 5.0, CV_RGB(255.0, 255.0, 255.0), 2);
//        line(edit, Point(600,400), Point(610,400), CV_RGB(255.0, 255.0, 255.0), 5);
//        line(edit, Point(600,410), Point(675,410), CV_RGB(255.0, 255.0, 255.0), 5);
//        putText(edit, max, Point(10,100), FONT_HERSHEY_PLAIN, 5.0, CV_RGB(255.0, 255.0, 255.0), 2);

//        if (maxKey == 1) {
//            putText(frameDest, "1", Point(10,100), FONT_HERSHEY_PLAIN, 5.0, CV_RGB(255.0, 255.0, 255.0), 2);
//        } else if (maxKey == 2) {
//            putText(frameDest, "2", Point(10,100), FONT_HERSHEY_PLAIN, 5.0, CV_RGB(255.0, 255.0, 255.0), 2);
//        } else if (maxKey == 3) {
//            putText(frameDest, "3", Point(10,100), FONT_HERSHEY_PLAIN, 5.0, CV_RGB(255.0, 255.0, 255.0), 2);
//        } else if (maxKey == 4) {
//            putText(frameDest, "4", Point(10,100), FONT_HERSHEY_PLAIN, 5.0, CV_RGB(255.0, 255.0, 255.0), 2);
//        }
        
        imshow("MyVideoFore", frameDest);
        
        //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
        if (waitKey(30) == 27)
        {
            cout << "esc key is pressed by user" << endl;
            break;
        }
        
    }
    cap.release();
    return 0;
}