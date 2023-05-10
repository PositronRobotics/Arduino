#include <Servo.h>
#include <Wire.h>
#include "AFMotor.h"

#define STOPPED 0
#define FORWARD 1
#define BACK 2

#define STRAIGHT 0
#define LEFT 1
#define RIGHT 2

int currDir=STOPPED;
int currSteering=STRAIGHT;
int trig=0;
int KeepAlive_came=0;

AF_DCMotor motorLeft(4);
AF_DCMotor motorRight(3);

void setup()
{
  Serial.begin(115200);

  Serial.print("program start:7may:100");
  Serial.println();

  motorLeft.setSpeed(255); 
  motorLeft.run(RELEASE);
  motorRight.setSpeed(255); 
  motorRight.run(RELEASE);  

  Wire.begin(8);
  Wire.onReceive(receiveEvent);
}

void loop()
{
  //Serial.print("In loop:7may:101");
  //Serial.println();
  delay(100);

  static int smallCounter=0;
  int stoppedForever=0;

  if(smallCounter++>=10)
  {
    smallCounter=0;

    if(KeepAlive_came==1)
    {
      KeepAlive_came=0;
      stoppedForever=0;
    }
    else
    {
      stoppedForever=1;
      Serial.println("STOPPED");
      motorLeft.run(RELEASE);
      motorRight.run(RELEASE);

      currDir=STOPPED;
      currSteering=STRAIGHT;      
    }
  }
  
  if(stoppedForever==0)
  {
    if(trig==1)
    {
      trig=0;
  
      Serial.println("Trigger came");
  
      if(currDir==FORWARD)
      {
        if(currSteering==STRAIGHT)
        {
          Serial.println("FWD");
          motorLeft.run(FORWARD);
          motorRight.run(FORWARD); 
        }
        else if(currSteering==LEFT)
        {
          Serial.println("FWD-LEFT");
          motorLeft.run(RELEASE);
          motorRight.run(FORWARD); 
        }
        else if(currSteering==RIGHT)
        {
          Serial.println("FWD-RIGHT");
          motorLeft.run(FORWARD);
          motorRight.run(RELEASE); 
        }        
      }
      else if(currDir==BACK)
      {
        if(currSteering==STRAIGHT)
        {
          Serial.println("BACK");
          motorLeft.run(BACKWARD);
          motorRight.run(BACKWARD); 
        }
        else if(currSteering==LEFT)
        {
          Serial.println("BACK-LEFT");
          motorLeft.run(RELEASE);
          motorRight.run(BACKWARD); 
        }
        else if(currSteering==RIGHT)
        {
          Serial.println("BACK-RIGHT");
          motorLeft.run(BACKWARD);
          motorRight.run(RELEASE); 
        }        
      }
      else if(currDir==STOPPED)
      {
        Serial.println("STOPPED");
        motorLeft.run(RELEASE);
        motorRight.run(RELEASE);

        currDir=STOPPED;
        currSteering=STRAIGHT;                       
      }
    }
  }
}

// function that executes whenever data is received from master
void receiveEvent(int howMany)
{
  String rcmd="";
    
  while (0 <Wire.available())
  {
    rcmd += (char)Wire.read();
  }
  //Serial.print("Recd Cmd:");
  //Serial.print(rcmd);
  //Serial.println();

  if(rcmd[0]=='A')
  {
    KeepAlive_came=1;
  }

  if((rcmd[0]=='u')||(rcmd[0]=='n')||(rcmd[0]=='h')||(rcmd[0]=='j'))
  {
    if(currDir==STOPPED)
    {
      if(rcmd[0]=='u')
      {
        currDir=FORWARD;
      }
      else if(rcmd[0]=='n')
      {
        currDir=BACK;
      }
    }
    else if(currDir==FORWARD)
    {
      if(rcmd[0]=='n')
      {
        currDir=STOPPED;
      }
    }
    else if(currDir==BACK)
    {
      if(rcmd[0]=='u')
      {
        currDir=STOPPED;
      }
    }
    
    if(currSteering==STRAIGHT)
    {
      if(rcmd[0]=='h')
      {
          currSteering=LEFT;
      }
      if(rcmd[0]=='j')
      {
          currSteering=RIGHT;
      }
    }
    else if(currSteering==LEFT)
    {
      if(rcmd[0]=='j')
      {
          currSteering=STRAIGHT;
      }
    }
    else if(currSteering==RIGHT)
    {
      if(rcmd[0]=='h')
      {
          currSteering=STRAIGHT;
      }
    }

    trig=1;
  }

    
}
