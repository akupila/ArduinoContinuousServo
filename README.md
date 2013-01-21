ContinuousServo
======================

An Arduino library for controlling continuous (full rotation) servos similar to stepper motors.


Limitations
-------------------------

Sorry to blow the fluffy duffy dream that you might see here: servos are notoriously inaccurate. While applications may differ you'll likely still need some sort of calibration in software.

ContinuousServo uses the AVR timer1, same as the [Servo library]. This means that both cannot be used at once. Also due to timer1 being used PWM on pins 9 and 10 won't work (3, 5, 6 and 11 are still fine though)


Support
-------------------------

The library currently has only been tested with ATMega328 but should work with everything from ATMega8 and up.


Why?
-------------------------

Servo motors are easy to use and require minimal hardware. You simply connect the power lines and a signal line to the digital output pin of the Arduino and you have a simple motor to control. However servos have limited motion, usually 0-180 degrees. Servos that rotate more (usually called sail winch servos) exist but they're limited to max 1260 degrees (3.5 turns) too. What if you want something that rotates more?

Stepper motors are great for accurate control and continuous rotation both ways, mostly because the steps are actually physical inside the motor. Most of the time they're the preferred solution for an application that requires these properties. The downside of steppers is that they require extra hardware; you cannot simply connect a servo to an Arduino or other microcontroller and be done with it. For 1 or 2 stepper motors this might be fine but controlling 10 or more becomes a problem (or at least expensive).

The ContinuousServo library attempts to bridge the gap by providing a way to use continuous servo motors in a similar way as stepper motors.


How servos work
-------------------------

A servo motor expects a pulse every ~20ms. The length of this pulse determines the angle for regular servos or speed for continuous servos. For this we'll only consider continuous servos, the regular [Servo library] is already great for normal servos.

A pulse of 1.5ms sets the zero position, a shorter pulse makes the servo turn left and a longer one makes it turn right. By counting these pulses we can simulate a stepper motor using servos.

![1ms = -90, 1.5ms = 90, 2ms = 180](https://raw.github.com/akupila/ArduinoContinuousServo/master/docs/operation.png "Servo duty cycle")

#### Example

`servo.step(100);` Gives the servo 100 pulses, a 2ms pulse every 20ms. The servo turns right for approx 2sec.

`servo.step(-100);` Gives the servo 100 1ms pulses, making it turn left instead


Using the library
-------------------------

Here's a simple example:

```c
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
```

Note that as the servo is using timer1 it's running independent of delay() etc, similar to running on a separate thread. In the example above the servo will run for 100 steps, 100 x 20ms = 2sec, and run again 3sec after it has stopped.

#### Callbacks

As the servo runs separate from the main loop it may be good to know when its motion has finished. For this we can use callbacks:

```c
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
```


#### Calibration
Not all servos are created equal. Even servos with the exact same model can (likely will) be different from each other. This is due to the way servos work internally.

You can adjust the min and max pulse that should be sent. If the datasheet for the servo states these values they're likely good to use, otherwise trial and error might be necessary.

I found a full backwards pulse to be 564us and a full forward pulse to be 1733us for the [medium continuous servo at sparkfun]. This makes the servo return to its starting position when going X steps forward and the same amount of steps backwards.

You can define the pulse lengths in the constructor:
`ContinuousServo servo(2, 564, 1733);`

```c
#include <ContinuousServo.h>

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
```

It's possible the servo moves faster in one direction than the other. In this case can you can vary the number of steps depending on direction.


Conclusion
-------------------------
Don't expect this to be perfect, it won't be. This is just the reality we have to live with when working with cheap servo motors. It still pushes accuracy to the highest possible level with these guys, the normal [Servo library] won't be as easy to work with as you cannot determine the number of pulses a servo got.

Any suggestions, bugfixes or optimizations are highly appreciated!


[Servo library]: http://arduino.cc/en/Reference/Servo
[medium continuous servo at sparkfun]: https://www.sparkfun.com/products/10189