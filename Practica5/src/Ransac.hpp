#ifndef RANSAC_HPP_
#define RANSAC_HPP_

#include "XnOpenNI.h"
#include "Plano.hpp"
#include <iostream>
#include <vector>

using namespace std;

#define MAXSIZE 15

class Ransac {
public:
	Ransac();
	~Ransac();
	void ransacAlgorithm(XnPoint3D * points, int xI, int yI, int width,
			int height, int widthImage, int n, float t, int d,
			Plano * modelResult, bool inicio);
	void lin_solve(float coeff[3][3], float U[3][3]);
	Plano * regresion(XnPoint3D * model, int tam);
protected:
};
#endif
