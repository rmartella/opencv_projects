/*
 * Imagen.hpp
 *
 *  Created on: 24/08/2014
 *      Author: rey
 */

#ifndef IMAGEN_HPP_
#define IMAGEN_HPP_
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

class Imagen {
public:
	Imagen();
	virtual ~Imagen();
	IplImage* cargaImagen(char * nombreImagen);
	void muestraImagen(IplImage * img, char * nombreVentana, bool banderaMouse,
			bool banderaEspera);
	static void on_mouse(int event, int x, int y, int flags, void* param) {
		IplImage* img = (IplImage*) param;
		if (event == CV_EVENT_FLAG_LBUTTON) {

			// Método 1

			/*unsigned char b = (unsigned char) img->imageData[3*(y*img->width+x)+0]; // a z u l
			 unsigned char g = (unsigned char) img->imageData[3*(y*img->width+x)+1]; // v e rde
			 unsigned char r = (unsigned char) img->imageData[3*(y*img->width+x)+2]; // r o j o*/

			// Método 2
			/*char* data = img->imageData + img->widthStep * y + x * 3;

			 unsigned char b = (unsigned char) data[0]; // a z u l
			 unsigned char g = (unsigned char) data[1]; // v e rde
			 unsigned char r = (unsigned char) data[2]; // r o j o*/

			/*cout << "Las componentes del color \n" << "Rojo:" << dec
			 << static_cast<int>(r) << "\nVerde:" << dec
			 << static_cast<int>(g) << "\nAzul:" << dec
			 << static_cast<int>(b) << endl;*/

			// Método 3
			unsigned char * blue = &CV_IMAGE_ELEM(img, uchar, y, x * 3); // a z u l
			unsigned char * gren = &CV_IMAGE_ELEM(img, uchar, y, x * 3 + 1); // a z u l
			unsigned char * red = &CV_IMAGE_ELEM(img, uchar, y, x * 3 + 2); // a z u l

			cout << "Las componentes del color \n" << "Rojo:" << dec
					<< static_cast<int>(red[0]) << "\nVerde:" << dec
					<< static_cast<int>(gren[0]) << "\nAzul:" << dec
					<< static_cast<int>(blue[0]) << endl;
		}
	}
	IplImage * copiaImagen(IplImage * img);
	IplImage ** separaCanalesImagen(IplImage * img);
	IplImage ** realizaOperacionesLogicas(IplImage * img1, IplImage * img2);
	IplImage * normalizeImage(IplImage* img);
protected:
};

#endif /* IMAGEN_HPP_ */
