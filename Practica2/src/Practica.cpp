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
int main(int argc, const char** argv) {
	const char * window1_name = "DISTORCIONADA";
	const char * window2_name = "CORREGIDA";

	//CvCapture* captura;
	cv::VideoCapture captura = cv::VideoCapture();
	captura.open(1);

	namedWindow(window1_name, WINDOW_AUTOSIZE);
	namedWindow(window2_name, WINDOW_AUTOSIZE);
	CvMat *intrinsic;
	CvMat *distortion;
	CvFileStorage * fs;
	fs = cvOpenFileStorage("datos/matrixInfraroja.xml", NULL, CV_STORAGE_READ);
	intrinsic = (CvMat *) cvReadByName(fs, NULL, "camera_matrix", NULL);
	distortion = (CvMat *) cvReadByName(fs, NULL, "distortion_coefficients",
	NULL);
	cvReleaseFileStorage(&fs);

	int framenum = 0;
	long diferencia = 0;

	Mat frame;
	IplImage * image;
	IplImage* mapx;
	IplImage* mapy;
	do {
		long inicio = getTickCount();
		captura.read(frame);
		image = cvCreateImage(cvSize(frame.cols,frame.rows),8,3);
		IplImage ipltemp = frame;
		cvCopy(&ipltemp,image);
		if (framenum == 0) {
			mapx = cvCreateImage(cvGetSize(image), IPL_DEPTH_32F, 1);
			mapy = cvCreateImage(cvGetSize(image), IPL_DEPTH_32F, 1);
			cvInitUndistortMap(intrinsic, distortion, mapx, mapy);
		}
		if (image != NULL) {
			IplImage * imagenCorreguida = cvCloneImage(image);
			// cvUndistort2(image, imagenCorreguida, intrinsic, distortion);
			cvRemap(image, imagenCorreguida, mapx, mapy);
			cvShowImage(window2_name, imagenCorreguida);
			long fin = getTickCount();
			cvShowImage(window1_name, image);
			if ((cvWaitKey(1) & 255) == 27)
				break;
			cvReleaseImage(&imagenCorreguida);
			cout << "Frame:" << framenum++ << endl;
			diferencia += fin - inicio;
			cout << "inicio:" << inicio << endl;
			cout << "fin:" << fin << endl;
			cout << "Tiempo carga frame:" << diferencia << endl;
		}
	} while (image != NULL);

	float frameRate = diferencia / (getTickFrequency() * framenum);

	cout << "Tick Frequency:" << getTickFrequency() << endl;
	cout << "Frame Rate:" << 1 / frameRate << endl;

	cvWaitKey(0);

	cvReleaseMat(&intrinsic);
	cvReleaseMat(&distortion);

	captura.release();
	cvDestroyWindow(window1_name);
	cvDestroyWindow(window2_name);
}
