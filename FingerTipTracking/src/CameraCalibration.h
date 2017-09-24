/*
 * CameraCalibration.h
 *
 *  Created on: 23/11/2015
 *      Author: rey
 */

#ifndef SRC_CAMERACALIBRATION_H_
#define SRC_CAMERACALIBRATION_H_

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <string>

using namespace cv;

class CameraCalibration {
public:
	CameraCalibration();
	void loadCalibFromFile(std::string path);
	void remap(const cv::Mat &frame, cv::Mat &frameundistorted);
	virtual ~CameraCalibration();

private:
	CvMat * intrinsic;
	CvMat * distortion;
	IplImage * mapx;
	IplImage * mapy;
	bool mustInitUndistort;
};

#endif /* SRC_CAMERACALIBRATION_H_ */
