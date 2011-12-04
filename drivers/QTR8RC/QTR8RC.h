/*
 * QTR8RC.h
 *
 *  Created on: Dec 3, 2011
 *      Author: neuromancer
 */

#ifndef QTR8RC_H_
#define QTR8RC_H_

#include "mbed.h"

class ReflectanceSensor {
public:
    ReflectanceSensor(PinName s1, PinName s2, PinName s3, PinName s4, PinName s5, PinName s6,
                        PinName s7, PinName s8);
    char* read();
    void calibrate();
    void convertToBW();
    int  convertToErrorNumber(char* sensorData);


private:
    DigitalInOut _s1;
    DigitalInOut _s2;
    DigitalInOut _s3;
    DigitalInOut _s4;
    DigitalInOut _s5;
    DigitalInOut _s6;
    DigitalInOut _s7;
    DigitalInOut _s8;
};

#endif /* QTR8RC_H_ */
