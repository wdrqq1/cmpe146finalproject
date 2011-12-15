/*
 * QIK_259V1.h
 *
 *  Created on: Dec 3, 2011
 *      Author: tony_sid
 */

#ifndef QIK_259V1_H_
#define QIK_259V1_H_

#include "../mbed/mbed.h"

using namespace mbed;

struct MetalGearMotor
{


	mbed:Serial _serial;
};


void MetalGearMotor(PinName tx, PinName rx);
	void forward(char speed);
	void reverse(char speed);
	void right(char speed, double time);
	void left(char speed, double time);
	void correctToRight(char speed);
	void correctToLeft(char speed);
	void uturn(char speed, double time);
	void stop();
	void pd_Control(int sensorError, int sensorError2, char speed);


#endif /* QIK_259V1_H_ */
