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

#define SERVOLEGFOOT_SQUATTED_DOWN_MID_POS 157
#define SERVOLEGKNEE_SQUATTED_DOWN_MID_POS 0
#define SERVOLEGHIP_SQUATTED_DOWN_MID_POS 53

#define SERVOLEGFOOT_SQUATTED_DOWN_FULL_POS 154
#define SERVOLEGKNEE_SQUATTED_DOWN_FULL_POS 5
#define SERVOLEGHIP_SQUATTED_DOWN_FULL_POS 56

float servoLegFoot_pos = SERVOLEGFOOT_UPRIGHT_POS;
float servoLegKnee_pos = SERVOLEGKNEE_UPRIGHT_POS;
float servoLegHip_pos = SERVOLEGHIP_UPRIGHT_POS;

float prev_servoLegFoot_pos = 0;
float prev_servoLegKnee_pos = 0;
float prev_servoLegHip_pos = 0;

char serInput=0;

#define UPRIGHT 0
#define SQUATTED_DOWN_MID 1
#define SQUATTED_DOWN_FULL 2
#define SQUATTED_UP_MID 4
#define SQUATTED_UP_FULL 5

int squatState=UPRIGHT;
int target_squatState=UPRIGHT;

int num_steps = 0; // You can adjust this based on how fast you want the servos to move
int cur_step=1;

float target_servoLegFoot_pos=0;
float target_servoLegKnee_pos=0;
float target_servoLegHip_pos=0;

float increment_LEGFOOT = 0;
float increment_LEGKNEE = 0;
float increment_LEGHIP = 0;

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
  Wire.onReceive(receiveI2C);
  
  /*Serial.print("increment_LEGFOOT:");
  Serial.print(increment_LEGFOOT);

  Serial.print("increment_LEGKNEE:");
  Serial.print(increment_LEGKNEE);

  Serial.print("increment_LEGHIP:");
  Serial.print(increment_LEGHIP);  */
}

void DC_motorControl()
{
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
}

void takeSerialInput()
{
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
          target_squatState=SQUATTED_DOWN_MID;
        }
      }
      else if(serInput==49)  
      {
        if(squatState==SQUATTED_DOWN_MID)
        {
          target_squatState=UPRIGHT;
        }
      }      
    }
  }
}

int maxOf3(int a, int b, int c)
{
    int max_value = a;

    if (b > max_value) {
        max_value = b;
    }

    if (c > max_value) {
        max_value = c;
    }

    return max_value;
}

int calc_num_steps()
{
  return maxOf3((abs((int)(target_servoLegFoot_pos - servoLegFoot_pos))),(abs((int)(target_servoLegKnee_pos - servoLegKnee_pos))),(abs((int)(target_servoLegHip_pos - servoLegHip_pos))));
}

void leg_transitionThruToTargetState()
{
  cur_step=1;
  num_steps=calc_num_steps();

  Serial.print("servoLegFoot_pos:");
  Serial.print((int)servoLegFoot_pos);
  Serial.print("target_servoLegFoot_pos:");
  Serial.println((int)target_servoLegFoot_pos);
  
  Serial.print("servoLegKnee_pos:");
  Serial.print((int)servoLegKnee_pos);
  Serial.print("target_servoLegKnee_pos:");
  Serial.println((int)target_servoLegKnee_pos);
  
  Serial.print("servoLegHip_pos:");
  Serial.print((int)servoLegHip_pos);
  Serial.print("target_servoLegHip_pos:");
  Serial.println((int)target_servoLegHip_pos);        
  
  Serial.println("-------");  

  Serial.print("num_steps:");
  Serial.println(num_steps);  
  
  increment_LEGFOOT = (float)(target_servoLegFoot_pos - servoLegFoot_pos) / num_steps;
  increment_LEGKNEE = (float)(target_servoLegKnee_pos - servoLegKnee_pos) / num_steps;
  increment_LEGHIP = (float)(target_servoLegHip_pos - servoLegHip_pos) / num_steps; 
      
  for(int i=cur_step;i<=num_steps;i++)
  {                        
    servoLegFoot_pos=servoLegFoot_pos+increment_LEGFOOT;
    servoLegKnee_pos=servoLegKnee_pos+increment_LEGKNEE;
    servoLegHip_pos=servoLegHip_pos+increment_LEGHIP;
  
    Serial.print("servoLegFoot_pos:");
    Serial.print((int)servoLegFoot_pos);
    Serial.print(" ;servoLegKnee_pos:");
    Serial.print((int)servoLegKnee_pos);
    Serial.print(" ;servoLegHip_pos:");
    Serial.println((int)servoLegHip_pos);

    updateServos();
        
    delay(50);
  }  
  squatState=target_squatState;
}

void legManuevers()
{
  if(squatState!=target_squatState)
  {
    Serial.print("squatState:");
    Serial.print(squatState);
    Serial.print("target_squatState:");
    Serial.println(target_squatState);
    
    if((squatState==UPRIGHT)&&(target_squatState==SQUATTED_DOWN_MID))
    {              
      target_servoLegFoot_pos=SERVOLEGFOOT_SQUATTED_DOWN_MID_POS;
      target_servoLegKnee_pos=SERVOLEGKNEE_SQUATTED_DOWN_MID_POS;
      target_servoLegHip_pos=SERVOLEGHIP_SQUATTED_DOWN_MID_POS;
    }
    else if((squatState==SQUATTED_DOWN_MID)&&(target_squatState==UPRIGHT))
    {              
      target_servoLegFoot_pos=SERVOLEGFOOT_UPRIGHT_POS;
      target_servoLegKnee_pos=SERVOLEGKNEE_UPRIGHT_POS;
      target_servoLegHip_pos=SERVOLEGHIP_UPRIGHT_POS;
    }

    leg_transitionThruToTargetState();
  }
}

void updateServos()
{
  if(prev_servoLegFoot_pos!=servoLegFoot_pos)
  {
    prev_servoLegFoot_pos=servoLegFoot_pos;
    servoLegFoot.write((int)servoLegFoot_pos);
  }
  
  if(prev_servoLegKnee_pos!=servoLegKnee_pos)
  {
    prev_servoLegKnee_pos=servoLegKnee_pos;
    servoLegKnee.write((int)servoLegKnee_pos);
  }  

  if(prev_servoLegHip_pos!=servoLegHip_pos)
  {
    prev_servoLegHip_pos=servoLegHip_pos;
    servoLegHip.write((int)servoLegHip_pos);
  }
}

void loop()
{
  DC_motorControl();
  takeSerialInput();
  legManuevers();  
}

// function that executes whenever data is received from master
void receiveI2C(int howMany)
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
