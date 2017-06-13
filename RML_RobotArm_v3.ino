#include <Servo.h>

#define NUMBER_OF_SERVOS (4)
#if 1

#define FWD_MIN (46)
#define FWD_MAX (166)
#define LIFT_MIN (80)
#define LIFT_MAX (138)
#define TURN_MIN (0)
#define TURN_MAX (180)
#define JAW_MIN (40)
#define JAW_MAX (140)

#else
//used for calibration - wide limits
#define FWD_MIN (0)
#define FWD_MAX (180)
#define LIFT_MIN (0)
#define LIFT_MAX (180)
#define TURN_MIN (0)
#define TURN_MAX (180)
#define JAW_MIN (0)
#define JAW_MAX (180)

#endif

#define FWD_MED ((FWD_MAX+FWD_MIN)/2)
#define LIFT_MED ((LIFT_MAX+LIFT_MIN)/2)
#define TURN_MED ((TURN_MAX+TURN_MIN)/2)
#define JAW_MED ((JAW_MAX+JAW_MIN)/2)

#define FWD (0)
#define LFT (1)
#define TRN (2)
#define JAW (3)

#define NUM_STATES (6)
#define STEP_DELAY (30)
#define DELAY_BETWEEN_STATES (500)

#define autoStartPin (3)
#define autoStopPin (14)

#define servo_fwd (11)
#define servo_lift (10)
#define servo_turn (9)
#define servo_jaw (5)

typedef struct {
  Servo servo_id;
  const char* servo_name;
  char pin;
  int value_current;
  int value_init;
  int value_min;
  int value_max;
} ServoState;

int states[NUM_STATES][NUMBER_OF_SERVOS];

Servo servos[NUMBER_OF_SERVOS];

/*
  ServoState ServoControl[NUMBER_OF_SERVOS] = {servos[FWD], "servo forward", servo_fwd, FWD_MED, 100, FWD_MIN, FWD_MAX,
                                             servos[LFT], "servo lift", servo_lift, LIFT_MED, 130, LIFT_MIN, LIFT_MAX,
                                             servos[TRN], "servo turn", servo_turn, TURN_MED, 164, TURN_MIN, TURN_MAX,
                                             servos[JAW], "servo jaw", servo_jaw, JAW_MED, 46, JAW_MIN, JAW_MAX
                                            };

*/

ServoState ServoControl[NUMBER_OF_SERVOS] = {servos[FWD], "servo forward", servo_fwd, FWD_MED, 120, FWD_MIN, FWD_MAX,
                                             servos[LFT], "servo lift", servo_lift, LIFT_MED, 100, LIFT_MIN, LIFT_MAX,
                                             servos[TRN], "servo turn", servo_turn, TURN_MED, 105, TURN_MIN, TURN_MAX,
                                             servos[JAW], "servo jaw", servo_jaw, JAW_MED, 46, JAW_MIN, JAW_MAX
                                            };

int autoStartPinValue = LOW;
int autoStopPinValue = LOW;

void setup()
{

  /*
    #define FWD_MIN (46)
    #define FWD_MAX (166)
    #define LIFT_MIN (80)
    #define LIFT_MAX (138)
    #define TURN_MIN (0)
    #define TURN_MAX (180)
    #define JAW_MIN (40)
    #define JAW_MAX (140)


    // positin ready to grab cube
    states[0][0] = 100;
    states[0][1] = 86;
    states[0][2] = 90;
    states[0][3] = 100;

    // position grab cube
    states[1][0] = 160;
    states[1][1] = 86;
    states[1][2] = 90;
    states[1][3] = 46;

    // position over cup
    states[2][0] = 134;
    states[2][1] = 134;
    states[2][2] = 130;
    states[2][3] = 46;

    // position drop cube into cup
    states[3][0] = 134;
    states[3][1] = 134;
    states[3][2] = 130;
    states[3][3] = 100;

    // position over STOP button
    states[4][0] = 134;
    states[4][1] = 134;
    states[4][2] = 165;
    states[4][3] = 46;

    // position push STOP button
    states[5][0] = 166;
    states[5][1] = 80;
    states[5][2] = 165;
    states[5][3] = 46;
  */

  // positin ready to grab cube
  states[0][0] = 100;
  states[0][1] = 86;
  states[0][2] = 30;
  states[0][3] = 100;

  // position grab cube
  states[1][0] = 160;
  states[1][1] = 86;
  states[1][2] = 30;
  states[1][3] = 46;

  // position over cup
  states[2][0] = 134;
  states[2][1] = 134;
  states[2][2] = 70;
  states[2][3] = 46;

  // position drop cube into cup
  states[3][0] = 134;
  states[3][1] = 134;
  states[3][2] = 70;
  states[3][3] = 100;

  // position over STOP button
  states[4][0] = 134;
  states[4][1] = 134;
  states[4][2] = 105;
  states[4][3] = 46;

  // position push STOP button
  states[5][0] = 166;
  states[5][1] = 80;
  states[5][2] = 105;
  states[5][3] = 46;



  for (int i = 0; NUMBER_OF_SERVOS > i; i++)
  {
    ServoControl[i].servo_id.attach(ServoControl[i].pin);
    ServoControl[i].servo_id.write(ServoControl[i].value_init);
    ServoControl[i].value_current = ServoControl[i].value_init;
  }
  Serial.begin(9600);
  PrintHelp();
  Serial.println("Attach done");
  MoveToInitialState();
  Serial.println("Setup done");
}

void loop()
{

  int use_servo = 0;
  String inData;
  String inOperation;
  String inValue;
  int  inValue_int = 0;
  static int do_once = 1;

  inOperation = "#"; // "#" means "No Operation"
  inValue = "";
  inData = "";


  while (Serial.available())
  {
    inData = Serial.readString();
    Serial.print("Arduino Received: ");
    Serial.println(inData);

    inOperation = inData.substring(0, 1);
    if (1 < inData.length())
    {
      inValue = inData.substring(1);
      inValue_int = inValue.toInt();
    }
    else
    {
      inValue = "";
      inValue_int = 0;
    }
  }

  if (1 == do_once)
  {
    do_once = 0;
    inOperation = "a";
    inValue = "";
    inData = "a";
    inValue_int = 0;

  }

#if 0
  autoStartPinValue = digitalRead(autoStartPin);
  if ((HIGH == autoStartPinValue) && (0 == do_once_power))
  {
    do_once_powr = 0;
    inOperation = "a";
    inValue = "";
    inData = "a";
    inValue_int = 0;
  }

#endif

#if 0
  autoStopPinValue = digitalRead(autoStopPin);
  if (HIGH == autoStopPinValue)
  {
    inOperation = "q";
    inValue = "";
    inData = "q";
    inValue_int = 0;

  }
#endif

  {
    switch (inOperation[0])
    {

      case '#' :
        {
          delay(10); // "No operation" sleep a bit to save power
        }
        break;

      case 'h' :
      case 'H' :
        {
          PrintHelp();
        }
        break;


      case 'q' :
        {
          MoveToInitialState();
        }
        break;

      case 'i' :
      case 'I' :
        {
          MoveToInitialState();
        }
        break;

      case 'a' :
      case 'A' :
        {
          Serial.println("Auto sequence start");
          MoveToInitialState();
          for (int i = 0; NUM_STATES > i; i++)
          {
            Serial.println("=====================");
            MoveToState(i);
            delay (DELAY_BETWEEN_STATES);

          }
          Serial.println("=====================");
          MoveToState(4);
          inOperation = "";
        }
        break;

      case 'z' :
      case 'Z' :
        {
          StoreState(inValue_int);
        }
        break;

      // Move Servo FWD (0)
      // ====================================
      case 'f' :
      case 'F' :
        {
          use_servo = FWD;
          MoveServoSmoothByValue(use_servo, inValue_int);
        }
        PrintCurrentServoValues();
        break;
      // ====================================


      // Move Servo LFT (1)
      // ====================================
      case 'l' :
      case 'L' :
        {
          use_servo = LFT;
          MoveServoSmoothByValue(use_servo, inValue_int);
        }
        PrintCurrentServoValues();
        break;
      // ====================================


      // Move Servo TRN (2)
      // ====================================
      case 't' :
      case 'T' :
        {
          use_servo = TRN;
          MoveServoSmoothByValue(use_servo, inValue_int);
        }
        PrintCurrentServoValues();
        break;
      // ====================================

      // Move Servo JAW (3)
      // ====================================
      case 'j' :
      case 'J' :
        {
          use_servo = JAW;
          MoveServoSmoothByValue(use_servo, inValue_int);
        }
        PrintCurrentServoValues();
        break;
      // ====================================


      case 'p' :
      case 'P' :
        PrintCurrentServoValues();
        PrintAllServoValues();
        break;



      default :
        {
          Serial.println("Input command is not decognised");
          break;
        }
    }
  }
}

void MoveToInitialState()
{
  Serial.println("MoveToInitialState entry");

  for (int i = 0; NUMBER_OF_SERVOS > i; i++)
  {
    MoveServoSmoothToPosition(i, ServoControl[i].value_init);
    Serial.print("ServoControl[");
    Serial.print(i);
    Serial.print("] set to ");
    Serial.println(ServoControl[i].value_init);
  }

  Serial.println("MoveToInitialState exit");
}

void MoveJawOpen()
{
  Serial.println("MoveJawOpen entry");
  ServoControl[JAW].value_current = JAW_MAX;
  ServoControl[JAW].servo_id.write(ServoControl[JAW].value_current);
  Serial.println("MoveJawOpen exit");
}

void MoveJawClose()
{
  Serial.println("MoveJawClose entry");
  ServoControl[JAW].value_current = JAW_MIN;
  ServoControl[JAW].servo_id.write(ServoControl[JAW].value_current);
  Serial.println("MoveJawClose exit");
}

void MoveToState(int state_id)
{
  Serial.print("MoveToState entry state=");
  Serial.println(state_id);
  for (int i = 0; NUMBER_OF_SERVOS > i; i++)
  {
    if ((ServoControl[i].value_min <= states[state_id][i]) && (states[state_id][i] <= ServoControl[i].value_max))
    {
      MoveServoSmoothToPosition(i, states[state_id][i]);
      Serial.print("ServoControl[");
      Serial.print(i);
      Serial.print("] set to ");
      Serial.println(ServoControl[i].value_current);
    }
    else
    {
      Serial.println("Error: value outside limits");
    }
  }
  Serial.println("MoveToState exit");
}

void StoreState(int state_id)
{
  Serial.print("StoreState entry. state=");
  Serial.println(state_id);
  if ((0 <= state_id) && (NUM_STATES > state_id))
  {
    for (int i = 0; NUMBER_OF_SERVOS > i; i++)
    {
      states[state_id][i] = ServoControl[i].value_current;

      Serial.print("states[");
      Serial.print(state_id);
      Serial.print("][");
      Serial.print(i);
      Serial.print("] = ");
      Serial.println(states[state_id][i]);
    }
  }
  Serial.println("StoreState exit");
}

void PrintCurrentServoValues()
{
  Serial.println("PrintCurrentServoValues entry");
  Serial.println("=====================");
  for (int i = 0; NUMBER_OF_SERVOS > i; i++)
  {
    Serial.print("[");
    Serial.print(i);
    Serial.print("] = ");
    Serial.print(ServoControl[i].value_current);
    Serial.println(";");
  }
  Serial.println("=====================");
  Serial.println("=====================");
  Serial.println("PrintCurrentServoValues exit");
}

void PrintAllServoValues()
{
  Serial.println("PrintAllServoValues entry");
  for (int i = 0; NUM_STATES > i; i++)
  {
    Serial.println("=====================");
    for (int j = 0; NUMBER_OF_SERVOS > j; j++)
    {
      Serial.print("states[");
      Serial.print(i);
      Serial.print("][");
      Serial.print(j);
      Serial.print("] = ");
      Serial.print(states[i][j]);
      Serial.println(";");
    }
  }
  Serial.println("=====================");
  Serial.println("=====================");
  Serial.println("PrintAllServoValues exit");
}

void MoveServoSmoothByValue(int use_servo, int by_value)
{
  int move_to = 0;
  int increment = 0;

  Serial.println("MoveServoSmoothByValue entry");

  move_to = ServoControl[use_servo].value_current + by_value;

  if ((ServoControl[use_servo].value_min <= move_to) && (move_to <= ServoControl[use_servo].value_max))
  {

    Serial.print("Moving servo ");
    Serial.print(use_servo);
    Serial.print(" by ");
    Serial.println(by_value);
    if  (move_to < ServoControl[use_servo].value_current)
    {
      increment = -1;
    }
    else
    {
      increment = 1;
    }
    while (ServoControl[use_servo].value_current != move_to)
    {
      ServoControl[use_servo].value_current += increment;
      ServoControl[use_servo].servo_id.write(ServoControl[use_servo].value_current);
      delay(STEP_DELAY);
    }
  }
  else
  {
    Serial.println("Error: value outside limits");
  }
  Serial.println("MoveServoSmoothByValue exit");
}


void MoveServoSmoothToPosition(int use_servo, int position_value)
{
  int move_to = 0;
  int increment = 0;
  Serial.println("MoveServoSmoothToPosition entry");
  move_to = position_value;

  if ((ServoControl[use_servo].value_min <= move_to) && (move_to <= ServoControl[use_servo].value_max))
  {
    Serial.print("Moving servo ");
    Serial.print(use_servo);
    Serial.print(" to position ");
    Serial.println(position_value);
    Serial.print(" from position ");
    Serial.println(ServoControl[use_servo].value_current);

    if  (move_to < ServoControl[use_servo].value_current)
    {
      increment = -1;
    }
    else
    {
      increment = 1;
    }
    while (ServoControl[use_servo].value_current != move_to)
    {
      ServoControl[use_servo].value_current += increment;
      ServoControl[use_servo].servo_id.write(ServoControl[use_servo].value_current);
      delay(STEP_DELAY);
    }
  }
  else
  {
    Serial.println("Error: value outside limits");
  }
  Serial.println("MoveServoSmoothToPosition exit");
}
void PrintHelp()
{
  Serial.println("=====================================");
  Serial.println("!!!!!!!!!!!!!!! Important:Set COM port to send Newline !!!!!!!!!!!!!!!");
  Serial.println("h - display this Help");
  Serial.println("i - move Arm to initial state");
  Serial.println("p - print current state and stored state values");
  Serial.println("z - store current state");
  Serial.println("a - auto-sequence start");
  Serial.println("f - Move Servo 0 by <value>; value can be positive or negative");
  Serial.println("l - Move Servo 1 by <value>; value can be positive or negative");
  Serial.println("t - Move Servo 2 by <value>; value can be positive or negative");
  Serial.println("j - Move Servo 3 by <value>; value can be positive or negative");
  Serial.println("=====================================");
}

