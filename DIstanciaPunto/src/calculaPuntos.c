#include <stdio.h>
#include <stdlib.h>

#define N 100

int main(int argc, char** argv) {

	if (argc != 2) {
		printf(
				"No se permite correr este programa, se debe indicar por que método se debe de realizar");
		return 0;
	}

	char * opcionStr = argv[1];

	int opcion = (int)atoi(opcionStr);

	float puntos[N][2] = { { 19, -25 }, { 30, 93 }, { 38, -87 }, { 33, -13 }, {
			-45, 90 }, { 61, 71 }, { 40, 70 }, { 20, -25 }, { 44, 82 }, { -73,
			48 }, { 73, -73 }, { -35, -18 }, { 36, -96 }, { 1, 25 }, { 90, 22 },
			{ -82, 5 }, { -77, 5 }, { -45, 92 }, { -8, -23 }, { 36, -24 }, {
					-19, 22 }, { 98, -70 }, { 44, 71 }, { 8, -93 }, { -65, 51 },
			{ 93, -90 }, { -2, 53 }, { 51, 14 }, { 30, 56 }, { 16, 79 }, { -48,
					41 }, { 37, -21 }, { 100, 12 }, { -34, 95 }, { -98, -34 }, {
					-79, -57 }, { 32, 19 }, { -82, 73 }, { 56, -74 },
			{ 77, -36 }, { -66, -17 }, { 100, -40 }, { 82, 46 }, { 85, -2 }, {
					-94, -74 }, { -93, 93 }, { -52, -59 }, { 71, -36 }, { -43,
					-79 }, { 76, -57 }, { -3, 17 }, { -64, 7 }, { 33, -22 }, {
					-89, 34 }, { 27, -22 }, { -75, 64 }, { 25, -60 },
			{ -50, -71 }, { 54, 87 }, { -30, 62 }, { -47, 2 }, { 32, 28 }, { 90,
					-36 }, { 71, 24 }, { -11, -69 }, { 27, -40 }, { -61, 71 }, {
					-62, -2 }, { -92, -16 }, { -29, 7 }, { -82, -83 },
			{ 40, -81 }, { -10, 47 }, { 67, -72 }, { 56, -82 }, { -84, 13 }, {
					4, 70 }, { -50, 75 }, { -9, -80 }, { -55, 56 }, { -79, -7 },
			{ 25, -93 }, { -43, -16 }, { -10, -4 }, { 97, -5 }, { 2, -33 }, {
					68, 47 }, { -89, -52 }, { -28, 54 }, { 28, -20 },
			{ 57, -42 }, { -6, 4 }, { 22, -56 }, { 18, 31 }, { 21, -41 }, { -63,
					3 }, { 35, 74 }, { 49, -18 }, { -22, -8 }, { 79, 45 } };

	switch (opcion) {
	case (1): {
		float pendiente = (puntos[1][1] - puntos[0][1])
				/ (puntos[1][0] - puntos[0][0]);
		float ordenada = -pendiente * puntos[1][0] + puntos[1][1];
		float y;

		printf("Punto2 recta (%f,%f) \n", puntos[0][0], puntos[0][1]);
		printf("Punto1 recta (%f,%f) \n", puntos[1][0], puntos[1][1]);

		printf("Pendiente = %f\n", pendiente);
		printf("Ordenada = %f \n", ordenada);

		printf("Se valida los puntos en ecuacion\n");
		printf("Punto (%f , %f) \n", puntos[0][0],
				puntos[0][0] * pendiente + ordenada);
		printf("Punto (%f , %f) \n", puntos[1][0],
				puntos[1][0] * pendiente + ordenada);

		int i;

		for (i = 2; i < N; i++) {
			y = puntos[i][0] * pendiente + ordenada;
			printf("valor y = %f \n", y);
			if (y < puntos[i][1]) {
				printf("Punto P(%f,%f) esta por debajo de la recta \n",
						puntos[i][0], puntos[i][1]);
			} else {
				printf("Punto P(%f,%f) esta por arriba de la recta \n",
						puntos[i][0], puntos[i][1]);
			}
		}
		break;
	}

	case (2): {
		int i = 0;
		int primerPuntoId = 0;
		int segundoPuntoId = 1;
		if (puntos[0][0] > puntos[1][0]) {
			primerPuntoId = 1;
			segundoPuntoId = 0;
		}
		printf("Primer punto P(%f,%f)\n", puntos[primerPuntoId][0],
				puntos[primerPuntoId][1]);
		printf("Segundo punto P(%f,%f)\n", puntos[segundoPuntoId][0],
				puntos[segundoPuntoId][1]);
		for (i = 2; i < N; i++) {

			double determinante = (puntos[i][0] - puntos[primerPuntoId][0])
					* (puntos[segundoPuntoId][1] - puntos[primerPuntoId][1])
					- (puntos[i][1] - puntos[primerPuntoId][1])
							* (puntos[segundoPuntoId][0]
									- puntos[primerPuntoId][0]);
			printf("determinante = %f \n", determinante);
			if (determinante < 0) {
				printf("Punto P(%f,%f) esta por debajo de la recta \n",
						puntos[i][0], puntos[i][1]);
			} else if (determinante > 0) {
				printf("Punto P(%f,%f) esta por arriba de la recta \n",
						puntos[i][0], puntos[i][1]);
			}
		}
		break;
	}

	}

}
