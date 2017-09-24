#ifndef _UNDISTORTER_H_
#define _UNDISTORTER_H_

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
//#include "opencv2/core/core_c.h"
//#include "opencv2/core/core.hpp"
//#include "opencv2/imgproc/imgproc_c.h"
//#include "opencv2/imgproc/imgproc.hpp"

typedef struct  {
	double cx;
	double cy;
	double fx;
	double fy;
} Intrinsics;

class Undistorter{
private:
	IplImage* r ;
	IplImage* g ;
	IplImage* b ;
	IplImage* ru ;
	IplImage* gu ;
	IplImage* bu ;

	int width;
	int height;

public:
	CvMat* intrinsic;
	CvMat* distortion;
	CvMat* mapX;
	CvMat* mapY;
	Intrinsics coefIntr;

	Undistorter( int w, int h, CvMat* &intr, CvMat* &dist  ){

		this->width = w;
		this->height = h;

		this->intrinsic = intr ;
		this->distortion = dist ;
		coefIntr.fx  = cvmGet(intrinsic, 0, 0);
		coefIntr.fy  = cvmGet(intrinsic, 1, 1);
		coefIntr.cx  = cvmGet(intrinsic, 0, 2);
		coefIntr.cy  = cvmGet(intrinsic, 1, 2);

		this->r = cvCreateImage(cvSize(this->width, this->height), IPL_DEPTH_8U, 1); //subpixel
		this->g = cvCreateImage(cvSize(this->width, this->height), IPL_DEPTH_8U, 1); //subpixel
		this->b = cvCreateImage(cvSize(this->width, this->height), IPL_DEPTH_8U, 1); //subpixel
		this->ru = cvCreateImage(cvSize(this->width, this->height), IPL_DEPTH_8U, 1); //subpixel
		this->gu = cvCreateImage(cvSize(this->width, this->height), IPL_DEPTH_8U, 1); //subpixel
		this->bu = cvCreateImage(cvSize(this->width, this->height), IPL_DEPTH_8U, 1); //subpixel


		mapX = cvCreateMat(this->height, this->width, CV_32FC1);
		mapY = cvCreateMat(this->height, this->width, CV_32FC1);

		/*Util::printMat(intrinsic);
		Util::printMat(distortion);*/

		cvInitUndistortMap(intrinsic, distortion, mapX, mapY);
	}

	void undistor_rgb_image(IplImage* img, IplImage* out) {
		cvSplit(img, r, g, b, NULL);
		cvRemap( r, ru, mapX, mapY, CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS, cvScalarAll(0) );
		cvRemap( g, gu, mapX, mapY, CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS, cvScalarAll(0) );
		cvRemap( b, bu, mapX, mapY, CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS, cvScalarAll(0) );
		cvMerge(ru, gu, bu, NULL, out);
	}

};

#endif
