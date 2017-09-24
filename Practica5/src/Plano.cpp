/*
 * Plano.cpp
 *
 *  Created on: 07/11/2014
 *      Author: rey
 */

#include "Plano.hpp"
#include <math.h>

Plano::Plano() {
}

Plano::Plano(float A, float B, float C, float D) {
	this->A = A;
	this->B = B;
	this->C = C;
	this->D = D;
}

Plano::Plano(XnPoint3D * p0, XnPoint3D * p1, XnPoint3D * p2) {
	XnPoint3D * normal = normalDelPlano(p0, p1, p2);
	this->A = normal[0].X;
	this->B = normal[0].Y;
	this->C = normal[0].Z;
	this->D = -normal[0].X * p0[0].X - normal[0].Y * p0[0].Y
			- normal[0].Z * p0[0].Z;
}

Plano::~Plano() {
	cout << "Plano Destruida" << endl;
}
float Plano::getA() {
	return this->A;
}
float Plano::getB() {
	return this->B;
}
float Plano::getC() {
	return this->C;
}
float Plano::getD() {
	return this->D;
}
void Plano::setA(float A) {
	this->A = A;
}
void Plano::setB(float B) {
	this->B = B;
}
void Plano::setC(float C) {
	this->C = C;
}
void Plano::setD(float D) {
	this->D = D;
}
void Plano::setConsenso(int consenso) {
	this->consenso = consenso;
}
vector<XnPoint3D*> Plano::getModelo() {
	return this->modelo;
}
void Plano::setModelo(vector<XnPoint3D*> modelo) {
	this->modelo = modelo;
}
int Plano::getConsenso(){
	return this->consenso;
}
XnPoint3D * Plano::normalDelPlano(XnPoint3D * p0, XnPoint3D * p1,
		XnPoint3D * p2) {
	XnPoint3D * u1 = (XnPoint3D*) malloc(sizeof(XnPoint3D));
	XnPoint3D * u2 = (XnPoint3D*) malloc(sizeof(XnPoint3D));
	XnPoint3D * n = (XnPoint3D*) malloc(sizeof(XnPoint3D));

	u1[0].X = p1[0].X - p0[0].X;
	u1[0].Y = p1[0].Y - p0[0].Y;
	u1[0].Z = p1[0].Z - p0[0].Z;
	u2[0].X = p2[0].X - p0[0].X;
	u2[0].Y = p2[0].Y - p0[0].Y;
	u2[0].Z = p2[0].Z - p0[0].Z;
	n[0].X = u1[0].Y * u2[0].Z - u1[0].Z * u2[0].Y;
	n[0].Y = - u1[0].X * u2[0].Z + u1[0].Z * u2[0].X;
	n[0].Z = u1[0].X * u2[0].Y - u1[0].Y * u2[0].X;
	free(u1);
	free(u2);
	return n;
}

float Plano::distanciaPunto(XnPoint3D * q) {
	float result;
	result = sqrt(A * A + B * B + C * C);
	return abs(q[0].X * A + q[0].Y * B + q[0].Z * C + D) / result;
}

float Plano::segmentaProfundidad(int x,int y) {
	if(C == -1){
		return A * x + B * y + D;
	}
	return A / C * x + B / C * y + D / C;
}
