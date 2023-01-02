#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"


// Use pins 2 and 3 to communicate with DFPlayer Mini
static const uint8_t PIN_MP3_TX = 10;  // Connects to module's RX
static const uint8_t PIN_MP3_RX = 11;  // Connects to module's TX
SoftwareSerial softwareSerial(PIN_MP3_RX, PIN_MP3_TX);

#define shuffle 9  // IR sensor connected to pin 9
#define plaing 12  // Check if player is bussy
int oldSong[5] = { 0, 0, 0, 0, 0 };
int const maxSong = 22;  // quantity of song
int const maxLight = 5;  // quantity light version

//For led light
#define ledLight 8      //led1
#define ledLight 7      //led2
long interval1 = 1000;  // interval at which to blink (milliseconds)
long interval2 = 500;
long previousMillis = 0;  // will store last time LED was updated
int ledState1 = LOW;      // ledState used to set the LED
int ledState2 = LOW;      // ledState used to set the LED
int ledRand = 1;
int ledOldRand = 0;



// Create the Player object
DFRobotDFPlayerMini player;

int random(int min, int max)  //range : [min, max]
{

  static bool first = true;
  return min + rand() % ((max + 1) - min);
}

void led() {

  unsigned long currentMillis = millis();

  switch (ledRand) {
    case 1:
      digitalWrite(8, HIGH);
      digitalWrite(7, HIGH);
      break;
    case 2:

      if (currentMillis - previousMillis > interval1) {
        // save the last time you blinked the LED
        previousMillis = currentMillis;

        // if the LED is off turn it on and vice-versa:
        if (ledState1 == LOW) {
          ledState1 = HIGH;
          ledState2 = LOW;
        } else {
          ledState1 = LOW;
          ledState2 = HIGH;
        }

        // set the LED with the ledState of the variable:
        digitalWrite(8, ledState1);
        digitalWrite(7, ledState2);
      }

      break;
    case 3:

      if (currentMillis - previousMillis > interval2) {
        // save the last time you blinked the LED
        previousMillis = currentMillis;

        // if the LED is off turn it on and vice-versa:
        if (ledState1 == LOW) {
          ledState1 = HIGH;
          ledState2 = LOW;
        } else {
          ledState1 = LOW;
          ledState2 = HIGH;
        }

        // set the LED with the ledState of the variable:
        digitalWrite(8, ledState1);
        digitalWrite(7, ledState2);
      }

      break;
    case 4:

      if (currentMillis - previousMillis > interval1) {
        // save the last time you blinked the LED
        previousMillis = currentMillis;

        // if the LED is off turn it on and vice-versa:
        if (ledState1 == LOW) {
          ledState1 = HIGH;
          ledState2 = HIGH;
          ;
        } else {
          ledState1 = LOW;
          ledState2 = LOW;
        }

        // set the LED with the ledState of the variable:
        digitalWrite(8, ledState1);
        digitalWrite(7, ledState2);
      }

      break;
    case 5:

      if (currentMillis - previousMillis > interval2) {
        // save the last time you blinked the LED
        previousMillis = currentMillis;

        // if the LED is off turn it on and vice-versa:
        if (ledState1 == LOW) {
          ledState1 = HIGH;
          ledState2 = HIGH;
          ;
        } else {
          ledState1 = LOW;
          ledState2 = LOW;
        }

        // set the LED with the ledState of the variable:
        digitalWrite(8, ledState1);
        digitalWrite(7, ledState2);
      }

      break;
  }
}
void songSelector() {

  int min = 1;
  int maxS = maxSong;
  int maxL = maxLight;

  int newSong = random(min, maxS);

  if (oldSong[0] != newSong && oldSong[1] != newSong && oldSong[2] != newSong && oldSong[3] != newSong && oldSong[4] != newSong) {
    // Set volume to maximum (0 to 30).
    player.volume(20);
    // Play the first MP3 file on the SD card
    player.play(newSong);

    for (int i = 4; i > 0; i--) {  //don't duplicate last 4 songs
      oldSong[i] = oldSong[i - 1];
    }
    oldSong[0] = newSong;

  } else {
    songSelector();
  }
  Serial.println(newSong);
  bool l = true;
  do {
    ledRand = random(min, maxL);
    if (ledRand != ledOldRand)
      l = false;
  } while (l);

  ledOldRand = ledRand;
}

void setup() {

  pinMode(9, INPUT);
  pinMode(12, INPUT);
  pinMode(8, OUTPUT);
  pinMode(7, OUTPUT);
  digitalWrite(8, HIGH);
  digitalWrite(7, HIGH);

  //CLKPR = 0x80;
  //CLKPR = 0x01;

  // Init USB serial port for debugging
  Serial.begin(9600);
  // Init serial port for DFPlayer Mini
  softwareSerial.begin(9600);

  // Start communication with DFPlayer Mini
  if (player.begin(softwareSerial)) {
    Serial.println("OK");

  } else {
    Serial.println("Connecting to DFPlayer Mini failed!");
  }
}

void loop() {

  if (digitalRead(shuffle) == 0 || digitalRead(plaing) == 1) {  //check if change song

    songSelector();
    delay(500);
  }
  led();
}