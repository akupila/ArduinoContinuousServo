/*
	ContinuousServo.h v.01 - Library for controlling continuous servo motors similar to stepper motors
	Copyright (c) 2013 Antti Kupila, http://supernode.io All rights reserved.
	Loosely based on Servo library by Michael Margolis

	Uses timer1

	vim: set ts=4:
*/

#ifndef ContinuousServo_h
#define ContinuousServo_h

#ifndef CONTINUOUS_SERVO_CLOCKSPEED
#define CONTINUOUS_SERVO_CLOCKSPEED 16000000
#endif // CONTINUOUS_SERVO_CLOCKSPEED
#ifndef CONTINUOUS_SERVO_MIN_PULSE
#define CONTINUOUS_SERVO_MIN_PULSE 544 // From Servo.h
#endif // CONTINUOUS_SERVO_MIN_PULSE
#ifndef CONTINUOUS_SERVO_MAX_PULSE
#define CONTINUOUS_SERVO_MAX_PULSE 2402 // From Servo.h
#endif // CONTINUOUS_SERVO_MAX_PULSE
#ifndef CONTINUOUS_SERVO_REFRESH_CYCLE
#define CONTINUOUS_SERVO_REFRESH_CYCLE 20000 // 20ms, 20000us. Standard for most servos. Decrease if the servo is jittering
#endif // CONTINUOUS_SERVO_REFRESH_CYCLE

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

typedef struct {
	unsigned int pin;
	long step;
	long targetStep;
	int direction;
	unsigned int stepSize; // in ticks
	bool active;
	void (*callback)();
} servo_t;

class ContinuousServo
{
public:
	ContinuousServo(uint8_t pin, unsigned int minPulse = CONTINUOUS_SERVO_MIN_PULSE, unsigned int maxPulse = CONTINUOUS_SERVO_MAX_PULSE);
	void step(int steps, void (*isr)() = NULL);
	void stepTo(int absoluteStep, void (*isr)() = NULL);
	void stop();
	int getSteps();
	void storeSteps(int steps);
	char getDirection();
	bool isBusy();
private:
	uint8_t index;
	int pin;
	unsigned int minPulse;
	unsigned int maxPulse;

	void initializeTimer(); // Uses timer1 for a 20ms timer
};

#endif // ContinuousServo_h