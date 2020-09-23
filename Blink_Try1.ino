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

int zeroTo180=1;
int m1=0;
long driveMotorctr=0;

char serInput=0;

// create servo object to control a servo
// twelve servo objects can be created on most boards

void setup() {
  Serial.begin(115200);

  Wire.begin(8);
  Wire.onReceive(receiveEvent);

  servoShoulderRightFrontal.attach(9);
}

void loop()
{
  delay(1);
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

  //m1=(rcmd.substring(3,4)).toInt();
}
