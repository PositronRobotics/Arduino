#include <Servo.h>
#include <Wire.h>

#define SERVOSHOULDERRIGHTFRONT_POS_HOME 38
#define servo10_POS_HOME 159
#define SERVOELBOWRIGHT 103

Servo servo9;
Servo servo10;
Servo servo11;
Servo servo13;
Servo servo2;

int servo9_pos = 0;
int servo10_pos = 0;
int servo11_pos = 0;

int zeroTo180=1;
int m1=1;
long driveMotorctr=0;

char serInput=0;

// create servo object to control a servo
// twelve servo objects can be created on most boards

void setup() {
  Serial.begin(115200);

  Wire.begin(8);
  Wire.onReceive(receiveEvent);

  servo9.attach(9);
  servo10.attach(10);
  servo11.attach(11);
  servo13.attach(13);
  servo2.attach(2);
}

void loop()
{

  //Serial.println("Starting...");
  
  if(m1==0)
  {
    Serial.print("M1 Off");
    Serial.println();
  }
  else
  {
    //Serial.print("M1 On");
    //Serial.println();

    if(driveMotorctr++>=6000)
    {
      driveMotorctr=0;
  
      if(zeroTo180==1)
      {
        if(servo9_pos<180)
        {
          servo9_pos++; 
        }
        else if(servo9_pos==180)
        {
          zeroTo180=0;
        }
      }
      else
      {
        if(servo9_pos>0)
        {
          servo9_pos--;
        }
        else if(servo9_pos==0)
        {
          zeroTo180=1;
        }
      }
      
      servo9.write(servo9_pos);
      servo10.write(servo9_pos);
      servo11.write(servo9_pos);
      servo13.write(servo9_pos);
      servo2.write(servo9_pos);
        
      Serial.print("servo9_pos:");
      Serial.print(servo9_pos);
      Serial.println();
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
  //Serial.print("Recd Cmd:");
  //Serial.print(rcmd.substring(3,4));
  //Serial.println();             /* to newline */

  m1=(rcmd.substring(3,4)).toInt();
}
