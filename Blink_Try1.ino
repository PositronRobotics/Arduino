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

Servo servoLegFoot;
Servo servoLegKnee;
Servo servoLegHip;

#define SERVOLEGFOOT_UPRIGHT_POS 38
#define SERVOLEGKNEE_UPRIGHT_POS 109
#define SERVOLEGHIP_UPRIGHT_POS 67

#define SERVOLEGFOOT_SQUATTED_DOWN_1_POS 142
#define SERVOLEGKNEE_SQUATTED_DOWN_1_POS 5
#define SERVOLEGHIP_SQUATTED_DOWN_1_POS 50

#define SERVOLEGFOOT_SQUATTED_DOWN_2_POS 154
#define SERVOLEGKNEE_SQUATTED_DOWN_2_POS 5
#define SERVOLEGHIP_SQUATTED_DOWN_2_POS 56

int servoLegFoot_pos = SERVOLEGFOOT_UPRIGHT_POS;
int servoLegKnee_pos = SERVOLEGKNEE_UPRIGHT_POS;
int servoLegHip_pos = SERVOLEGHIP_UPRIGHT_POS;

int prev_servoLegFoot_pos = 0;
int prev_servoLegKnee_pos = 0;
int prev_servoLegHip_pos = 0;

char serInput=0;

#define UPRIGHT 0
#define SQUATTING_DOWN_1 1
#define SQUATTING_DOWN_2 2
#define SQUATTED 3
#define SQUATTING_UP_1 4
#define SQUATTING_UP_2 5

int squatState=UPRIGHT;

void setup()
{
  Serial.begin(115200);

  Serial.print("program start:17sep:100");
  Serial.println();

  motorLeft.setSpeed(255); 
  motorLeft.run(RELEASE);
  motorRight.setSpeed(255); 
  motorRight.run(RELEASE);  
  
  servoLegFoot.attach(9);  // attaches the servo on pin 9 to the servo object
  servoLegKnee.attach(10);
  servoLegHip.attach(11);
  
  servoLegFoot.write(servoLegFoot_pos);
  servoLegKnee.write(servoLegKnee_pos);
  servoLegHip.write(servoLegHip_pos);  

  Wire.begin(8);
  Wire.onReceive(receiveEvent);
}

void loop()
{
  //Serial.print("In loop:7may:101");
  //Serial.println();
  //delay(100);

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
      //Serial.println("STOPPED");
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
  
      //Serial.println("Trigger came");
  
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
        //Serial.println("STOPPED");
        motorLeft.run(RELEASE);
        motorRight.run(RELEASE);

        currDir=STOPPED;
        currSteering=STRAIGHT;                       
      }
    }
  }
  
  if (Serial.available() > 0)
  {
    serInput=0;
    serInput = Serial.read();

    //Serial.print("Received:");
    //Serial.println(serInput);

    if((serInput==97)||(serInput==122)||(serInput==115)||(serInput==120)||(serInput==100)||(serInput==99))
    {
      if(serInput==97)
      {
        if(servoLegFoot_pos<180)
        {
          servoLegFoot_pos++;
        }
      }
      else if(serInput==122)
      {
        if(servoLegFoot_pos>0)
        {
          servoLegFoot_pos--;
        }
      }
      else if(serInput==115)
      {
        if(servoLegKnee_pos<180)
        {
          servoLegKnee_pos++;
        }
      }
      else if(serInput==120)
      {
        if(servoLegKnee_pos>0)
        {
          servoLegKnee_pos--;
        }
      }
      else if(serInput==100)
      {
        if(servoLegHip_pos<180)
        {
          servoLegHip_pos++;
        }
      }
      else if(serInput==99)
      {
        if(servoLegHip_pos>0)
        {
          servoLegHip_pos--;
        }
      }

      Serial.print("servoLegFoot_pos:");
      Serial.print(servoLegFoot_pos);
      Serial.print(" ;servoLegKnee_pos:");
      Serial.print(servoLegKnee_pos);
      Serial.print(" ;servoLegHip_pos:");
      Serial.println(servoLegHip_pos);              
    }
    else
    {
      if(serInput==48)  
      {
        if(squatState==UPRIGHT)
        {
          squatState=SQUATTING_DOWN_1;
        }
      }
    }
	}

  if(squatState==SQUATTING_DOWN_1)
  {
    if(servoLegFoot_pos<SERVOLEGFOOT_SQUATTED_DOWN_1_POS)
    {
      servoLegFoot_pos++;
      servoLegKnee_pos--;

      if(servoLegFoot_pos%6==0)
      {
          servoLegHip_pos--;
      }                  
      delay(50);
    }
    else
    {
      squatState=SQUATTING_DOWN_2;
    }
  }
  else if(squatState==SQUATTING_DOWN_2)
  {
    if(servoLegFoot_pos<SERVOLEGFOOT_SQUATTED_DOWN_2_POS)
    {
      servoLegFoot_pos++;

      if(servoLegFoot_pos%2==0)
      {
          servoLegHip_pos++;
      }                  
      delay(50);
    }
    else
    {
      squatState=SQUATTED;
    }    
  }

  //Update all Servos via PWM
  if(prev_servoLegFoot_pos!=servoLegFoot_pos)
  {
    prev_servoLegFoot_pos=servoLegFoot_pos;
    servoLegFoot.write(servoLegFoot_pos);
  }
  
  if(prev_servoLegKnee_pos!=servoLegKnee_pos)
  {
    prev_servoLegKnee_pos=servoLegKnee_pos;
    servoLegKnee.write(servoLegKnee_pos);
  }  

  if(prev_servoLegHip_pos!=servoLegHip_pos)
  {
    prev_servoLegHip_pos=servoLegHip_pos;
    servoLegHip.write(servoLegHip_pos);
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
