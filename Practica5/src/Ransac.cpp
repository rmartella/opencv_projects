#include "Ransac.hpp"

#define MAXSIZE 15

Ransac::Ransac() {
	cout << "Ransac Creada" << endl;
}

Ransac::~Ransac() {
	cout << "Ransac Destruida" << endl;
}

void Ransac::ransacAlgorithm(XnPoint3D * points, int xI, int yI, int width,
		int height, int widthImage, int n, float t, int d, Plano * modelResult,
		bool inicio) {

	bool bandera = true;
	int maxConseno = 0;
	XnPoint3D * model = (XnPoint3D *) malloc(sizeof(XnPoint3D));
	Plano * plano;
	for (int k = 0; k < n; k++) {
		if (bandera) {
			int inxP1 = 0, inxP2 = 0, inxP3 = 0;
			int x = xI + rand() % width;
			int y = yI + rand() % height;
			for (; x < xI + width; x++) {
				for (; y < yI + height; y++) {
					inxP1 = y * widthImage + x;
					if (points[inxP1].X != 0 && points[inxP1].Y != 0
							&& points[inxP1].Z != 0) {
						goto segundo;
					}
				}
			}
			segundo: x = xI + rand() % width;
			y = yI + rand() % height;
			for (; x < xI + width; x++) {
				for (; y < yI + height; y++) {
					inxP2 = y * widthImage + x;
					if (points[inxP2].X != 0 && points[inxP2].Y != 0
							&& points[inxP2].Z != 0 && inxP1 != inxP2) {
						goto tercero;
					}
				}
			}
			tercero: x = xI + rand() % width;
			y = yI + rand() % height;
			for (; x < xI + width; x++) {
				for (; y < yI + height; y++) {
					inxP3 = y * widthImage + x;
					if (points[inxP3].X != 0 && points[inxP3].Y != 0
							&& points[inxP3].Z != 0 && inxP1 != inxP3
							&& inxP2 != inxP3) {
						goto salir;
					}
				}
			}
			salir:
			//salir: cout << "inxP1:" << inxP1 << ",inxP2:" << inxP2 << ",inxP3:"
			//<< inxP3 << endl;
			plano = (Plano *) malloc(sizeof(Plano));
			XnPoint3D * normal = plano->normalDelPlano(&points[inxP1],
					&points[inxP2], &points[inxP3]);
			plano->setA(normal->X);
			plano->setB(normal->Y);
			plano->setC(normal->Z);
			plano->setD(
					-normal->X * (points[inxP1].X)
							- normal->Y * (points[inxP1].Y)
							- normal->Z * (points[inxP1].Z));
			plano->setConsenso(0);
			free(normal);
		} else {
			plano = regresion(model, maxConseno);
			plano->setConsenso(0);
		}
		int consenso = 0;
		for (int i = 0; i < height * width; i++) {
			if (points[yI * widthImage + xI + i].X != 0
					&& points[yI * widthImage + xI + i].Y != 0
					&& points[yI * widthImage + xI + i].Z != 0) {
				float distancia = plano->distanciaPunto(&points[yI * widthImage + xI + i]);
				if (distancia < t) {
					model[consenso].X = points[yI * widthImage + xI + i].X;
					model[consenso].Y = points[yI * widthImage + xI + i].Y;
					model[consenso].Z = points[yI * widthImage + xI + i].Z;
					consenso++;
					size_t tam = maxConseno + consenso + 1;
					model = (XnPoint3D *) realloc(model,
							tam * sizeof(XnPoint3D));
					// newModel->push_back(points[i]);
				}
			}
		}

		if (consenso > d && consenso > maxConseno) {
			maxConseno = consenso;
			free(modelResult);
			Plano * modelResult = (Plano*) malloc(1 * sizeof(Plano));
			bandera = false;
			modelResult->setA(plano->getA());
			modelResult->setB(plano->getB());
			modelResult->setC(plano->getC());
			modelResult->setD(plano->getD());
			modelResult->setConsenso(consenso);
		} else {
			free(model);
			model = (XnPoint3D *) malloc(sizeof(XnPoint3D));
			bandera = true;
		}
		/*cout << "A=" << plano->getA() << cout << ",B=" << plano->getB() << cout
		 << ",C=" << plano->getC() << endl;*/
	}
	free(plano);
}

Plano * Ransac::regresion(XnPoint3D * model, int tam) {
	float workspace[3][3];
	float U[3][3];
	float X[3][1];

	workspace[0][0] = 0;
	workspace[1][0] = 0;
	workspace[2][0] = 0;
	workspace[2][1] = 0;
	workspace[1][1] = 0;
	X[0][0] = 0;
	X[1][0] = 0;
	X[2][0] = 0;

	for (unsigned int i = 0; i < tam; i++) {
		workspace[0][0] = workspace[0][0] + (model[i].X * model[i].X);
		workspace[1][0] = workspace[1][0] + (model[i].X * model[i].Y);
		workspace[2][0] = workspace[2][0] + model[i].X;
		workspace[2][1] = workspace[2][1] + model[i].Y;
		workspace[1][1] = workspace[1][1] + (model[i].Y * model[i].Y);

		X[0][0] = X[0][0] + model[i].X * model[i].Z;
		X[1][0] = X[1][0] + model[i].Y * model[i].Z;
		X[2][0] = X[2][0] + model[i].Z;
	}

	workspace[2][2] = tam;
	workspace[0][1] = workspace[1][0];
	workspace[0][2] = workspace[2][0];
	workspace[1][2] = workspace[2][1];

	float L[3][3];
	/*Sacar solucion de ecuaciones por LU*/
	for (int k = 0; k < 3; k++) {
		L[k][k] = 1;
		for (int i = k + 1; i < 3; i++) {
			L[i][k] = workspace[i][k] / workspace[k][k];
			for (int j = k + 1; j < 3; j++) {
				workspace[i][j] = workspace[i][j] - L[i][k] * workspace[k][j];
			}
		}
		for (int l = 0; l < 3; l++) {
			U[k][l] = workspace[k][l];
		}
	}

	float a, b, c;
	a = X[0][0] / U[0][0];
	b = X[1][0] / U[1][1];
	c = X[2][0] / U[2][2];
	Plano * plano = (Plano *) malloc(sizeof(Plano));
	plano->setA(a);
	plano->setB(b);
	plano->setC(-1);
	plano->setD(c);
	return plano;
}

/* solve using Gaussian elimination */
void Ransac::lin_solve(float coeff[3][3], float U[3][3]) {
	float L[3][3];
	/*Sacar solucion de ecuaciones por LU*/
	for (int k = 0; k < 3; k++) {
		L[k][k] = 1;
		for (int i = k; i < 3; i++) {
			L[i][k] = coeff[i][k] / coeff[k][k];
			for (int j = k; j < 3; j++) {
				coeff[i][j] = coeff[i][j] - L[i][k] * coeff[k][j];
			}
		}
		for (int l = 0; l < 3; l++) {
			U[k][l] = coeff[k][l];
		}
	}
}
