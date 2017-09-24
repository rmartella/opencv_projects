#define NOMINMAX // to avoid an error compilation time on Windows (Eigen)
#define _CRT_SECURE_NO_DEPRECATE 1	// Un-deprecate CRT functions

#include <iostream>
#include <string.h>

#include "Camera/CameraKinect.h"
#include "Camera/MotorKinect.h"

#include "Log.h"
#include "Util.h"
#include "boost/thread.hpp"

using namespace std;
using namespace ModViPumas;
using namespace ModViPumas;

void help(char *prg);

int main(int argc, char** argv) {
	bool isReady;
	Log log("visionModule.log");

	int fails = 0;
	int i;

	string basePath = Util::getCurrentPath();
	string calibrationFile = basePath + "calibration_rgb.yaml";
	string recording = "";
	int port = 2070;

	for (i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-i")) {
			recording.assign(argv[i + 1]);
		}
	}

	printf("Base Path: %s\n", basePath.c_str());
	printf("Calibration file: %s\n", calibrationFile.c_str());

	CameraKinect *camKinect = 0;	//Kinect camera manager
	MotorKinect *motKinect = 0;	//Kinect camera manager
	CameraKinectImages *imageskinect = 0;	//images from kinect
	IplImage *rgbDepth = 0;

	/************************************************************************/
	/* Starting kinect motor                                                */
	/************************************************************************/
	motKinect = new MotorKinect();

	isReady = motKinect->Open();

	if (!isReady) {
		cerr << "Cannot open Kinect motor" << endl;
	} else {
		motKinect->Move(-15);
	}

	/************************************************************************/
	/* Starting camera kinect                                               */
	/************************************************************************/
	camKinect = new CameraKinect();
	string data = string(recording.c_str());
	//string data = string("\opt\codigo\opencv\VisionKinect\drinks.oni");
	transform(data.begin(), data.end(), data.begin(), ::tolower);
	if (strstr(data.c_str(), ".oni") != NULL) {
		camKinect->openRecording("SamplesConfig.xml",
						(char*) calibrationFile.c_str(), (char *) data.c_str());
	} else {
		camKinect->openCamera("SamplesConfig.xml",
				(char*) calibrationFile.c_str());
	}

	isReady = camKinect->getOpened();

	if (!isReady) {
		cerr << "Cannot open camera Kinect" << endl;
	}

	/************************************************************************/
	/*  Output                                        */
	/***********************************************************************/

	int key = 0;
	do {
		imageskinect = camKinect->getFrame();



		rgbDepth = imageskinect->getImageBetween(400, 2000);
		cvNamedWindow("Kinect", CV_WINDOW_AUTOSIZE);
		cvShowImage("Kinect", rgbDepth);

		cout << motKinect->Pose() << endl;
		//motKinect->Move( 0 );

		key = cvWaitKey(10);

		cvReleaseImage(&rgbDepth);

	} while (key != 27);

	return EXIT_SUCCESS;
}
