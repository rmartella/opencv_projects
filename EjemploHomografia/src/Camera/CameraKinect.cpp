#include "CameraKinect.h"
using namespace ModViPumas;
CameraKinect::CameraKinect() {
	images = 0;
	kUndistorter = 0;
	opened = false;
	connected = false;
	pointList = 0;

	numBadFrames = 0;
	errorCamera = true;

	fileConfOpenNI = 0;
	fileCalibKinect = 0;

	this->allocateMem();
}

CameraKinect::~CameraKinect() {
	delete kUndistorter;
	delete images;
}

bool CameraKinect::getOpened() {
	return this->opened;
}

bool CameraKinect::getErrorCamera() {
	return !this->errorCamera;
}

xn::DepthGenerator CameraKinect::getG_depth() const {
	return g_depth;
}

bool CameraKinect::openCamera(char* fileConf, char* fileCalib) {
	this->fileConfOpenNI = fileConf;
	this->fileCalibKinect = fileCalib;

	return openCamera();
}

bool CameraKinect::openCamera() {
	CvFileStorage* fs;
	CvMat* intrRGB;
	CvMat* disRGB;
	bool operationComplete;

	boost::mutex::scoped_lock scoped_lock(mutexOpen);

	operationComplete = false;

	if (this->opened) {
		return true;
	} else {

		//Xml
		rc = g_context.InitFromXmlFile(fileConfOpenNI, &errors);

		if (rc != XN_STATUS_OK) {
			clog << "Error: [CameraKinect.h] Open failed: "
					<< xnGetStatusString(rc) << endl;
			if (rc == XN_STATUS_NO_NODE_PRESENT) {
				XnChar strError[1024];
				errors.ToString(strError, 1024);
				this->connected = false;
				Util::writeOnImage(images->imageRGB, "No Kinect Connected");
				clog << "Error: [CameraKinect.h] " << strError << endl;
			}
			this->opened = false;
			operationComplete = false;
		} else {

			this->connected = true;

			//XnStatus
			rc = g_context.FindExistingNode(XN_NODE_TYPE_DEPTH, g_depth);
			rc = g_context.FindExistingNode(XN_NODE_TYPE_IMAGE, g_image);

			g_depth.GetMetaData(g_depthMD);
			g_image.GetMetaData(g_imageMD);

			fs = cvOpenFileStorage(fileCalibKinect, NULL, CV_STORAGE_READ);

			if (!fs) {
				clog
						<< "Error: [CameraKinect.h] Cannot read configuration camera file"
						<< endl;
				operationComplete = false;
			}

			//RGB Camera calibration parameters
			intrRGB = (CvMat *) cvReadByName(fs, NULL, "camera_matrix", NULL);
			disRGB = (CvMat *) cvReadByName(fs, NULL, "distortion_coefficients",
					NULL);

			kUndistorter = new Undistorter(g_imageMD.XRes(), g_imageMD.YRes(),
					intrRGB, disRGB);

			cvReleaseFileStorage(&fs);
			g_depth.GetAlternativeViewPointCap().SetViewPoint(g_image);

			clog << "[CameraKinect.h] Camera Kinect opened, max depth: "
					<< g_depth.GetDeviceMaxDepth() << endl;

			operationComplete = true;
		}

	}

	this->opened = operationComplete;
	this->errorCamera = !operationComplete;
	return operationComplete;
}

bool CameraKinect::openRecording(char* fileConf, char* fileCalib,
		char *fileRecording) {
	this->fileConfOpenNI = fileConf;
	this->fileCalibKinect = fileCalib;
	this->fileRecordOpenNI = fileRecording;

	return openRecording();
}

bool CameraKinect::openRecording() {
	CvFileStorage* fs;
	CvMat* intrRGB;
	CvMat* disRGB;
	bool operationComplete;

	boost::mutex::scoped_lock scoped_lock(mutexOpen);

	operationComplete = false;

	if (this->opened) {
		return true;
	} else {

		//Xml
		XnStatus nRetVal = XN_STATUS_OK;
		nRetVal = g_context.Init();

		if (nRetVal != XN_STATUS_OK) {
			printf("%s failed: %s\n", "Init", xnGetStatusString(nRetVal));
			return nRetVal;
		}
		Player g_Player;
		rc = g_context.OpenFileRecording(fileRecordOpenNI, g_Player);

		if (rc != XN_STATUS_OK) {
			clog << "Error: [CameraKinect.h] Open failed: "
					<< xnGetStatusString(rc) << endl;
			if (rc == XN_STATUS_NO_NODE_PRESENT) {
				XnChar strError[1024];
				errors.ToString(strError, 1024);
				this->connected = false;
				Util::writeOnImage(images->imageRGB, "No Kinect Connected");
				clog << "Error: [CameraKinect.h] " << strError << endl;
			}
			this->opened = false;
			operationComplete = false;
		} else {

			this->connected = true;

			//XnStatus
			rc = g_context.FindExistingNode(XN_NODE_TYPE_DEPTH, g_depth);
			rc = g_context.FindExistingNode(XN_NODE_TYPE_IMAGE, g_image);

			g_depth.GetMetaData(g_depthMD);
			g_image.GetMetaData(g_imageMD);

			fs = cvOpenFileStorage(fileCalibKinect, NULL, CV_STORAGE_READ);

			if (!fs) {
				clog
						<< "Error: [CameraKinect.h] Cannot read configuration camera file"
						<< endl;
				operationComplete = false;
			}

			//RGB Camera calibration parameters
			intrRGB = (CvMat *) cvReadByName(fs, NULL, "camera_matrix", NULL);
			disRGB = (CvMat *) cvReadByName(fs, NULL, "distortion_coefficients",
					NULL);

			kUndistorter = new Undistorter(g_imageMD.XRes(), g_imageMD.YRes(),
					intrRGB, disRGB);

			cvReleaseFileStorage(&fs);
			g_depth.GetAlternativeViewPointCap().SetViewPoint(g_image);

			clog << "[CameraKinect.h] Camera Kinect opened, max depth: "
					<< g_depth.GetDeviceMaxDepth() << endl;

			operationComplete = true;
		}
	}

	this->opened = operationComplete;
	this->errorCamera = !operationComplete;
	return operationComplete;
}

void CameraKinect::retroProyect(double xW, double yW, double zW, int &xi,
		int &yi) {
	XnPoint3D* xnPointsMetric;
	XnPoint3D* xnPointsPixels;

	xnPointsMetric = (XnPoint3D*) malloc(sizeof(XnPoint3D));

	xnPointsMetric[0].X = xW;
	xnPointsMetric[0].Y = yW;
	xnPointsMetric[0].Z = zW;

	xnPointsPixels = (XnPoint3D*) malloc(sizeof(XnPoint3D));

	g_depth.ConvertRealWorldToProjective(1, xnPointsMetric, xnPointsPixels);

	xi = xnPointsPixels[0].X;
	yi = xnPointsPixels[0].Y;

	free(xnPointsMetric);
	free(xnPointsPixels);
}

CameraKinectImages* CameraKinect::getFrame(bool newImages /*= false */) {
	XnStatus status;
	CameraKinectImages* imgKnct;

	boost::mutex::scoped_lock scoped_lock(mutexGetFrame);

	captureFrame();
	if (!connected) {
		openCamera();
	}
	if (connected) {
		if (rc == XN_STATUS_WAIT_DATA_TIMEOUT) {
			Util::writeOnImage(images->imageRGB, "No new data acquired");
			numBadFrames++;
			if (numBadFrames > MAX_BAD_FRAMES)
				errorCamera = true;
		} else {
			numBadFrames = 0;
			for (int y = 0; y < XN_VGA_Y_RES; y++) {
				for (int x = 0; x < XN_VGA_X_RES; x++) {
					pointList[y * XN_VGA_X_RES + x].Z = g_depthMD(x, y);
				}
			}

			status = g_depth.ConvertProjectiveToRealWorld(
					XN_VGA_Y_RES * XN_VGA_X_RES, pointList, images->realWorld);
			if (status == XN_STATUS_INVALID_OPERATION) {
				cout << "Invalid operation" << endl;
			}

			pImageRow = g_imageMD.RGB24Data();
			memcpy(images->imageRGB->imageData, pImageRow,
					640 * 480 * sizeof(unsigned char) * 3);

			cvCvtColor(images->imageRGB, images->imageRGB, CV_RGB2BGR);
		}

	}

	if (newImages) {
		imgKnct = this->copyKinectData();
		imgKnct->setKUndistorter(this->kUndistorter);
	} else {
		imgKnct = this->images;
	}

	return imgKnct;
}

Undistorter* CameraKinect::getUndistorter() {
	return this->kUndistorter;
}

CameraKinectImages* CameraKinect::copyKinectData() {
	IplImage* rgb;
	XnPoint3D* realWorld;

	rgb = cvCloneImage(this->images->imageRGB);
	realWorld = (XnPoint3D*) malloc(
			sizeof(XnPoint3D) * XN_VGA_Y_RES * XN_VGA_X_RES);
	memcpy(realWorld, this->images->realWorld,
			sizeof(XnPoint3D) * XN_VGA_Y_RES * XN_VGA_X_RES);

	return new CameraKinectImages(rgb, realWorld);
}

void CameraKinect::allocateMem() {
	IplImage* rgb;
	XnPoint3D* realWorld;

	pointList = (XnPoint3D*) malloc(
			sizeof(XnPoint3D) * XN_VGA_Y_RES * XN_VGA_X_RES);
	realWorld = (XnPoint3D*) malloc(
			sizeof(XnPoint3D) * XN_VGA_Y_RES * XN_VGA_X_RES);

	rgb = cvCreateImage(cvSize( XN_VGA_X_RES, XN_VGA_Y_RES), IPL_DEPTH_8U, 3);

	for (int y = 0; y < XN_VGA_Y_RES; y++) {
		for (int x = 0; x < XN_VGA_X_RES; x++) {
			pointList[y * XN_VGA_X_RES + x].X = (XnFloat) x;
			pointList[y * XN_VGA_X_RES + x].Y = (XnFloat) y;
		}
	}

	images = new CameraKinectImages(rgb, realWorld);
	images->setKUndistorter(this->kUndistorter);
}

void CameraKinect::captureFrame() {
	rc = XN_STATUS_OK;

	//rc = g_context.WaitAnyUpdateAll();
	rc = g_context.WaitAnyUpdateAll();
	//rc = g_context.WaitNoneUpdateAll();

	if (rc != XN_STATUS_OK) {
		clog << "Error: [CameraKinect.h] Read failed: " << xnGetStatusString(rc)
				<< endl;
		return;
	}
	g_depth.GetMetaData(g_depthMD);
	g_image.GetMetaData(g_imageMD);
}
