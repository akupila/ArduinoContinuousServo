#include <ContinuousServo.h>

// Initialize ContinuousServo library with servo signal wire connected to pin 2
ContinuousServo servo(2); 

void setup()
{
	// Step 100 steps forward
	servo.step(100);

	// Wait 5sec (..while servo is running "in the background")
	delay(5000);

	// Step 100 steps backwards
	servo.step(-100);
}

// Do nothing in loop
void loop()
{
}