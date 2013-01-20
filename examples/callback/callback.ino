#include <ContinuousServo.h>

// Initialize ContinuousServo library with servo signal wire connected to pin 2
ContinuousServo servo(2); 

void setup()
{
	Serial.begin(57600);
	
	// Step 50 steps forward, then output steps
	servo.step(50, stepDone);
}

void stepDone()
{
	Serial.print(F("Servo completed. Now at "));
	Serial.print(servo.getSteps());
	Serial.println(F(" steps."));
}

// Do nothing in loop
void loop()
{
}