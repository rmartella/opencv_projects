#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <time.h>

int hueRangeMin = 0;
int satRangeMin = 0;
int valRangeMin = 0;
int hueRangeMax = 255;
int satRangeMax = 255;
int valRangeMax = 255;

cv::Mat frame;
cv::Mat frameWork;
cv::Mat frameHSV;
cv::Mat mask;
cv::Mat maskInverse;
cv::Mat maskRange;

int xmin, ymin, xmax, ymax;
bool pressed = false;
bool cropping = false;
bool getRoi = false;
bool regionOfInters = false;

void on_trackbar(int, void*) {
	/*if (bloques <= 1) {
	 bloques = 3;
	 } else {
	 if (bloques % 2 != 1) {
	 bloques += 1;
	 }
	 }*/
}
void on_mouse(int event, int x, int y, int flags, void* param) {
	if (event == CV_EVENT_LBUTTONDOWN) {
		xmin = x;
		xmax = x;
		ymin = y;
		ymax = y;
		cropping = true;
	} else if (event == CV_EVENT_MOUSEMOVE && cropping) {
		xmax = x;
		ymax = y;
	} else if (event == CV_EVENT_LBUTTONUP) {
		xmax = x;
		ymax = y;
		cropping = false, getRoi = true;
	}

	/*if (event == CV_EVENT_FLAG_LBUTTON && !pressed) {
	 pressed = true;
	 regionOfInters = false;
	 xmin = x;
	 ymin = y;
	 std::cout << "xmin:" << xmin << std::endl;
	 std::cout << "ymin:" << ymin << std::endl;
	 } else if (event == CV_EVENT_FLAG_LBUTTON && pressed) {
	 pressed = false;
	 regionOfInters = true;
	 xmax = x;
	 ymax = y;
	 std::cout << "xmax:" << xmax << std::endl;
	 std::cout << "ymax:" << ymax << std::endl;
	 }*/
}

void calculaCentroide(cv::Mat mask, cv::Mat imagenSegmentada) {
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::Mat canny_output;
	mask.copyTo(canny_output);
	cv::findContours(canny_output, contours, CV_RETR_TREE,
			CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
	int indiceContornoMaximo = 0;
	int pixelesMaximo = 0;
	// vector<Point> contornoMaximo;
	// vector<Moments> mu(contours.size());
	for (int i = 0; i < contours.size(); i++) {
		if (contours[i].size() > pixelesMaximo) {
			pixelesMaximo = contours[i].size();
			indiceContornoMaximo = i;
		}
	}
	if (contours.size() > 0) {
		std::vector<cv::Point> contour_poly(
				contours[indiceContornoMaximo].size());
		cv::approxPolyDP(cv::Mat(contours[indiceContornoMaximo]), contour_poly,
				3, true);
		cv::boundingRect(contour_poly);
		cv::rectangle(imagenSegmentada, cv::boundingRect(contour_poly).tl(),
				cv::boundingRect(contour_poly).br(), CV_RGB(124, 40, 30), 2, 8,
				0);
		cv::Moments centroide = moments(contours[indiceContornoMaximo], false);
		cv::Point punto(centroide.m10 / centroide.m00,
				centroide.m01 / centroide.m00);
		cv::circle(imagenSegmentada, punto, 4, CV_RGB(124, 40, 30), -1, 8, 0);
		std::cout << cv::Mat(punto) << std::endl;
	}

}

void computeCentroid() {

}

int main(int argc, char** argv) {
	/*Variables de calibración*/
	cv::VideoCapture captura = cv::VideoCapture();
	captura.open(0);

	int key;

	while (key != 27) {
		captura.read(frame);

		frame.copyTo(frameWork);

		if (!cropping && !getRoi)
			imshow("Original", frame);
		else if (cropping && !getRoi) {
			cv::rectangle(frame, cv::Point(xmin, ymin), cv::Point(xmax, ymax),
					cv::Scalar(0, 255, 0), 2);
			imshow("Original", frame);
		} else if (!cropping && getRoi) {
			cv::rectangle(frame, cv::Point(xmin, ymin), cv::Point(xmax, ymax),
					cv::Scalar(0, 255, 0), 2);
			imshow("Original", frame);
		}

		cv::createTrackbar("HMIN", "Original", &hueRangeMin, 255, on_trackbar);
		cv::createTrackbar("HMAX", "Original", &hueRangeMax, 255, on_trackbar);
		cv::createTrackbar("SMIN", "Original", &satRangeMin, 255, on_trackbar);
		cv::createTrackbar("SMAX", "Original", &satRangeMax, 255, on_trackbar);
		cv::createTrackbar("VMIN", "Original", &valRangeMin, 255, on_trackbar);
		cv::createTrackbar("VMAX", "Original", &valRangeMax, 255, on_trackbar);
		setMouseCallback("Original", on_mouse, &frame);
		if (getRoi) {
			cv::Rect rect(xmin, ymin, xmax - xmin, ymax - ymin);
			cv::Mat roi = frameWork(rect);
			cv::Mat roiHSV;
			cv::cvtColor(roi, roiHSV, CV_BGR2HSV);
			std::vector<cv::Mat> channels;
			cv::split(roiHSV, channels);
			double minVal, maxVal;
			cv::Point minPos, maxPos;
			cv::minMaxLoc(channels[0], &minVal, &maxVal, &minPos, &maxPos);
			hueRangeMin = minVal;
			hueRangeMax = maxVal;
			cv::minMaxLoc(channels[1], &minVal, &maxVal, &minPos, &maxPos);
			satRangeMin = minVal;
			satRangeMax = maxVal;
			cv::minMaxLoc(channels[2], &minVal, &maxVal, &minPos, &maxPos);
			valRangeMin = minVal;
			valRangeMax = maxVal;
			cvSetTrackbarPos("HMIN", "Original", hueRangeMin);
			cvSetTrackbarPos("HMAX", "Original", hueRangeMax);
			cvSetTrackbarPos("SMIN", "Original", satRangeMin);
			cvSetTrackbarPos("SMAX", "Original", satRangeMax);
			cvSetTrackbarPos("VMIN", "Original", valRangeMin);
			cvSetTrackbarPos("VMAX", "Original", valRangeMax);
			cv::imshow("Roi", roi);
			cv::imshow("RoiHSV", roiHSV);
			getRoi = false;
		}
		cv::cvtColor(frameWork, frameHSV, CV_BGR2HSV);
		cv::inRange(frameHSV, cv::Scalar(hueRangeMin, satRangeMin, valRangeMin),
				cv::Scalar(hueRangeMax, satRangeMax, valRangeMax), maskRange);
		/*erode(imageSegmentada, maskRange,
		 getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		 dilate(maskRange, maskRange,
		 getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));*/
		cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE,
				cv::Size(1.5, 1.5));
		cv::morphologyEx(maskRange, mask, cv::MORPH_ERODE, kernel,
				cv::Point(-1, -1), 1);
		cv::morphologyEx(mask, mask, cv::MORPH_DILATE, kernel,
				cv::Point(-1, -1), 7);
		cv::Mat maskedImage;
		frameWork.copyTo(maskedImage, mask);
		// Compute the centorid mask
		std::vector<std::vector<cv::Point> > contours;
		std::vector<cv::Vec4i> hierarchy;
		cv::Mat canny_output;
		mask.copyTo(canny_output);
		cv::findContours(canny_output, contours, CV_RETR_TREE,
				CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
		int indexContourMax = 0;
		int maxPixels = 0;
		// vector<Point> contornoMaximo;
		// vector<Moments> mu(contours.size());
		for (unsigned int i = 0; i < contours.size(); i++) {
			if (contours[i].size() > maxPixels) {
				maxPixels = contours[i].size();
				indexContourMax = i;
			}
		}
		if (contours.size() > 0) {
			/*std::vector<cv::Point> contour_poly(contours[indexContourMax].size());
			 cv::approxPolyDP(cv::Mat(contours[indexContourMax]), contour_poly, 3,
			 true);
			 cv::boundingRect(contour_poly);
			 cv::rectangle(maskedImage, cv::boundingRect(contour_poly).tl(),
			 cv::boundingRect(contour_poly).br(), CV_RGB(124, 40, 30), 2,
			 8, 0);
			 cv::Moments centroide = moments(contours[indexContourMax], false);
			 cv::Point punto(centroide.m10 / centroide.m00,
			 centroide.m01 / centroide.m00);
			 cv::circle(maskedImage, punto, 4, CV_RGB(124, 40, 30), -1, 8, 0);*/
			std::vector<cv::Point> contour_poly;
			cv::approxPolyDP(cv::Mat(contours[indexContourMax]), contour_poly,
					3, true);
			cv::boundingRect(contour_poly);
			cv::rectangle(maskedImage, cv::boundingRect(contour_poly).tl(),
					cv::boundingRect(contour_poly).br(), CV_RGB(124, 40, 30), 2,
					8, 0);
			cv::Moments centroide = moments(contours[indexContourMax], false);
			cv::Point punto(centroide.m10 / centroide.m00,
					centroide.m01 / centroide.m00);
			cv::circle(maskedImage, punto, 4, CV_RGB(124, 40, 30), -1, 8, 0);
			std::cout << cv::Mat(punto) << std::endl;
		}

		imshow("Color mask", mask);
		cv::imshow("Image with mask", maskedImage);
		key = cv::waitKey(10);
	}

	frame.release();
	frameWork.release();
	frameHSV.release();
	mask.release();
	maskRange.release();
}

