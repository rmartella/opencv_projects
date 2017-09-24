#define NOMINMAX // to avoid an error compilation time on Windows (Eigen)
#define _CRT_SECURE_NO_DEPRECATE 1	// Un-deprecate CRT functions

#include <iostream>
#include <string.h>
#include "Ransac.hpp"

#include "Camera/CameraKinect.h"
#include "Camera/MotorKinect.h"

#include "Log.h"
#include "Util.h"
#include "boost/thread.hpp"

using namespace std;
using namespace ModViPumas;
using namespace ModViPumas;
//using namespace Ransac;

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

	Ransac * ransac = new Ransac();

	/*float x[121] = { -9.00636, -7.1759, -5.44797, -3.69565, -1.08023, 0.826645,
	 2.638,

	 5.27596, 7.23666, 9.10706, 11.7676, -9.00636, -7.1759, -5.44797,
	 -3.69565, -1.08023,

	 0.826645, 2.638, 5.27596, 7.23666, 9.10706, 11.0018, -9.00636,
	 -7.1759, -5.44797, -

	 3.69565, -1.08023, 0.826645, 2.638, 5.27596, 7.23666, 9.10706,
	 11.0018, -9.00636, -

	 7.1759, -5.44797, -2.84468, -1.08023, 0.826645, 3.4525, 5.27596,
	 7.23666, 9.10706,

	 11.0018, -9.00636, -7.1759, -5.44797, -3.69565, -1.79921, 0, 2.638,
	 4.58824, 6.44649,

	 8.32904, 11.0018, -9.00636, -6.42566, -5.44797, -3.69565, -1.79921,
	 0, 1.93987,

	 3.7859, 6.44649, 8.32904, 11.0018, -9.76701, -7.1759, -5.44797,
	 -3.69565, -1.79921, -

	 0.708572, 1.93987, 3.7859, 6.44649, 8.32904, 11.0018, -9.76701,
	 -7.1759, -5.44797, -

	 3.69565, -1.79921, -0.708572, 1.93987, 3.7859, 5.6563, 8.32904,
	 10.2359, -9.76701, -

	 7.1759, -5.44797, -4.42505, -2.63802, -0.708572, 1.12537, 3.7859,
	 5.6563, 8.32904,

	 11.0018, -9.76701, -8.05118, -6.31108, -4.42505, -2.63802,
	 -0.708572, 1.12537, 3.7859

	 , 5.6563, 8.32904, 11.0018, -9.76701, -8.05118, -6.31108, -4.42505,
	 -2.63802, -

	 0.708572, 1.12537, 3.7859, 5.6563, 8.32904, 10.2359 };

	 float y[121] = { 12.6169, 12.7211, 12.8427, 12.9642, 12.9642, 13.0684,
	 13.19, 13.19,

	 13.2942, 13.4158, 13.4158, 10.0362, 10.13, 10.2394, 10.3488,
	 10.3488, 10.4426, 10.552

	 , 10.552, 10.6458, 10.7552, 10.8646, 7.45554, 7.5389, 7.63615,
	 7.73341, 7.73341,

	 7.81677, 7.91402, 7.91402, 7.99738, 8.09463, 8.19189, 4.87486,
	 4.9478, 5.03289,

	 5.03289, 5.11799, 5.19093, 5.19093, 5.27603, 5.34897, 5.43406,
	 5.51916, 2.29417,

	 2.35669, 2.42963, 2.50257, 2.56509, 2.63803, 2.63803, 2.70055,
	 2.77349, 2.84643,

	 2.84643, -0.286549, -0.286549, -0.173666, -0.112883, -0.0607834, 0,
	 0.0521002,

	 0.112883, 0.112883, 0.173666, 0.173666, -2.82555, -2.82555,
	 -2.77693, -2.7283, -

	 2.68662, -2.59631, -2.59631, -2.54769, -2.54769, -2.49906, -2.49906,
	 -5.41666, -

	 5.41666, -5.38019, -5.34372, -5.31246, -5.24473, -5.24473, -5.20826,
	 -5.17179, -

	 5.17179, -5.13532, -8.00776, -8.00776, -7.98345, -7.9383, -7.91398,
	 -7.89314, -

	 7.86883, -7.86883, -7.84452, -7.84452, -7.84452, -10.5989, -10.5867,
	 -10.5746, -

	 10.5641, -10.552, -10.5416, -10.5294, -10.5294, -10.5172, -10.5172,
	 -10.5172, -

	 13.1899, -13.1899, -13.1899, -13.1899, -13.1899, -13.1899, -13.1899,
	 -13.1899, -

	 13.1899, -13.1899, -13.1899 };

	 float z[121] = { -33, -27, -20, -13, -13, -7, 0, 0, 6, 13, 13, -33, -27,
	 -20, -13, -13, -

	 7, 0, 0, 6, 13, 20, -33, -27, -20, -13, -13, -7, 0, 0, 6, 13, 20,
	 -33, -27, -20, -20,

	 -13, -7, -7, 0, 6, 13, 20, -33, -27, -20, -13, -7, 0, 0, 6, 13, 20,
	 20, -33, -33, -

	 20, -13, -7, 0, 6, 13, 13, 20, 20, -27, -27, -20, -13, -7, 6, 6, 13,
	 13, 20, 20, -27,

	 -27, -20, -13, -7, 6, 6, 13, 20, 20, 27, -27, -27, -20, -7, 0, 6,
	 13, 13, 20, 20, 20

	 , -27, -20, -13, -7, 0, 6, 13, 13, 20, 20, 20, -27, -20, -13, -7, 0,
	 6, 13, 13, 20,

	 20, 27 };

	 XnPoint3D * puntosPrueba = (XnPoint3D *)malloc(121*sizeof(XnPoint3D));

	 for (int i = 0; i < 121; i++) {
	 puntosPrueba[i].X = x[i];
	 puntosPrueba[i].Y = y[i];
	 puntosPrueba[i].Z = z[i];
	 }

	 Plano * planoPrueba = ransac->regresion(puntosPrueba,121);*/

	int key = 0;

	Mat elementErosion = getStructuringElement(2, Size(2 * 4 + 1, 2 * 4 + 1),
			Point(2, 2));
	Mat elementDilatacion = getStructuringElement(2, Size(2 * 8 + 1, 2 * 8 + 1),
			Point(2, 2));
	do {
		imageskinect = camKinect->getFrame();

		//camKinect->images->realWorld;
		//ransac->ransacAlgorithm();

		//namedWindow("Kinect", 1);
		//IplImage* img = cvLoadImage("images/coche1.jpg");

		Plano * modelResult = (Plano*) malloc(1 * sizeof(Plano));
		modelResult->setConsenso(0);

		int wf = imageskinect->imageRGB->width
				- imageskinect->imageRGB->width / 2;
		int hf = imageskinect->imageRGB->height
				- imageskinect->imageRGB->height / 2;
		int xi = (imageskinect->imageRGB->width - wf) / 2;
		int yi = (imageskinect->imageRGB->height - wf) / 2;

		ransac->ransacAlgorithm(imageskinect->realWorld, xi, yi, wf, hf, 70,
				15.00, 10000, modelResult, true);

		//cout << "maxConsenso:" << (*modelResult).getConsenso() << endl;

		Mat mask = Mat::zeros(imageskinect->imageRGB->height,
				imageskinect->imageRGB->width, CV_8UC3);
		Mat imageSegmentada = Mat::zeros(imageskinect->imageRGB->height,
				imageskinect->imageRGB->width, CV_8UC3);
		Mat imagenOriginal = Mat(cv::cvarrToMat(imageskinect->imageRGB));
		//Mat imagenOriginal = Mat(&imageskinect->imageRGB);
		//GaussianBlur(mask, mask, Size(5, 5), 0, 0);
		if ((*modelResult).getConsenso() > 0) {
			for (int y = 0; y < imageskinect->imageRGB->height; y++) {
				for (int x = 0; x < imageskinect->imageRGB->width; x++) {
					XnPoint3D get3DPoint = imageskinect->get3DPoint(x, y);
					if (get3DPoint.Z > 0) {
						float distancia = (*modelResult).distanciaPunto(
								&get3DPoint);
						if (distancia < 50.00) {
							mask.at<Vec3b>(Point(x, y))[0] = 255;
							mask.at<Vec3b>(Point(x, y))[1] = 255;
							mask.at<Vec3b>(Point(x, y))[2] = 255;
						}
					} else {
						mask.at<Vec3b>(Point(x, y))[0] = 0;
						mask.at<Vec3b>(Point(x, y))[1] = 0;
						mask.at<Vec3b>(Point(x, y))[2] = 0;
					}
				}
			}
		}
		/*erode(mask, mask, elementErosion, Point(-1, -1), 1);
		 dilate(mask, mask, elementDilatacion, Point(-1, -1), 1);*/
		bitwise_not(mask, mask);
		imagenOriginal.copyTo(imageSegmentada, mask);

		// rgbDepth = imageskinect->getImageBetween(0, 40000);

		//cvNamedWindow("Kinect", CV_WINDOW_AUTOSIZE);
		//cvShowImage("Kinect", imageskinect->imageRGB);
		imshow("Kinect", imageSegmentada);
		free(modelResult);
		//cout << motKinect->Pose() << endl;
		mask.release();
		imageSegmentada.release();
		imagenOriginal.release();
		//motKinect->Move( 0 );

		key = cvWaitKey(10);

		//cvReleaseImage(&tmp);

	} while (key != 27);

	return EXIT_SUCCESS;
}
