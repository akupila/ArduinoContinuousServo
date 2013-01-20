#include <ContinuousServo.h>

// Not all servos are created equal. Set min and max timing (defaults are min: 544, max: 2402).
// Use this to try to get the steps forward and backwards to match
ContinuousServo servo(2, 564, 1733);

int steps = 250;
int step = 0;
boolean fwd = true;

void setup()
{
	Serial.begin(57600);
	
	servo.step(steps, doneStepping);
}

void loop()
{
	if (step != 0)
	{
		delay(2000);
		servo.step(step, doneStepping);
		step = 0;
	}
}

void doneStepping()
{
	Serial.println(F("Done stepping, switching direction"));

	fwd = !fwd;
	step = fwd ? steps : -steps;
}