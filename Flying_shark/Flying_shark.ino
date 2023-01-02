//#include <Servo.h>
#define Drunk 600  // Define min value that we consider drunk
#define MQ3pin 4   //Analog input for alco   sensor
#include <NewPing.h>

#define RIGHT A2          // Right IR sensor connected to analog pin A2 of Arduino Uno:
#define LEFT A3           // Left IR sensor connected to analog pin A3 of Arduino Uno:
#define TRIGGER_PIN A1    // Trigger pin connected to analog pin A1 of Arduino Uno:
#define ECHO_PIN A0       // Echo pin connected to analog pin A0 of Arduino Uno:
#define MAX_DISTANCE 200  // Maximum ping distance:

// Servo myservo;
float sensorValue;
unsigned int distance = 0;     //Variable to store ultrasonic sensor distance:
unsigned int Right_Value = 0;  //Variable to store Right IR sensor value:
unsigned int Left_Value = 0;   //Variable to store Left IR sensor value:

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);  //NewPing setup of pins and maximum distance:

void setup() {
  Serial.begin(9600);
  pinMode(6, OUTPUT);  //Engines speed

  pinMode(7, OUTPUT);  // Rotation direction of back engine
  pinMode(8, OUTPUT);

  pinMode(9, OUTPUT);  // Rotation direction of bottom engine
  pinMode(10, OUTPUT);

  pinMode(13, OUTPUT);  //Red led output (eye)


  pinMode(RIGHT, INPUT);  //set analog pin RIGHT as an input:
  pinMode(LEFT, INPUT);   //set analog pin RIGHT as an input:
}

char cmd[100];
byte cmdIndex;
boolean mod = false;   //Variable on/off alko mode
boolean foll = false;  //Variable on/off hunt mode

void exeCmd() {

  if (cmd[0] == 'f') {
    // unsigned int val = atof(cmd + 2);
    // myservo.write(val);
    forward();
  } else if (cmd[0] == 'l') {
    left();
  } else if (cmd[0] == 'r') {
    right();
  } else if (cmd[0] == 'u') {
    up();
  } else if (cmd[0] == 'd') {
    down();
  } else if (cmd[0] == 'm') {
    //mod = (mod == true) ? false : true;
    if (mod == true) {
      mod = false;
      Serial.println("ledm 0\n");
    } else {
      mod = true;
      Serial.println("ledm 1\n");
    }
    foll = false;
    Serial.println("ledh 0\n");
  } else if (cmd[0] == 'h') {
    // foll = (foll == true) ? false : true;
    if (foll == true) {
      foll = false;
      Serial.println("ledh 0\n");
    } else {
      foll = true;
      Serial.println("ledh 1\n");
    }
    mod = false;
    Serial.println("ledm 0\n");
  }
}

void forward() {  //Move forward
  digitalWrite(13, HIGH);
  analogWrite(6, 40);
  digitalWrite(7, LOW);
  digitalWrite(8, HIGH);
  delay(500);

  digitalWrite(7, HIGH);
  digitalWrite(8, LOW);
  delay(500);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);
  digitalWrite(13, LOW);
  delay(250);
}

void left() {  //Turn left
  digitalWrite(13, HIGH);
  analogWrite(6, 40);
  digitalWrite(7, LOW);
  digitalWrite(8, HIGH);
  delay(500);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);
  digitalWrite(13, LOW);
  digitalWrite(13, LOW);
  delay(250);
}

void right() {  //Turn right
  digitalWrite(13, HIGH);
  analogWrite(6, 40);
  digitalWrite(7, HIGH);
  digitalWrite(8, LOW);
  delay(500);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);
  digitalWrite(13, LOW);
  delay(250);
}
void up() {  //Trim up
  analogWrite(6, 40);
  digitalWrite(9, LOW);
  digitalWrite(10, HIGH);
  delay(250);
  digitalWrite(9, LOW);
  digitalWrite(10, LOW);
}

void down() {  //Trim down
  analogWrite(6, 40);
  digitalWrite(9, HIGH);
  digitalWrite(10, LOW);
  delay(250);
  digitalWrite(9, LOW);
  digitalWrite(10, LOW);
}

void hunt() {

  delay(50);                   //wait 50ms between pings:
  distance = sonar.ping_cm();  //send ping, get distance in cm and store it in 'distance' variable:

  Right_Value = digitalRead(RIGHT);  // Right IR value
  Left_Value = digitalRead(LEFT);    // Left IR value

  if ((distance > 1) && (distance < 20)) {  //Chcek if Ultrasonic sensor found object bettwene 1-20 cm

    forward();

  } else if ((Right_Value == 0) && (Left_Value == 1)) {  //Chcek if shark have to turn left

    left();

  } else if ((Right_Value == 1) && (Left_Value == 0)) {  //Chcek if shark have to turn right

    right();

  } else if (distance > 20) {  //Stop flying, object is to far
  }
}

void alkoTest() {                    //Check Alcohol lvl
  sensorValue = analogRead(MQ3pin);  // read analog input pin 0

  // Determine the status
  if (sensorValue > Drunk) {
    digitalWrite(13, HIGH);
    delay(100);
    digitalWrite(13, LOW);
    delay(100);
    digitalWrite(13, HIGH);
    delay(100);
    digitalWrite(13, LOW);
    delay(100);
    for (int i = 0; i <= 2; i++) {

      forward();
    }
    mod = false;
  }
}

void loop() {
  if (Serial.available() > 0) {
    char c = (char)Serial.read();
    if (c == '\n') {
      cmd[cmdIndex] = 0;
      exeCmd();
      cmdIndex = 0;
    } else {
      cmd[cmdIndex] = c;
      if (cmdIndex < 99) cmdIndex++;
    }
  } else if (mod == true) {
    alkoTest();
  } else if (foll == true) {
    hunt();
  }
}