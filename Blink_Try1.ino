#include <Servo.h>
#include <Wire.h>
#include "AFMotor.h"

//PreProcessor
//------------

//PreProcessor - Servo Basics
#define NOOFSERVOSARMED 5

#define ARM_RSF_POS_HOME 43
#define ARM_RSL_POS_HOME 159
#define ARM_REL_POS_HOME 103
#define ARM_NAZ_POS_HOME 90
#define ARM_NEL_POS_HOME 90

#define ARM_RSF_PIN 9
#define ARM_RSL_PIN 10
#define ARM_REL_PIN 11
#define ARM_NAZ_PIN 14
#define ARM_NEL_PIN 15

#define RANDOM_INITIAL_POS 9
#define MOVING_TO_HOME_POS 0
#define MOVED_TO_HOME_POS 1

#define UNDEFINED_GENERAL 999

#define DELAY_BETWEEN_SERVOS_HOME_COMING 2000
#define COUNT_FOR_A_SECOND 200000
#define DELAY_AFTER_POWERINGON_BEFORE_HOME_COMING 4000

//PreProcessor - Choreography
#define MANUAL 0
#define CHOREOGRAPHED 1

#define NOOFCHOREOSTATES 3

#define CHOREO_STATE_IDLE 999

#define CHOREO_STATE_INITAL_WAIT 0
#define CHOREO_STATE_WALK_GAIT 1
#define CHOREO_STATE_DUMMY 2

//PreProcessor - Vehicle
#define WHEEL_SPEED_HIGH 255

//Structures and Tables
//---------------------

//Structures and Tables - Servo Basics
enum ArmServoEnum{RSF,RSL,REL,NAZ,NEL};

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
  {ARM_NAZ_PIN,ARM_NAZ_POS_HOME,3},
  {ARM_NEL_PIN,ARM_NEL_POS_HOME,4},
};

Servo servoHW[NOOFSERVOSARMED];

struct sservoCurrData
{
  int prev;
  int curr;
};

struct sservoCurrData servoCurrData[NOOFSERVOSARMED];

//Structures and Tables - Choreo
struct schoreoTable
{
  int choreoState;
  void (*proc)(void);
  int duration;
};

void choreo_state_walk_gait(void);
void choreo_state_dummy(void);

struct schoreoTable choreoTable[NOOFCHOREOSTATES]=
{
  {CHOREO_STATE_INITAL_WAIT,NULL,2},
  {CHOREO_STATE_WALK_GAIT,choreo_state_walk_gait,10},
  {CHOREO_STATE_DUMMY,choreo_state_dummy,3},
};

//Variables
//---------

//Variables - for Manual Control from Blynk
int servo_control_flag=0;
int currServo=UNDEFINED_GENERAL;
int controlMode=CHOREOGRAPHED;

//Variables - Servo Basics
int move_home_process=RANDOM_INITIAL_POS;

//Variables - Vehicle
AF_DCMotor motorLeft(3);
AF_DCMotor motorRight(4);
int wheelsDir=0;

//func declarations
//-----------------

void receiveEvent(int howMany);
void requestEvent();
void UpdateServos(void);

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
    servoCurrData[i].prev=UNDEFINED_GENERAL;
    servoCurrData[i].curr=servoConstData[i].initialPos;    
  }
}

void loop()
{
  static int delayCtr_beforeHomeComing=0;
  
  if(move_home_process==RANDOM_INITIAL_POS)
  {
    if(delayCtr_beforeHomeComing++>=DELAY_AFTER_POWERINGON_BEFORE_HOME_COMING)
    {
      move_home_process=MOVING_TO_HOME_POS;            
    }
  }
  else if(move_home_process==MOVING_TO_HOME_POS)
  {  
    moveHome_allServos1by1();
    move_home_process=MOVED_TO_HOME_POS;
  }
  else if(move_home_process==MOVED_TO_HOME_POS)
  {
    if(controlMode==MANUAL)
    {
      manualChangeFromBlynk();
    }
    else if(controlMode=CHOREOGRAPHED)
    {
      choreography(); 
    }    
  }
}

// function that executes whenever data is received from master
void receiveEvent(int howMany)
{
  String rcmd = "";
  
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

  if(move_home_process==RANDOM_INITIAL_POS)
  {
    if((rcmd[0]=='m') && (rcmd[1]=='1'))
    {  
      move_home_process=MOVING_TO_HOME_POS;
    }
  }
  else if(move_home_process==MOVED_TO_HOME_POS)
  {
    if((rcmd[0]=='c') && (rcmd[1]=='m') && (rcmd[2]=='m'))
    {
      controlMode=MANUAL;
    }
    else if((rcmd[0]=='c') && (rcmd[1]=='m') && (rcmd[2]=='c'))
    {
      controlMode=CHOREOGRAPHED;
    }     
    
    if(controlMode==MANUAL)
    {
      if((rcmd[0]=='r') && (rcmd[1]=='s') && (rcmd[2]=='f'))
      {
        currServo=RSF;
      }
      else if((rcmd[0]=='r') && (rcmd[1]=='s') && (rcmd[2]=='l'))
      {
        currServo=RSL;
      }
      else if((rcmd[0]=='r') && (rcmd[1]=='e') && (rcmd[2]=='l'))
      {
        currServo=REL;
      }
      else if((rcmd[0]=='n') && (rcmd[1]=='a') && (rcmd[2]=='z'))
      {
        currServo=NAZ;
      }
      else if((rcmd[0]=='n') && (rcmd[1]=='e') && (rcmd[2]=='l'))
      {
        currServo=NEL;
      }   
      else if((rcmd[0]=='V') && (rcmd[1]=='2'))
      {
        if(rcmd[2]=='0')
        {
          servo_control_flag=0;
        }
        else  if(rcmd[2]=='1')
        {
          servo_control_flag=1;
        }
      }
      else if((rcmd[0]=='V') && (rcmd[1]=='3'))
      {
        if(rcmd[2]=='0')
        {
          servo_control_flag=0;
        }
        else  if(rcmd[2]=='1')
        {
          servo_control_flag=-1;
        }
      }
    }
  }
}

void requestEvent()
{
  char values[26];
  sprintf(values,"ARF%03d,L%03d,E%03d,A%03d,Z%03d",servoCurrData[RSF].curr,servoCurrData[RSL].curr,servoCurrData[REL].curr,servoCurrData[NAZ].curr,servoCurrData[NEL].curr);
  Wire.write(values);
}

void moveHome_allServos1by1(void)
{
  int initCtr,j;
  
  for(initCtr=0;initCtr<NOOFSERVOSARMED;initCtr++)
  {    
    for(j=0;j<NOOFSERVOSARMED;j++)
    {              
      if(initCtr==servoConstData[j].initOrder)
      {
        servoHW[j].attach(servoConstData[j].pin);
        servoHW[j].write(servoConstData[j].initialPos);
  
        Serial.print("Servo:");
        Serial.print(j);
        Serial.print(", servoConstData[j].initialPos:");
        Serial.print(servoConstData[j].initialPos);
        Serial.print(", servoConstData[j].pin:");
        Serial.println(servoConstData[j].pin);                    
  
        break;
      }
    }
    delay(DELAY_BETWEEN_SERVOS_HOME_COMING);
  }
}

void manualChangeFromBlynk(void)
{
  static int changeCtr=0;
  
  if(changeCtr++>15000)
  {    
    changeCtr=0;
    
    if(servo_control_flag==1)
    {
      if(currServo==RSL)
      {
        if(servoCurrData[currServo].curr<165)
        {
          servoCurrData[currServo].curr++;
        }          
      }
      else
      {
        if(servoCurrData[currServo].curr<180)
        {
          servoCurrData[currServo].curr++;
        }
      }
    }
    else if(servo_control_flag==-1)
    {
      if(servoCurrData[currServo].curr>0)
      {
        servoCurrData[currServo].curr--;
      }
    }  
    UpdateServos();
  }
}

void choreography(void)
{
  static int choreo_state_cur=0;

  static long choreo1SecondCtr=0;
  static int choreoSeconds=0;  

  if(choreo_state_cur<NOOFCHOREOSTATES)
  {
    //Serial.print("If - choreo1SecondCtr"); 

    if(choreo1SecondCtr==0)
    {
      Serial.println(choreoSeconds);
    } 
    
    if(choreoSeconds<=choreoTable[choreo_state_cur].duration)
    {
      if(choreoTable[choreo_state_cur].proc!=NULL)
      {
        choreoTable[choreo_state_cur].proc();
      }  
      
      if(choreo1SecondCtr++>COUNT_FOR_A_SECOND)
      {
        choreo1SecondCtr=0;
        choreoSeconds++;
      }   
    }
    else
    {
      choreo1SecondCtr=0;
      choreoSeconds=0;
      choreo_state_cur++;
      Serial.print("Else - choreoSeconds"); 
      Serial.println(choreoSeconds);      
    }
  }
}

void choreo_state_walk_gait(void)
{
  static int zeroTo180=1;
  static long driveMotorctr=0;

  if(driveMotorctr++>=3000)
  {
    driveMotorctr=0;
  
    if(zeroTo180==1)
    {
      if(servoCurrData[RSF].curr<95)
      {
        servoCurrData[RSF].curr++; 
      }
      else if(servoCurrData[RSF].curr==95)
      {
        zeroTo180=0;
      }
    }
    else
    {
      if(servoCurrData[RSF].curr>0)
      {
        servoCurrData[RSF].curr--;
      }
      else if(servoCurrData[RSF].curr==0)
      {
        zeroTo180=1;
      }
    }
    UpdateServos();
  }  
}

void choreo_state_dummy(void)
{
  static int ctr=0;

  if(ctr++>1000)
  {
    ctr=0;
    Serial.println("choreo_state_dummy");
  }   
}

void UpdateServos(void)
{
  int updateCtr;  
  for(updateCtr=0;updateCtr<NOOFSERVOSARMED;updateCtr++)
  {
    if(servoCurrData[updateCtr].prev!=servoCurrData[updateCtr].curr)
    {      
      servoHW[updateCtr].write(servoCurrData[updateCtr].curr);
      servoCurrData[updateCtr].prev=servoCurrData[updateCtr].curr;
    }
  }
}
