#include <Servo.h>
#include <Wire.h>
#include "AFMotor.h"

#define VEHICLE_STOPPED 0
#define VEHICLE_MOVING_FWD 1
#define VEHICLE_MOVING_BCK 2

#define VEHICLE_TURN_STRAIGHT 0
#define VEHICLE_TURN_LEFT 1
#define VEHICLE_TURN_RIGHT 2

AF_DCMotor motorLeft(4);
AF_DCMotor motorRight(3);

int vehicle_state;
int prev_vehicle_state;

int vehicle_turn_state;
int prev_vehicle_turn_state;
int vehicle_some_state_changed;

int vehicle_leftSpeed=0;
int vehicle_rightSpeed=0;

char vehicle_state_text[3][20] =
{ 
  "VEHICLE_STOPPED    ",
  "VEHICLE_MOVING_FWD ",
  "VEHICLE_MOVING_BCK ",
};

char vehicle_turn_state_text[3][20] =
{ 
  "VEHICLE_TURN_STRAIT",
  "VEHICLE_TURN_LEFT  ",
  "VEHICLE_TURN_RIGHT ",
};

void setup()
{
  Serial.begin(115200);

  Wire.begin(8);
  Wire.onReceive(receiveEvent);

  motorLeft.run(RELEASE);
  motorRight.run(RELEASE);

  vehicle_state=VEHICLE_STOPPED;
  prev_vehicle_state=999;
  
  vehicle_turn_state=VEHICLE_TURN_STRAIGHT;
  prev_vehicle_turn_state=999;
  
  vehicle_some_state_changed=0;

  Serial.println("check1");
}

void loop()
{
  if(vehicle_some_state_changed==1)
  {
    Serial.println("------------------");
    Serial.println(vehicle_state_text[vehicle_state]);
    Serial.println(vehicle_turn_state_text[vehicle_turn_state]);
    
    vehicle_some_state_changed=0;
  }
}

// function that executes whenever data is received from master
void receiveEvent(int howMany)
{
  String rcmd="";

  char angleStr[4];
  int angle=0;
  int m1=0;

  int joystick_x=0,joystick_y=0;
  char joystick_xy[1];
  static int prev_vehicle_turn_state=999;
  
  while (0 <Wire.available())
  {
    rcmd += (char)Wire.read();
  }
  //Serial.print("Recd Cmd:");
  //Serial.print(rcmd);
  //Serial.println();

  if((rcmd[0]=='m') && (rcmd[1]=='1'))
  {
    m1=(rcmd.substring(3,4)).toInt();
    Serial.print("m1=");
    Serial.print(m1);
    Serial.println();
  }
  else if((rcmd[0]=='j') && (rcmd[1]=='1'))
  {
    if(vehicle_some_state_changed==0)
    {
      joystick_xy[0]=rcmd[2];
      sscanf(joystick_xy,"%01d",&joystick_x);
      
      joystick_xy[0]=rcmd[3];
      sscanf(joystick_xy,"%01d",&joystick_y);
  
      //Serial.print("joystick_x=");
      //Serial.print(joystick_x); 
  
      //Serial.print("joystick_y=");
      //Serial.print(joystick_y);
      //Serial.println();
  
      if(joystick_x<3)
      {
        vehicle_turn_state=VEHICLE_TURN_LEFT;
      }
      else if(joystick_x>7)
      {
        vehicle_turn_state=VEHICLE_TURN_RIGHT;
      }
      else
      {
        vehicle_turn_state=VEHICLE_TURN_STRAIGHT;
      }

      if(joystick_y>=7)
      {
        vehicle_state=VEHICLE_MOVING_FWD;
      }
      else if(joystick_y<=3)
      {
        vehicle_state=VEHICLE_MOVING_BCK;
      }
      else
      {
        vehicle_state=VEHICLE_STOPPED;
      }

      if(prev_vehicle_state!=vehicle_state)
      {
        prev_vehicle_state=vehicle_state;
        vehicle_some_state_changed=1;
      }

      if(prev_vehicle_turn_state!=vehicle_turn_state)
      {
        prev_vehicle_turn_state=vehicle_turn_state;
        vehicle_some_state_changed=1;
      }
    }
  }
  else
  {   
     angleStr[0]=rcmd[0];
     angleStr[1]=rcmd[1];
     angleStr[2]=rcmd[2];
     angleStr[3]=rcmd[3];
    
     Serial.print("angleStr:");
     Serial.print(angleStr);
     Serial.println();  
    
     Serial.print("angle:");
     sscanf(angleStr,"%04d",&angle);
     Serial.print(angle);
    
     //servoShoulderRightFrontal.write(angle);
      
     //Serial.print(rcmd.substring(3,4));
     Serial.println();
  }

  //
}
