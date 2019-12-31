#include <TimerOne.h>
//#define LIBCALL_ENABLEINTERRUPT
#include <EnableInterrupt.h>

// Pins
const int ledPin = 9;
const int encPinA = 2;
const int encPinB = 3;
const int buttonPin = 4;

// Maximum intensity used when LED is fully on
// The numerical range is 0-1023 which corresponds to 0-5V
volatile int maxBr = 1023;
volatile float percBr = 1;
volatile int state = 0;

// Number of (true) intensity levels during ramp-up/down.
// Due to the eye's square-law detection small increments
// are needed for low intensities for which reason this
// number is high.
const int N=1000;

// Time for ramping up/down brightness
const int rampTime = 200; // ms

const unsigned long debounce = 1000; // us
const unsigned long debounceBtn = 300000; // us
volatile unsigned long encPinATime;
volatile unsigned long encPinBTime;
volatile unsigned long buttonTime;

void decr(){
	if(percBr>0) percBr -= 0.02;
	maxBr = 1023*pow(percBr,2);
}

void incr(){
	if(percBr<1) percBr += 0.02;
	maxBr = 1023*pow(percBr,2);
}

void doButton();

void doEncoderA(){

	if(micros()-encPinATime>debounce){

		if (digitalRead(encPinA) == HIGH){ 
			if (digitalRead(encPinB) == LOW){  
				incr();
			} else {
				decr();
			}
		} else { 
			if (digitalRead(encPinB) == HIGH){
				incr();
			} else {
				decr();
			}
		}
		Serial.println(maxBr);
		
	}
	
	encPinATime=micros();
}

void doEncoderB(){
	
	if(micros()-encPinATime>debounce){


		if(digitalRead(encPinB) == HIGH){   
			if(digitalRead(encPinA) == HIGH){  
				incr();
			} else {
				decr();
			}
		} else { 
			if(digitalRead(encPinA) == LOW){   
				incr();
			} else {
				decr();
			}
		}
		Serial.println(maxBr);
	}

	encPinATime=micros();
	
} 

void setup(){
	
	Timer1.initialize(500);
	pinMode(ledPin, OUTPUT);
	
	pinMode(encPinA, INPUT_PULLUP); 
	pinMode(encPinB, INPUT_PULLUP);
	pinMode(buttonPin, INPUT_PULLUP);
	enableInterrupt(encPinA, doEncoderA, CHANGE);
	enableInterrupt(encPinB, doEncoderB, CHANGE);  
	enableInterrupt(buttonPin, doButton, FALLING);
	
	Serial.begin(9600);

}

void ledOn(int T){
  long d=T/(0.001*(N+1));  // Delay time (us)
  for(int m=0;m<=N;m++){
    float timestep = (float) m/N;
    float perceived = pow(sin(1.5708*timestep),2);
    float intensity = pow(perceived,2);
    Timer1.pwm(ledPin, intensity*maxBr);
    delayMicroseconds(d);
  }
}

void ledOff(int T){
  long d=T/(0.001*(N+1));  // Delay time (us)  
  for(int m=N;m>=0;m--){
    float timestep = (float) m/N;
    float perceived = pow(sin(1.5708*timestep),2);
    float intensity = pow(perceived,2);
    Timer1.pwm(ledPin, intensity*maxBr);    
    delayMicroseconds(d);      
  }
}

void ledBlink(int onTime, int offTime){
	ledOn(rampTime);
	delay(onTime);
	ledOff(rampTime);
	delay(offTime);
}

void loop(){
	
	if(state==0){
		Timer1.pwm(ledPin,0);
	} else if(state==1) {
		ledBlink(500,500);
		ledBlink(500,500);
		ledBlink(500,500);
		ledBlink(3000,1000);
	} else {
		Timer1.pwm(ledPin,maxBr);
	}
}


void doButton(){

	if(micros()-buttonTime>debounceBtn){

		state++;
		if(state>2) state=0;
		
	}
	
	buttonTime = micros();

}