#include <Servo.h>
#include <Wire.h>
#include "AFMotor.h"

#define NOOFSERVOSARMED 3

#define ARM_RSF_POS_HOME 38
#define ARM_RSL_POS_HOME 159
#define ARM_REL_POS_HOME 103

#define ARM_RSF_PIN 9
#define ARM_RSL_PIN 10
#define ARM_REL_PIN 11

#define UNDEFINED 9
#define MOVING_TO_HOME_POS 0
#define MOVED_TO_HOME_POS 1

#define WHEEL_SPEED_HIGH 255

#define UNDEFINED 999

enum ArmServoEnum{RSF,RSL,REL};

struct sservoConstData
{
  int pin;
  int initialPos;
  int initOrder;
};

struct sservoConstData servoConstData[NOOFSERVOSARMED]=
{
  {ARM_RSF_PIN,ARM_RSF_POS_HOME,2},
  {ARM_RSL_PIN,ARM_RSL_POS_HOME,1},
  {ARM_REL_PIN,ARM_REL_POS_HOME,0},    
};

Servo servoHW[3];

struct sservoCurrData
{
  int prev;
  int curr;
};

struct sservoCurrData servoCurrData[3];

//Temp for Control from Blynk
int servo_control_flag[3]={0,0,0};
int currServo=UNDEFINED;

AF_DCMotor motorLeft(3);
AF_DCMotor motorRight(4);

int curr_m1=0;
int prev_m1=0;
int move_home_process=UNDEFINED;

char serInput=0;

int wheelsDir=0;

void receiveEvent(int howMany);
void requestEvent();

void setup()
{ 
  Serial.begin(115200);

  Wire.begin(8);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  motorLeft.run(RELEASE);
  motorRight.run(RELEASE);

  motorLeft.setSpeed(WHEEL_SPEED_HIGH);
  motorRight.setSpeed(WHEEL_SPEED_HIGH);  

  for(int i=0;i<NOOFSERVOSARMED;i++)
  {    
    servoCurrData[i].prev=999;
    servoCurrData[i].curr=servoConstData[i].initialPos;    
    //Serial.println("test102");
  }
}

void loop()
{
  //Serial.println("in LOOP");
  int initCtr,j;
  
  if((prev_m1==0) && (curr_m1==1))
  {
    Serial.println("inside If");
    
    move_home_process=MOVING_TO_HOME_POS;    
    prev_m1=curr_m1;    
    
    /*arm_REL.attach(11);
    arm_REL.write(arm_REL_pos);
    Serial.println("arm_REL");
    delay(5000);

    arm_RSL.attach(10);
    arm_RSL.write(arm_RSL_pos);
    Serial.println("arm_RSL");
    delay(5000);

    arm_RSF.attach(9);
    arm_RSF.write(arm_RSF_pos);
    Serial.println("arm_RSF");
    delay(5000);*/

    for(initCtr=0;initCtr<NOOFSERVOSARMED;initCtr++)
    {
      //Serial.print("initCtr loop:");
      //Serial.println(initCtr);
      
      for(j=0;j<NOOFSERVOSARMED;j++)
      {
        //Serial.print("j loop:");
        //Serial.println(j);
        //Serial.print("servoConstData[j].initOrder:");
        //Serial.println(servoConstData[j].initOrder);
                
        if(initCtr==servoConstData[j].initOrder)
        {
          //Serial.println("if condn");
          //servoHW[j].attach(servoConstData[j].pin);
          //servoHW[j].write(servoConstData[j].InitialPos);

          Serial.print("Servo:");
          Serial.print(j);
          Serial.print(", servoConstData[j].initialPos:");
          Serial.print(servoConstData[j].initialPos);
          Serial.print(", servoConstData[j].pin:");
          Serial.println(servoConstData[j].pin);                    

          break;
        }
      }
      delay(4000);
    }

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
  Serial.print("Recd Cmd:");
  Serial.print(rcmd);
  //Serial.print(rcmd.substring(3,4));
  Serial.println();             /* to newline */

  if((rcmd[0]=='m') && (rcmd[1]=='1'))
  {  
    if(move_home_process!=MOVING_TO_HOME_POS)
    {
      Serial.println("Assigned to curr_m1");
      curr_m1=1;      
    }    
  }
  
  /*if((rcmd[0]=='r') && (rcmd[1]=='s') && (rcmd[f]=='f'))
  {
    currServo=RSF;
  }
  else if((rcmd[0]=='r') && (rcmd[1]=='s') && (rcmd[f]=='l'))
  {
    currServo=RSL;
  }
  else if((rcmd[0]=='r') && (rcmd[1]=='e') && (rcmd[f]=='l'))
  {
    currServo=REL;
  }
  else if((rcmd[0]=='V') && (rcmd[1]=='2'))
  {
    
  }*/
}

void requestEvent()
{
  char values[26];
  sprintf(values,"AR:RSF:%03d,RSL:%03d,REL:%03d",servoCurrData[0].curr,servoCurrData[1].curr,servoCurrData[REL].curr);
  //Serial.println(values); 
  Wire.write(values);
}

/*void UpdateServos(void)
{
  if(serInput==97)
  {
    if(arm_RSL_pos<180)
    {
      arm_RSL_pos++;
    }
  }
  else if(serInput==122)
  {
    if(arm_RSL_pos>0)
    {
      arm_RSL_pos--;
    }
  }
  else if(serInput==115)
  {
    if(arm_RSF_pos<180)
    {
      arm_RSF_pos++;
    }
  }
  else if(serInput==120)
  {
    if(arm_RSF_pos>0)
    {
      arm_RSF_pos--;
    }
  }
  else if(serInput==100)
  {
    if(arm_REL_pos<114)
    {
      arm_REL_pos++;
    }
  }
  else if(serInput==99)
  {
    if(arm_REL_pos>0)
    {
      arm_REL_pos--;
    }
  }
  
  Serial.print("arm_RSL_pos:");
  Serial.print(arm_RSL_pos);
  Serial.print(" ;arm_RSF_pos:");
  Serial.print(arm_RSF_pos);
  Serial.print(" ;Uarm_REL_pos:");
  Serial.println(arm_REL_pos);            
  
  if(prev_arm_RSL_pos!=arm_RSL_pos)
  {
    prev_arm_RSL_pos=arm_RSL_pos;
    RSL.write(arm_RSL_pos);
  }
  
  if(prev_arm_RSF_pos!=arm_RSF_pos)
  {
    prev_arm_RSF_pos=arm_RSF_pos;
    servoShoulderLeftFrontal.write(arm_RSF_pos);
  }  
  
  if(prev_arm_REL_pos!=arm_REL_pos)
  {
    prev_arm_REL_pos=arm_REL_pos;
    servoElbowLeft.write(arm_REL_pos);
  }
}*/
