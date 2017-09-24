#include "Video.h"

void Video::VideoDisplaying(int device) {
	char key;
	namedWindow("Displaying_Video", 1);    //Create window

	VideoCapture capture(device);
	if (!capture.isOpened())  // check if we succeeded
		throw "Error when opening steam_avi";

	Mat frame;

	while (1) { //Create infinte loop for live streaming

		bool bSuccess = capture.read(frame);
		if (!bSuccess) {
			break;
		}

		imshow("Displaying_Video", frame);

		if (waitKey(10) == 27) {
			break;
		}

	}
	capture.release();
}

void Video::VideoDisplaying(int device, FingerTipDetection fingers) {
	char key;
	namedWindow("Displaying_Video", 1);    //Create window

	VideoCapture capture(device);
	if (!capture.isOpened())  // check if we succeeded
		throw "Error when opening steam_avi";

	while (1) { //Create infinte loop for live streaming

		Mat frame, frame2, dst;

		bool bSuccess = capture.read(frame);
		if (!bSuccess) {
			break;
		}

		cvtColor(frame, frame2, CV_BGR2GRAY);

		threshold(frame2, dst, 138, 150, 0);

		fingers.Countours(dst, frame);

		imshow("Fingers Dectected", fingers.output);
		imshow("Displaying_Video", frame);

		if (waitKey(120) == 27) {
			break;
		}

	}
	capture.release();
}

void Video::VideoRecording(int device, string videoName) {
	char key;
	namedWindow("Recording_Video", 1);    //Create window
	VideoCapture capture(device);
	if (!capture.isOpened())  // if not success, exit program
	{
		throw "ERROR: Cannot open the video file";
	}
	double dWidth = capture.get(CV_CAP_PROP_FRAME_WIDTH);
	double dHeight = capture.get(CV_CAP_PROP_FRAME_HEIGHT);

	Size frameSize(static_cast<int>(dWidth), static_cast<int>(dHeight));

	VideoWriter videoWriter(videoName + ".avi", CV_FOURCC('M', 'P', '4', '2'),
			20, frameSize, true);

	if (!videoWriter.isOpened()) {
		throw "ERROR: Cannot open write the video file";

	}
	while (1) {
		Mat frame;
		bool bSuccess = capture.read(frame);

		if (!bSuccess) {
			break;
		}

		videoWriter.write(frame);
		imshow("Recording_Video", frame);

		if (waitKey(10) == 27) {
			break;
		}

	}
	capture.release();

}

void Video::VideoLoading(string filename) {
	VideoCapture capture(filename);

	if (!capture.isOpened())
		throw "Error when reading steam_avi";

	namedWindow("Loading_Video", 1);

	Mat frame;

	while (1) {

		bool bSuccess = capture.read(frame); // read a new frame from video

		if (!bSuccess) //if not success, break loop
		{
			throw "Error when reading steam_avi";
		}

		imshow("Loading_Video", frame);

		if (waitKey(30) == 27) {
			break;
		}
	}
	capture.release();
}

void Video::VideoLoading(string filename, FingerTipDetection fingers) {
	VideoCapture capture(filename);

	if (!capture.isOpened())
		throw "Error when reading steam_avi";

	namedWindow("Loading_Video", 1);

	Mat frame, frame2, dst;

	do {

		bool bSuccess = capture.read(frame); // read a new frame from video

		if (!bSuccess) //if not success, break loop
		{
			throw "Error when reading steam_avi";
		}

		cvtColor(frame, frame2, CV_BGR2GRAY);

		threshold(frame2, dst, 138, 150, 0);

		fingers.Countours(dst, frame);

		imshow("Fingers Dectected", fingers.output);
		imshow("Loading_Video", frame);

		if (waitKey(60) == 27) {
			break;
		}
	} while (frame.data != nullptr);

	capture.release();
}
