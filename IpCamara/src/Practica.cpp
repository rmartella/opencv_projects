#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <time.h>
int main(int, char**) {

	cv::VideoCapture vcap;

	const std::string video = "http://192.168.42.168:8080/shot.jpg";
	namedWindow("Video", WINDOW_NORMAL);
	CvMat *intrinsic;
	CvMat *distortion;
	CvFileStorage * fs;
	fs = cvOpenFileStorage("datos/matrixIP.xml", NULL, CV_STORAGE_READ);
	intrinsic = (CvMat *) cvReadByName(fs, NULL, "camera_matrix", NULL);
	distortion = (CvMat *) cvReadByName(fs, NULL, "distortion_coefficients",
	NULL);
	cvReleaseFileStorage(&fs);
	int frame = 0;
	long diferencia = 0;
	cv::Mat matImage;
	IplImage image;

	do {
		long inicio = getTickCount();
		vcap.open(video);
		vcap.read(matImage);
		image = IplImage(matImage);
		if (&image != NULL) {
			IplImage * imagenCorreguida = cvCloneImage(&image);
			cvUndistort2(&image, imagenCorreguida, intrinsic, distortion);
			cvShowImage("Video", imagenCorreguida);
			long fin = getTickCount();
			if ((cvWaitKey(1) & 255) == 27)
				break;
			cvReleaseImage(&imagenCorreguida);
			cout << "Frame:" << frame++ << endl;
			diferencia += fin - inicio;
			cout << "inicio:" << inicio << endl;
			cout << "fin:" << fin << endl;
			cout << "Tiempo carga frame:" << diferencia << endl;
		}

	} while (&image != NULL);

	float frameRate = diferencia / (getTickFrequency() * frame);

	cout << "Tick Frequency:" << getTickFrequency() << endl;
	cout << "Frame Rate:" << 1 / frameRate << endl;

	cvWaitKey(0);

	cvReleaseMat(&intrinsic);
	cvReleaseMat(&distortion);

	cvDestroyWindow("Video");

}
