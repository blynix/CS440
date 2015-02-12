/*	CS585_Lab2.cpp
 *	CS585 Image and Video Computing Fall 2014
 *	Lab 2
 *	--------------
 *	This program introduces the following concepts:
 *		a) Reading a stream of images from a webcamera, and displaying the video
 *		b) Skin color detection
 *		c) Background differencing
 *		d) Visualizing motion history
 *	--------------
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

//function declarations

/**
	Function that returns the maximum of 3 integers
	@param a first integer
	@param b second integer
	@param c third integer
 */
int myMax(int a, int b, int c);

/**
	Function that returns the minimum of 3 integers
	@param a first integer
	@param b second integer
	@param c third integer
 */
int myMin(int a, int b, int c);

/**
	Function that detects whether a pixel belongs to the skin based on RGB values
	@param src The source color image
	@param dst The destination grayscale image where skin pixels are colored white and the rest are colored black
 */
void mySkinDetect(Mat& src, Mat& dst);

/**
	Function that does frame differencing between the current frame and the previous frame
	@param src The current color image
	@param prev The previous color image
	@param dst The destination grayscale image where pixels are colored white if the corresponding pixel intensities in the current
 and previous image are not the same
 */
void myFrameDifferencing(Mat& prev, Mat& curr, Mat& dst);

/**
	Function that accumulates the frame differences for a certain number of pairs of frames
	@param mh Vector of frame difference images
	@param dst The destination grayscale image to store the accumulation of the frame difference images
 */
void myMotionEnergy(Vector<Mat> mh, Mat& dst);

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
    namedWindow("MyVideo0",WINDOW_AUTOSIZE);
    Mat frame0;
    Mat frameO;
    
    // read a new frame from video
    bool bSuccess0 = cap.read(frame0);
    bool bSuccessO = cap.read(frameO);
    
    //if not successful, break loop
    if (!bSuccess0 || !bSuccessO)
    {
        cout << "Cannot read a frame from video stream" << endl;
    }
    
    //show the frame in "MyVideo" window
    imshow("MyVideo0", frame0);
    
    //create a window called "MyVideo"
    namedWindow("MyVideo",WINDOW_AUTOSIZE);
    namedWindow("MyVideoMH",WINDOW_AUTOSIZE);
    
    vector<Mat> myMotionHistory;
    Mat fMH1, fMH2, fMH3;
    fMH1 = Mat::zeros(frame0.rows, frame0.cols, CV_8UC1);
    fMH2 = fMH1.clone();
    fMH3 = fMH1.clone();
    myMotionHistory.push_back(fMH1);
    myMotionHistory.push_back(fMH2);
    myMotionHistory.push_back(fMH3);
    
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
//        //----------------
//        //	b) Skin color detection
//        //----------------
//        //mySkinDetect(frame, frameDest, 0);
//        
//        //----------------
//        //	c) Background differencing
//        //----------------
//        
//        
//        //call myFrameDifferencing function
//        myFrameDifferencing(frame0, frame, frameDest);
//        imshow("MyVideo", frameDest);
//        myMotionHistory.erase(myMotionHistory.begin());
//        myMotionHistory.push_back(frameDest);
//        Mat myMH = Mat::zeros(frame0.rows, frame0.cols, CV_8UC1);
//        
//        //----------------
//        //  d) Visualizing motion history
//        //----------------
//        
//        //call myMotionEnergy function
//        myMotionEnergy(myMotionHistory, myMH);
//        
//        
//        imshow("MyVideoMH", myMH); //show the frame in "MyVideo" window
//        frame0 = frame;
        
        //----------------
        //  e) foreground
        //------
        
        myFrameDifferencing(frameO, frame, frameDest);
        
        std::map<int,int> maxChanges;
        int changeCount = 0;
        int prevColor = 0;
        int widthCount = 0;
        for (int i = 0; i < frameDest.rows; i++) {
            changeCount = 0;
            prevColor = 0;
            widthCount = 0;
            for (int j = 0; j < frameDest.cols; j++) {
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
                        if (widthCount > 5 && widthCount < 100) {
                            changeCount++;
                            line(frameDest, Point(i,j), Point(i-100,j), CV_RGB(100.0, 100.0, 100.0), 3);
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
        cout << maxKey << "\n";
        
        std::string max = std::to_string(maxKey);
        line(frameDest, Point(600,400), Point(605,400), CV_RGB(255.0, 255.0, 255.0), 5);
        line(frameDest, Point(600,410), Point(700,410), CV_RGB(255.0, 255.0, 255.0), 5);
        putText(frameDest, max, Point(10,100), FONT_HERSHEY_PLAIN, 5.0, CV_RGB(255.0, 255.0, 255.0), 2);

//        if (maxKey == 1) {
//            putText(frameDest, "1", Point(100,500), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(255.0, 255.0, 255.0), 5);
//        } else if (maxKey == 2) {
//            putText(frameDest, "2", Point(100,500), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(255.0, 255.0, 255.0), 5);
//        } else if (maxKey == 3) {
//            putText(frameDest, "3", Point(100,500), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(255.0, 255.0, 255.0), 5);
//        } else if (maxKey == 4) {
//            putText(frameDest, "4", Point(100,500), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(255.0, 255.0, 255.0), 5);
//        }
        
        imshow("MyVideoFore", frameDest);
        
        //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
        if (waitKey(300) == 27)
        {
            cout << "esc key is pressed by user" << endl;
            break;
        }
        
    }
    cap.release();
    return 0;
}

//Function that returns the maximum of 3 integers
int myMax(int a, int b, int c) {
    int m = a;
    (void)((m < b) && (m = b));
    (void)((m < c) && (m = c));
    return m;
}

//Function that returns the minimum of 3 integers
int myMin(int a, int b, int c) {
    int m = a;
    (void)((m > b) && (m = b));
    (void)((m > c) && (m = c));
    return m;
}

//Function that detects whether a pixel belongs to the skin based on RGB values
void mySkinDetect(Mat& src, Mat& dst) {
    //Surveys of skin color modeling and detection techniques:
    //Vezhnevets, Vladimir, Vassili Sazonov, and Alla Andreeva. "A survey on pixel-based skin color detection techniques." Proc. Graphicon. Vol. 3. 2003.
    //Kakumanu, Praveen, Sokratis Makrogiannis, and Nikolaos Bourbakis. "A survey of skin-color modeling and detection methods." Pattern recognition 40.3 (2007): 1106-1122.
    for (int i = 0; i < src.rows; i++){
        for (int j = 0; j < src.cols; j++){
            //For each pixel, compute the average intensity of the 3 color channels
            Vec3b intensity = src.at<Vec3b>(i,j); //Vec3b is a vector of 3 uchar (unsigned character)
            int B = intensity[0]; int G = intensity[1]; int R = intensity[2];
            if ((R > 95 && G > 40 && B > 20) && (myMax(R,G,B) - myMin(R,G,B) > 15) && (abs(R-G) > 15) && (R > G) && (R > B)){
                dst.at<uchar>(i,j) = 255;
            }
        }
    }
}

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

//Function that accumulates the frame differences for a certain number of pairs of frames
void myMotionEnergy(Vector<Mat> mh, Mat& dst) {
    Mat mh0 = mh[0];
    Mat mh1 = mh[1];
    Mat mh2 = mh[2];
    
    for (int i = 0; i < dst.rows; i++){
        for (int j = 0; j < dst.cols; j++){
            if (mh0.at<uchar>(i,j) == 255 || mh1.at<uchar>(i,j) == 255 ||mh2.at<uchar>(i,j) == 255){
                dst.at<uchar>(i,j) = 255;
            }
        }
    }
}