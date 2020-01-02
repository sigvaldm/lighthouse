#include <PciManager.h>
#include <SoftTimer.h>
#include <TimerOne.h>
#include <Rotary.h>
#include <Debouncer.h>
#include <EEPROM.h>

// Current LED bulb is 1W. Circuitry supports 2W.

// Pins
const int ledPin = 9;
const int encPinA = 2;
const int encPinB = 3;
const int buttonPin = 4;

// State variables
volatile int state = 0;			// 0=off, 1=blinking program, 2=on
volatile int maxBr = 1023;		// Peak brightness [0,1023]. Not linear.
volatile float percMaxBr = 1;	// Peak brighness linearized [0,1]. Used by incr() and decr().

// EEPROM adresses
const int stateAdr = 0;

// Increase/decrease brightness
void incr();
void decr();

// LED actions
void updateLed(Task *me);				// Frequent update to change PWM according to instantaneous brightness
void changeLed(Task *me);				// Infrequent update to turn LED on/off according to blinkig program

const int N=1000;						// Number of steps during ramp up/down
volatile int m=0;						// Current perceived intensity level
volatile int increment = 1;				// Perceived intensity increment of -1, 0 or +1 per update
const int T = 350;						// [ms] duration of LED ramp up/down
const long d = T/(0.001*(N+1));			// [us] time between updates
volatile int loopTime = 0;				// Time in LED's program's loop
Task taskUpdateLed(0, updateLed);		// Update analog output. Library handles microsecond but constructor only milliseconds. See setup.
Task taskChangeLed(1, changeLed);		// Change what led is doing according to whic ms we're in.


void onRotate(short direction, Rotary* rotary);
void onButtonPress();
void onButtonRelease(unsigned long pressTime);
Rotary rotaryEncoder(encPinA, encPinB, onRotate, true);
Debouncer button(buttonPin, MODE_CLOSE_ON_PUSH, onButtonPress, onButtonRelease, true);

void setup(){
	
	Timer1.initialize(500);
	pinMode(ledPin, OUTPUT);
	
	pinMode(encPinA, INPUT_PULLUP); 
	pinMode(encPinB, INPUT_PULLUP);
	pinMode(buttonPin, INPUT_PULLUP);

    EEPROM.get(stateAdr, state);
    updateStateVars();

	PciManager.registerListener(buttonPin, &button);
	
	taskUpdateLed.periodMicros=d;	// Hard-set period to get microsecond resolution
	SoftTimer.add(&taskUpdateLed);

}

void updateLed(Task *me){
	m += increment;
	if(m>=N || m<=0){
		increment = 0;
	}
	float perceived = pow(sin(1.5708*((float)m/N)),2);
	float intensity = pow(perceived,2);
	Timer1.pwm(ledPin, intensity*maxBr);
}

void changeLed(Task *me){
	
	// increment = +1 (-1) will ramp up (down) until max (min) level is reached
/*	
	if(loopTime==0)		increment = +1;
	if(loopTime==900)	increment = -1;
	if(loopTime==1800)	increment = +1;
	if(loopTime==2700)	increment = -1;
	if(loopTime==3600)	increment = +1;
	if(loopTime==4500)	increment = -1;
	if(loopTime==5400)	increment = +1;
	if(loopTime==8800)	increment = -1;

	loopTime++;
	
	if(loopTime==10200)	loopTime=0;
*/
	
	if(loopTime==0)		increment = +1;
	if(loopTime==1500)	increment = -1;
	if(loopTime==3000)	increment = +1;
	if(loopTime==4500)	increment = -1;
	if(loopTime==6000)	increment = +1;
	if(loopTime==7500)	increment = -1;
	if(loopTime==9000)	increment = +1;
	if(loopTime==15000)	increment = -1;

	loopTime++;
	
	if(loopTime==18000)	loopTime=0;	
	
}

void decr(){
	if(percMaxBr>0) percMaxBr -= 0.05;
	maxBr = 1023*pow(percMaxBr,2);
}

void incr(){
	if(percMaxBr<1) percMaxBr += 0.05;
	maxBr = 1023*pow(percMaxBr,2);
}

void onRotate(short direction, Rotary* rotary) {
  if(direction == DIRECTION_CW) {
    decr();
  }
  if(direction == DIRECTION_CCW) {
    incr();
  }
}

void onButtonPress(){
	state++;
	if(state>2) state=0;
    EEPROM.put(stateAdr, state);
    updateStateVars();
}

void onButtonRelease(unsigned long pressTime){
}

void updateStateVars(){

    if(state<0 || state>2) state=0;
	
	if(state==0){
		increment = 0;
		m=0;
	} else if(state==1){
		m=0;
		loopTime=0;
		SoftTimer.add(&taskChangeLed);
	} else {
		increment = 0;
		SoftTimer.remove(&taskChangeLed);
		m=N;
	}
}
