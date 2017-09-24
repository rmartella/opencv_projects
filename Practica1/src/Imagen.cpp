/*
 * Imagen.cpp
 *
 *  Created on: 24/08/2014
 *      Author: rey
 */

#include "Imagen.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

Imagen::Imagen() {
	cout << "Imagen Creada" << endl;
}

Imagen::~Imagen() {
	cout << "Imagen Destruida" << endl;
}

IplImage* Imagen::cargaImagen(char * nombreImagen) {
	IplImage* img = cvLoadImage(nombreImagen);
	cout << "Referencia de imagen." << img << endl;
	if (img == NULL) {
		cout << "No se pudo carga la imagen." << endl;
	} else {
		cout << "Tamanio imagen. " << img->nSize << endl;
	}
	return img;
}

void Imagen::muestraImagen(IplImage * img, char * nombreVentana,
		bool banderaMouse, bool banderaEspera) {
	cvNamedWindow(nombreVentana, CV_WINDOW_AUTOSIZE);
	if (banderaMouse) {
		cvSetMouseCallback(nombreVentana, Imagen::on_mouse, img);
	}
	cvShowImage(nombreVentana, img);
	if (banderaEspera) {
		waitKey(0);
		cvReleaseImage(&img);
		cvDestroyWindow(nombreVentana);
	}
}

IplImage * Imagen::copiaImagen(IplImage * img) {
	IplImage *tmp = cvCreateImage(cvGetSize(img), img->depth, img->nChannels);
	cvCopy(img, tmp, NULL);
	if (img == NULL) {
		cout << "No se pudo carga la imagen." << endl;
	} else {
		cout << "Tamanio imagen. " << img->nSize << endl;
	}
	return tmp;
}

IplImage ** Imagen::separaCanalesImagen(IplImage * img) {
	IplImage * array[3];
	IplImage* output_r = cvCreateImage(cvSize(img->width, img->height),
			img->depth, 1);
	IplImage* output_g = cvCreateImage(cvSize(img->width, img->height),
			img->depth, 1);
	IplImage* output_b = cvCreateImage(cvSize(img->width, img->height),
			img->depth, 1);
	cvSplit(img, output_b, output_g, output_r, NULL);
	array[0] = output_r;
	array[1] = output_g;
	array[2] = output_b;
	return array;
}

IplImage ** Imagen::realizaOperacionesLogicas(IplImage * img1,
		IplImage * img2) {
	IplImage * array[4];
	IplImage * opAnd = cvCreateImage(cvSize(img1->width, img1->height),
			img1->depth, 3);
	IplImage * opOr = cvCreateImage(cvSize(img1->width, img1->height),
			img1->depth, 3);
	IplImage * opNot = cvCreateImage(cvSize(img1->width, img1->height),
			img1->depth, 3);
	IplImage * opXor = cvCreateImage(cvSize(img1->width, img1->height),
			img1->depth, 3);
	cvAnd(img1, img2, opAnd, NULL);
	cvOr(img1, img2, opOr, NULL);
	cvNot(img1, opNot);
	cvXor(img1, img2, opXor, NULL);
	array[0] = opAnd;
	array[1] = opOr;
	array[2] = opNot;
	array[3] = opXor;
	return array;
}

IplImage * Imagen::normalizeImage(IplImage* img) {
	IplImage* out;
	out = cvCreateImage(cvGetSize(img), img->depth, img->nChannels);
	for (int y = 0; y < img->height; y++) {
		for (int x = 0; x < img->width; x++) {

			unsigned char * b = &CV_IMAGE_ELEM(img, uchar, y, x * 3); // a z u l
			unsigned char * g = &CV_IMAGE_ELEM(img, uchar, y, x * 3 + 1); // v e r d e
			unsigned char * r = &CV_IMAGE_ELEM(img, uchar, y, x * 3 + 2); // r o j o
			int sumaCanales = r[0] + g[0] + b[0];
			int bn;
			int gn;
			int rn;
			if (sumaCanales == 0) {
				bn = 0;
				gn = 0;
				rn = 0;
			} else {
				bn = 255 * b[0] / sumaCanales;
				gn = 255 * g[0] / sumaCanales;
				rn = 255 * r[0] / sumaCanales;
			}
			unsigned char * bo = &CV_IMAGE_ELEM(out, unsigned char, y,
					x * out->nChannels); // azul
			unsigned char * go = &CV_IMAGE_ELEM(out, unsigned char, y,
					x * out->nChannels + 1); //verde
			unsigned char * ro = &CV_IMAGE_ELEM(out, unsigned char, y,
					x * out->nChannels + 2); //rojo
			bo[0] = bn;
			go[0] = gn;
			ro[0] = rn;
		}
	}
	return out;
}
