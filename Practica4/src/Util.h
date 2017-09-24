#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <fstream>
#include <stdarg.h>


using namespace std;

#ifndef ABS
#define ABS(x) ( ( x < 0 )? -x : x )
#endif

// Record the execution time of some code, in milliseconds.
#define DECLARE_TIMING(s)  int64 timeStart_##s; double timeDiff_##s; double timeTally_##s = 0; int countTally_##s = 0
#define START_TIMING(s)    timeStart_##s = cvGetTickCount()
#define STOP_TIMING(s) 	   timeDiff_##s = (double)(cvGetTickCount() - timeStart_##s); timeTally_##s += timeDiff_##s; countTally_##s++
#define GET_TIMING(s) 	   (double)(timeDiff_##s / (cvGetTickFrequency()*1000.0))
#define GET_AVERAGE_TIMING(s)   (double)(countTally_##s ? timeTally_##s/ ((double)countTally_##s * cvGetTickFrequency()*1000.0) : 0)
#define CLEAR_AVERAGE_TIMING(s) timeTally_##s = 0; countTally_##s = 0s


class Util
{
public:


	//************************************
	// Method:    PrintMat
	// FullName:  Util::PrintMat
	// Access:    public
	// Returns:   void
	// Qualifier: Print matrix on console
	// Parameter: CvMat * A
	//************************************
	static void printMat(CvMat *A){
		int i, j;
		for (i = 0; i < A->rows; i++)
		{
			printf("\n");
			switch (CV_MAT_DEPTH(A->type))
			{
			case CV_32F:
			case CV_64F:
				for (j = 0; j < A->cols; j++)
					printf ("%8.4f ", (float)cvGetReal2D(A, i, j));
				break;
			case CV_8U:
			case CV_16U:
				for(j = 0; j < A->cols; j++)
					printf ("%6d ",(int) A->data.i[i * A->cols + j]);
				break;
			default:
				break;
			}
		}
		printf("\n");
	}



	/**
	* Joins two images
	*/
	static void joinImages( IplImage* srcLeft, IplImage* srcRight, IplImage** imgOut ){

		int width;
		int height;

		width = srcLeft->width + srcRight->width +3;
		if( srcLeft->height > srcRight->height)
			height = srcLeft->height;
		else
			height = srcRight->height;


		if((*imgOut)==NULL )
		{
			(*imgOut)=cvCreateImage( cvSize( width, height ) ,srcLeft->depth ,srcLeft->nChannels );
		}

		if((*imgOut)->height != height || (*imgOut)->width != width )
		{
			cvReleaseImage(&(*imgOut));
			(*imgOut)=cvCreateImage( cvSize( width, height ) ,srcLeft->depth ,srcLeft->nChannels );
		}



		cvZero((*imgOut));
		cvSetImageROI( (*imgOut), cvRect( 0, 0, srcLeft->width, srcLeft->height ) );
		cvCopy(srcLeft, (*imgOut));
		cvResetImageROI( (*imgOut) );

		cvSetImageROI( (*imgOut), cvRect(srcLeft->width, 0, srcRight->width, srcRight->height) );
		cvCopy( srcRight, (*imgOut) );
		cvResetImageROI( (*imgOut) );
	}

	/**
	* Joins two images
	*/
	static void joinImagesHalfSize( IplImage* srcLeft, IplImage* srcRight, IplImage** imgOut ){

		int width;
		int height;

		IplImage *imgTemp = 0;

		width = srcLeft->width + srcRight->width;

		if( srcLeft->height > srcRight->height)
			height = srcLeft->height;
		else
			height = srcRight->height;


		if((*imgOut) == NULL )
		{
			(*imgOut) = cvCreateImage( cvSize( width/2, height/2 ) ,srcLeft->depth ,srcLeft->nChannels );
		}

		if((*imgOut)->height != ( height/2 ) || (*imgOut)->width != width/2 )
		{
			cvReleaseImage(&(*imgOut));
			(*imgOut) = cvCreateImage( cvSize( width/2, height/2 ) ,srcLeft->depth ,srcLeft->nChannels );
		}

		imgTemp   = cvCreateImage( cvSize( width, height ) ,srcLeft->depth ,srcLeft->nChannels );

		cvZero(imgTemp);

		cvSetImageROI( imgTemp, cvRect( 0, 0, srcLeft->width, srcLeft->height ) );
		cvCopy(srcLeft, imgTemp);
		cvResetImageROI( imgTemp );

		cvSetImageROI( imgTemp, cvRect(srcLeft->width, 0, srcRight->width, srcRight->height) );
		cvCopy( srcRight, imgTemp );
		cvResetImageROI( imgTemp );

		cvResize(imgTemp, (*imgOut));

		cvReleaseImage(&imgTemp);

	}


	static  IplImage* putBottomMargin( IplImage* imgSrc, int bottomMargin ){

		IplImage* imgOut;
		int width;
		int height;

		if( imgSrc == NULL )
		{
			return NULL;
		}

		width = imgSrc->width;
		height = imgSrc->height + bottomMargin;

		imgOut = cvCreateImage( cvSize( width, height ) ,imgSrc->depth ,imgSrc->nChannels );

		cvZero( imgOut );
		cvSetImageROI( imgOut, cvRect( 0, 0, width, imgSrc->height ) );
		cvCopy( imgSrc, imgOut );
		cvResetImageROI( imgOut );

		return imgOut;

	}


	/**
	* Dibuja l�neas con puntos en los extremos
	*/
	static void drawLinesWithDots(IplImage* img, int ancho, int X1, int Y1, int X2, int Y2)
	{
		//Dibujo la linea
		cvLine(img, cvPoint(X1,Y1), cvPoint(ancho+X2, Y2), CV_RGB(255,0,0), 1);
		cvLine(img, cvPoint(X1,Y1), cvPoint(X1,Y1), CV_RGB(0,255,0), 2);
		cvLine(img, cvPoint(ancho+X2,Y2), cvPoint(ancho+X2,Y2), CV_RGB(0,255,0), 2);
	}




	/*static CvRect getMatchesBox(vector<Empatado*> matches)
	{
		CvPoint minimum = cvPoint(100000,100000);
		CvPoint maximum = cvPoint(0,0);
		CvPoint current;
		CvRect box;
		int dx, dy;
		for(int  i = 0; i < int(matches.size()) ; i++ )
		{
			current = cvPoint(cvRound( matches[i]->escena->xi),	cvRound( matches[i]->escena->yi));
			if(current.x < minimum.x && current.y < minimum.y){
				minimum.x = current.x;
				minimum.y = current.y;
			}
			if(current.x > maximum.x && current.y > maximum.y){
				maximum.x = current.x;
				maximum.y = current.y;
			}
		}
		dx = maximum.x - minimum.x;
		dx = (dx < 0) ? -dx : dx;
		dy = maximum.y - minimum.y;
		dy = (dy < 0) ? -dy : dy;
		box = cvRect(minimum.x, minimum.y, dx, dy);
		return box;
	}*/




	/*
	Prints an error message and aborts the program.  The error message is
	of the form "Error: ...", where the ... is specified by the \a format
	argument

	@param format an error message format string (as with \c printf(3)).
	*/
	static void fatal_error(char* format, ...)
	{
		va_list ap;

		fprintf( stderr, "Error: ");

		va_start( ap, format );
		vfprintf( stderr, format, ap );
		va_end( ap );
		fprintf( stderr, "\n" );
		abort();
	}

	/**
	A function to get a pixel value from a 32-bit floating-point image.

	@param img an image
	@param r row
	@param c column
	@return Returns the value of the pixel at (\a r, \a c) in \a img
	*/
	static __inline float pixval32f( IplImage* img, int r, int c )
	{
		return ( (float*)(img->imageData + img->widthStep*r) )[c];
	}


	static void writeOnImage(IplImage* img, char* cadena, int line, int col, CvScalar color, int typeFont){
		int line_type = CV_AA;
		CvFont font;
		//CvSize text_size;
		CvPoint pt1;
		pt1.x = 10*col;
		pt1.y = 18*line;
		cvInitFont( &font, typeFont, 0.5, 0.5, 1,1, line_type );
		cvPutText( img, cadena, pt1, &font, color);
	}



	static void writeOnImage(IplImage* img, const char* cadena){
		int line_type = CV_AA;
		CvFont font;
		//CvSize text_size;
		CvPoint pt1;
		pt1.x = 10;
		pt1.y = 30;
		cvInitFont( &font, CV_FONT_HERSHEY_SIMPLEX, 0.5, 0.5, 1,1, line_type );
		cvPutText( img, cadena, pt1, &font, CV_RGB(255,0,0));
	}


	/**
	 Return the current path
	*/
	static string getCurrentPath()
	{
		string path;
		char *buffer = getcwd(NULL,0);
		path.assign(buffer);
		path = path+"/";
		return path;
	}

	static IplImage* GetSubImage(IplImage *image, CvRect roi)
	{
		IplImage *result;
		cvSetImageROI(image,roi);
		result = cvCreateImage( cvSize(roi.width, roi.height), image->depth, image->nChannels );
		cvCopy(image,result);
		cvResetImageROI(image); // release image ROI
		return result;
	}


};
#endif
