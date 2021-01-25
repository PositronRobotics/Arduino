#include <Servo.h>
#include <Wire.h>
#include "AFMotor.h"

#define ARM_RSF_POS_HOME 38
#define ARM_RSL_POS_HOME 159
#define ARM_REL_POS_HOME 103

#define UNDEFINED 9
#define MOVING_TO_HOME_POS 0
#define MOVED_TO_HOME_POS 1

#define WHEEL_SPEED_HIGH 255

Servo arm_RSF;
Servo arm_RSL;
Servo arm_REL;

AF_DCMotor motorLeft(3);
AF_DCMotor motorRight(4);

int curr_m1=0;
int prev_m1=0;
int move_home_process=UNDEFINED;

char serInput=0;

int wheelsDir=0;

// create servo object to control a servo
// twelve servo objects can be created on most boards

void setup() {
  Serial.begin(115200);

  Wire.begin(8);
  Wire.onReceive(receiveEvent);

  motorLeft.run(RELEASE);
  motorRight.run(RELEASE);

  motorLeft.setSpeed(WHEEL_SPEED_HIGH);
  motorRight.setSpeed(WHEEL_SPEED_HIGH);    
}

void loop()
{
  if((prev_m1==0) && (curr_m1==1))
  {
    move_home_process=MOVING_TO_HOME_POS;    
    prev_m1=curr_m1;    
    
    arm_REL.attach(11);
    arm_REL.write(ARM_REL_POS_HOME);
    Serial.println("arm_REL");
    delay(5000);

    arm_RSL.attach(10);
    arm_RSL.write(ARM_RSL_POS_HOME);
    Serial.println("arm_RSL");
    delay(5000);

    arm_RSF.attach(9);
    arm_RSF.write(ARM_RSF_POS_HOME);
    Serial.println("arm_RSF");
    delay(5000);

    move_home_process=MOVED_TO_HOME_POS;
  }
}

// function that executes whenever data is received from master
void receiveEvent(int howMany)
{
  String rcmd = "";

  int m1=0;
  
  while (0 <Wire.available())
  {
    //char c = Wire.read();      /* receive byte as a character */
    //Serial.print(c);           /* print the character */
    rcmd += (char)Wire.read();
  }
  //Serial.print("Recd Cmd:");
  //Serial.print(rcmd.substring(3,4));
  //Serial.println();             /* to newline */

  if((rcmd[0]=='m') && (rcmd[1]=='1'))
  {
    m1=(rcmd.substring(3,4)).toInt();
    
    if(move_home_process!=MOVING_TO_HOME_POS)
    {
      Serial.println("Assigned to curr_m1");
      curr_m1=m1;      
    }    
  }
}
