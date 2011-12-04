/*
 * QIK_259V1.h
 *
 *  Created on: Dec 3, 2011
 *      Author: tony_sid
 */

#ifndef QIK_259V1_H_
#define QIK_259V1_H_

#include "mbed.h"



class MetalGearMotor{
    public:
        MetalGearMotor(PinName tx, PinName rx);
        void forward(char speed);
        void reverse(char speed);
        void right(char speed, double time);
        void left(char speed, double time);
        void correctToRight(char speed);
        void correctToLeft(char speed);
        void uturn(char speed, double time);
        void stop();
        void pd_Control(int sensorError, int sensorError2, char speed);

    private:
        Serial _serial;
};

#endif


#endif /* QIK_259V1_H_ */
