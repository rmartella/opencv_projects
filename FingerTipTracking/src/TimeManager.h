/*
 * TimeManager.h
 *
 *  Created on: 24/11/2015
 *      Author: rey
 */

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;

#ifndef SRC_TIMEMANAGER_H_
#define SRC_TIMEMANAGER_H_

class TimeManager {
public:
	TimeManager();
	virtual ~TimeManager();

	static TimeManager& Instance() {
		static TimeManager instance;
		return instance;
	}

	double CalculateFrameRate(bool writeToConsole);
private:
	double currentTicks;
};

#endif /* SRC_TIMEMANAGER_H_ */
