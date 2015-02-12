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
    
    //create a window called MyVideoSkin
    cvNamedWindow("MyVideoSkin",0);
    cvResizeWindow("MyVideoSkin",200,200);

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
//----------------
//	b) Skin color detection
//----------------
        mySkinDetect(frame, frameDest);
//        dilate(frameDest, frameDest, Mat());
        erode(frameDest, frameDest, Mat());
        
        std::map<int,int> maxChanges;
        int changeCount = 0;
        int prevColor = 0;
        for (int i = 0; i < frameDest.rows; i++) {
            changeCount = 0;
            prevColor = 0;
            for (int j = 0; j < frameDest.cols; j++) {
                int gray_intensity = frameDest.at<uchar>(i,j);
                if (gray_intensity == 255){
                    if (prevColor == 0) {
                        changeCount++;
                        prevColor = 255;
                    } else {
                        frameDest.at<uchar>(i,j) = 0;
                    }
                } else if (gray_intensity == 0) {
                    if (prevColor == 255) {
                        changeCount++;
                        frameDest.at<uchar>(i,j) = 255;
                        prevColor = 0;
                    }
                }
            }
            
            if (maxChanges.count(changeCount) > 0) {
                maxChanges[changeCount]++;
            } else {
                maxChanges[changeCount] = 1;
            }
        }
        
        imshow("MyVideoSkin", frameDest);
        
        int max = max_element(maxChanges.begin(), maxChanges.end())->first;
        
//        std::map<int, int>::const_iterator it;
//        int key = -1;
//        
//        for (it = maxChanges.begin(); it != maxChanges.end(); ++it)
//        {
//            if (it->second == max)
//            {
//                key = it->first;
//                break;
//            }
//        }
        cout << max / 2;


		//wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
        if (waitKey(3000) == 27)
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