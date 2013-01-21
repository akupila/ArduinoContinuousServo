#include <ContinuousServo.h>

// Only one servo will run at once so current consumption stays on a good level.
// We can connect a lot of servos and still be able to supply power from USB (or other limited source)

#define SERVO_COUNT 12

ContinuousServo *servos[SERVO_COUNT];	// Array of pointers to ContinuousServo objects
int currentServoIndex = 0;				// To keep track of the currently active servo

void setup()
{
	// Initialize array of servo objects
	for (unsigned int s = 0; s < SERVO_COUNT; s++)
	{
		servos[s] = new ContinuousServo(s + 2);
	}

	// Start moving the first one
	ContinuousServo *servo = servos[currentServoIndex];
	servo->step(50, processNextServo);
}

void processNextServo()
{
	currentServoIndex++;	// Move to next servo
	if (currentServoIndex < SERVO_COUNT)
	{
		ContinuousServo *servo = servos[currentServoIndex];
		servo->step(50, processNextServo); // The previous one stopped so we can move this one
	}
}

// Do nothing in loop
void loop()
{
}