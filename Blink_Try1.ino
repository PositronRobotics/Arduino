/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Sweep
*/

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

  servoShoulderRightFrontal.attach(9);
}

void loop()
{
  delay(2000);
   
  int16_t bigNum;
   
  byte a,b;
  Wire.requestFrom(54,2);
   
  a = Wire.read();
  b = Wire.read();
   
  bigNum = a;
  bigNum = bigNum << 8 | b;
   
  Serial.print(bigNum);
  Serial.print("\n");
}
