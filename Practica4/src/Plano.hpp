/*
 * Plano.h
 *
 *  Created on: 07/11/2014
 *      Author: rey
 */

#ifndef SRC_PLANO_HPP_
#define SRC_PLANO_HPP_

#include "XnOpenNI.h"
#include <iostream>
#include <vector>

using namespace std;

class Plano {
private:
	float A, B, C, D;
	int consenso;
	vector<XnPoint3D *> modelo;
public:
	Plano();
	Plano(float A, float B, float C, float D);
	Plano(XnPoint3D * p0, XnPoint3D * p1, XnPoint3D * p2);
	virtual ~Plano();
	float getA();
	float getB();
	float getC();
	float getD();
	void setA(float A);
	void setB(float B);
	void setC(float C);
	void setD(float D);
	void setConsenso(int consenso);
	int getConsenso();
	vector<XnPoint3D*> getModelo();
	void setModelo(vector<XnPoint3D*> modelo);
	XnPoint3D * normalDelPlano(XnPoint3D * p0, XnPoint3D * p1, XnPoint3D * p2);
	float distanciaPunto(XnPoint3D * q);
	float segmentaProfundidad(int x,int y);
};

#endif /* SRC_PLANO_HPP_ */
