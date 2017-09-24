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

using namespace cv;
using namespace std;

int hueRangeMin = 255;
int satRangeMin = 255;
int valRangeMin = 255;
int hueRangeMax = 0;
int satRangeMax = 0;
int valRangeMax = 0;

Mat imageDistorcionada;
Mat imageCalibrada;
Mat imageHSV;
Mat imageSegmentada;
Mat mask;
Mat maskInverse;
Mat maskRange;
Mat imageGray;
Mat imageThresholdSimple;
Mat imageThresholdAdaptative;
Mat elementDilatacion;
Mat elementErosion;

int umbral = 100;
int bloques = 45;
int offset = 2;
int typeThreshold = CV_THRESH_BINARY;
int typeAdaptativeThreshold = CV_THRESH_BINARY;
Vec3b hsvColor = Vec3b(0, 0, 0);

int erosion_elem = 0;
int erosion_size = 0;
int dilation_elem = 0;
int dilation_size = 0;
int const max_elem = 2;
int const max_kernel_size = 21;

int xmin, ymin, xmax, ymax;
bool pressed = false;
bool regionOfInters = false;

bool to_bool(string str) {
	transform(str.begin(), str.end(), str.begin(), ::tolower);
	istringstream is(str);
	bool b;
	is >> boolalpha >> b;
	return b;
}

void on_trackbar(int, void*) {
	/*if (bloques <= 1) {
	 bloques = 3;
	 } else {
	 if (bloques % 2 != 1) {
	 bloques += 1;
	 }
	 }*/
}
/**  @function Erosion  */
void Erosion(int, void*) {
	int erosion_type;
	if (erosion_elem == 0) {
		erosion_type = MORPH_RECT;
	} else if (erosion_elem == 1) {
		erosion_type = MORPH_CROSS;
	} else if (erosion_elem == 2) {
		erosion_type = MORPH_ELLIPSE;
	}

	elementErosion = getStructuringElement(erosion_type,
			Size(2 * erosion_size + 1, 2 * erosion_size + 1),
			Point(erosion_size, erosion_size));
}

/** @function Dilation */
void Dilation(int, void*) {
	int dilation_type;
	if (dilation_elem == 0) {
		dilation_type = MORPH_RECT;
	} else if (dilation_elem == 1) {
		dilation_type = MORPH_CROSS;
	} else if (dilation_elem == 2) {
		dilation_type = MORPH_ELLIPSE;
	}

	elementDilatacion = getStructuringElement(dilation_type,
			Size(2 * dilation_size + 1, 2 * dilation_size + 1),
			Point(dilation_size, dilation_size));
}
void on_mouse(int event, int x, int y, int flags, void* param) {
	if (event == CV_EVENT_FLAG_LBUTTON && !pressed) {
		pressed = true;
		regionOfInters = false;
		/*Mat * img = (Mat*) param;
		 Vec3b colorBGR = img->at<Vec3b>(Point(x, y));
		 Mat colorRGB = Mat(1, 1, CV_8UC3);
		 Mat colorHSV = Mat(1, 1, CV_8UC3);
		 int red = colorBGR[2];
		 int green = colorBGR[1];
		 int blue = colorBGR[0];

		 cout << "Las componentes del color \n" << "Rojo:" << dec
		 << static_cast<int>(red) << "\nVerde:" << dec
		 << static_cast<int>(green) << "\nAzul:" << dec
		 << static_cast<int>(blue) << endl;

		 colorRGB.at<Vec3b>(0, 0)[0] = red;
		 colorRGB.at<Vec3b>(0, 0)[1] = green;
		 colorRGB.at<Vec3b>(0, 0)[2] = blue;

		 cvtColor(colorRGB, colorHSV, CV_RGB2HSV);
		 int h = colorHSV.at<Vec3b>(0, 0)[0];
		 int s = colorHSV.at<Vec3b>(0, 0)[1];
		 int v = colorHSV.at<Vec3b>(0, 0)[2];
		 hsvColor = Vec3b(h, s, v);*/
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
	}
}

void calculaCentroide(Mat mask, Mat imagenSegmentada) {
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	Mat canny_output;
	mask.copyTo(canny_output);
	findContours(canny_output, contours, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE,
			Point(0, 0));
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
		vector<Point> contour_poly(contours[indiceContornoMaximo].size());
		approxPolyDP(Mat(contours[indiceContornoMaximo]), contour_poly, 3,
				true);
		boundingRect(contour_poly);
		rectangle(imagenSegmentada, boundingRect(contour_poly).tl(),
				boundingRect(contour_poly).br(), CV_RGB(124, 40, 30), 2, 8, 0);
		Moments centroide = moments(contours[indiceContornoMaximo], false);
		Point punto(centroide.m10 / centroide.m00,
				centroide.m01 / centroide.m00);
		circle(imagenSegmentada, punto, 4, CV_RGB(124, 40, 30), -1, 8, 0);
		cout << Mat(punto) << endl;
	}

}

int main(int argc, char** argv) {

	/*Variables de calibración*/
	cv::VideoCapture captura = cv::VideoCapture();
	captura.open(0);

	int key;

	while (key != 27) {
		captura.read(imageDistorcionada);
		imshow("Original", imageDistorcionada);
		createTrackbar("HMIN", "Original", &hueRangeMin, 255, on_trackbar);
		createTrackbar("HMAX", "Original", &hueRangeMax, 255, on_trackbar);
		createTrackbar("SMIN", "Original", &satRangeMin, 255, on_trackbar);
		createTrackbar("SMAX", "Original", &satRangeMax, 255, on_trackbar);
		createTrackbar("VMIN", "Original", &valRangeMin, 255, on_trackbar);
		createTrackbar("VMAX", "Original", &valRangeMax, 255, on_trackbar);
		setMouseCallback("Original", on_mouse, &imageCalibrada);
		if (regionOfInters) {
			Rect rect(xmin, ymin, xmax - xmin, ymax - ymin);
			Mat roi = imageDistorcionada(rect);
			Mat roiHSV;
			cvtColor(roi, roiHSV, CV_BGR2HSV);
			for (int x = 0; x < roiHSV.cols; x++)
				for (int y = 0; y < roiHSV.rows; y++) {
					Vec3b hsv = roiHSV.at<Vec3b>(x, y);
					int h = hsv[0];
					int s = hsv[1];
					int v = hsv[2];
					if (h < hueRangeMin)
						hueRangeMin = h;
					if (h > hueRangeMax)
						hueRangeMax = h;
				}
			std::cout << "hueRangeMin:" << hueRangeMin << std::endl;
			std::cout << "hueRangeMax:" << hueRangeMax << std::endl;
			std::cout << "satRangeMin:" << satRangeMin << std::endl;
			std::cout << "satRangeMax:" << satRangeMax << std::endl;
			std::cout << "valRangeMin:" << valRangeMin << std::endl;
			std::cout << "valRangeMax:" << valRangeMax << std::endl;
			imshow("Roi", roi);
			imshow("RoiHSV", roiHSV);
			regionOfInters = false;
		}
		cvtColor(imageDistorcionada, imageHSV, CV_BGR2HSV);
		inRange(imageHSV, Scalar(hueRangeMin, satRangeMin, valRangeMin),
				Scalar(hueRangeMax, satRangeMax, valRangeMax), imageSegmentada);
		erode(imageSegmentada, maskRange,
				getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		dilate(maskRange, maskRange,
				getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		/*imshow("HSV", imageHSV);
		imshow("Segmentacion", imageSegmentada);*/
		imshow("Color mask", maskRange);
		Mat maskedImage;
		imageDistorcionada.copyTo(maskedImage, maskRange);
		imshow("Image with mask", maskedImage);
		key = waitKey(10);
	}
	/*
	 // Primera lectura de la camara
	 captura.read(imageDistorcionada);
	 Size size = imageDistorcionada.size();
	 Mat map1 = Mat(imageDistorcionada.rows, imageDistorcionada.cols, CV_8UC3);
	 Mat map2 = Mat(imageDistorcionada.rows, imageDistorcionada.cols, CV_8UC3);
	 if (banderaCalibracion) {
	 initUndistortRectifyMap(intrinsic, distortion, Mat(), intrinsic, size,
	 CV_32FC1, map1, map2);
	 }
	 do {
	 if (firstFrame) {
	 namedWindow("Original", 1);
	 imageCalibrada = Mat(imageDistorcionada.rows,
	 imageDistorcionada.cols,
	 CV_8UC3);
	 }
	 if (banderaCalibracion) {
	 remap(imageDistorcionada, imageCalibrada, map1, map2, INTER_LINEAR);
	 } else {
	 imageDistorcionada.copyTo(imageCalibrada);
	 }
	 imshow("Original", imageCalibrada);
	 switch (opcion) {
	 case (1): {
	 if (firstFrame) {
	 createTrackbar("Umbral", "Original", &umbral, 255, on_trackbar);
	 createTrackbar("Bloques", "Original", &bloques, 255,
	 on_trackbar);
	 createTrackbar("OffSet", "Original", &offset, 255, on_trackbar);
	 createTrackbar("Type", "Original", &typeThreshold, 1,
	 on_trackbar);
	 createTrackbar("TypeAdaptative", "Original",
	 &typeAdaptativeThreshold, 1, on_trackbar);
	 imageGray = Mat(imageDistorcionada.rows,
	 imageDistorcionada.cols,
	 CV_8UC3);
	 imageThresholdSimple = Mat(imageDistorcionada.rows,
	 imageDistorcionada.cols,
	 CV_8UC3);
	 imageThresholdAdaptative = Mat(imageDistorcionada.rows,
	 imageDistorcionada.cols,
	 CV_8UC3);
	 }
	 cvtColor(imageCalibrada, imageGray, CV_BGR2GRAY);
	 threshold(imageGray, imageThresholdSimple, umbral, 255,
	 typeThreshold);
	 adaptiveThreshold(imageGray, imageThresholdAdaptative, 255,
	 typeAdaptativeThreshold, typeThreshold, bloques, offset);
	 imshow("ThresholdSimple", imageThresholdSimple);
	 imshow("ThresholdAdaptative", imageThresholdAdaptative);
	 if (banderaCalibracion) {
	 //captura.release();
	 //captura.open("http://192.168.1.89:8080/shot.jpg");
	 captura.read(imageDistorcionada);
	 }
	 break;
	 }
	 case (2):
	 case (3): {
	 if (firstFrame) {
	 createTrackbar("H", "Original", &hueRange, 255, on_trackbar);
	 createTrackbar("S", "Original", &satRange, 255, on_trackbar);
	 createTrackbar("V", "Original", &valRange, 255, on_trackbar);
	 createTrackbar("Erosion Element; 0: Rect,1: Cross, 2: Ellipse",
	 "Original", &erosion_elem, max_elem, Erosion);
	 createTrackbar("Erosion Kernel size: 2n +1", "Original",
	 &erosion_size, max_kernel_size, Erosion);
	 /// Create Dilation Trackbar
	 createTrackbar("Dilate Element; 0: Rect,1: Cross, 2: Ellipse",
	 "Original", &dilation_elem, max_elem, Dilation);
	 createTrackbar("Dilate Kernel size: 2n +1", "Original",
	 &dilation_size, max_kernel_size, Dilation);

	 mask = Mat(imageDistorcionada.rows, imageDistorcionada.cols,
	 CV_8UC3);
	 maskRange = Mat(imageDistorcionada.rows,
	 imageDistorcionada.cols,
	 CV_8UC3);
	 maskInverse = Mat(imageDistorcionada.rows,
	 imageDistorcionada.cols,
	 CV_8UC3);
	 }
	 imageSegmentada = Mat::zeros(imageDistorcionada.rows,
	 imageDistorcionada.cols, CV_8UC3);
	 int hueLow = hsvColor[0] - hueRange;
	 int satLow = hsvColor[1] - satRange;
	 int valLow = hsvColor[2] - valRange;
	 int hueUp = hsvColor[0] + hueRange;
	 int satUp = hsvColor[1] + satRange;
	 int valUp = hsvColor[2] + valRange;
	 const Scalar lower = Scalar(hueLow, satLow, valLow);
	 const Scalar upper = Scalar(hueUp, satUp, valUp);
	 setMouseCallback("Original", on_mouse, &imageCalibrada);
	 cvtColor(imageCalibrada, imageHSV, CV_BGR2HSV);
	 GaussianBlur(imageHSV, maskRange, Size(5, 5), 0, 0);
	 erode(maskRange, maskRange, elementErosion, Point(-1, -1), 1);
	 dilate(maskRange, maskRange, elementDilatacion, Point(-1, -1), 1);
	 inRange(maskRange, lower, upper, mask);
	 bitwise_not(mask, maskInverse);
	 imageCalibrada.copyTo(imageSegmentada, maskInverse);
	 calculaCentroide(mask, imageSegmentada);
	 imshow("imageSegmentada", imageSegmentada);
	 imageSegmentada.release();
	 if (banderaCalibracion) {
	 //captura.release();
	 //captura.open("http://192.168.1.89:8080/shot.jpg");
	 captura.read(imageDistorcionada);
	 }
	 break;
	 }
	 }
	 firstFrame = false;

	 } while (imageDistorcionada.isContinuous());*/

	imageCalibrada.release();
	imageDistorcionada.release();
	imageHSV.release();
	imageSegmentada.release();
	imageThresholdSimple.release();
	imageThresholdAdaptative.release();
	mask.release();
	maskInverse.release();
	maskRange.release();
	captura.release();

}

