#include <Servo.h>
#include <Wire.h>

#define SERVOSHOULDERRIGHTFRONT_POS_HOME 38
#define SERVOSHOULDERRIGHTLATERAL_POS_HOME 159
#define SERVOELBOWRIGHT 103

Servo servoShoulderRightFrontal;
Servo servoShoulderRightLateral;
Servo servoElbowRight;

int servoShoulderRightFrontal_pos = SERVOSHOULDERRIGHTFRONT_POS_HOME;
int servoShoulderRightLateral_pos = SERVOSHOULDERRIGHTLATERAL_POS_HOME;
int servoElbowRight_pos = SERVOELBOWRIGHT;

int m1=0;

// create servo object to control a servo
// twelve servo objects can be created on most boards

void setup()
{
  Serial.begin(115200);

  Wire.begin(8);
  Wire.onReceive(receiveEvent);

  servoShoulderRightFrontal.attach(9);
  servoShoulderRightLateral.attach(10);
  servoElbowRight.attach(11);
}

void loop()
{
  delay(1);
}

// function that executes whenever data is received from master
void receiveEvent(int howMany)
{
  String rcmd="";

  char angleStr[4];
  int angle=0;
  
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
  
    servoShoulderRightFrontal.write(angle);
    
    //Serial.print(rcmd.substring(3,4));
    Serial.println();             /* to newline */
  }

  //
}
