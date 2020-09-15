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
  if (Serial.available() > 0)
  {
    serInput=0;
    serInput = Serial.read();

    //Serial.print("Received:");
    //Serial.println(serInput);

    if((serInput==97)||(serInput==122)||(serInput==115)||(serInput==120)||(serInput==100)||(serInput==99))
    {
      if(serInput==97)
      {
        if(servoShoulderLeftLateral_pos<180)
        {
          servoShoulderLeftLateral_pos++;
        }
      }
      else if(serInput==122)
      {
        if(servoShoulderLeftLateral_pos>0)
        {
          servoShoulderLeftLateral_pos--;
        }
      }
      else if(serInput==115)
      {
        if(servoShoulderLeftFrontal_pos<180)
        {
          servoShoulderLeftFrontal_pos++;
        }
      }
      else if(serInput==120)
      {
        if(servoShoulderLeftFrontal_pos>0)
        {
          servoShoulderLeftFrontal_pos--;
        }
      }
      else if(serInput==100)
      {
        if(servoElbowLeft_pos<114)
        {
          servoElbowLeft_pos++;
        }
      }
      else if(serInput==99)
      {
        if(servoElbowLeft_pos>0)
        {
          servoElbowLeft_pos--;
        }
      }

      Serial.print("servoShoulderLeftLateral_pos:");
      Serial.print(servoShoulderLeftLateral_pos);
      Serial.print(" ;servoShoulderLeftFrontal_pos:");
      Serial.print(servoShoulderLeftFrontal_pos);
      Serial.print(" ;UservoElbowLeft_pos:");
      Serial.println(servoElbowLeft_pos);            

      if(prev_servoShoulderLeftLateral_pos!=servoShoulderLeftLateral_pos)
      {
        prev_servoShoulderLeftLateral_pos=servoShoulderLeftLateral_pos;
        servoShoulderLeftLateral.write(servoShoulderLeftLateral_pos);
      }

      if(prev_servoShoulderLeftFrontal_pos!=servoShoulderLeftFrontal_pos)
      {
        prev_servoShoulderLeftFrontal_pos=servoShoulderLeftFrontal_pos;
        servoShoulderLeftFrontal.write(servoShoulderLeftFrontal_pos);
      }  

      if(prev_servoElbowLeft_pos!=servoElbowLeft_pos)
      {
        prev_servoElbowLeft_pos=servoElbowLeft_pos;
        servoElbowLeft.write(servoElbowLeft_pos);
      }  
    }
  }
}

// function that executes whenever data is received from master
void receiveEvent(int howMany)
{
  while (0 <Wire.available())
  {
    char c = Wire.read();      /* receive byte as a character */
    Serial.print(c);           /* print the character */
  }
  Serial.println();             /* to newline */
}
