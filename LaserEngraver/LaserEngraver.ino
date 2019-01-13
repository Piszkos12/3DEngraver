/*
 *  3D laser engraver with
 *    1 × ESP8266 Wemos D1 Mini Pro hardware version 1.1
 *    2 × 28BYJ-48 step motor
 *    2 × ULN2003 driver board  *    
 *  
 *  Units: 
 *          Laser driver: Beatrix
 *  X-Axis stepper motor: Alice
 *  Y-Axis stepper motor: Bob
 *  
 *  Beatrix
 *  [PowerOn] -> D0 : GPIO16
 *  
 *  Alice
 *        [0] -> D5 : GPIO14
 *        [1] -> D6 : GPIO12
 *        [2] -> D7 : GPIO13
 *        [3] -> D8 : GPIO15
 *  
 *  Bob
 *        [0] -> D1 : GPIO05 (i2c unfortunately missed)
 *        [1] -> D2 : GPIO04 (i2c unfortunately missed)
 *        [2] -> D3 : GPIO00
 *        [3] -> D4 : GPIO02
 */

// Beatrix
#define Beatrix 16

// Alice
#define Alice1 14
#define Alice2 12
#define Alice3 13
#define Alice4 15

// Bob
#define Bob1 05
#define Bob2 04
#define Bob3 00
#define Bob4 02

const int NBSTEPS = 4095; // 360°
int AliceStep = 0;
int BobStep = 0;

int outArrayHlt[4] = {LOW, LOW, LOW, LOW};

int stepsMatrix[8][4] = {
  {LOW,  LOW,  LOW,  HIGH},
  {LOW,  LOW,  HIGH, HIGH},
  {LOW,  LOW,  HIGH, LOW},
  {LOW,  HIGH, HIGH, LOW},
  {LOW,  HIGH, LOW,  LOW},
  {HIGH, HIGH, LOW,  LOW},
  {HIGH, LOW,  LOW,  LOW},
  {HIGH, LOW,  LOW,  HIGH}
}; 
 
void setup() {
  Serial.begin(9600);
  Serial.println();
  Serial.println("Starting...");
  
  pinMode(Beatrix, OUTPUT);
  pinMode(Alice1, OUTPUT);
  pinMode(Alice2, OUTPUT);
  pinMode(Alice3, OUTPUT);
  pinMode(Alice4, OUTPUT);
  pinMode(Bob1, OUTPUT);
  pinMode(Bob2, OUTPUT);
  pinMode(Bob3, OUTPUT);
  pinMode(Bob4, OUTPUT);

  writeAlice(outArrayHlt);
  writeBob(outArrayHlt);
}

void writeAlice(int outArray[4]) {
  digitalWrite(Alice1, outArray[0]);
  digitalWrite(Alice2, outArray[1]);
  digitalWrite(Alice3, outArray[2]);
  digitalWrite(Alice4, outArray[3]);
}

void writeBob(int outArray[4]) {
  digitalWrite(Bob1, outArray[0]);
  digitalWrite(Bob2, outArray[1]);
  digitalWrite(Bob3, outArray[2]);
  digitalWrite(Bob4, outArray[3]);
}

void stepForwardAlice() {
  AliceStep++;
  if (AliceStep > 7) {
    AliceStep = 0;
  }
  writeAlice(stepsMatrix[AliceStep]);
}

void stepBackwardAlice() {
  AliceStep--;
  if (AliceStep < 0) {
    AliceStep = 7;
  }
  writeAlice(stepsMatrix[AliceStep]);
}

void stepForwardBob() {
  BobStep++;
  if (BobStep > 7) {
    BobStep = 0;
  }
  writeBob(stepsMatrix[BobStep]);
}

void stepBackwardBob() {
  BobStep--;
  if (BobStep < 0) {
    BobStep = 7;
  }
  writeBob(stepsMatrix[BobStep]);
}

void loop() {
}
