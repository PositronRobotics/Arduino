#include "AFMotor.h"

AF_DCMotor motorLeft(4);
AF_DCMotor motorRight(3);

void setup() {
  Serial.begin(115200);

  motorLeft.setSpeed(255); 
  motorLeft.run(RELEASE);
  motorRight.setSpeed(255); 
  motorRight.run(RELEASE);

  Serial.println("In setup");
}

void loop()
{
    while(1)
    {
      Serial.println("FWD");   
      motorLeft.run(FORWARD);
      motorRight.run(BACKWARD);
      delay(4000);

      Serial.println("STOPPED");
      motorLeft.run(RELEASE);
      motorRight.run(RELEASE);
      delay(4000);

      Serial.println("BACK");   
      motorLeft.run(BACKWARD);
      motorRight.run(FORWARD);
      delay(4000);

      Serial.println("STOPPED");
      motorLeft.run(RELEASE);
      motorRight.run(RELEASE);
      delay(4000);            
    }
}
