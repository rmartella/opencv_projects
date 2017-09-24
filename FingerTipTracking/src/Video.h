#include <iostream>
#include <fstream>
#include <string.h>
#include <math.h>

#include <opencv2/opencv.hpp>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "FingerTipDetection.h"

using namespace cv;
using namespace std;

class Video{

public:

	void VideoDisplaying(int device);
	void VideoDisplaying(int device, FingerTipDetection fingers);
	void VideoRecording(int device, string videoName);
	void VideoLoading(string filename);
	void VideoLoading(string filename, FingerTipDetection fingers);

};
