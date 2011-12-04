#include "QTR8RC.h"
#include "mbed.h"
#include "TextLCD.h"
#include <sstream>

TextLCD lcd2(p16,p15,p14,p13,p12,p11);
//LocalFileSystem local("local");

static float sensorData[8];
static char BW[9];
static float BWValue;

ReflectanceSensor::ReflectanceSensor(PinName s1, PinName s2, PinName s3, PinName s4, PinName s5,
PinName s6, PinName s7, PinName s8) : _s1(s1), _s2(s2), _s3(s3), _s4(s4),
_s5(s5), _s6(s6), _s7(s7), _s8(s8){
    _s1 = 20, _s2 = 21, _s3 = 22, _s4 = 23, _s5 = 24, _s6 = 25, _s7 = 26, _s8 = 27;
    BWValue = 0.000270;
}

char* ReflectanceSensor::read() {
    Timer t1, t2, t3, t4, t5, t6, t7, t8;
    _s1.output();
    _s1=1;
    _s2.output();
    _s2=1;
    _s3.output();
    _s3=1;
    _s4.output();
    _s4=1;
    _s5.output();
    _s5=1;
    _s6.output();
    _s6=1;
    _s7.output();
    _s7=1;
    _s8.output();
    _s8=1;

    //wait 10us
    wait(0.000002);

    t1.start();
    while(1)
    {
        _s1.input();
        if(_s1 == 0)
        {
            t1.stop();
            sensorData[0] = t1;
            break;
        }
    }
        t2.start();
    while(1)
    {
        _s2.input();
        if(_s2 == 0)
        {
            t2.stop();
            sensorData[1] = t2;
            break;
        }
    }
        t3.start();
    while(1)
    {
        _s3.input();
        if(_s3 == 0)
        {
            t3.stop();
            sensorData[2] = t3;
            break;
        }
    }
        t4.start();
    while(1)
    {
        _s4.input();
        if(_s4 == 0)
        {
            t4.stop();
            sensorData[3] = t4;
            break;
        }
    }
        t5.start();
    while(1)
    {
        _s5.input();
        if(_s5 == 0)
        {
            t5.stop();
            sensorData[4] = t5;
            break;
        }
    }
        t6.start();
    while(1)
    {
        _s6.input();
        if(_s6 == 0)
        {
            t6.stop();
            sensorData[5] = t6;
            break;
        }
    }
        t7.start();
    while(1)
    {
        _s7.input();
        if(_s7 == 0)
        {
            t7.stop();
            sensorData[6] = t7;
            break;
        }
    }
        t8.start();
    while(1)
    {
        _s8.input();
        if(_s8 == 0)
        {
            t8.stop();
            sensorData[7] = t8;
            break;
        }
    }
    for(int i = 0; i < 8; i++)
    {
    if(sensorData[i] < BWValue)
        BW[i] = '0';
    else
        BW[i] = '1';
    }
    BW[8] = '\0';
    /*
    FILE *fp = fopen("/local/out.txt", "w");  // Open "out.txt" on the local file system for writing
    fprintf(fp,"%f, %f, %f, %f, %f, %f, %f, %f\n\n",sensorData[0],sensorData[1],sensorData[2],sensorData[3],sensorData[4],sensorData[5],sensorData[6],sensorData[7]);
    fclose(fp);
    */
    return BW;
}

int ReflectanceSensor::convertToErrorNumber(char* sensorData)
{
int sensorError = 0;

if(strcmp(sensorData,"10000000")==0)
    sensorError = -7;
else if(strcmp(sensorData,"11000000")==0)
    sensorError = -6;
else if(strcmp(sensorData,"01000000")==0)
    sensorError = -5;
else if(strcmp(sensorData,"01100000")==0)
    sensorError = -4;
else if(strcmp(sensorData,"00100000")==0)
    sensorError = -3;
else if(strcmp(sensorData,"00110000")==0)
    sensorError = -2;
else if(strcmp(sensorData,"00010000")==0)
    sensorError = -1;
else if(strcmp(sensorData,"00011000")==0)
    sensorError = 0;
else if(strcmp(sensorData,"00001000")==0)
    sensorError = 1;
else if(strcmp(sensorData,"00001100")==0)
    sensorError = 2;
else if(strcmp(sensorData,"00000100")==0)
    sensorError = 3;
else if(strcmp(sensorData,"00000110")==0)
    sensorError = 4;
else if(strcmp(sensorData,"00000010")==0)
    sensorError = 5;
else if(strcmp(sensorData,"00000011")==0)
    sensorError = 6;
else if(strcmp(sensorData,"00000001")==0)
    sensorError = 7;
else if(strcmp(sensorData,"00000000")==0)
    sensorError = 8;
else if(strcmp(sensorData,"11110011")==0 || strcmp(sensorData,"11100111")==0 || strcmp(sensorData,"11001111")==0 ||strcmp(sensorData,"10011111")==0 || strcmp(sensorData,"10111111")==0 || strcmp(sensorData,"11011111")==0 || strcmp(sensorData,"11101111")==0 || strcmp(sensorData,"11110111")==0 || strcmp(sensorData,"11111011")==0 || strcmp(sensorData,"11111101")==0)
    sensorError = -30;
else if(strcmp(sensorData,"11110000")==0 || strcmp(sensorData,"11100000")==0 || strcmp(sensorData,"11111000")==0 || strcmp(sensorData,"11111100")==0)
    sensorError = -10;
else if(strcmp(sensorData,"00001111")==0 || strcmp(sensorData,"00000111")==0 || strcmp(sensorData,"00011111")==0 || strcmp(sensorData,"00111111")==0)
    sensorError = -20;
else
    return sensorError = -30;

return sensorError;
}

void ReflectanceSensor::calibrate() {
    Timer t1, t2, t3, t4, t5, t6, t7, t8;
    _s1.output();
    _s1=1;
    _s2.output();
    _s2=1;
    _s3.output();
    _s3=1;
    _s4.output();
    _s4=1;
    _s5.output();
    _s5=1;
    _s6.output();
    _s6=1;
    _s7.output();
    _s7=1;
    _s8.output();
    _s8=1;

    //wait 10us
    wait(0.000002);

    t1.start();
    while(1)
    {
        _s1.input();
        if(_s1 == 0)
        {
            t1.stop();
            sensorData[0] = t1;
            break;
        }
    }
        t2.start();
    while(1)
    {
        _s2.input();
        if(_s2 == 0)
        {
            t2.stop();
            sensorData[1] = t2;
            break;
        }
    }
        t3.start();
    while(1)
    {
        _s3.input();
        if(_s3 == 0)
        {
            t3.stop();
            sensorData[2] = t3;
            break;
        }
    }
        t4.start();
    while(1)
    {
        _s4.input();
        if(_s4 == 0)
        {
            t4.stop();
            sensorData[3] = t4;
            break;
        }
    }
        t5.start();
    while(1)
    {
        _s5.input();
        if(_s5 == 0)
        {
            t5.stop();
            sensorData[4] = t5;
            break;
        }
    }
        t6.start();
    while(1)
    {
        _s6.input();
        if(_s6 == 0)
        {
            t6.stop();
            sensorData[5] = t6;
            break;
        }
    }
        t7.start();
    while(1)
    {
        _s7.input();
        if(_s7 == 0)
        {
            t7.stop();
            sensorData[6] = t7;
            break;
        }
    }
        t8.start();
    while(1)
    {
        _s8.input();
        if(_s8 == 0)
        {
            t8.stop();
            sensorData[7] = t8;
            break;
        }
    }

    BWValue = 0;
    BWValue = 3*(sensorData[3]+sensorData[4]);
    BWValue += (sensorData[0]+sensorData[1]+sensorData[2]+sensorData[5]+sensorData[6]+sensorData[7]);
    BWValue /= 12;
    float BWValue2;
    stringstream ss(stringstream::in | stringstream::out);
    string value;
    /*
    for(int i = 0; i < 8; i++)
    {
        sensorData[i] = sensorData[i]*1000000;
        sensorData[i] = (float)((int)(sensorData[i]*100)/100);
        ss << sensorData[i];
        value = ss.str();
        lcd2.printf("%.0f,",sensorData[i]);
    }*/
    BWValue2 = BWValue*1000000;
    BWValue2 = (float)((int)(BWValue2*100)/100);
    ss << BWValue2;
    value = ss.str();
    lcd2.cls();
    lcd2.printf("%.0f",BWValue2);
    wait(2);
    lcd2.cls();
}
