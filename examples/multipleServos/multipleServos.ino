#include <ContinuousServo.h>

// NOTE: If you plan on connecting a lot of servos make sure you have a separate power supply. Count with 0.2A/servo
// Another option is to run servos in sequence using the callback, see 'multipleServosCallback example

#define SERVO_COUNT 3	// 3 is pretty much the max without external power (~170mA * 3 = 0.51A..)

ContinuousServo *servos[SERVO_COUNT];	// Array of pointers to ContinuousServo objects

void setup()
{
	// Initialize array of servo objects
	for (unsigned int s = 0; s < SERVO_COUNT; s++)
	{
		servos[s] = new ContinuousServo(s + 2);
	}
	
	// Move all of them at once
	for (int i = 0; i < SERVO_COUNT; i++)
	{
		ContinuousServo *servo = servos[i];
		servo->step(i * 50 + 50);
	}
}

// Do nothing in loop
void loop()
{
}