/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Sweep
*/

#include <Servo.h>
#include <Wire.h>

Servo servoShoulderLeftLateral;
Servo servoShoulderLeftFrontal;
Servo servoElbowLeft;

int servoShoulderLeftLateral_pos = 38;
int servoShoulderLeftFrontal_pos = 159;
int servoElbowLeft_pos = 103;

int prev_servoShoulderLeftLateral_pos = 0;
int prev_servoShoulderLeftFrontal_pos = 0;
int prev_servoElbowLeft_pos = 0;

int zeroTo180=1;
int m1=0;

char serInput=0;

// create servo object to control a servo
// twelve servo objects can be created on most boards

void setup() {
  Serial.begin(115200);
  
  servoShoulderLeftLateral.attach(9);  // attaches the servo on pin 9 to the servo object
  servoShoulderLeftFrontal.attach(10);
  servoElbowLeft.attach(11);
  
  servoShoulderLeftLateral.write(servoShoulderLeftLateral_pos);
  servoShoulderLeftFrontal.write(servoShoulderLeftFrontal_pos);
  servoElbowLeft.write(servoElbowLeft_pos);

  Wire.begin(8);
  Wire.onReceive(receiveEvent);
}

void loop()
{
  if(m1==0)
  {
    Serial.print("M1 Off");
    Serial.println();
  }
  else
  {
    Serial.print("M1 Off");
    Serial.println();    
  }
  Serial.print(m1);
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
  //Serial.print("Recd Cmd:");
  //Serial.print(rcmd.substring(3,4));
  //Serial.println();             /* to newline */

  m1=(rcmd.substring(3,4)).toInt();
}
