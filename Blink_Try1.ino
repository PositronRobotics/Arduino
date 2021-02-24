#include <Servo.h>
#include "AFMotor.h"

//PreProcessor
//------------

//PreProcessor - Servo Basics
#define NOOFSERVOSARMED 8

#define ARM_RSF_POS_HOME 43
#define ARM_RSL_POS_HOME 159
#define ARM_REL_POS_HOME 103
#define ARM_LSF_POS_HOME 137
#define ARM_LSL_POS_HOME 26
#define ARM_LEL_POS_HOME 75
#define ARM_NAZ_POS_HOME 90
#define ARM_NEL_POS_HOME 90

#define ARM_RSF_PIN 9
#define ARM_RSL_PIN 10
#define ARM_REL_PIN 11
#define ARM_LSF_PIN 2
#define ARM_LSL_PIN 3
#define ARM_LEL_PIN 13
#define ARM_NAZ_PIN 14
#define ARM_NEL_PIN 15

#define RANDOM_INITIAL_POS 9
#define MOVING_TO_HOME_POS 0
#define MOVED_TO_HOME_POS 1

#define UNDEFINED_GENERAL 999

#define DELAY_BETWEEN_SERVOS_HOME_COMING 2000
#define COUNT_FOR_A_SECOND 200000
#define DELAY_AFTER_POWERINGON_BEFORE_HOME_COMING 4000

//PreProcessor - Choreography
#define MANUAL 0
#define CHOREOGRAPHED 1

#define NOOFCHOREOSTATES 3

#define CHOREO_STATE_IDLE 999

#define CHOREO_STATE_INITAL_WAIT 0
#define CHOREO_STATE_WALK_GAIT 1
#define CHOREO_STATE_DUMMY 2

//PreProcessor - Vehicle
#define WHEEL_SPEED_HIGH 255

#define NORMAL_RUNNING_TIME_OF_STATE 0
#define STOP_GIVEN_FROM_STATE_MACHINE 1
#define PROCEDURE_GAVE_GO_AHEAD_TO_STOP 2

//Structures and Tables
//---------------------

//Structures and Tables - Servo Basics
enum ArmServoEnum{RSF,RSL,REL,LSF,LSL,LEL,NAZ,NEL};

struct sservoConstData
{
  int pin;
  int initialPos;
  int initOrder;
};

struct sservoConstData servoConstData[NOOFSERVOSARMED]=
{
  {ARM_RSF_PIN,ARM_RSF_POS_HOME,2},
  {ARM_RSL_PIN,ARM_RSL_POS_HOME,1},
  {ARM_REL_PIN,ARM_REL_POS_HOME,0},
  {ARM_LSF_PIN,ARM_LSF_POS_HOME,5},
  {ARM_LSL_PIN,ARM_LSL_POS_HOME,4},
  {ARM_LEL_PIN,ARM_LEL_POS_HOME,3},
  {ARM_NAZ_PIN,ARM_NAZ_POS_HOME,6},
  {ARM_NEL_PIN,ARM_NEL_POS_HOME,7},
};

Servo servoHW[NOOFSERVOSARMED];

struct sservoCurrData
{
  int prev;
  int curr;
};

struct sservoCurrData servoCurrData[NOOFSERVOSARMED];

//Structures and Tables - Choreo
struct schoreoTable
{
  int choreoState;
  void (*proc)(void);
  int duration;
};

void choreo_state_walk_gait(void);
void choreo_state_dummy(void);

struct schoreoTable choreoTable[NOOFCHOREOSTATES]=
{
  {CHOREO_STATE_INITAL_WAIT,NULL,2},
  {CHOREO_STATE_WALK_GAIT,choreo_state_walk_gait,7},
  {CHOREO_STATE_DUMMY,choreo_state_dummy,3},
};

//Variables
//---------

//Variables - for Manual Control from Blynk
int servo_control_flag=0;
int currServo=UNDEFINED_GENERAL;
int controlMode=CHOREOGRAPHED;

//Variables - Servo Basics
int move_home_process=RANDOM_INITIAL_POS;

//Variables - Vehicle
AF_DCMotor motorLeft(3);
AF_DCMotor motorRight(4);
int wheelsDir=0;

//func declarations
//-----------------

void UpdateServos(void);

void setup()
{ 
  Serial.begin(115200);

  motorLeft.run(RELEASE);
  motorRight.run(RELEASE);

  motorLeft.setSpeed(WHEEL_SPEED_HIGH);
  motorRight.setSpeed(WHEEL_SPEED_HIGH);  

  for(int i=0;i<NOOFSERVOSARMED;i++)
  {    
    servoCurrData[i].prev=UNDEFINED_GENERAL;
    servoCurrData[i].curr=servoConstData[i].initialPos;    
  }
}

void loop()
{
  static int delayCtr_beforeHomeComing=0;
  
  if(move_home_process==RANDOM_INITIAL_POS)
  {
    if(controlMode==CHOREOGRAPHED)
    {
      if(delayCtr_beforeHomeComing++>=DELAY_AFTER_POWERINGON_BEFORE_HOME_COMING)
      {
        move_home_process=MOVING_TO_HOME_POS;            
      }
    }
  }
  else if(move_home_process==MOVING_TO_HOME_POS)
  {  
    moveHome_allServos1by1();
    move_home_process=MOVED_TO_HOME_POS;
  }
  else if(move_home_process==MOVED_TO_HOME_POS)
  {
    if(controlMode==MANUAL)
    {
      //manualChangeFromBlynk();
    }
    else if(controlMode=CHOREOGRAPHED)
    {
      choreography(); 
    }    
  }
}

void moveHome_allServos1by1(void)
{
  int initCtr,j;
  
  for(initCtr=0;initCtr<NOOFSERVOSARMED;initCtr++)
  {    
    for(j=0;j<NOOFSERVOSARMED;j++)
    {              
      if(initCtr==servoConstData[j].initOrder)
      {
        servoHW[j].attach(servoConstData[j].pin);
        servoHW[j].write(servoConstData[j].initialPos);
  
        Serial.print("Servo:");
        Serial.print(j);
        Serial.print(", servoConstData[j].initialPos:");
        Serial.print(servoConstData[j].initialPos);
        Serial.print(", servoConstData[j].pin:");
        Serial.println(servoConstData[j].pin);                    
  
        break;
      }
    }
    delay(DELAY_BETWEEN_SERVOS_HOME_COMING);
  }
}

int StateEndProcess=NORMAL_RUNNING_TIME_OF_STATE;

void choreography(void)
{
  static int choreo_state_cur=0;

  static long choreo1SecondCtr=0;
  static int choreoSeconds=0;  

  if(choreo_state_cur<NOOFCHOREOSTATES)
  {
    //Serial.println("choreography1");
    if(StateEndProcess==NORMAL_RUNNING_TIME_OF_STATE)
    {
      //Serial.println("choreography2");
      if(choreoSeconds<=choreoTable[choreo_state_cur].duration)
      {
        //Serial.println("choreography3");
        if(choreoTable[choreo_state_cur].proc!=NULL)
        {
          //Serial.println("choreography4");
          choreoTable[choreo_state_cur].proc();
        }  
        
        if(choreo1SecondCtr++>COUNT_FOR_A_SECOND)
        {
          //Serial.println("choreography5");
          choreo1SecondCtr=0;
          choreoSeconds++;
        }   
      }
      else
      {
        //Serial.println("choreography6"); 
        if(choreoTable[choreo_state_cur].proc!=NULL)
        {
          //Serial.println("choreography7");
          choreoTable[choreo_state_cur].proc();
          StateEndProcess=STOP_GIVEN_FROM_STATE_MACHINE;
        }
        else
        {
          //Serial.println("choreography8");
          StateEndProcess=PROCEDURE_GAVE_GO_AHEAD_TO_STOP;
        }
      }
    }
    else if(StateEndProcess==STOP_GIVEN_FROM_STATE_MACHINE)
    {
      //Serial.println("choreography9");        
      if(choreoTable[choreo_state_cur].proc!=NULL)
      {
        //Serial.println("choreography10");
        choreoTable[choreo_state_cur].proc();
      }      
    }     
    else if(StateEndProcess==PROCEDURE_GAVE_GO_AHEAD_TO_STOP)
    {
      //Serial.println("choreography11");
      StateEndProcess=NORMAL_RUNNING_TIME_OF_STATE;
            
      choreo1SecondCtr=0;
      choreoSeconds=0;
      choreo_state_cur++;        
    }       
  }
}

#define COMMON_LATERAL_SPREAD_MAX 34
#define COMMON_FROMT_SWING_MAX 62
#define COMMON_BACK_SWING_MAX ARM_RSF_POS_HOME
#define COMMON_GNG_FRONT_SLOW_BOUNDARY 18
#define COMMON_GNG_BACK_SLOW_BOUNDARY 21
#define COMMON_LATERAL_INNER_SWING_MAX 6

#define RSL_GAIT_LATERAL_SPREAD_LIMIT ARM_RSL_POS_HOME-COMMON_LATERAL_SPREAD_MAX
#define LSL_GAIT_LATERAL_SPREAD_LIMIT ARM_LSL_POS_HOME+COMMON_LATERAL_SPREAD_MAX //Not used

#define RSF_GAIT_FRONT_LIMIT ARM_RSF_POS_HOME+COMMON_FROMT_SWING_MAX
#define RSF_GAIT_BACK_LIMIT ARM_RSF_POS_HOME-COMMON_BACK_SWING_MAX
#define RSF_GAIT_GNG_FRONT_SLOW_BOUNDARY RSF_GAIT_BACK_LIMIT+COMMON_GNG_FRONT_SLOW_BOUNDARY
#define RSF_GAIT_GNG_BACK_SLOW_BOUNDARY RSF_GAIT_BACK_LIMIT+COMMON_GNG_BACK_SLOW_BOUNDARY

#define LSF_GAIT_FRONT_LIMIT ARM_LSF_POS_HOME-COMMON_FROMT_SWING_MAX  //Not used
#define LSF_GAIT_BACK_LIMIT ARM_LSF_POS_HOME+COMMON_BACK_SWING_MAX
#define LSF_GAIT_GNG_FRONT_SLOW_BOUNDARY LSF_GAIT_BACK_LIMIT-COMMON_GNG_FRONT_SLOW_BOUNDARY
#define LSF_GAIT_GNG_BACK_SLOW_BOUNDARY LSF_GAIT_BACK_LIMIT-COMMON_GNG_BACK_SLOW_BOUNDARY

#define RSL_INNER_SWING_MAX ARM_RSL_POS_HOME+COMMON_LATERAL_INNER_SWING_MAX
#define LSL_INNER_SWING_MAX ARM_LSL_POS_HOME-COMMON_LATERAL_INNER_SWING_MAX

void choreo_walk_gait_derive_RSL_and_REL(void)
{
  if(servoCurrData[RSF].curr>=ARM_RSF_POS_HOME)
  {
    if(servoCurrData[RSF].curr+82<=RSL_INNER_SWING_MAX)
    {
      servoCurrData[RSL].curr=servoCurrData[RSF].curr+82;
    }
    if(servoCurrData[RSL].curr>=RSL_INNER_SWING_MAX)
    {
      servoCurrData[REL].curr=(servoCurrData[RSF].curr+20-((servoCurrData[RSF].curr-83)*2));
    }
  }
  else
  {
    if((servoCurrData[RSF].curr+84+((42-servoCurrData[RSF].curr)*2))<=RSL_INNER_SWING_MAX)
    {
      servoCurrData[RSL].curr=(servoCurrData[RSF].curr+84+((42-servoCurrData[RSF].curr)*2));
    }
  }
}

void choreo_walk_gait_derive_LSL_and_LEL(void)
{
  if(servoCurrData[LSF].curr<=ARM_LSF_POS_HOME)
  {
    if(servoCurrData[LSF].curr-77>=LSL_INNER_SWING_MAX)
    {
      servoCurrData[LSL].curr=servoCurrData[LSF].curr-77;
    }
    if(servoCurrData[LSL].curr<=LSL_INNER_SWING_MAX)
    {
      servoCurrData[LEL].curr=(servoCurrData[LSF].curr-20+((97-servoCurrData[LSF].curr)*2));
    }
  }
  else
  {
    if( ( (servoCurrData[LSF].curr-79)  - ((servoCurrData[LSF].curr-138)*2) ) >=LSL_INNER_SWING_MAX)
    {
      servoCurrData[LSL].curr=( (servoCurrData[LSF].curr-79)  - ((servoCurrData[LSF].curr-138)*2) );
    }
  }
}

void choreo_state_walk_gait(void)
{ 
  static int zeroTo180=1;
  static long driveMotorctr=0;

  static int walkGait_SubState=0;

  static int timeCtr;

  static int OnlyEven=0;

  int servoTraverse=0;
  int walkGait_SubState_3_in_Progress=0;

  if(driveMotorctr++>=3000)
  {
    Serial.println("choreo_state_walk_gait()");
    
    driveMotorctr=0;

    if(walkGait_SubState==0)
    {
      if(servoCurrData[RSL].curr>RSL_GAIT_LATERAL_SPREAD_LIMIT)
      {
        servoCurrData[RSL].curr--;
        servoCurrData[LSL].curr++;
      }
      else
      {
        walkGait_SubState=1;
      }
    }
    else if(walkGait_SubState==1)
    {
      if(zeroTo180==1)
      {
        if(servoCurrData[RSF].curr<RSF_GAIT_FRONT_LIMIT)
        {
          //Right 
          if(servoCurrData[RSF].curr>=RSF_GAIT_GNG_FRONT_SLOW_BOUNDARY)
          {
            servoCurrData[RSF].curr++;
            choreo_walk_gait_derive_RSL_and_REL();
          }
          else
          {
            if(OnlyEven==0)
            {
              servoCurrData[RSF].curr++;
              choreo_walk_gait_derive_RSL_and_REL();
              OnlyEven=1;
            }
            else
            {
              OnlyEven=0;
            }
          }
          //Right Ends

          //Left
          if(servoCurrData[LSF].curr<=LSF_GAIT_GNG_BACK_SLOW_BOUNDARY)
          {
            servoCurrData[LSF].curr++;
            choreo_walk_gait_derive_LSL_and_LEL();
          }
          else
          {
            if(OnlyEven==0)
            {
              servoCurrData[LSF].curr++;
              choreo_walk_gait_derive_LSL_and_LEL();
              OnlyEven=1;             
            }
            else
            {
              OnlyEven=0;
            }
          }
          //Left Ends 
                    
        }
        else if(servoCurrData[RSF].curr==RSF_GAIT_FRONT_LIMIT)
        {
          zeroTo180=0;

          motorLeft.run(BACKWARD);
          motorRight.run(BACKWARD);          
        }
      }
      else
      {
        if(servoCurrData[RSF].curr>RSF_GAIT_BACK_LIMIT)
        {

          //Right
          if(servoCurrData[RSF].curr>=RSF_GAIT_GNG_BACK_SLOW_BOUNDARY)
          {
            servoCurrData[RSF].curr--;
            choreo_walk_gait_derive_RSL_and_REL();
          }
          else
          {
            if(OnlyEven==0)
            {
              servoCurrData[RSF].curr--;
              choreo_walk_gait_derive_RSL_and_REL();
              OnlyEven=1;             
            }
            else
            {
              OnlyEven=0;
            }
          }
          //Right Ends

          //Left
          if(servoCurrData[LSF].curr<=LSF_GAIT_GNG_FRONT_SLOW_BOUNDARY)
          {
            servoCurrData[LSF].curr--;
            choreo_walk_gait_derive_LSL_and_LEL();
          }
          else
          {
            if(OnlyEven==0)
            {
              servoCurrData[LSF].curr--;
              choreo_walk_gait_derive_LSL_and_LEL();
              OnlyEven=1;
            }
            else
            {
              OnlyEven=0;
            }
          }
          //Left Ends
        }
        else if(servoCurrData[RSF].curr==RSF_GAIT_BACK_LIMIT)
        {
          zeroTo180=1;

          //motorLeft.run(BACKWARD);
          //motorRight.run(BACKWARD);          
        }
      }

      if(StateEndProcess==STOP_GIVEN_FROM_STATE_MACHINE)
      {
        if(servoCurrData[RSF].curr==ARM_RSF_POS_HOME)
        {
          walkGait_SubState=2;
        }
      }
    }
    else if(walkGait_SubState==2)
    {
      walkGait_SubState_3_in_Progress=0;
      
      for(int servoTraverse=0;servoTraverse<NOOFSERVOSARMED;servoTraverse++)
      {              
        if(servoCurrData[servoTraverse].curr>servoConstData[servoTraverse].initialPos)
        {
          servoCurrData[servoTraverse].curr--;
          walkGait_SubState_3_in_Progress=1;
        }
        if(servoCurrData[servoTraverse].curr<servoConstData[servoTraverse].initialPos)
        {
          servoCurrData[servoTraverse].curr++;
          walkGait_SubState_3_in_Progress=1;
        }
      }

      if(walkGait_SubState_3_in_Progress==0)
      {
        StateEndProcess=PROCEDURE_GAVE_GO_AHEAD_TO_STOP;
      }
    }
    
    UpdateServos();
  }  
}

void choreo_state_dummy(void)
{
  static int ctr=0;

  if(ctr++>1000)
  {
    ctr=0;
    Serial.println("choreo_state_dummy");
  }

  motorLeft.run(RELEASE);
  motorRight.run(RELEASE);      
}

void UpdateServos(void)
{
  int updateCtr;  
  for(updateCtr=0;updateCtr<NOOFSERVOSARMED;updateCtr++)
  {
    if(servoCurrData[updateCtr].prev!=servoCurrData[updateCtr].curr)
    {      
      servoHW[updateCtr].write(servoCurrData[updateCtr].curr);
      servoCurrData[updateCtr].prev=servoCurrData[updateCtr].curr;
    }
  }
}
