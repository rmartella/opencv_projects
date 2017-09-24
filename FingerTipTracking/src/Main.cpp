#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <time.h>
using namespace std;

#include "CameraCalibration.h"
#include "Video.h"

#include "HandTracking.h"

int main(int argc, const char** argv) {

	Video * video = new Video();
	FingerTipDetection fingerDet;

	video->VideoLoading("VideoTest/video_con_lente_de_4.3mm.avi", fingerDet);
	delete video;

	/*HandTracking handT;
	 vector<Point2d> positionDet;
	 positionDet.push_back(Point2d(0.1, 0.1));
	 //positionDet.push_back(Point2d(0, 0));
	 //positionDet.push_back(Point2d(0, 0));
	 positionDet.push_back(Point2d(0.1, 0.2));
	 //positionDet.push_back(Point2d(0, 0));

	 vector<Point2d> positionPred;
	 positionPred.push_back(Point2d(0.1, 0.0));
	 positionPred.push_back(Point2d(0.11, 0.1));
	 positionPred.push_back(Point2d(0.13, 0.23));
	 positionPred.push_back(Point2d(0, 0));
	 positionPred.push_back(Point2d(0, 0));
	 positionPred.push_back(Point2d(0, 0));
	 positionPred.push_back(Point2d(0, 0));

	 FingerKalmanFilter* fk = FingerKalmanFilter::createHandKalmanFilterptr(
	 cv::Point2d(2, 0));
	 vector<FingerKalmanFilter*> vectorfk;
	 vectorfk.push_back(fk);
	 vectorfk.push_back(fk);
	 vectorfk.push_back(fk);
	 handT.setFskfs(vectorfk);

	 handT.setFingerDetect(positionDet);
	 handT.setFingerPredict(positionPred);

	 handT.doFilter();*/

}
