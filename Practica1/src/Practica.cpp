#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "Imagen.hpp"
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char** argv) {

	if (argc != 4) {
		cout << " Usage: display_image ImageToLoadAndDisplay" << endl;
		return -1;
	}

	char * opcion = argv[3];
	int ejercicio = atoi(opcion);
	Imagen imagen = Imagen();
	switch (ejercicio) {
	case 1: {
		IplImage * img = imagen.cargaImagen(argv[1]);
		if (img == NULL) {
			return -1;
		}
		imagen.muestraImagen(img, "Ejercicio1", false, true);
		break;
	}
	case 2: {
		IplImage * img = imagen.cargaImagen(argv[1]);
		if (img == NULL) {
			return -1;
		}
		imagen.muestraImagen(img, "Ejercicio2", true, true);
		break;
	}
	case 3: {
		IplImage * img = imagen.cargaImagen(argv[1]);
		if (img == NULL) {
			return -1;
		}
		IplImage * tmp = imagen.copiaImagen(img);
		imagen.muestraImagen(tmp, "Ejercicio3", false, true);
		break;
	}
	case 4: {
		IplImage * img = imagen.cargaImagen(argv[1]);
		if (img == NULL) {
			return -1;
		}
		IplImage ** canales = imagen.separaCanalesImagen(img);
		IplImage * imgR = canales[0];
		IplImage * imgG = canales[1];
		IplImage * imgB = canales[2];
		imagen.muestraImagen(imgR, "Ejercicio 4 Canal R", false, false);
		imagen.muestraImagen(imgG, "Ejercicio 4 Canal G", false, false);
		imagen.muestraImagen(imgB, "Ejercicio 4 Canal B", false, false);
		IplImage * imgMerge = cvCreateImage(cvSize(img->width, img->height),
				img->depth, 3);
		cvMerge(imgB, imgG, imgR, NULL, imgMerge);
		imagen.muestraImagen(imgMerge, "imgMerge", false, false);
		waitKey(0);
		cvReleaseImage(&imgR);
		cvDestroyWindow("Ejercicio 4 Canal R");
		cvReleaseImage(&imgG);
		cvDestroyWindow("Ejercicio 4 Canal G");
		cvReleaseImage(&imgB);
		cvDestroyWindow("Ejercicio 4 Canal B");
		cvReleaseImage(&imgMerge);
		cvDestroyWindow("ImagenMerge");
		break;
	}
	case 5: {
		IplImage * img = imagen.cargaImagen(argv[1]);
		if (img == NULL) {
			return -1;
		}
		IplImage * imgCopia = imagen.copiaImagen(img);
		cvRectangle(img, cvPoint(100, 100),
				cvPoint(img->width - 100, img->height - 100),
				cvScalar(0, 0, 0), -1);
		cvSetImageROI(imgCopia,
				cvRect(100, 100, img->width - 200, img->height - 200));
		imagen.muestraImagen(img, "Ejercicio 5 REC", false, false);
		imagen.muestraImagen(imgCopia, "Ejercicio 5 ROI", false, false);
		waitKey(0);
		cvReleaseImage(&img);
		cvReleaseImage(&imgCopia);
		cvDestroyWindow("Ejercicio 5 REC");
		cvDestroyWindow("Ejercicio 5 ROI");
		break;
	}
	case 6: {
		IplImage * img1 = imagen.cargaImagen(argv[1]);
		IplImage * img2 = imagen.cargaImagen(argv[2]);
		if (img1 == NULL || img2 == NULL) {
			return -1;
		}
		IplImage ** operacionesLogicas = imagen.realizaOperacionesLogicas(img1,
				img2);
		IplImage * opAnd = operacionesLogicas[0];
		IplImage * opOr = operacionesLogicas[1];
		IplImage * opNot = operacionesLogicas[2];
		IplImage * opXor = operacionesLogicas[3];
		imagen.muestraImagen(opAnd, "Ejercicio 6 AND", false, false);
		imagen.muestraImagen(opOr, "Ejercicio 6 OR", false, false);
		imagen.muestraImagen(opNot, "Ejercicio 6 NOT", false, false);
		imagen.muestraImagen(opXor, "Ejercicio 6 XOR", false, false);
		waitKey(0);
		cvReleaseImage(&opAnd);
		cvDestroyWindow("Ejercicio 6 AND");
		cvReleaseImage(&opOr);
		cvDestroyWindow("Ejercicio 6 OR");
		cvReleaseImage(&opNot);
		cvDestroyWindow("Ejercicio 6 NOT");
		cvReleaseImage(&opXor);
		cvDestroyWindow("Ejercicio 6 XOR");
		break;
	}
	case 7: {
		IplImage * img = imagen.cargaImagen(argv[1]);
		if (img == NULL) {
			return -1;
		}
		Mat destinoGray;
		Mat destinoHSV;
		const Mat origen = cvarrToMat(img);
		cvtColor(origen, destinoGray, CV_RGB2GRAY, 0);
		cvtColor(origen, destinoHSV, CV_RGB2HSV, 0);
		IplImage imgGray = IplImage(destinoGray);
		IplImage imgHSV = IplImage(destinoHSV);
		IplImage * imgGrayPtr = &imgGray;
		IplImage * imgHSVPtr = &imgHSV;
		imagen.muestraImagen(imgGrayPtr, "Ejercicio 7 GRAY", false, false);
		imagen.muestraImagen(imgHSVPtr, "Ejercicio 7 HSV", false, false);
		waitKey(0);
		cvReleaseImage(&imgGrayPtr);
		cvDestroyWindow("Ejercicio 7 GRAY");
		cvReleaseImage(&imgHSVPtr);
		cvDestroyWindow("Ejercicio 7 HSV");
		break;
	}
	case 8: {
		IplImage * img = imagen.cargaImagen(argv[1]);
		if (img == NULL) {
			return -1;
		}
		Mat destinoHSV;
		const Mat origen = cvarrToMat(img);
		cvtColor(origen, destinoHSV, CV_RGB2HSV, 0);
		IplImage imgHSV = IplImage(destinoHSV);
		IplImage * imgHSVPtr = &imgHSV;

		IplImage ** canales = imagen.separaCanalesImagen(imgHSVPtr);
		IplImage * imgR = canales[0];
		IplImage * imgG = canales[1];
		IplImage * imgB = canales[2];
		imagen.muestraImagen(imgR, "Ejercicio 8 Canal R", false, false);
		imagen.muestraImagen(imgG, "Ejercicio 8 Canal G", false, false);
		imagen.muestraImagen(imgB, "Ejercicio 8 Canal B", false, false);
		IplImage * imgMerge = cvCreateImage(
				cvSize(imgHSVPtr->width, imgHSVPtr->height), imgHSVPtr->depth,
				3);
		cvMerge(imgB, imgG, imgR, NULL, imgMerge);
		imagen.muestraImagen(imgMerge, "imgMerge", false, false);
		waitKey(0);
		cvReleaseImage(&imgR);
		cvDestroyWindow("Ejercicio 8 Canal R");
		cvReleaseImage(&imgG);
		cvDestroyWindow("Ejercicio 8 Canal G");
		cvReleaseImage(&imgB);
		cvDestroyWindow("Ejercicio 8 Canal B");
		cvReleaseImage(&imgMerge);
		cvDestroyWindow("ImagenMerge");
		break;
	}
	case 9:
		IplImage * img = imagen.cargaImagen(argv[1]);
		if (img == NULL) {
			return -1;
		}
		IplImage * imgN = imagen.normalizeImage(img);
		imagen.muestraImagen(imgN,"Ejercicio 9",false,true);
	}

	return 0;

}
