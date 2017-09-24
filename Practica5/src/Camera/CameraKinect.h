#ifndef _CAMERAKINECTTHREAD_H_
#define _CAMERAKINECTTHREAD_H_

#include "opencv/cv.h"

#include "Undistorter.h"
#include "CameraKinectImages.h"
#include "../Util.h"

/// Added for OpenNI ///
#include "XnOS.h"

#include "XnOpenNI.h"
#include "XnLog.h"
#include "XnCppWrapper.h"
#include "XnFPSCalculator.h"

#include "XnCppWrapper.h"
#include <cstring>
#include <math.h>
#include <iostream>

//thread safe
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------

#define WIN_SIZE_X 640
#define WIN_SIZE_Y 480

#define MAX_DEPTH 10000

#define MAX_BAD_FRAMES 2


using namespace std;
using namespace xn;
using namespace cv;

namespace ModViPumas
{
	class CameraKinect {
	private:


		XnPoint3D			*pointList;
		const XnRGB24Pixel	*pImageRow;
		CameraKinectImages	*images;
		Undistorter			*kUndistorter;

		//OpenNI
		Context g_context;
		DepthGenerator g_depth;
		ImageGenerator g_image;
		DepthMetaData  g_depthMD;
		ImageMetaData  g_imageMD;
		XnStatus       rc;

		EnumerationErrors errors;


		//conf read camera
		char* fileConfOpenNI;
		char* fileRecordOpenNI;
		char* fileCalibKinect;

		bool opened;
		bool connected;

		int numBadFrames;
		bool errorCamera;

		//to make it thread safe
		boost::mutex mutexGetFrame;
		boost::mutex mutexOpen;


	public:



		//constructor
		CameraKinect();

		~CameraKinect();

		bool getOpened();
		bool getErrorCamera();

		xn::DepthGenerator getG_depth() const;

		bool openCamera(char* fileConf, char* fileCalib);

		bool openRecording(char* fileConf, char* fileCalib, char *fileRecording);

		void retroProyect( double xW, double yW, double zW, int &xi, int &yi );



		//************************************
		// Method:    getFrame
		// FullName:  CameraKinect::getFrame
		// Access:    public
		// Returns:   CameraKinectImages*
		// Qualifier: obtiene un contenedor de imagens de la camara y la guarda en la variables de instancia correspondientes
		// Parameter: bool newImages  si newImages = false devuelve la imagen actual en la variable de instancia *image*
		//								  newImages = true  devuerlve un nuevo objeto con la imagen
		//************************************
		CameraKinectImages* getFrame( bool newImages = false );

		//devuelve la imagen actual en la variable de instancia *image*
		Undistorter* getUndistorter();

	private :

		CameraKinectImages* copyKinectData();

		void allocateMem();


		bool openCamera();

		bool openRecording();


		void captureFrame();


	};
}

#endif
