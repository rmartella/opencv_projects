#ifndef _KINECTIMAGE_H_
#define _KINECTIMAGE_H_

////////////////Librer�as agregadas debido al uso de OpenNI////////////
#include <XnOpenNI.h>
#include <XnCppWrapper.h>
#include <stdio.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "Undistorter.h"

#include <iostream>
#include <fstream>

#define MAX_DEPTH 10000

using namespace xn;
using namespace cv;
using namespace std;



class CameraKinectImages{
private:
	//IplImage *imageDepth;
	//IplImage *imageDepth16;
	Undistorter *undistorter;
public:
	XnPoint3D* realWorld;
	IplImage *imageRGB;

	CameraKinectImages(IplImage*  &rgb, XnPoint3D* &realW ){
		this->imageRGB		= rgb;
		this->realWorld		= realW;
		this->undistorter  = 0;
	}

	CameraKinectImages()
	{
		this->imageRGB		= 0;
		this->realWorld		= 0;
		this->undistorter	= 0;
	}



	void setKUndistorter(Undistorter* &undis)
	{
		this->undistorter = undis;
	}

	int getDistance(int x, int y)
	{
		return (int)get3DPoint( x , y ).Z;
	}

	XnPoint3D get3DPoint(int x, int y){
		return this->realWorld[y * imageRGB->width + x];
	}



	void getImageRGBUndistorted( IplImage* &inOut )
	{
		if( inOut == 0 || inOut->width != imageRGB->width || inOut->height != imageRGB->height
			|| inOut->depth != imageRGB->depth || inOut->nChannels != imageRGB->nChannels )
		{
			cvReleaseImage( &inOut );
			inOut = cvCloneImage( imageRGB );
		}
		if( this->undistorter != 0 )
			undistorter->undistor_rgb_image(this->imageRGB, inOut);


	}


	void getImageGray( IplImage* &inOut ){
		if( inOut == 0 || inOut->width != imageRGB->width || inOut->height != imageRGB->height
			|| inOut->depth != imageRGB->depth || inOut->nChannels != 1 )
		{
			cvReleaseImage( &inOut );
			inOut = cvCreateImage( cvGetSize( imageRGB ), imageRGB->depth, 1 );
		}

		cvCvtColor( imageRGB, inOut , CV_BGR2GRAY );
	}


	void getImageDepth16(IplImage* &inOut)
	{
		if ( inOut == 0 )
		{
			inOut= cvCreateImage( cvGetSize(imageRGB), IPL_DEPTH_16U, 1 );
		}
		if( inOut->width != imageRGB->width || inOut->height != imageRGB->height
			|| inOut->depth != IPL_DEPTH_16U || inOut->nChannels != 1 )
		{
			cvReleaseImage( &inOut );
			inOut = cvCreateImage( cvGetSize( imageRGB ), IPL_DEPTH_16U, 1 );
		}

		unsigned short *pixel  = 0;

		pixel = &CV_IMAGE_ELEM(inOut, unsigned short, 0, 0 );

		for (int y = 0; y < imageRGB->height; ++y){
			for (int x = 0; x < imageRGB->width; ++x){
				*pixel = (unsigned short)this->getDistance(x,y);
				pixel++;

			}
		}
	}

	void dumpImageDepth16(const char* file)
	{
		FILE *out = fopen(file, "w");
		fprintf(out, "P2\n#\n");
		fprintf(out, "%d %d\n", imageRGB->width, imageRGB->height);
		fprintf(out, "6250\n");


		for (int y = 0; y < imageRGB->height; ++y)
		{
			for (int x = 0; x < imageRGB->width; ++x)
			{
				fprintf (out, "%d ", this->getDistance(x,y));
			}
		}
		fclose(out);
	}

	void dumpDepth(const char *file)
	{
		IplImage      *imD16;
		CvFileStorage *fs;

		imD16 = 0;
		fs    = 0;

		fs = cvOpenFileStorage(file, NULL, CV_STORAGE_WRITE);

		this->getImageDepth16(imD16);

		cvWrite(fs, "Depth", imD16);

		cvReleaseFileStorage(&fs);
		cvReleaseImage(&imD16);
	}

	void dump3DData(const char *file)
	{
		int indice;
		fstream filestr( file, fstream::out);
		indice			= 0;

		for( int y=0; y < imageRGB->height ; y++ )
		{
			for( int x=0; x < imageRGB->width ; x++ )
			{
				if ( get3DPoint(x,y).Z > 0 )
					filestr<<x<<" "<<y<<" "<< get3DPoint(x,y).X<<" "<<get3DPoint(x,y).Y<<" "<<get3DPoint(x,y).Z<<endl;

			}
		}

		filestr.close();
	}
	void dumpRGB(const char *file)
	{
		cvSaveImage(file, this->imageRGB);
	}

	IplImage* getImageBetween(int lowDepth, int highDepth)
	{
		IplImage* out;

		out= cvCreateImage( cvGetSize(imageRGB), imageRGB->depth, imageRGB->nChannels );

		getImageBetween( lowDepth, highDepth, out );

		return out;
	}

	void getImageBetween(int lowDepth, int highDepth, IplImage* &inOut){

		int pointer;

		if ( inOut == 0 )
		{
			inOut= cvCreateImage( cvGetSize(imageRGB), imageRGB->depth, imageRGB->nChannels );
		}

		if( inOut->width != imageRGB->width || inOut->height != imageRGB->height
			|| inOut->depth != imageRGB->depth || inOut->nChannels != imageRGB->nChannels )
		{

			cvReleaseImage( &inOut );
			inOut= cvCreateImage( cvGetSize(imageRGB), imageRGB->depth, imageRGB->nChannels );
		}

		pointer = 0;

		for(int y = 0; y < inOut->height; y++){
			for(int  x = 0; x < inOut->width; x++){
				if ( (this->getDistance(x,y) > lowDepth)&&(this->getDistance(x,y) < highDepth) ){
					inOut->imageData[pointer] = imageRGB->imageData[pointer++];
					inOut->imageData[pointer] = imageRGB->imageData[pointer++];
					inOut->imageData[pointer] = imageRGB->imageData[pointer++];
				}
				else{
					inOut->imageData[pointer++] = 0;
					inOut->imageData[pointer++] = 0;
					inOut->imageData[pointer++] = 0;
				}
			}
		}

	}

	void cutDataToBetween(int lowDepth, int highDepth, IplImage* &inOut){

		int pointer;

		if ( inOut == 0 )
		{
			inOut= cvCreateImage( cvGetSize(imageRGB), imageRGB->depth, 1 );
		}

		if( inOut->width != imageRGB->width || inOut->height != imageRGB->height
			|| inOut->depth != imageRGB->depth || inOut->nChannels != 1 )
		{

			cvReleaseImage( &inOut );
			inOut= cvCreateImage( cvGetSize(imageRGB), imageRGB->depth, 1 );
		}

		pointer = 0;

		for(int y = 0; y < imageRGB->height; y++){
			for(int  x = 0; x < imageRGB->width; x++){
				if ( (this->getDistance(x,y) > lowDepth)&&(this->getDistance(x,y) < highDepth) ){
					imageRGB->imageData[pointer] = imageRGB->imageData[pointer++];
					imageRGB->imageData[pointer] = imageRGB->imageData[pointer++];
					imageRGB->imageData[pointer] = imageRGB->imageData[pointer++];
					inOut->imageData[ y*imageRGB->width + x ] = 255;
				}
				else{
					imageRGB->imageData[pointer++] = 0;
					imageRGB->imageData[pointer++] = 0;
					imageRGB->imageData[pointer++] = 0;
					realWorld[y * imageRGB->width + x].X = 0;
					realWorld[y * imageRGB->width + x].Y = 0;
					realWorld[y * imageRGB->width + x].Z = 0;
					inOut->imageData[ y*imageRGB->width + x ] = 0;
				}
			}
		}

	}


	~CameraKinectImages()
	{
		cvReleaseImage(&imageRGB);
		free(realWorld);
	}


	CameraKinectImages* SubSample(int subSamplingFactor)
	{
		CameraKinectImages* subSampledImages = new CameraKinectImages();
		int width = this->imageRGB->width;
		int height = this->imageRGB->height;
		subSampledImages->realWorld = 0;
		subSampledImages->imageRGB = 0;
		IplImage headerImg;

		Mat points3D = Mat(height, width, CV_32FC3, this->realWorld);
		int subSampledRows = height / subSamplingFactor;
		int subSampledCols = width / subSamplingFactor;
		Mat subSampledPointsCols;
		Mat subSampledPointsRows;
		Mat matImageRGB;
		Mat subSampledImageCols;
		Mat subSampledImageRows;
		size_t row, col, idx;

		subSampledPointsCols = Mat(height, subSampledCols, points3D.type());
		subSampledPointsRows = Mat(subSampledRows, subSampledCols, points3D.type());
		matImageRGB = cvarrToMat(this->imageRGB).clone();
		subSampledImageCols = Mat(height, subSampledCols, matImageRGB.type());
		subSampledImageRows = Mat(subSampledRows, subSampledCols, matImageRGB.type());
		subSampledPointsCols.setTo(0);
		subSampledPointsRows.setTo(0);
		subSampledImageCols.setTo(0);
		subSampledImageRows.setTo(0);


		for(idx = 0, col = 0;
			col < points3D.cols && idx < subSampledCols;
			col += subSamplingFactor, idx++)
		{
			points3D.col(col).copyTo(subSampledPointsCols.col(idx));
			matImageRGB.col(col).copyTo(subSampledImageCols.col(idx));
		}
		for(idx = 0, row = 0;
			row < subSampledPointsCols.rows && idx < subSampledRows;
			row += subSamplingFactor, idx++)
		{
			subSampledPointsCols.row(row).copyTo(subSampledPointsRows.row(idx));
			subSampledImageCols.row(row).copyTo(subSampledImageRows.row(idx));
		}

		size_t totalBytes = subSampledPointsRows.total() * subSampledPointsRows.elemSize();
		XnPoint3D* pSubSampledPointsRows = (XnPoint3D*) subSampledPointsRows.data;
		subSampledImages->realWorld = (XnPoint3D*) malloc( totalBytes );
		memcpy( subSampledImages->realWorld, pSubSampledPointsRows, totalBytes );
		headerImg = subSampledImageRows;
		subSampledImages->imageRGB = cvCloneImage(&headerImg);

		return subSampledImages;
	}
};



#endif
