#define NOMINMAX // to avoid an error compilation time on Windows (Eigen)
#define _CRT_SECURE_NO_DEPRECATE 1	// Un-deprecate CRT functions

#include <iostream>
#include <string>
#include "Ransac.hpp"

#include "Camera/CameraKinect.h"
#include "Camera/MotorKinect.h"

#include "Log.h"
#include "Util.h"
#include "boost/thread.hpp"
#include <math.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <opencv2/calib3d/calib3d.hpp>

using namespace std;
using namespace ModViPumas;
using namespace cv;
using namespace cv::xfeatures2d;
//using namespace Ransac;

#define PI 3.14159265

CvRect box;
bool drawing_box = false;
bool bandera = true;

void help(char *prg);

void draw_box(Mat img, CvRect rect) {
	/*cvRectangle(img, cvPoint(box.x, box.y),
	 cvPoint(box.x + box.width, box.y + box.height),
	 CV_RGB(124, 40, 30));*/
	rectangle(img, Point(box.x, box.y),
			Point(box.x + box.width, box.y + box.height), CV_RGB(124, 40, 30),
			2, 8, 0);
}

void mouse_callback(int event, int x, int y, int flags, void* param) {
	Mat * image = (Mat*) param;

	switch (event) {
	case CV_EVENT_MOUSEMOVE:
		if (drawing_box) {
			box.width = x - box.x;
			box.height = y - box.y;
		}
		break;

	case CV_EVENT_LBUTTONDOWN:
		drawing_box = true;
		box = cvRect(x, y, 0, 0);
		break;

	case CV_EVENT_LBUTTONUP:
		drawing_box = false;
		if (box.width < 0) {
			box.x += box.width;
			box.width *= -1;
		}
		if (box.height < 0) {
			box.y += box.height;
			box.height *= -1;
		}
		bandera = false;
		break;
	}
}

int main(int argc, char** argv) {
	bool isReady;
	Log log("visionModule.log");

	int fails = 0;
	int i;

	string basePath = Util::getCurrentPath();
	string calibrationFile = basePath + "calibration_rgb.yaml";
	string recording = "";
	int port = 2070;
	char * opcion = argv[1];
	int ejercicio = atoi(opcion);

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

	int key = 0;
	Mat image;
	switch (ejercicio) {
	case 1: {
		box = cvRect(-1, -1, 0, 0);
		do {
			imageskinect = camKinect->getFrame();
			image = cvarrToMat(imageskinect->imageRGB); //Mat(imageskinect->imageRGB, false);
			imshow("Kinect", image);
			setMouseCallback("Kinect", mouse_callback, &image);
			key = cvWaitKey(10);
			if (key != 27 && bandera) {
				image.release();
			}
		} while (key != 27 && bandera);
		//destroyAllWindows();
		draw_box(image, box);
		imshow("Kinect", image);
		Ransac * ransac = new Ransac();

		Plano * modelResult = (Plano*) malloc(1 * sizeof(Plano));
		modelResult->setConsenso(0);

		int wf = box.width;
		int hf = box.height;
		int xi = box.x;
		int yi = box.y;

		ransac->ransacAlgorithm(imageskinect->realWorld, xi, yi, wf, hf,
				image.cols, 70, 30.00, 500, modelResult, true);

		Plano * normalPlano = modelResult;

		cout << "normal::: X = " << (*normalPlano).getA() << ",Y = "
				<< (*normalPlano).getB() << ",Z = " << (*normalPlano).getC()
				<< endl;

		float modN;
		double cosX;
		double cosY;
		double cosZ;
		modN = sqrt(
				(*normalPlano).getA() * (*normalPlano).getA()
						+ (*normalPlano).getB() * (*normalPlano).getB()
						+ (*normalPlano).getC() * (*normalPlano).getC());
		cout << "modN = " << modN << endl;
		cosX = (*normalPlano).getA() / modN;
		cosY = (*normalPlano).getB() / modN;
		cosZ = (*normalPlano).getC() / modN;

		//ang = 0.0;

		double angX = acos(cosX);
		double angY = acos(cosY);
		double angZ = acos(cosZ);
		cout << "angX = " << angX << ",angY = " << angY << ",gama = " << angZ
				<< endl;
		//angX = angX;
		if((*normalPlano).getA() > 0 && angX < PI / 2){
			angY = angX - PI / 2;
		}
		else if((*normalPlano).getA() < 0 && angX > PI / 2){
			angY = PI / 2 - angX;
		}

		angX = angY - PI / 2;

		/*if (cosbeta < 0) {
		 beta = abs(beta) - PI / 2;
		 } else {
		 beta = PI / 2 - abs(beta);
		 }*/

		cout << "cosX = " << cosX << ",cosY = " << cosY << ",cosZ = " << cosZ
				<< endl;
		cout << "angX = " << angX << ",angY = " << angY << ",gama = " << angZ
				<< endl;

		int size = box.width * box.height;
		Mat mask = Mat::zeros(imageskinect->imageRGB->height,
				imageskinect->imageRGB->width, CV_8UC3);
		double xW = 0;
		double yW = 0;
		double zW = 0;
		int xP = 0;
		int yP = 0;
		std::vector<Point2f> obj;
		std::vector<Point2f> scene;
		XnPoint3D pointInit = imageskinect->get3DPoint(box.x, box.y);
		//angY = -PI/4;
		float xPointTrans = pointInit.X * cos(angY) + pointInit.Z * sin(angY);
		float zPointTrans = -pointInit.X * sin(angY) + pointInit.Z * cos(angY);
		for (int y = 0; y < box.height; y++) {
			for (int x = 0; x < box.width; x++) {
				XnPoint3D point = imageskinect->get3DPoint(box.x + x,
						box.y + y);
				if (point.X != 0 && point.Y != 0 && point.Z != 0) {
					cout << "PR" << (point.X * cos(angY) + point.Z * sin(angY)) << endl;
					cout << "xPointTrans = " << xPointTrans << endl;
					xW = point.X * cos(angY) + point.Z * sin(angY) - xPointTrans;
					//xW = point.X * cos(angY) + point.Z * sin(angY);
					yW = point.Y;
					//zW = -point.X * sin(angY) + point.Z * cos(angY) + zPointTrans;
					zW = -point.X * sin(angY) + point.Z * cos(angY);
					camKinect->retroProyect(xW, yW, zW, xP, yP);
					unsigned char * blue = &CV_IMAGE_ELEM(
							imageskinect->imageRGB, uchar, box.y + y,
							(box.x + x) * 3); // a z u l
					unsigned char * gren = &CV_IMAGE_ELEM(
							imageskinect->imageRGB, uchar, box.y + y,
							(box.x + x) * 3 + 1); // a z u l
					unsigned char * red = &CV_IMAGE_ELEM(imageskinect->imageRGB,
							uchar, box.y + y, (box.x + x) * 3 + 2); // a z u l
					mask.at<Vec3b>(Point(xP, yP))[0] =
							static_cast<int>(blue[0]);
					mask.at<Vec3b>(Point(xP, yP))[1] =
							static_cast<int>(gren[0]);
					mask.at<Vec3b>(Point(xP, yP))[2] = static_cast<int>(red[0]);
					obj.push_back(Point2f(box.x + x, box.y + y));
					scene.push_back(Point2f(xP, yP));
				}
			}
		}

		Mat homography = findHomography(obj, scene, CV_RANSAC);
		Mat dest = Mat::zeros(imageskinect->imageRGB->height,
				imageskinect->imageRGB->width, CV_8UC3);
		warpPerspective(image, dest, homography, dest.size(), INTER_LINEAR,
				BORDER_CONSTANT, 0);
		imshow("Kinect3", dest);

		imshow("Kinect2", mask);

		free(modelResult);
		cvWaitKey(0);
		image.release();
		mask.release();
		//cvNamedWindow("Kinect", CV_WINDOW_AUTOSIZE);
		//cvShowImage("Kinect", imageskinect->imageRGB);
		//cout << motKinect->Pose() << endl;

		/*Mat homography = findHomography(obj, obj, CV_RANSAC);
		 Mat dest = Mat::zeros(imageskinect->imageRGB->height,
		 imageskinect->imageRGB->width, CV_8UC3);
		 warpPerspective(image, dest, homography, dest.size(), INTER_LINEAR,
		 BORDER_CONSTANT, 0);
		 imshow("Kinect", dest);
		 dest.release();
		 image.release();*/

		//imshow("Kinect", image);
		//std::vector<Point2f> obj1;
		//std::vector<Point2f> scene;
		/*for (int x = box.x; x < box.width; x++) {
		 for (int y = box.y; y < box.height; y++) {
		 obj1.push_back(Point(x, y));
		 }
		 }*/

		destroyAllWindows();
		break;
	}
	case 2: {
		do {
			imageskinect = camKinect->getFrame();
			image = cv::cvarrToMat(imageskinect->imageRGB); //Mat(imageskinect->imageRGB, false);
			imshow("Kinect", image);
			key = cvWaitKey(10);
			if (key != 27) {
				image.release();
			}
		} while (key != 27);

		Mat img_object;
		cvtColor(image, img_object, CV_BGR2GRAY);
		image.release();
		destroyAllWindows();
		do {
			imageskinect = camKinect->getFrame();
			Mat image = cv::cvarrToMat(imageskinect->imageRGB); //Mat(imageskinect->imageRGB, false);
			Mat img_scene;
			cvtColor(image, img_scene, CV_BGR2GRAY);
			//-- Step 1: Detect the keypoints using SURF Detector
			int minHessian = 400;
			//SurfFeatureDetector detector(minHessian);
			Ptr<SURF> detector = SURF::create( minHessian );
			std::vector<KeyPoint> keypoints_object, keypoints_scene;
			detector->detect(img_object, keypoints_object);
			detector->detect(img_scene, keypoints_scene);
			//-- Step 2: Calculate descriptors (feature vectors)
			//SurfDescriptorExtractor extractor;
			Ptr<SURF> extractor = SURF::create();
			Mat descriptors_object, descriptors_scene;
			extractor->compute(img_object, keypoints_object, descriptors_object);
			extractor->compute(img_scene, keypoints_scene, descriptors_scene);
			//-- Step 3: Matching descriptor vectors using FLANN matcher
			FlannBasedMatcher matcher;
			std::vector<DMatch> matches;
			matcher.match(descriptors_object, descriptors_scene, matches);
			double max_dist = 0;
			double min_dist = 2;
			//-- Quick calculation of max and min distances between keypoints
			for (int i = 0; i < descriptors_object.rows; i++) {
				double dist = matches[i].distance;
				if (dist < min_dist)
					min_dist = dist;
				if (dist > max_dist)
					max_dist = dist;
			}
			printf("-- Max dist : %f \n", max_dist);
			printf("-- Min dist : %f \n", min_dist);
			//-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
			std::vector<DMatch> good_matches;
			for (int i = 0; i < descriptors_object.rows; i++) {
				if (matches[i].distance < 3 * min_dist) {
					good_matches.push_back(matches[i]);
				}
			}
			Mat img_matches;
			drawMatches(img_object, keypoints_object, img_scene,
					keypoints_scene, good_matches, img_matches, Scalar::all(-1),
					Scalar::all(-1), vector<char>(),
					DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
			//-- Localize the object
			std::vector<Point2f> obj;
			std::vector<Point2f> scene;
			for (int i = 0; i < good_matches.size(); i++) {
				//-- Get the keypoints from the good matches
				obj.push_back(keypoints_object[good_matches[i].queryIdx].pt);
				scene.push_back(keypoints_scene[good_matches[i].trainIdx].pt);
			}
			if (obj.size() > 4 && scene.size() > 4) {

				Mat H = findHomography(obj, scene, CV_RANSAC);
				//-- Get the corners from the image_1 ( the object to be "detected" )
				std::vector<Point2f> obj_corners(4);
				obj_corners[0] = cvPoint(0, 0);
				obj_corners[1] = cvPoint(img_object.cols, 0);
				obj_corners[2] = cvPoint(img_object.cols, img_object.rows);
				obj_corners[3] = cvPoint(0, img_object.rows);
				std::vector<Point2f> scene_corners(4);
				perspectiveTransform(obj_corners, scene_corners, H);
				//-- Draw lines between the corners (the mapped object in the scene - image_2 )
				line(img_matches,
						scene_corners[0] + Point2f(img_object.cols, 0),
						scene_corners[1] + Point2f(img_object.cols, 0),
						Scalar(0, 255, 0), 4);
				line(img_matches,
						scene_corners[1] + Point2f(img_object.cols, 0),
						scene_corners[2] + Point2f(img_object.cols, 0),
						Scalar(0, 255, 0), 4);
				line(img_matches,
						scene_corners[2] + Point2f(img_object.cols, 0),
						scene_corners[3] + Point2f(img_object.cols, 0),
						Scalar(0, 255, 0), 4);
				line(img_matches,
						scene_corners[3] + Point2f(img_object.cols, 0),
						scene_corners[0] + Point2f(img_object.cols, 0),
						Scalar(0, 255, 0), 4);
				//-- Show detected matches
				imshow("Good Matches & Object detection", img_matches);
			}

			key = cvWaitKey(10);

			image.release();
			img_scene.release();

		} while (key != 27);
		break;
	}
	}

	return EXIT_SUCCESS;
}
