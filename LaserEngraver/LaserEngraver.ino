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

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

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

ESP8266WebServer server(80);

const char* ssid     = "Anyad Picsaja";
const char* password = "muslinca";

const int NBSTEPS = 4095; // 360°
int period = 10; // wait milliseconds in one step = 100Hz by default

int pageTop = 0;
int pageBottom = 0;
int pageLeft = 0;
int pageRight = 0;
int thisTop = 0;
int thisBottom = 0;
int thisLeft = 0;
int thisRight = 0;

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

unsigned long time_now = 0;

void setup() {
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

  Serial.begin(9600);
  while (!Serial) {}
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.print(ssid);
  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print('.');
  }
  Serial.println();
  Serial.println("Connection established!");  
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());    

  server.on("/BeatrixOn", [](){
    server.send(200, "text/plain", "BeatrixOn");
    BeatrixOn();
  });
  server.on("/BeatrixOff", [](){
    server.send(200, "text/plain", "BeatrixOff");
    BeatrixOff();
  });
  server.on("/fastForwardAlice", [](){
    server.send(200, "text/plain", "fastForwardAlice");
    fastForwardAlice(server.arg("steps").toInt());
    writeAlice(outArrayHlt);
  });
  server.on("/fastBackwardAlice", [](){
    server.send(200, "text/plain", "fastBackwardAlice");
    fastBackwardAlice(server.arg("steps").toInt());
    writeAlice(outArrayHlt);
  });
  server.on("/fastForwardBob", [](){
    server.send(200, "text/plain", "fastForwardBob");
    fastForwardBob(server.arg("steps").toInt());
    writeBob(outArrayHlt);
  });
  server.on("/fastBackwardBob", [](){
    server.send(200, "text/plain", "fastBackwardBob");
    fastBackwardBob(server.arg("steps").toInt());
    writeBob(outArrayHlt);
  });
  server.on("/setPeriod", [](){
    server.send(200, "text/plain", "setPeriod");
    period=server.arg("period").toInt();
  });
  server.on("/do", [](){
    String job=server.arg("job");
    int joblen=job.length()-1;
    for (int i=0; i <= joblen; i++) {
      time_now = millis();
      // "WASD" directions
      switch (job.charAt(i)) {
        case 'B': // Power on Beatrix
          BeatrixOn();
          break;
        case 'b': // Switch off Beatrix
          BeatrixOff();
          break;
        case 'w': // UP
          stepBackwardBob();
          delay(period);
          break;
        case 'x': // DOWN
          stepForwardBob();
          delay(period);
          break;
        case 'a': // LEFT
          stepBackwardAlice();
          delay(period);
          break;
        case 'd': // RIGHT
          stepForwardAlice();
          delay(period);
          break;
        case 'e': // UPRIGHT
          stepBackwardBob();
          stepForwardAlice();
          delay(period);
          break;
        case 'c': // DOWNRIGHT
          stepForwardBob();
          stepForwardAlice();
          delay(period);
          break;
        case 'y': // DOWNLEFT
          stepBackwardAlice();
          stepForwardBob();
          delay(period);
          break;
        case 'q': // TOPRIGHT
          stepBackwardBob();
          stepBackwardAlice();
          delay(period);
          break;
        case 's': // wait
          delay(period);
          break;
      }
    }
    BeatrixOff();
    writeAlice(outArrayHlt);
    writeBob(outArrayHlt);
    server.send(200, "text/plain", "do");
  });

  server.begin();
}

void BeatrixOn() {
  digitalWrite(Beatrix, HIGH);
}

void BeatrixOff() {
  digitalWrite(Beatrix, LOW);
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

void fastForwardAlice(int steps) {
  for (int i=1; i <= steps; i++){
    stepForwardAlice();
    delay(1);
  }
}

void fastForwardBob(int steps) {
  for (int i=1; i <= steps; i++){
    stepForwardBob();
    delay(1);
  }
}

void fastBackwardAlice(int steps) {
  for (int i=1; i <= steps; i++){
    stepBackwardAlice();
    delay(1);
  }
}

void fastBackwardBob(int steps) {
  for (int i=1; i <= steps; i++){
    stepBackwardBob();
    delay(1);
  }
}

void loop() {
  server.handleClient();
/*
  time_now = millis();
  stepForwardAlice();
  stepForwardBob();
  while(millis() < time_now + period) {}
*/
}
