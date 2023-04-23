#include <Servo.h>
#include <Wire.h>

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
  Serial.print("In loop:22-apr:101i");
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
