/*
 * HandTracking.h
 *
 *  Created on: 05/01/2016
 *      Author: rey
 */

#ifndef SRC_HANDTRACKING_H_
#define SRC_HANDTRACKING_H_

#include <opencv2/core/core.hpp>
#include <opencv2/video.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <vector>

#include <iostream>

using namespace cv;
using namespace std;

class FingerKalmanFilter {
public:

	static const int stateSize = 4;
	static const int measSize = 2;
	static const int contrSize = 0;
	static const unsigned int type = CV_32F;

	static FingerKalmanFilter * createHandKalmanFilterptr(
			cv::Point2f pointInit) {
		FingerKalmanFilter * hkf = new FingerKalmanFilter();
		cv::Mat meas = cv::Mat(measSize, 1, type);    // [z_x,z_y,z_w,z_h]
		meas.at<float>(0) = pointInit.x;
		meas.at<float>(1) = pointInit.y;
		hkf->InitKalmanFilter(meas);
		return hkf;
	}

	static FingerKalmanFilter createHandKalmanFilter(cv::Point2f pointInit) {
		FingerKalmanFilter hkf;
		cv::Mat meas = cv::Mat(measSize, 1, type);    // [z_x,z_y,z_w,z_h]
		meas.at<float>(0) = pointInit.x;
		meas.at<float>(1) = pointInit.y;
		hkf.InitKalmanFilter(meas);
		return hkf;
	}

	FingerKalmanFilter() {
		unsigned int type = CV_32F;
		state = cv::Mat(stateSize, 1, type);  // [x,y,v_x,v_y,w,h]
		kf = cv::KalmanFilter(stateSize, measSize, contrSize, type);
	}
	void InitKalmanFilter(cv::Mat meas) {

		// Transition State Matrix A
		// Note: set dT at each processing step!
		// [ 1 0 dT 0  ]
		// [ 0 1 0  dT ]
		// [ 0 0 1  0  ]
		// [ 0 0 0  1  ]
		cv::setIdentity(kf.transitionMatrix);

		// Measure Matrix H
		// [ 1 0 0 0 ]
		// [ 0 1 0 0 ]
		/*kf.measurementMatrix = cv::Mat::zeros(measSize, stateSize, type);
		 kf.measurementMatrix.at<float>(0) = 1.0f;
		 kf.measurementMatrix.at<float>(5) = 1.0f;*/
		cv::setIdentity(kf.measurementMatrix);

		// Process Noise Covariance Matrix Q
		// [ Ex   0   0     0		]
		// [ 0    Ey  0     0		]
		// [ 0    0   Ev_x  0		]
		// [ 0    0   0     Ev_y	]
		//cv::setIdentity(kf.processNoiseCov, cv::Scalar(1e-2));
		/*kf.processNoiseCov.at<float>(0) = 1e-2;
		 kf.processNoiseCov.at<float>(5) = 1e-2;
		 kf.processNoiseCov.at<float>(10) = 5.0f;
		 kf.processNoiseCov.at<float>(15) = 5.0f;*/
		setIdentity(kf.processNoiseCov, Scalar::all(4.1));

		// Measures Noise Covariance Matrix R
		cv::setIdentity(kf.measurementNoiseCov, cv::Scalar(1e-1));

		setIdentity(kf.errorCovPost, Scalar::all(1e-1));

		// >>>> Initialization
		/*kf.errorCovPre.at<float>(0) = 1; // px
		 kf.errorCovPre.at<float>(5) = 1; // px
		 kf.errorCovPre.at<float>(10) = 1;
		 kf.errorCovPre.at<float>(15) = 1;*/

		kf.statePre.setTo(0);
		kf.statePre.at<float>(0, 0) = meas.at<float>(0);
		kf.statePre.at<float>(1, 0) = meas.at<float>(1);

		kf.statePost.setTo(0);
		kf.statePost.at<float>(0, 0) = meas.at<float>(0);
		kf.statePost.at<float>(1, 0) = meas.at<float>(1);

		state.at<float>(0) = meas.at<float>(0);
		state.at<float>(1) = meas.at<float>(1);
		state.at<float>(2) = 0;
		state.at<float>(3) = 0;
		// <<<< Initialization

	}
	void correctKalmanFilter(cv::Mat meas) {
		kf.correct(meas);
	}
	void predictKalmanFilter() {
		double precTick = ticks;
		ticks = (double) cv::getTickCount();
		double dT = (ticks - precTick) / cv::getTickFrequency(); //seconds
		kf.transitionMatrix.at<float>(2) = dT;
		kf.transitionMatrix.at<float>(9) = dT;
		state = kf.predict();
	}

	const cv::KalmanFilter& getKf() const {
		return kf;
	}

	void setKf(const cv::KalmanFilter& kf) {
		this->kf = kf;
	}

	const cv::Mat& getState() const {
		return state;
	}

	void setState(const cv::Mat& state) {
		this->state = state;
	}

private:
	cv::Mat state;
	cv::KalmanFilter kf;
	double ticks;
};

class HandTracking {

	enum TypeMatcher {
		MORE_DETECT_THAT_PREDICT, MORE_PREDICT_OR_EQUAL_THAT_DETECT
	};

public:
	HandTracking();
	virtual ~HandTracking();
	static cv::KalmanFilter * createKalmanFilterPtr();
	void doFilter();

	const vector<Point2f>& getFingerDetect() const {
		return fingerDetect;
	}

	void setFingerDetect(const vector<Point2f>& fingerDetect) {
		this->fingerDetect = fingerDetect;
	}

	const vector<Point2f>& getFingerFinal() const {
		return fingerFinal;
	}

	void setFingerFinal(const vector<Point2f>& fingerFinal) {
		this->fingerFinal = fingerFinal;
	}

	const vector<Point2f>& getFingerPredict() const {
		return fingerPredict;
	}

	void setFingerPredict(const vector<Point2f>& fingerPredict) {
		this->fingerPredict = fingerPredict;
	}

	const RotatedRect& getHandRect() const {
		return handRect;
	}

	void setHandRect(const RotatedRect& handRect) {
		this->handRect = handRect;
	}

	const Point2f& getPalmCenter() const {
		return palmCenter;
	}

	void setPalmCenter(const Point2f& palmCenter) {
		this->palmCenter = palmCenter;
	}

	const Vec3d& getPalmDirection() const {
		return palmDirection;
	}

	void setPalmDirection(const Vec3d& palmDirection) {
		this->palmDirection = palmDirection;
	}

	vector<FingerKalmanFilter> getFskfs() {
		return fskfs;
	}

	void setFskfs(vector<FingerKalmanFilter> fskfs) {
		this->fskfs = fskfs;
	}

	vector<Point2f> getFingerDefect() {
		return fingerDefect;
	}

	void setFingerDefect(vector<Point2f> fingerDefect) {
		this->fingerDefect = fingerDefect;
	}

protected:
	void matching(vector<Point2f> source, vector<Point2f> mattcher, int * CMIN);

private:
	vector<Point2f> fingerDetect;
	vector<Point2f> fingerPredict;
	vector<Point2f> fingerDefect;
	vector<FingerKalmanFilter> fskfs;
	vector<Point2f> fingerFinal;
	Point2f palmCenter;
	Vec3d palmDirection;
	RotatedRect handRect;
};

#endif /* SRC_HANDTRACKING_H_ */
