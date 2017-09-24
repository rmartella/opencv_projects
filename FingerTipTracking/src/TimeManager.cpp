/*
 * TimeManager.cpp
 *
 *  Created on: 24/11/2015
 *      Author: rey
 */

#include "TimeManager.h"

TimeManager::TimeManager() {
}

TimeManager::~TimeManager() {
}

double TimeManager::CalculateFrameRate(bool writeToConsole) {
	// Variables estaticas que almacenan los incrementos del tiempo
	static double framesPerSecond = 0.0f; // Se almacenan los frames por segundo
	static double startTicks = getTickCount(); // Se almacena el tiempo de inicio
	static double lastTicks = getTickCount(); //  Se almacena el tiempo del ulimo frame
	static char strFrameRate[50] = { 0 }; // Almacenamos la cadena para el titulo de la ventana
	static double currentFPS = 0.0f; // Se almacena el valor actual de los frames por segundos

	currentTicks = getTickCount();
	int DeltaTime = currentTicks - lastTicks;
	++framesPerSecond;
	if ((currentTicks - startTicks) / getTickFrequency() > 1.0f) {
		startTicks = currentTicks;
		if (writeToConsole)
			fprintf(stderr, "Current Frames Per Second: %d\n",
					int(framesPerSecond));
		currentFPS = framesPerSecond;
		framesPerSecond = 0;
	}
	return currentFPS;
}

