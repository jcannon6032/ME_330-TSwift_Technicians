/*
 * File:   BaseMobility.c
 * Author: svoss
 *
 * Created on October 24, 2017, 10:51 AM
 */


#pragma config FNOSC = LPFRC
#include "xc.h"


enum {LOOKING_FOR_CORNER, GOING_TO_CORNER, GOING_TO_CENTER, EXTENDING_RAMP, LOOKING_FOR_GOAL, SHOOTING, POISION_BALL} state;
    

//interrupt for switches and poision ball detect
void __attribute__((interrupt, no_auto_psv)) _CNInterrupt(void)
{
  _CNIF=0;
 
  //CN interrupt for when the robot hits the corner 
  if (state == GOING_TO_CORNER)
    {
      state = GOING_TO_CENTER;
      PR1 = num_PWM_pulses_wheel (2420); //distance to center
      _LATB9 = 1; //move forward
      _LATB8 = 0; //move forward
      TMR1=0;
    }

  //this is where the poison ball detect CN will go
  
}

void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void)
{
    _T1IF = 0;
    TMR1=0;
    
    //this state is triggered when the robot reaches the center of the arena
    if (state == GOING_TO_CENTER)
    {
        _LATB7 = 0; //turn off (sleep) wheel motor driver
        state = EXTENDING_RAMP;
        
        /*
         turn on ramp motor
         pr1 = ???
         */
    }
    
    
   TMR1=0;
}



void wheel_PWM_config (void)
{
    OC1CON1 = 0;  //pin 14, controls both motors PWM
    OC1CON2 = 0;
    OC1RS = 1999; //this is a PWM frequency of 125 Hz (250,000/2000)
    OC1R = 1000;
    OC1CON1bits.OCTSEL = 0b111;      
    OC1CON2bits.SYNCSEL = 0x1F;
    OC1CON2bits.OCTRIG = 0;          
    OC1CON1bits.OCM = 0b110;
    
    //Direction control, pins 12, 13
    TRISBbits.TRISB9=0; //setting pin #13, direction control for ?right? motor
    TRISBbits.TRISB8=0; //Setting pin #12, direction control for ?left? motor
    LATBbits.LATB9=0; // _LATB9=0 causes the ?right? motor to go ?forward?          VERFIY THESE
    LATBbits.LATB8=1; // _LATB8= 1 causes the ?left? motor to go ?forward?
    
    //Setting up pin 11, the wheel motor sleep pin
    _TRISB7 = 0;
    _LATB7 = 1; //motor active  
}


//this function returns the number of PWM pulses required for the motor to turn a specified number of degrees
int num_PWM_pulses_wheel (int Degrees_Turned)
{
    TimeActive = Degrees_Turned * 1.0 / 225.0; // for full step size
    //Modify this equation with motor parameters to determine how long the motors will be on (TimeActive) in order to rotate a desired angle (DegreesTurned)
    //250000 (clock time) / 2000 (PWM frequency) = steps per second *1.8 (degrees per step if full step) = degrees per second = 225
    Pulses = TimeActive*3906.0;
    return Pulses;
}



//timer 1 is the wheel timer
void wheel_timer_config (void)
{
    _TON = 1;
    _TCS = 0;
    _TCKPS = 0b10;
    _T1IP = 4; //priority
    _T1IE = 1; //enable
    _T1IF = 0; //flag
    PR1 = num_PWM_pulses_wheel (360*5);
   
    TMR1=0;
}



//the corner detection switches are connected to pin 13
void switches_config (void)
{
    _CN13IE = 1; //look at pin 13 to sense a change?
    _CN13PUE = 0;
    _CNIP = 6; //priority
    _CNIE = 1; //enable
    _CNIF = 0; //flag
}



int main(void) {
   
   //as currently coded, these functions turn on the motors and motor timer and start watching for the switches
   wheel_PWM_config();
   wheel_timer_config ();
   switches_config ()
   
    while (1)
    {}
    
    return 0;
}
