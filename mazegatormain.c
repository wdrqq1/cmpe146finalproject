#include "mbed.h"
#include "QTR8RC.h"
#include "QIK_2S9V1.h"
#include "TextLCD.h"
#include "DebounceIn.h"

const char slow = 0x30;
const char med = 0x40;
const char fast = 0x70;

Serial usb(USBTX,USBRX);
ReflectanceSensor sensor(p17,p18,p19,p20,p21,p22,p23,p24);
MetalGearMotor motor(p9,p10);
TextLCD lcd(p16,p15,p14,p13,p12,p11);
DebounceIn GoSP(p29);
DebounceIn calibrate(p28);
DebounceIn GoLH(p30);
LocalFileSystem local("local");


char path[100];
int input = 0;

int intersectionSP(char speed, int sensorError)
{
    motor.stop();
    wait(.1);
    motor.forward(slow);
    wait(.375);
    motor.stop();
    if(path[input] == 'L')
    {
            int error;
            char* pError;
            motor.left(slow,0);
            wait(.25);
            do
            {
                pError=sensor.read();
                error = sensor.convertToErrorNumber(pError);
                lcd.locate(0,1);
                lcd.printf(pError);
                wait(.005);
            }while(error != 0);
            motor.stop();
            wait(.5);
            input++;
            return 0;
    }

    else if(path[input] == 'R')
    {
            int error;
            char* pError;
            motor.right(slow,0);
            wait(.25);
            do
            {
                pError=sensor.read();
                error = sensor.convertToErrorNumber(pError);
                lcd.locate(0,1);
                lcd.printf(pError);
                wait(.005);
            }while(error != 0);
            motor.stop();
            wait(.5);
            input++;
            return 0;
    }

    else if(path[input] == 'U')
    {
            int error;
            char* pError;
            motor.uturn(slow,0);
            wait(.75);
            do
            {
                pError=sensor.read();
                error = sensor.convertToErrorNumber(pError);
                lcd.locate(0,1);
                lcd.printf(pError);
                wait(.005);
            }while(error != 0);

            motor.stop();
            wait(.5);
            input++;
    }
    else if(path[input] == 'F')
    {
        motor.stop();
        return 1;
    }
    else
    {
        motor.forward(slow);
        input++;
        return 0;
    }
    return 0;
}

int GoShort(int speed)
{
        char* sensorValues = sensor.read();
        wait(.05);
        char* sensorValues2 = sensor.read();
        lcd.locate(0,0);
        lcd.printf(sensorValues2);
        int sensorError = sensor.convertToErrorNumber(sensorValues);
        int sensorError2 = sensor.convertToErrorNumber(sensorValues2);


        if(sensorError == -30 || sensorError == -20 || sensorError == -10) //Found an intersection
        {
            int stop = intersectionSP(slow, sensorError);//Returns 1 if Finish is found, anything else 0 is returned
            if(stop == 1)
            {
                return 1;
            }
            else
                return 0;
        }

        else if(sensorError == 0) //MazeGator is centered on line, so go straight
        {
            motor.forward(speed);
        }
        else if(sensorError < 8 && sensorError > -8)//MazeGator is not centered use PD Control to correct
        {
            motor.pd_Control(sensorError, sensorError2, speed);
        }

        return 0;  //Keep going until Finish
}

void readPath()
{
    FILE *fp = fopen("/local/Path.txt", "r");
    fscanf(fp,"%s",path);
    lcd.locate(0,1);
    lcd.printf(path);
    fclose(fp);
    input = 0;

}

void writePath()
{

    FILE *fp = fopen("/local/Path.txt", "w");
    fprintf(fp,path);
    lcd.locate(0,1);
    lcd.printf(path);
    fclose(fp);
    input = 0;
    return;
}

void shortestPath()
{
    lcd.cls();
    lcd.printf("Busy");
    wait(.5);
    int i = 0;
    while(path[i] != 'F')
    {
        if(path[i+1] != 'U')
        {
            ++i;
        }
        else
        {
            if(path[i] == 'R' && path[i+2] == 'R')
                path[i] = 'S';
            else if(path[i] == 'L' && path[i+2] == 'L')
                path[i] = 'S';
            else if(path[i] == 'L' && path[i+2] == 'R')
                path[i] = 'U';
            else if(path[i] == 'L' && path[i+2] == 'S')
                path[i] = 'R';
            else if(path[i] == 'R' && path[i+2] == 'L')
                path[i] = 'U';
            else if(path[i] == 'S' && path[i+2] == 'L')
                path[i] = 'R';
            else if(path[i] == 'S' && path[i+2] == 'R')
                path[i] = 'L';
            else if(path[i] == 'S' && path[i+2] == 'S')
                path[i] = 'U';
            ++i;
            while(path[i] != 'F')
            {
                path[i] = path[i+2];
                ++i;
            }
            path[i] = 'F';
            i = 0;
        }

    }
    path[i+1] = '\0';
    lcd.cls();
    lcd.printf("Done!");
    return;
}

void inputTurn(char turn)
{
    if(input < 99)
    {
        path[input] = turn;
        input++;

        if(turn == 'F')
        {
            path[input] = '\0';
            shortestPath();
            writePath();
        }
    }
}

int intersection(char speed, int sensorError)
{
    char* sensorValues2;
    motor.stop();
    wait(.5);
    sensorError = sensor.convertToErrorNumber(sensorValues2 = sensor.read());
    lcd.cls();
    lcd.printf(sensorValues2);
    motor.forward(slow);
    wait(.375);
    motor.stop();
    char* sensorValues;
    int sensorError2 = sensor.convertToErrorNumber(sensorValues = sensor.read());
    lcd.printf("\n");
    lcd.printf(sensorValues);
    wait(2);


    if(sensorError == -30) //Could be finish or T or 4 way intersection
    {
        if(sensorError2 == -30)
            return 1;
        else
        {
            int error;
            char* pError;
            motor.left(slow,0);
            inputTurn('L');
            wait(.25);
            do
            {
                pError=sensor.read();
                error = sensor.convertToErrorNumber(pError);
                lcd.locate(0,1);
                lcd.printf(pError);
                wait(.005);
            }while(error != 0);
            motor.stop();
            wait(.5);
        }
    }

    else if(sensorError == -20)//Could be R only or R and S
    {
        if(sensorError2 == 8)
        {
            int error;
            char* pError;
            motor.right(slow,0);
            inputTurn('R');
            wait(.25);
            do
            {
                pError=sensor.read();
                error = sensor.convertToErrorNumber(pError);
                lcd.locate(0,1);
                lcd.printf(pError);
                wait(.005);
            }while(error != 0);
            motor.stop();
            wait(.5);
        }
        else
        {
            motor.forward(speed);
            inputTurn('S');
            return 0;
        }
    }

    else if(sensorError == -10)
    {
            int error;
            char* pError;
            motor.left(slow,0);
            inputTurn('L');
            wait(.25);
            do
            {
                pError=sensor.read();
                error = sensor.convertToErrorNumber(pError);
                lcd.locate(0,1);
                lcd.printf(pError);
                wait(.005);
            }while(error != 0);

            motor.stop();
            wait(.5);
    }
    return 0;
}

int GoLeftHand(int speed)
{
        char* sensorValues = sensor.read();
        wait(.05);
        char* sensorValues2 = sensor.read();
        lcd.locate(0,0);
        lcd.printf(sensorValues2);
        int sensorError = sensor.convertToErrorNumber(sensorValues);
        int sensorError2 = sensor.convertToErrorNumber(sensorValues2);

        if(sensorError == -30 || sensorError == -20 || sensorError == -10) //Found an intersection
        {
            int stop = intersection(slow, sensorError);//Returns 1 if Finish is found, anything else 0 is returned
            if(stop == 1)
            {
                inputTurn('F');
                //motor.stop();
                return 1;
            }
            else
                return 0;
        }
        else if(sensorError == 8) //Dead End
        {
            int error;
            char* pError;
            motor.uturn(slow,0);
            inputTurn('U');
            wait(.25);
            do
            {
                pError=sensor.read();
                error = sensor.convertToErrorNumber(pError);
                lcd.locate(0,1);
                lcd.printf(pError);
                wait(.001);
            }while(error != 0 );
            motor.stop();
            wait(.5);
            return 0;
        }
        else if(sensorError == 0) //MazeGator is centered on line, so go straight
        {
            motor.forward(speed);
            return 0;
        }
        else if(sensorError < 8 && sensorError > -8)//MazeGator is not centered use PD Control to correct
        {
            motor.pd_Control(sensorError, sensorError2, speed);
            return 0;
        }

        return 0;  //Keep going until Finish is found
}



int main()
{
    int stop = 0;
    input = 0;
    int SP,LH,C;
    lcd.cls();
    while(1)
    {
        SP = 0;
        LH = 0;
        C = 0;
        stop = 0;
        lcd.locate(0,0);
        lcd.printf("1:LeftHand 2:Short 3:Calibrate");
        SP = GoSP.read();
        LH = GoLH.read();
        C = calibrate.read();
        if(LH == 1)
        {
            lcd.cls();
            lcd.printf("GO");
            wait(3);
            motor.forward(slow);
            while(stop != 1)
                stop = GoLeftHand(slow);
            motor.stop();
            wait(5);
        }
        else if(SP == 1)
        {
            readPath();
            lcd.cls();
            lcd.printf("GO");
            wait(3);
            motor.forward(slow);
            while(stop != 1)
                stop = GoShort(slow);

            motor.stop();
            input = 0;
            while(input < 100)
            {
                path[input]=NULL;
                input++;
            }
        }
        else if(C == 1)
        {
            lcd.cls();
            lcd.printf("Calibrating");
            wait(3);
            sensor.calibrate();
            lcd.cls();
        }
    }
}


