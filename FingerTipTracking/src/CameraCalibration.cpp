/*
 * CameraCalibration.cpp
 *
 *  Created on: 23/11/2015
 *      Author: rey
 */

#include "CameraCalibration.h"

CameraCalibration::CameraCalibration() {
	mustInitUndistort = true;
}

CameraCalibration::~CameraCalibration() {
	cvReleaseMat(&intrinsic);
	cvReleaseMat(&distortion);
	cvReleaseImage(&mapx);
	cvReleaseImage(&mapy);
}

void CameraCalibration::loadCalibFromFile(std::string path) {
	FileStorage fs(path, CV_STORAGE_READ);
	intrinsic = (CvMat *) cvReadByName(fs.fs, NULL, "camera_matrix",
	NULL);
	distortion = (CvMat *) cvReadByName(fs.fs, NULL, "distortion_coefficients",
	NULL);
	fs.release();
}

void CameraCalibration::remap(const cv::Mat &frame, cv::Mat &frameundistorted) {
	if (mustInitUndistort) {
		mustInitUndistort = false;
		CvSize sizeFrame(frame.cols, frame.rows);
		mapx = cvCreateImage(sizeFrame, IPL_DEPTH_32F, 1);
		mapy = cvCreateImage(sizeFrame, IPL_DEPTH_32F, 1);
		cvInitUndistortMap(intrinsic, distortion, mapx, mapy);
	}
	IplImage * image = cvCreateImage(cvSize(frame.cols, frame.rows), 8, 3);
	IplImage ipltemp = frame;
	cvCopy(&ipltemp, image);
	IplImage * imagenCorreguida = cvCloneImage(image);
	cvRemap(image, imagenCorreguida, mapx, mapy);
	frameundistorted = cvarrToMat(imagenCorreguida);
	cvReleaseImage(&image);
	cvReleaseImage(&imagenCorreguida);
}

