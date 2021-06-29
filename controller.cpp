#include <Arduino.h>
#include <math.h>

// this code was written to control KNF NF-12 pumps with a Teensy 3.5 microcontroller

// WHITE WIRES, SPEED CONTROL
#define PIN_1P_PWM 2
#define PIN_1N_PWM 3
// #define PIN_2P_PWM 4
// #define PIN_2N_PWM 5
// #define PIN_3P_PWM 6
// #define PIN_3N_PWM 7
// #define PIN_4P_PWM 8
// #define PIN_4N_PWM 9
// #define PIN_5P_PWM 10
// #define PIN_5N_PWM 29

// GREEN WIRES, PUMP SPEED OUTPUT FREQUENCY
#define PIN_1P_TACHO 23
#define PIN_1N_TACHO 22  
// #define PIN_2P_TACHO 21
// #define PIN_2N_TACHO 20
// #define PIN_3P_TACHO 19
// #define PIN_3N_TACHO 18
// #define PIN_4P_TACHO 17
// #define PIN_4N_TACHO 16
// #define PIN_5P_TACHO 15
// #define PIN_5N_TACHO 36

// INPUT PUMP SPEED/DUTY CYCLE, 0-255 is 0-100%, PUMPS MINIMUM SPEED STARTS FROM ~3.8 - 7.6 %
int speed1P = 0;
int speed1N = 0;
// int speed2P = 0;
// int speed2N = 0;
// int speed3P = 0;
// int speed3N = 0;
// int speed4N = 0;
// int speed4P = 0;
// int speed5N = 0;
// int speed5P = 0;

// OUTPUT PUMP SPEED

unsigned long rpm1P = 0;
unsigned long rpm1N = 0;
// unsigned long rpm2P = 0;
// unsigned long rpm2N = 0;
// unsigned long rpm3P = 0;
// unsigned long rpm3N = 0;
// unsigned long rpm4N = 0;
// unsigned long rpm4P = 0;
// unsigned long rpm5N = 0;
// unsigned long rpm5P = 0;

//PUMP COUNTERS

unsigned long count1P = 0;
unsigned long count1N = 0;
// unsigned long count2P = 0;
// unsigned long count2N = 0;
// unsigned long count3P = 0;
// unsigned long count3N = 0;
// unsigned long count4N = 0;
// unsigned long count4P = 0;
// unsigned long count5N = 0;
// unsigned long count5P = 0;


const byte buffSize = 40;
char inputBuffer[buffSize];
const char startMarker = '<';
const char endMarker = '>';
byte bytesRecvd = 0;
boolean readInProgress = false;
boolean newDataFromPC = false;
char messageFromPC[buffSize] = {0};
char cmd;
String val;

const int updateInterval = 1000; // serial update time period (ms)
elapsedMillis updateTimer = 0;

bool ledState = LOW;

void parseData()
{
  cmd = strtok(inputBuffer, ",")[0];
  val = strtok(NULL, ",");
}

void recieveFromPC()
{

  // receive data from PC and save it into inputBuffer

  if (Serial.available() > 0)
  {

    char x = Serial.read();

    // the order of these IF clauses is significant

    if (x == endMarker)
    {
      readInProgress = false;
      newDataFromPC = true;
      inputBuffer[bytesRecvd] = 0;
      parseData();
    }

    if (readInProgress)
    {
      inputBuffer[bytesRecvd] = x;
      bytesRecvd++;
      if (bytesRecvd == buffSize)
      {
        bytesRecvd = buffSize - 1;
      }
    }

    if (x == startMarker)
    {
      bytesRecvd = 0;
      readInProgress = true;
    }
  }
}

void ISR_1P()
{
  count1P++;
}
void ISR_1N()
{
  count1N++;
}
// void ISR_2P()
// {
//   count2P++;
// }
// void ISR_2N()
// {
//   count2N++;
// }
// void ISR_3P()
// {
//   count3P++;
// }
// void ISR_3N()
// {
//   count3N++;
// }
// void ISR_4P()
// {
//   count4P++;
// }
// void ISR_4N()
// {
//   count4N++;
// }
// void ISR_5P()
// {
//   count5P++;
// }
// void ISR_5N()
// {
//   count5N++;
// }

void setup()
{
  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, ledState);

  pinMode(PIN_1P_PWM, OUTPUT);
  pinMode(PIN_1N_PWM, OUTPUT);
  // pinMode(PIN_2P_PWM, OUTPUT);
  // pinMode(PIN_2N_PWM, OUTPUT);
  // pinMode(PIN_3P_PWM, OUTPUT);
  // pinMode(PIN_3N_PWM, OUTPUT);
  // pinMode(PIN_4P_PWM, OUTPUT);
  // pinMode(PIN_4N_PWM, OUTPUT);
  // pinMode(PIN_5P_PWM, OUTPUT);
  // pinMode(PIN_5N_PWM, OUTPUT);

  analogWriteFrequency(PIN_1P_PWM, 10000);
  analogWriteFrequency(PIN_1N_PWM, 10000);
  // analogWriteFrequency(PIN_2P_PWM, 10000);
  // analogWriteFrequency(PIN_2N_PWM, 10000);
  // analogWriteFrequency(PIN_3P_PWM, 10000);
  // analogWriteFrequency(PIN_3N_PWM, 10000);
  // analogWriteFrequency(PIN_4P_PWM, 10000);
  // analogWriteFrequency(PIN_4N_PWM, 10000);
  // analogWriteFrequency(PIN_5P_PWM, 10000);
  // analogWriteFrequency(PIN_5N_PWM, 10000);

  analogWrite(PIN_1P_PWM, 0);
  analogWrite(PIN_1N_PWM, 0);
  // analogWrite(PIN_2P_PWM, 0);
  // analogWrite(PIN_2N_PWM, 0);
  // analogWrite(PIN_3P_PWM, 0);
  // analogWrite(PIN_3N_PWM, 0);
  // analogWrite(PIN_4P_PWM, 0);
  // analogWrite(PIN_4N_PWM, 0);
  // analogWrite(PIN_5P_PWM, 0);
  // analogWrite(PIN_5N_PWM, 0);

  attachInterrupt(digitalPinToInterrupt(PIN_1P_TACHO), ISR_1P, FALLING);
  attachInterrupt(digitalPinToInterrupt(PIN_1N_TACHO), ISR_1N, FALLING);
  // attachInterrupt(digitalPinToInterrupt(PIN_2P_TACHO), ISR_2P, FALLING);
  // attachInterrupt(digitalPinToInterrupt(PIN_2N_TACHO), ISR_2N, FALLING);
  // attachInterrupt(digitalPinToInterrupt(PIN_3P_TACHO), ISR_3P, FALLING);
  // attachInterrupt(digitalPinToInterrupt(PIN_3N_TACHO), ISR_3N, FALLING);
  // attachInterrupt(digitalPinToInterrupt(PIN_4P_TACHO), ISR_4P, FALLING);
  // attachInterrupt(digitalPinToInterrupt(PIN_4N_TACHO), ISR_4N, FALLING);
  // attachInterrupt(digitalPinToInterrupt(PIN_5P_TACHO), ISR_5P, FALLING);
  // attachInterrupt(digitalPinToInterrupt(PIN_5N_TACHO), ISR_5N, FALLING);
}

void controlPumps()
{
  if (newDataFromPC)
  {
    switch (cmd)
    {
    case 'a':
      flowRateA = val.toInt();
      analogWrite(PIN_1P_PWM, flowRateA);
      break;
    case 'b':
      flowRateB = val.toInt();
      analogWrite(PIN_1N_PWM, flowRateB);
      break;
    // case 'c':
    //   analogWrite(PIN_2P_PWM, val.toInt());
    //   break;
    // case 'd':
    //   analogWrite(PIN_2N_PWM, val.toInt());
    //   break;
    // case 'e':
    //   analogWrite(PIN_3P_PWM, val.toInt());
    //   break;
    // case 'f':
    //   analogWrite(PIN_3N_PWM, val.toInt());
    //   break;
    // case 'g':
    //   analogWrite(PIN_4P_PWM, val.toInt());
    //   break;
    // case 'h':
    //   analogWrite(PIN_4N_PWM, val.toInt());
    //   break;
    // case 'i':
    //   analogWrite(PIN_5P_PWM, val.toInt());
    //   break;
    // case 'j':
    //   analogWrite(PIN_5N_PWM, val.toInt());
    //   break;
    }
  }
}

void replyToPC()
{
  if (updateTimer >= updateInterval)
  {

    detachInterrupt(digitalPinToInterrupt(PIN_1P_TACHO));
    detachInterrupt(digitalPinToInterrupt(PIN_1N_TACHO));
    // detachInterrupt(digitalPinToInterrupt(PIN_2P_TACHO));
    // detachInterrupt(digitalPinToInterrupt(PIN_2N_TACHO));
    // detachInterrupt(digitalPinToInterrupt(PIN_3P_TACHO));
    // detachInterrupt(digitalPinToInterrupt(PIN_3N_TACHO));
    // detachInterrupt(digitalPinToInterrupt(PIN_4P_TACHO));
    // detachInterrupt(digitalPinToInterrupt(PIN_4N_TACHO));
    // detachInterrupt(digitalPinToInterrupt(PIN_5P_TACHO));
    // detachInterrupt(digitalPinToInterrupt(PIN_5N_TACHO));

    rpm1P = count1P * 10;
    rpm1N = count1N * 10;
    // rpm2P = count2P * 10;
    // rpm2N = count2N * 10;
    // rpm3P = count3P * 10;
    // rpm3N = count3N * 10;
    // rpm4N = count4N * 10;
    // rpm4P = count4P * 10;
    // rpm5N = count5N * 10;
    // rpm5P = count5P * 10;

    Serial.print(flowRateA);
    Serial.print(',');
    Serial.print(flowRateB);
    Serial.print(',');
    Serial.print(rpm1P);
    Serial.print(',');
    Serial.println(rpm1N);


    updateTimer = 0;

    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState);

    count1P = 0;
    count1N = 0;
    // count2P = 0;
    // count2N = 0;
    // count3P = 0;
    // count3N = 0;
    // count4N = 0;
    // count4P = 0;
    // count5N = 0;
    // count5P = 0;

    attachInterrupt(digitalPinToInterrupt(PIN_1P_TACHO), ISR_1P, FALLING);
    attachInterrupt(digitalPinToInterrupt(PIN_1N_TACHO), ISR_1N, FALLING);
    // attachInterrupt(digitalPinToInterrupt(PIN_2P_TACHO), ISR_2P, FALLING);
    // attachInterrupt(digitalPinToInterrupt(PIN_2N_TACHO), ISR_2N, FALLING);
    // attachInterrupt(digitalPinToInterrupt(PIN_3P_TACHO), ISR_3P, FALLING);
    // attachInterrupt(digitalPinToInterrupt(PIN_3N_TACHO), ISR_3N, FALLING);
    // attachInterrupt(digitalPinToInterrupt(PIN_4P_TACHO), ISR_4P, FALLING);
    // attachInterrupt(digitalPinToInterrupt(PIN_4N_TACHO), ISR_4N, FALLING);
    // attachInterrupt(digitalPinToInterrupt(PIN_5P_TACHO), ISR_5P, FALLING);
    // attachInterrupt(digitalPinToInterrupt(PIN_5N_TACHO), ISR_5N, FALLING);
  }
}

void loop()
{
  recieveFromPC();
  controlPumps();
  replyToPC();
}