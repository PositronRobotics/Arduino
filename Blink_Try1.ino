#include <Servo.h>
#include <Wire.h>

#define SERVOSHOULDERRIGHTFRONT_POS_HOME 38
#define SERVOSHOULDERRIGHTLATERAL_POS_HOME 159
#define SERVOELBOWRIGHT_POS_HOME 103

#define UNDEFINED 9
#define MOVING_TO_HOME_POS 0
#define MOVED_TO_HOME_POS 1

Servo servoShoulderRightFrontal;
Servo servoShoulderRightLateral;
Servo servoElbowRight;

int servoShoulderRightFrontal_pos = SERVOSHOULDERRIGHTFRONT_POS_HOME;
int servoShoulderRightLateral_pos = SERVOSHOULDERRIGHTLATERAL_POS_HOME;
int servoElbowRight_pos = SERVOELBOWRIGHT_POS_HOME;

int curr_m1=0;
int prev_m1=0;
int move_home_process=UNDEFINED;

// create servo object to control a servo
// twelve servo objects can be created on most boards

void setup()
{
  Serial.begin(115200);

  Serial.print("program start:19-mar:100");
  Serial.println();  

  Wire.begin(8);
  Wire.onReceive(receiveEvent);
}

void loop()
{
  /*if((prev_m1==0) && (curr_m1==1))
  {
    move_home_process=MOVING_TO_HOME_POS;    
    prev_m1=curr_m1;    
    
    servoElbowRight.attach(11);
    servoElbowRight.write(SERVOELBOWRIGHT_POS_HOME);
    Serial.println("servoElbowRight");
    delay(5000);

    servoShoulderRightLateral.attach(10);
    servoShoulderRightLateral.write(SERVOSHOULDERRIGHTLATERAL_POS_HOME);
    Serial.println("servoShoulderRightLateral_pos");
    delay(5000);

    servoShoulderRightFrontal.attach(9);
    servoShoulderRightFrontal.write(SERVOSHOULDERRIGHTFRONT_POS_HOME);
    Serial.println("servoShoulderRightFrontal");
    delay(5000);

    move_home_process=MOVED_TO_HOME_POS;
  }*/

  Serial.print("In loop:25-mar:101");
  Serial.println();

  delay(2000);
}

// function that executes whenever data is received from master
void receiveEvent(int howMany)
{
  String rcmd="";

  char angleStr[4];
  int angle=0;
  int m1=0;
  
  while (0 <Wire.available())
  {
    rcmd += (char)Wire.read();
  }
  Serial.print("Recd Cmd:");
  Serial.print(rcmd);
  Serial.println();

  /*if((rcmd[0]=='m') && (rcmd[1]=='1'))
  {
    m1=(rcmd.substring(3,4)).toInt();
    Serial.print("m1=");
    Serial.print(m1);
    Serial.println();
    
    if(move_home_process!=MOVING_TO_HOME_POS)
    {
      Serial.println("Assigned to curr_m1");
      curr_m1=m1;      
    }
  }
  else
  {
    if(move_home_process==MOVED_TO_HOME_POS)
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
      Serial.println();
    }
  }*/
}
