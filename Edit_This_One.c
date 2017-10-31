/*
 * File:   BaseMobility.c
 * Author: svoss
 *
 * Created on October 24, 2017, 10:51 AM
 */


#pragma config FNOSC = LPFRC

#include "xc.h"

//initializing volatile variables
volatile int k = 0;
volatile int Pulses = 0;
volatile float DegreesTurned = 0;   
volatile double TimeActive = 0;

enum {FORWARD,BACKWARD} state;
    

//CN interrupt for when the robot hits the corner 
void __attribute__((interrupt, no_auto_psv)) _CNInterrupt(void)
{
  _CNIF=0;
  
  
  if (_RB13==1)
     {
          DegreesTurned = 2420; //distance to center
          _LATB9 = 1;
          _LATB8 = 0;
     }
  
    TimeActive = DegreesTurned * 1.0 / 225.0;
    //Modify this equation with motor parameters to determine how long the motors will be on (TimeActive) in order to rotate a desired angle (DegreesTurned)
    //250000 (clock time) / 2000 (PWM frequency) = steps per second *1.8 (degrees per step if full step) = degrees per second = 450
    Pulses = TimeActive*3906.0; //FIX ME
    PR1 = Pulses;
  
    TMR1=0;
  
    state = BACKWARD;
}

void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void)
{
    _T1IF = 0;
    TMR1=0;
    
    
    //this state is triggered when the robot reaches the center of the arena
    if (state == BACKWARD)
    {
        _LATB7 = 0; //turn off (sleep) wheel motor driver
        state = BACKWARD;
    }
    
    //this runs when the robot is driving into the corner and the timer runs out
    //the purpose is just to reset the timer to keep the robot moving toward the corner
    if (state == FORWARD)
    {
        _LATB8 = 0;
        _LATB9 = 1;
        _LATB7 = 1;
        TimeActive = DegreesTurned * 1.0 / 450.0;
    
        Pulses = TimeActive*3906.0; //This is for full step size. Double this number for half step size
    
        PR1 = Pulses;
  
        TMR1=0;
        
        state = FORWARD;
    }

    
    
    TimeActive = DegreesTurned * 1.0 / 225.0;
    //Modify this equation with motor parameters to determine how long the motors will be on (TimeActive) in order to rotate a desired angle (DegreesTurned)
    //250000 (clock time) / 2000 (PWM frequency) = steps per second *1.8 (degrees per step if full step) = degrees per second = 450
    Pulses = TimeActive*3906.0; //FIX ME
    PR1 = Pulses;
    
    //incrementing the motor state WILL NOT BE USED IN FINAL DESIGN, WILL BE REPLACED WITH AN ENUM CLASS VARIABLE
    k++;
    
    //resetting timer 1, which times wheel turning
    TMR1=0;
}




int main(void) {
    //initializing motor state
    k = 1;
    
    TRISBbits.TRISB9=0; //setting pin #13, direction control for ?right? motor
    TRISBbits.TRISB8=0; //Setting pin #12, direction control for ?left? motor
    LATBbits.LATB9=0; // _LATB9=0 causes the ?right? motor to go ?forward?          VERFIY THESE
    LATBbits.LATB8=1; // _LATB8= 1 causes the ?left? motor to go ?forward?
    
    //Setting up PWM outputs for the wheel motors
    OC1CON1 = 0;                //pin 14, controls both motors PWM
    OC1CON2 = 0;
    OC1RS = 1999; //Motor PWM period (# of clock cycles per pulse)
    OC1R = 1000;
    OC1CON1bits.OCTSEL = 0b111;      
    OC1CON2bits.SYNCSEL = 0x1F;
    OC1CON2bits.OCTRIG = 0;          
    OC1CON1bits.OCM = 0b110;
    
    
    //Set DegreesTurned to desired angle of wheel rotation
    DegreesTurned = 0;
    
    TimeActive = DegreesTurned * 1.0 / 225.0;
    //Modify this equation with motor parameters to determine how long the motors will be on (TimeActive) in order to rotate a desired angle (DegreesTurned)
    //250000 (clock time) / 2000 (PWM frequency) = steps per second *1.8 (degrees per step if full step) = degrees per second = 225
    Pulses = TimeActive*3906.0;

    //Setting up timer 1, which fires when the wheels have turned a desired angle
    _TON = 1;
    _TCS = 0;
    _TCKPS = 0b10;
    _T1IP = 4;
    _T1IE = 1;
    _T1IF = 0;
    PR1 = Pulses;
    TMR1=0;
        
    //Setting up pin 11, the wheel motor sleep pin
    _TRISB7 = 0;
    _LATB7 = 1;
    
    _CN13IE = 1;
    _CN13PUE = 0;
    _CNIP = 6;
    _CNIE = 1;
    _CNIF = 0;
   
    while (1)
    {}
    
    return 0;
}