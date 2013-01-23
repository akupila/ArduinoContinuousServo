/*
	ContinuousServo.h v.01 - Library for controlling continuous servo motors similar to stepper motors
	Copyright (c) 2013 Antti Kupila, http://supernode.io All rights reserved.
	Loosely based on Servo library by Michael Margolis

	Uses timer1

	vim: set ts=4:
*/

#include <avr/interrupt.h>
#include <Arduino.h> 

#include "ContinuousServo.h"

#define TRIM_DURATION	3

#define usToTicks(_us)	(( clockCyclesPerMicrosecond() * _us) / 8)	// converts microseconds to ticks, assumes /8 prescaler

static volatile int8_t activeServo = -1;	// index of current servo. -1 = no servos need to be pulsed in this duty cycle
static volatile servo_t servos[12];			// array of servo objects
bool busy = false;							// in pulse, other libraries should check for isBusy() and not disable interrupts
uint8_t servoCount = 0;		   				// total number of servos
bool timerInitialized = false; 				// timer should be initialized only once


// Constructor
// ------------------------------------

ContinuousServo::ContinuousServo(uint8_t _pin, unsigned int _minPulse, unsigned int _maxPulse)
{
	// Very broad limits, likely the servo won't work even close to these limits..
	if (_minPulse <= 1) _minPulse = CONTINUOUS_SERVO_MIN_PULSE;
	if (_maxPulse >= CONTINUOUS_SERVO_REFRESH_CYCLE) _maxPulse = CONTINUOUS_SERVO_MAX_PULSE;
	pin = _pin;
	minPulse = _minPulse;
	maxPulse = _maxPulse;
	index = servoCount++;
	servos[index].step = 0;
	servos[index].pin = _pin;
}

// Public
// ------------------------------------

// Step a relative amount of steps
void ContinuousServo::step(int steps, void (*isr)())
{
	if (!timerInitialized)
	{
		initializeTimer();
	}

	pinMode(pin, OUTPUT);

	int value = ( steps == 0 ? 1 : ( steps > 0 ? 2 : 0 ) );
	int stepSizeInUs = map(value, 0, 2, minPulse, maxPulse);

	stepSizeInUs -= TRIM_DURATION; // compensate for digitalWrite timing
	int stepSizeInTicks = usToTicks(stepSizeInUs); // converts microseconds to timer ticks

	uint8_t oldSREG = SREG; // Store previous state before turning off interrupts
	cli(); // clear interrupts
    servos[index].direction = (steps == 0 ? 0 : (steps > 0 ? 1 : -1));
	servos[index].targetStep = servos[index].step + steps;
	servos[index].stepSize = stepSizeInTicks; // in ms
	servos[index].active = (steps != 0);
	servos[index].callback = isr;
	SREG = oldSREG; // Restore state register
}

// Sets absolute position (steoTo(0) will return to start position)
void ContinuousServo::stepTo(int absoluteStep, void (*isr)())
{
	step(absoluteStep - servos[index].step, isr);
}

// Stops movement
void ContinuousServo::stop()
{
	step(0);
}

// Returns current rotation in steps
int ContinuousServo::getSteps()
{
	return servos[index].step;
}

// Set servo position (for instance after calibration)
void ContinuousServo::storeSteps(int steps)
{
	servos[index].step = steps;
}

bool ContinuousServo::isBusy()
{
	return busy;
}

char ContinuousServo::getDirection()
{
	return servos[index].direction; // 1 or -1, 0 = stopped
}

// Private
// ------------------------------------

void ContinuousServo::initializeTimer()
{
	TCCR1A	= 0;				// normal counting mode
	TCNT1 	= 0;				// zero out 16bit counter register
	TCCR1B	= _BV(CS11);		// start timer with /8 prescaler

#if defined(__AVR_ATmega8__)|| defined(__AVR_ATmega128__)
	TIFR |= _BV(OCF1A);      // clear any pending interrupts; 
	TIMSK |=  _BV(OCIE1A) ;  // enable the output compare interrupt  
#else
	// here if not ATmega8 or ATmega128
	TIFR1 |= _BV(OCF1A);     // clear any pending interrupts; 
	TIMSK1 |=  _BV(OCIE1A) ; // enable the output compare interrupt 
#endif
	timerInitialized = true;
}

// Timer compare interrupt
SIGNAL (TIMER1_COMPA_vect)
{
	if (activeServo < 0)
	{
		// All servos have been updated
		TCNT1 = 0; // Reset timer1 counter
	}
	else
	{
		if (servos[activeServo].active)
		{
			// The cycle from the previous interrupt has completed, switch off pin
			digitalWrite(servos[activeServo].pin, LOW);  // End pulse
			if (activeServo >= servoCount - 1) busy = false; // This was the last servo, if necessary interrupts can be disabled for a couple ms..

			servos[activeServo].step += servos[activeServo].direction;
			
			if (servos[activeServo].step == servos[activeServo].targetStep)
			{
				// servo has completed, no need to process any more
				servos[activeServo].active = false;
				if (servos[activeServo].callback != NULL)
				{
					servos[activeServo].callback();
				}
				pinMode(servos[activeServo].pin, INPUT);
			}
		}
	}
	activeServo++;

	if (activeServo < servoCount)
	{
		// // Only process servo if it needs to be moved
		if (servos[activeServo].active)
		{
			// OCR1A = when to fire interrupt
			// TCNT1 = current counter value
			OCR1A = TCNT1 + servos[activeServo].stepSize; // Trigger next interrupt after ticks for active servo
			digitalWrite(servos[activeServo].pin, HIGH); // Start pulse
			busy = true;
		}
		else
		{
			OCR1A = TCNT1 + 4;
		}
	}
	else
	{
		// All servos processed
		if ((unsigned)TCNT1 + 4 < usToTicks(CONTINUOUS_SERVO_REFRESH_CYCLE))
		{
			// Full cycle not reached yet, simply continue to end
			OCR1A = (unsigned int)usToTicks(CONTINUOUS_SERVO_REFRESH_CYCLE);
		}
		else
		{
			// Cycle passed, trigger next one in 4 ticks
			OCR1A = TCNT1 + 4; // wait 4 more ticks to ensure we hit the next interrupt
		}
		activeServo = -1; // no more servos to process in this cycle
	}
}
