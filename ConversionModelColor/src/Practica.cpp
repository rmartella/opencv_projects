#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

using namespace cv;
using namespace std;

void HSVtoRGB(float *r, float *g, float *b, float h, float s, float v);

int main(int argc, char** argv) {

	if (argc != 2) {
		cout << " Usage: display_image ImageToLoadAndDisplay" << endl;
		return -1;
	}

	IplImage* img = cvLoadImage(argv[1]);
	Mat origen = cvarrToMat(img);
	Mat targetHSV;
	Mat outRGB = Mat::zeros(img->height, img->width, CV_8UC3);
	cvtColor(origen, targetHSV, CV_RGB2HSV, 0);

	for (int x = 0; x < img->width; x++) {
		for (int y = 0; y < img->height; y++) {
			Vec3b vecColorHSV = targetHSV.at<Vec3b>(Point(x, y));
			Mat colorBGR = Mat(1, 1, CV_8UC3);
			Mat colorHSV = Mat(1, 1, CV_8UC3);
			colorHSV.at<Vec3b>(0, 0)[0] = vecColorHSV[0];
			colorHSV.at<Vec3b>(0, 0)[1] = vecColorHSV[1];
			colorHSV.at<Vec3b>(0, 0)[2] = vecColorHSV[2];

			float h = (float) vecColorHSV[0];
			float s = (float) vecColorHSV[1];
			float v = (float) vecColorHSV[2];
			float r, g, b;
			HSVtoRGB(&r, &g, &b, h, s, v);
			cvtColor(colorHSV, colorBGR, CV_HSV2BGR);

			std::cout << "r: " << r << ",rCV:"
					<< (float) colorBGR.at<Vec3b>(0, 0)[2];
			std::cout << "-g: " << g << ",gCV:"
					<< (float) colorBGR.at<Vec3b>(0, 0)[1];
			std::cout << "-b: " << b << ",bCV:"
					<< (float) colorBGR.at<Vec3b>(0, 0)[0] << std::endl;

			outRGB.at<Vec3b>(Point(x, y))[0] = colorBGR.at<Vec3b>(0, 0)[2];
			outRGB.at<Vec3b>(Point(x, y))[1] = colorBGR.at<Vec3b>(0, 0)[1];
			outRGB.at<Vec3b>(Point(x, y))[2] = colorBGR.at<Vec3b>(0, 0)[0];
		}
	}

	imshow("Original HSV", origen);
	imshow("Imagen HSV", targetHSV);
	imshow("Imagen RGB", outRGB);

	waitKey(0);
	cvReleaseImage(&img);
	origen.release();
	targetHSV.release();
	outRGB.release();

	return 0;

}

void HSVtoRGB(float *r, float *g, float *b, float h, float s, float v) {
	int i;
	float f, p, q, t;
	if (s == 0) {
		// achromatic (grey)
		*r = *g = *b = v;
		return;
	}
	h /= 60.0;			// sector 0 to 5
	i = (int) h;
	f = h - i;			// factorial part of h
	p = v * (255.0 - s);
	q = v * (255.0 - s * f);
	t = v * (255.0 - s * (255.0 - f));
	switch (i) {
	case 0:
		*r = v;
		*g = t;
		*b = p;
		std::cout << "Caso 0" << std::endl;
		break;
	case 1:
		*r = q;
		*g = v;
		*b = p;
		std::cout << "Caso 1" << std::endl;
		break;
	case 2:
		*r = p;
		*g = v;
		*b = t;
		std::cout << "Caso 2" << std::endl;
		break;
	case 3:
		*r = p;
		*g = q;
		*b = v;
		std::cout << "Caso 3" << std::endl;
		break;
	case 4:
		*r = t;
		*g = p;
		*b = v;
		std::cout << "Caso 4" << std::endl;
		break;
	default:		// case 5:
		*r = v;
		*g = p;
		*b = q;
		std::cout << "Caso 5" << std::endl;
		break;
	}
}
