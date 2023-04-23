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

  Serial.print("program start:19-mar:101");
  Serial.println();  

  Wire.begin(8);
  Wire.onReceive(receiveEvent);
}

void loop()
{
  Serial.print("In loop:22-apr:101");
  Serial.println();

  delay(2000);
}

// function that executes whenever data is received from master
void receiveEvent(int howMany)
{
  String rcmd="";
  
  while (0 <Wire.available())
  {
    rcmd += (char)Wire.read();
  }
  Serial.print("Recd Cmd:");
  Serial.print(rcmd);
  Serial.println();
}
