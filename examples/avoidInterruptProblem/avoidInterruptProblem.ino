#include <ContinuousServo.h>
#include <SoftwareSerial.h>

// Initialize ContinuousServo library with servo signal wire connected to pin 2
ContinuousServo servo(2); 
SoftwareSerial softSerial(A2, A3);

void setup()
{
	Serial.begin(57600);
	softSerial.begin(9600);

	// Step 100 steps forward
	servo.step(100);

}

// Do nothing in loop
void loop()
{
	if (!servo.isBusy())
	{
		// SoftwareSerial will disable interrupts and make the servo(s) not function correctly
		// Only do something that might disable interrupts when isBusy() is false
		if (softSerial.available())
		{
			Serial.print(softSerial.read());
		}
	}
}