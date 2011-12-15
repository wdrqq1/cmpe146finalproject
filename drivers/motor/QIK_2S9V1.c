//#include "mbed.h"
#include "QIK_2S9V1.h"

#define slow        0x10
#define med         0x40
#define fast        0x50
#define RM_coast    0x86
#define LM_coast    0x87
#define stop_cmd    0x00
#define RM_reverse  0x88
#define LM_reverse  0x8C
#define RM_forward  0x8A
#define LM_forward  0x8E

MetalGearMotor::MetalGearMotor(PinName tx, PinName rx) : _serial(tx, rx)
{
	_serial.baud(38400);
	_serial.putc(0xaa); //0xaa sets baud rate on motor controller
	wait(.05);
	_serial.putc(0x84);
	_serial.putc(0x01);
	_serial.putc(0x01);
	_serial.putc(0x55);
	_serial.putc(0x2a);
	wait(0.05);
}

void MetalGearMotor::reverse(char speed)
{
	_serial.putc(RM_reverse);
	_serial.putc(speed-0x01);
	_serial.putc(LM_reverse);
	_serial.putc(speed);
	return;
}

void MetalGearMotor::forward(char speed)
{
	_serial.putc(RM_forward);
	_serial.putc(speed);
	_serial.putc(LM_forward);
	if(speed == 0x30)
	_serial.putc(speed-0x0C);
	else if(speed == 0x40)
	_serial.putc(speed-0x0F);
	else if(speed == 0x70);
	_serial.putc(speed-0x16);
	return;
}

void MetalGearMotor::left(char speed, double time)
{
	_serial.putc(RM_reverse);
	_serial.putc(speed);

	_serial.putc(LM_forward);
	_serial.putc(speed-0x0C);
	wait(time);
	if(time != 0)
	{
		_serial.putc(RM_reverse);
		_serial.putc(stop_cmd);

		_serial.putc(LM_forward);
		_serial.putc(stop_cmd);
	}

}

void MetalGearMotor::right(char speed, double time)
{
	_serial.putc(LM_reverse);
	_serial.putc(speed-0x0C);

	_serial.putc(RM_forward);
	_serial.putc(speed);
	wait(time);
	if(time != 0)
	{
		_serial.putc(LM_reverse);
		_serial.putc(stop_cmd);

		_serial.putc(RM_forward);
		_serial.putc(stop_cmd);
	}
}

void MetalGearMotor::uturn(char speed, double time)
{
	stop();
	wait(.5);
	_serial.putc(RM_reverse);
	_serial.putc(speed);

	_serial.putc(LM_forward);
	_serial.putc(speed-0x0C);
	wait(time);
	if(time != 0)
	{
		_serial.putc(RM_reverse);
		_serial.putc(stop_cmd);

		_serial.putc(LM_forward);
		_serial.putc(stop_cmd);
	}
}

void MetalGearMotor::stop()
{
	_serial.putc(0x8A);
	_serial.putc(stop_cmd);
	_serial.putc(0x8E);
	_serial.putc(stop_cmd);
}

void MetalGearMotor::correctToLeft(char speed)
{
	_serial.putc(LM_forward);
	_serial.putc(speed-0x19);
	_serial.putc(RM_forward);
	_serial.putc(speed);
}

void MetalGearMotor::correctToRight(char speed)
{
	_serial.putc(LM_forward);
	_serial.putc(speed-0x9);
	_serial.putc(RM_forward);
	_serial.putc(speed-0x0F);
}

void MetalGearMotor::pd_Control(int sensorError, int sensorError2, char speed)
{
	/*  Equation for PD control
	 c = (E*Kp)+(Ed*Kd)
	 c = value added to speed of motor
	 E = sensorError
	 Kp = gain constant (trial and error)
	 Ed = sensorError - sensorError(previous)
	 Kd = derivitive gain constant (trial and error)
	 */

	int c = 0, E = sensorError, Kp = 3, Ed = 0, Kd = 3;
	char changeSpeed = 0x00;
	Ed = sensorError - sensorError2;
	c = (E*Kp)+(Ed*Kd);

	if(c >= 0)
	{
		for (int i = 0; i < c; i++)
		changeSpeed += 0x01;
		if(speed+changeSpeed <= 0x7F)
		{
			_serial.putc(RM_forward);
			_serial.putc(speed+changeSpeed);
		}
	}
	else if(c <= 0)
	{
		for (int i = 0; i < c; i++)
		changeSpeed += 0x01;
		if(speed+changeSpeed+0x0C <= 0x7F)
		{
			_serial.putc(LM_forward);
			_serial.putc(speed+0x0C+changeSpeed);
		}
	}
}
