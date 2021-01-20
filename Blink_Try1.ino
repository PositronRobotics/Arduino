#include <Servo.h>
#include <Wire.h>
#include "AFMotor.h"

#define VEHICLE_STOPPED 0
#define VEHICLE_MOVING_FWD 1
#define VEHICLE_MOVING_BCK 2

#define VEHICLE_TURN_STRAIGHT 0
#define VEHICLE_TURN_LEFT 1
#define VEHICLE_TURN_RIGHT 2

#define WHEEL_SPEED_HIGH 255
#define WHEEL_SPEED_LOW 220

AF_DCMotor motorLeft(3);
AF_DCMotor motorRight(4);

Servo servoNeckAzimuth;
Servo servoNeckElevation;

Servo ArmRSF;

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

  servoNeckAzimuth.attach(9);
  servoNeckElevation.attach(10);

  ArmRSF.attach(11);

  vehicle_state=VEHICLE_STOPPED;
  prev_vehicle_state=999;
  
  vehicle_turn_state=VEHICLE_TURN_STRAIGHT;
  prev_vehicle_turn_state=999;
  
  vehicle_some_state_changed=0;

  Serial.println("check1");
}

void loop()
{
  changeDCmotorConfigs();
}

void changeDCmotorConfigs(void)
{
  if(vehicle_some_state_changed==1)
  {
    Serial.println("------------------");
    Serial.println(vehicle_state_text[vehicle_state]);
    Serial.println(vehicle_turn_state_text[vehicle_turn_state]);
    
    vehicle_some_state_changed=0;
  }

  if(vehicle_turn_state==VEHICLE_TURN_STRAIGHT)
  {
    motorLeft.setSpeed(WHEEL_SPEED_HIGH);
    motorRight.setSpeed(WHEEL_SPEED_HIGH);
  }
  else if(vehicle_turn_state==VEHICLE_TURN_LEFT)
  {
    motorLeft.setSpeed(WHEEL_SPEED_LOW);
    motorRight.setSpeed(WHEEL_SPEED_HIGH);
  }
  else if(vehicle_turn_state==VEHICLE_TURN_RIGHT)
  {
    motorLeft.setSpeed(WHEEL_SPEED_HIGH);
    motorRight.setSpeed(WHEEL_SPEED_LOW);
  }  

  if(vehicle_state==VEHICLE_MOVING_FWD)
  {
    motorLeft.run(FORWARD);
    motorRight.run(FORWARD);
  }
  else if(vehicle_state==VEHICLE_MOVING_BCK)
  {
    motorLeft.run(BACKWARD);
    motorRight.run(BACKWARD);
  }
  else
  {
    motorLeft.run(RELEASE);
    motorRight.run(RELEASE);
  }
}

// function that executes whenever data is received from master
void receiveEvent(int howMany)
{
  String rcmd="";

  char angleStr[4];
  int angle=0;
  int m1=0;

  int joystick_x_vehicle=0,joystick_y_vehicle=0;
  char joystick_xy_vehicle[1];
  static int prev_vehicle_turn_state=999;

  int joystick_x_head=0,joystick_y_head=0;
  char joystick_xy_head[3];  

  int ArmServoPos=0;
  char ArmServoPos_str[3];   
  
  while (0 <Wire.available())
  {
    rcmd += (char)Wire.read();
  }
  Serial.print("Recd Cmd:");
  Serial.print(rcmd);
  Serial.println();

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
      joystick_xy_vehicle[0]=rcmd[2];
      sscanf(joystick_xy_vehicle,"%01d",&joystick_x_vehicle);
      
      joystick_xy_vehicle[0]=rcmd[3];
      sscanf(joystick_xy_vehicle,"%01d",&joystick_y_vehicle);
  
      Serial.print("joystick_x_vehicle=");
      Serial.print(joystick_x_vehicle); 
  
      Serial.print("joystick_y_vehicle=");
      Serial.print(joystick_y_vehicle);
      Serial.println();
  
      if(joystick_x_vehicle<3)
      {
        vehicle_turn_state=VEHICLE_TURN_LEFT;
      }
      else if(joystick_x_vehicle>7)
      {
        vehicle_turn_state=VEHICLE_TURN_RIGHT;
      }
      else
      {
        vehicle_turn_state=VEHICLE_TURN_STRAIGHT;
      }

      if(joystick_y_vehicle>=7)
      {
        vehicle_state=VEHICLE_MOVING_FWD;
      }
      else if(joystick_y_vehicle<=3)
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
  else if((rcmd[0]=='j') && (rcmd[1]=='2'))
  {
      joystick_xy_head[0]=rcmd[2];
      joystick_xy_head[1]=rcmd[3];
      joystick_xy_head[2]=rcmd[4];
      sscanf(joystick_xy_head,"%03d",&joystick_x_head);
      
      joystick_xy_head[0]=rcmd[5];
      joystick_xy_head[1]=rcmd[6];
      joystick_xy_head[2]=rcmd[7];
      sscanf(joystick_xy_head,"%03d",&joystick_y_head);
  
      Serial.print("joystick_x_head=");
      Serial.print(joystick_x_head); 
  
      Serial.print("joystick_y_head=");
      Serial.print(joystick_y_head);
      Serial.println();

      servoNeckAzimuth.write(joystick_x_head);
      servoNeckElevation.write(joystick_y_head);
  }
  else if((rcmd[0]=='R') && (rcmd[1]=='S') && (rcmd[2]=='F'))
  {
      ArmServoPos_str[0]=rcmd[3];
      ArmServoPos_str[1]=rcmd[4];
      ArmServoPos_str[2]=rcmd[5];
      sscanf(ArmServoPos_str,"%03d",&ArmServoPos);
  
      Serial.print("ArmServoPos=");
      Serial.print(ArmServoPos); 
      Serial.println();

      ArmRSF.write(ArmServoPos);
  }  
}
