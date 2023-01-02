
#include <virtuabotixRTC.h>  //Library used
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

using namespace std;

// Define the number of devices we have in the chain and the hardware interface
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW  //GENERIC_HW
#define MAX_DEVICES 1
#define CLK_PIN 12
#define DATA_PIN 10
#define CS_PIN 11

// HARDWARE SPI
//MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
// SOFTWARE SPI
MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

uint8_t scrollSpeed = 100;  // default frame delay value
textEffect_t effectIn = PA_SCROLL_LEFT;
textPosition_t scrollAlign = PA_LEFT;
textEffect_t effectOut = PA_SPRITE;
uint16_t scrollPause = 500;  // in milliseconds

// Global message buffers shared by Serial and Scrolling functions
#define BUF_SIZE 75
char curMessage[BUF_SIZE] = { "" };
char newMessage[BUF_SIZE] = { "5H 25M LEFT TO 2023" };
bool newMessageAvailable = true;

//Variables for time calculation
int newYear = 2023;
int hH = 23;
int mM = 59;
int sS = 59;

// Global variables for Sprites
int numSprite = 0;
int oldNumSprite = 0;  //to not displeay the same Sprites two times in the row
int minSprite = 0;     //Sprite versions from 0 - 14 (1-15)
int maxSprite = 14;

// Sprite Definitions
const uint8_t F_PMAN1 = 6;
const uint8_t W_PMAN1 = 8;
const uint8_t PROGMEM pacman1[F_PMAN1 * W_PMAN1] =  // gobbling pacman animation
  {
    0x00,
    0x81,
    0xc3,
    0xe7,
    0xff,
    0x7e,
    0x7e,
    0x3c,
    0x00,
    0x42,
    0xe7,
    0xe7,
    0xff,
    0xff,
    0x7e,
    0x3c,
    0x24,
    0x66,
    0xe7,
    0xff,
    0xff,
    0xff,
    0x7e,
    0x3c,
    0x3c,
    0x7e,
    0xff,
    0xff,
    0xff,
    0xff,
    0x7e,
    0x3c,
    0x24,
    0x66,
    0xe7,
    0xff,
    0xff,
    0xff,
    0x7e,
    0x3c,
    0x00,
    0x42,
    0xe7,
    0xe7,
    0xff,
    0xff,
    0x7e,
    0x3c,
  };

const uint8_t F_PMAN2 = 6;
const uint8_t W_PMAN2 = 18;
const uint8_t PROGMEM pacman2[F_PMAN2 * W_PMAN2] =  // ghost pursued by a pacman
  {
    0x00,
    0x81,
    0xc3,
    0xe7,
    0xff,
    0x7e,
    0x7e,
    0x3c,
    0x00,
    0x00,
    0x00,
    0xfe,
    0x7b,
    0xf3,
    0x7f,
    0xfb,
    0x73,
    0xfe,
    0x00,
    0x42,
    0xe7,
    0xe7,
    0xff,
    0xff,
    0x7e,
    0x3c,
    0x00,
    0x00,
    0x00,
    0xfe,
    0x7b,
    0xf3,
    0x7f,
    0xfb,
    0x73,
    0xfe,
    0x24,
    0x66,
    0xe7,
    0xff,
    0xff,
    0xff,
    0x7e,
    0x3c,
    0x00,
    0x00,
    0x00,
    0xfe,
    0x7b,
    0xf3,
    0x7f,
    0xfb,
    0x73,
    0xfe,
    0x3c,
    0x7e,
    0xff,
    0xff,
    0xff,
    0xff,
    0x7e,
    0x3c,
    0x00,
    0x00,
    0x00,
    0xfe,
    0x7b,
    0xf3,
    0x7f,
    0xfb,
    0x73,
    0xfe,
    0x24,
    0x66,
    0xe7,
    0xff,
    0xff,
    0xff,
    0x7e,
    0x3c,
    0x00,
    0x00,
    0x00,
    0xfe,
    0x7b,
    0xf3,
    0x7f,
    0xfb,
    0x73,
    0xfe,
    0x00,
    0x42,
    0xe7,
    0xe7,
    0xff,
    0xff,
    0x7e,
    0x3c,
    0x00,
    0x00,
    0x00,
    0xfe,
    0x7b,
    0xf3,
    0x7f,
    0xfb,
    0x73,
    0xfe,
  };

const uint8_t F_WAVE = 14;
const uint8_t W_WAVE = 14;
const uint8_t PROGMEM wave[F_WAVE * W_WAVE] =  // triangular wave / worm
  {
    0x08,
    0x04,
    0x02,
    0x01,
    0x02,
    0x04,
    0x08,
    0x10,
    0x20,
    0x40,
    0x80,
    0x40,
    0x20,
    0x10,
    0x10,
    0x08,
    0x04,
    0x02,
    0x01,
    0x02,
    0x04,
    0x08,
    0x10,
    0x20,
    0x40,
    0x80,
    0x40,
    0x20,
    0x20,
    0x10,
    0x08,
    0x04,
    0x02,
    0x01,
    0x02,
    0x04,
    0x08,
    0x10,
    0x20,
    0x40,
    0x80,
    0x40,
    0x40,
    0x20,
    0x10,
    0x08,
    0x04,
    0x02,
    0x01,
    0x02,
    0x04,
    0x08,
    0x10,
    0x20,
    0x40,
    0x80,
    0x80,
    0x40,
    0x20,
    0x10,
    0x08,
    0x04,
    0x02,
    0x01,
    0x02,
    0x04,
    0x08,
    0x10,
    0x20,
    0x40,
    0x40,
    0x80,
    0x40,
    0x20,
    0x10,
    0x08,
    0x04,
    0x02,
    0x01,
    0x02,
    0x04,
    0x08,
    0x10,
    0x20,
    0x20,
    0x40,
    0x80,
    0x40,
    0x20,
    0x10,
    0x08,
    0x04,
    0x02,
    0x01,
    0x02,
    0x04,
    0x08,
    0x10,
    0x10,
    0x20,
    0x40,
    0x80,
    0x40,
    0x20,
    0x10,
    0x08,
    0x04,
    0x02,
    0x01,
    0x02,
    0x04,
    0x08,
    0x08,
    0x10,
    0x20,
    0x40,
    0x80,
    0x40,
    0x20,
    0x10,
    0x08,
    0x04,
    0x02,
    0x01,
    0x02,
    0x04,
    0x04,
    0x08,
    0x10,
    0x20,
    0x40,
    0x80,
    0x40,
    0x20,
    0x10,
    0x08,
    0x04,
    0x02,
    0x01,
    0x02,
    0x02,
    0x04,
    0x08,
    0x10,
    0x20,
    0x40,
    0x80,
    0x40,
    0x20,
    0x10,
    0x08,
    0x04,
    0x02,
    0x01,
    0x01,
    0x02,
    0x04,
    0x08,
    0x10,
    0x20,
    0x40,
    0x80,
    0x40,
    0x20,
    0x10,
    0x08,
    0x04,
    0x02,
    0x02,
    0x01,
    0x02,
    0x04,
    0x08,
    0x10,
    0x20,
    0x40,
    0x80,
    0x40,
    0x20,
    0x10,
    0x08,
    0x04,
    0x04,
    0x02,
    0x01,
    0x02,
    0x04,
    0x08,
    0x10,
    0x20,
    0x40,
    0x80,
    0x40,
    0x20,
    0x10,
    0x08,
  };

const uint8_t F_ROLL1 = 4;
const uint8_t W_ROLL1 = 8;
const uint8_t PROGMEM roll1[F_ROLL1 * W_ROLL1] =  // rolling square
  {
    0xff,
    0x8f,
    0x8f,
    0x8f,
    0x81,
    0x81,
    0x81,
    0xff,
    0xff,
    0xf1,
    0xf1,
    0xf1,
    0x81,
    0x81,
    0x81,
    0xff,
    0xff,
    0x81,
    0x81,
    0x81,
    0xf1,
    0xf1,
    0xf1,
    0xff,
    0xff,
    0x81,
    0x81,
    0x81,
    0x8f,
    0x8f,
    0x8f,
    0xff,
  };

const uint8_t F_ROLL2 = 4;
const uint8_t W_ROLL2 = 8;
const uint8_t PROGMEM roll2[F_ROLL2 * W_ROLL2] =  // rolling octagon
  {
    0x3c,
    0x4e,
    0x8f,
    0x8f,
    0x81,
    0x81,
    0x42,
    0x3c,
    0x3c,
    0x72,
    0xf1,
    0xf1,
    0x81,
    0x81,
    0x42,
    0x3c,
    0x3c,
    0x42,
    0x81,
    0x81,
    0xf1,
    0xf1,
    0x72,
    0x3c,
    0x3c,
    0x42,
    0x81,
    0x81,
    0x8f,
    0x8f,
    0x4e,
    0x3c,
  };

const uint8_t F_LINES = 3;
const uint8_t W_LINES = 8;
const uint8_t PROGMEM lines[F_LINES * W_LINES] =  // spaced lines
  {
    0xff,
    0xff,
    0xff,
    0x00,
    0x00,
    0xff,
    0x00,
    0x00,
    0xff,
    0xff,
    0x00,
    0xff,
    0x00,
    0x00,
    0xff,
    0x00,
    0xff,
    0xff,
    0x00,
    0x00,
    0xff,
    0x00,
    0x00,
    0xff,
  };

const uint8_t F_ARROW1 = 3;
const uint8_t W_ARROW1 = 10;
const uint8_t PROGMEM arrow1[F_ARROW1 * W_ARROW1] =  // arrow fading to center
  {
    0x18,
    0x3c,
    0x7e,
    0xff,
    0x7e,
    0x00,
    0x00,
    0x3c,
    0x00,
    0x00,
    0x18,
    0x3c,
    0x7e,
    0xff,
    0x00,
    0x7e,
    0x00,
    0x00,
    0x18,
    0x00,
    0x18,
    0x3c,
    0x7e,
    0xff,
    0x00,
    0x00,
    0x3c,
    0x00,
    0x00,
    0x18,
  };

const uint8_t F_ARROW2 = 3;
const uint8_t W_ARROW2 = 9;
const uint8_t PROGMEM arrow2[F_ARROW2 * W_ARROW2] =  // arrow fading to outside
  {
    0x18,
    0x3c,
    0x7e,
    0xe7,
    0x00,
    0x00,
    0xc3,
    0x00,
    0x00,
    0x18,
    0x3c,
    0x7e,
    0xe7,
    0xe7,
    0x00,
    0x00,
    0x81,
    0x00,
    0x18,
    0x3c,
    0x7e,
    0xe7,
    0x00,
    0xc3,
    0x00,
    0x00,
    0x81,
  };

const uint8_t F_SAILBOAT = 1;
const uint8_t W_SAILBOAT = 11;
const uint8_t PROGMEM sailboat[F_SAILBOAT * W_SAILBOAT] =  // sail boat
  {
    0x10,
    0x30,
    0x58,
    0x94,
    0x92,
    0x9f,
    0x92,
    0x94,
    0x98,
    0x50,
    0x30,
  };

const uint8_t F_STEAMBOAT = 2;
const uint8_t W_STEAMBOAT = 11;
const uint8_t PROGMEM steamboat[F_STEAMBOAT * W_STEAMBOAT] =  // steam boat
  {
    0x10,
    0x30,
    0x50,
    0x9c,
    0x9e,
    0x90,
    0x91,
    0x9c,
    0x9d,
    0x90,
    0x71,
    0x10,
    0x30,
    0x50,
    0x9c,
    0x9c,
    0x91,
    0x90,
    0x9d,
    0x9e,
    0x91,
    0x70,
  };

const uint8_t F_HEART = 5;
const uint8_t W_HEART = 9;
const uint8_t PROGMEM heart[F_HEART * W_HEART] =  // beating heart
  {
    0x0e,
    0x11,
    0x21,
    0x42,
    0x84,
    0x42,
    0x21,
    0x11,
    0x0e,
    0x0e,
    0x1f,
    0x33,
    0x66,
    0xcc,
    0x66,
    0x33,
    0x1f,
    0x0e,
    0x0e,
    0x1f,
    0x3f,
    0x7e,
    0xfc,
    0x7e,
    0x3f,
    0x1f,
    0x0e,
    0x0e,
    0x1f,
    0x33,
    0x66,
    0xcc,
    0x66,
    0x33,
    0x1f,
    0x0e,
    0x0e,
    0x11,
    0x21,
    0x42,
    0x84,
    0x42,
    0x21,
    0x11,
    0x0e,
  };

const uint8_t F_INVADER = 2;
const uint8_t W_INVADER = 10;
const uint8_t PROGMEM invader[F_INVADER * W_INVADER] =  // space invader
  {
    0x0e,
    0x98,
    0x7d,
    0x36,
    0x3c,
    0x3c,
    0x36,
    0x7d,
    0x98,
    0x0e,
    0x70,
    0x18,
    0x7d,
    0xb6,
    0x3c,
    0x3c,
    0xb6,
    0x7d,
    0x18,
    0x70,
  };

const uint8_t F_ROCKET = 2;
const uint8_t W_ROCKET = 11;
const uint8_t PROGMEM rocket[F_ROCKET * W_ROCKET] =  // rocket
  {
    0x18,
    0x24,
    0x42,
    0x81,
    0x99,
    0x18,
    0x99,
    0x18,
    0xa5,
    0x5a,
    0x81,
    0x18,
    0x24,
    0x42,
    0x81,
    0x18,
    0x99,
    0x18,
    0x99,
    0x24,
    0x42,
    0x99,
  };

const uint8_t F_FBALL = 2;
const uint8_t W_FBALL = 11;
const uint8_t PROGMEM fireball[F_FBALL * W_FBALL] =  // fireball
  {
    0x7e,
    0xab,
    0x54,
    0x28,
    0x52,
    0x24,
    0x40,
    0x18,
    0x04,
    0x10,
    0x08,
    0x7e,
    0xd5,
    0x2a,
    0x14,
    0x24,
    0x0a,
    0x30,
    0x04,
    0x28,
    0x08,
    0x10,
  };

const uint8_t F_CHEVRON = 1;
const uint8_t W_CHEVRON = 9;
const uint8_t PROGMEM chevron[F_CHEVRON * W_CHEVRON] =  // chevron
  {
    0x18,
    0x3c,
    0x66,
    0xc3,
    0x99,
    0x3c,
    0x66,
    0xc3,
    0x81,
  };

const uint8_t F_WALKER = 5;
const uint8_t W_WALKER = 7;
const uint8_t PROGMEM walker[F_WALKER * W_WALKER] =  // walking man
  {
    0x00,
    0x48,
    0x77,
    0x1f,
    0x1c,
    0x94,
    0x68,
    0x00,
    0x90,
    0xee,
    0x3e,
    0x38,
    0x28,
    0xd0,
    0x00,
    0x00,
    0xae,
    0xfe,
    0x38,
    0x28,
    0x40,
    0x00,
    0x00,
    0x2e,
    0xbe,
    0xf8,
    0x00,
    0x00,
    0x00,
    0x10,
    0x6e,
    0x3e,
    0xb8,
    0xe8,
    0x00,
  };

struct
{
  const uint8_t* data;
  uint8_t width;
  uint8_t frames;
} sprite[] = {
  { walker, W_WALKER, F_WALKER },
  { invader, W_INVADER, F_INVADER },
  { chevron, W_CHEVRON, F_CHEVRON },
  { heart, W_HEART, F_HEART },
  { arrow1, W_ARROW1, F_ARROW1 },
  { steamboat, W_STEAMBOAT, F_STEAMBOAT },
  { rocket, W_ROCKET, F_ROCKET },
  { roll2, W_ROLL2, F_ROLL2 },
  { pacman2, W_PMAN2, F_PMAN2 },
  { lines, W_LINES, F_LINES },
  { roll1, W_ROLL1, F_ROLL1 },
  { sailboat, W_SAILBOAT, F_SAILBOAT },
  { arrow2, W_ARROW2, F_ARROW2 },
  //{ wave, W_WAVE, F_WAVE },
  { pacman1, W_PMAN1, F_PMAN1 },
  { fireball, W_FBALL, F_FBALL },
};

//Clock Variables
//Wiring SCLK -> 9, I/O -> 8, CE -> 7
//Or CLK -> 9 , DAT -> 8, Reset -> 7

virtuabotixRTC myRTC(9, 8, 7);  //If you change the wiring change the pins here also

long interval1 = 1000;    // interval at which update LED display
long previousMillis = 0;  // will store last time LED display was updated
int finalCount = 60;
bool firstLoop = true;

void setup() {
  Serial.begin(9600);

  // Set the current date, and time in the following format:
  // seconds, minutes, hours, day of the week, day of the month, month, year
  myRTC.setDS1302Time(30, 59, 23, 6, 31, 12, 2022);  //Here you write your actual time/date as shown above
  //but remember to "comment/remove" this function once you're done
  //The setup is done only one time and the module will continue counting it automatically

  P.begin();
  P.displayText(curMessage, scrollAlign, scrollSpeed, scrollPause, effectIn, effectOut);
  P.setSpriteData(pacman1, W_PMAN1, F_PMAN1, pacman1, W_PMAN1, F_PMAN1);
}

int random(int min, int max)  //range : [min, max]
{

  static bool first = true;
  return min + rand() % ((max + 1) - min);
}

void timeCalc() {

  bool n = true;  //Random new Sprites
  do {
    numSprite = random(minSprite, maxSprite);
    if (numSprite != oldNumSprite)
      n = false;
  } while (n);
  oldNumSprite = numSprite;

  hH = 23 - myRTC.hours;
  mM = 59 - myRTC.minutes;
  sS = 59 - myRTC.seconds;

  unsigned long currentMillis = millis();

  if (myRTC.year < newYear) {
    if (mM > 0) {

      String combined = String(hH) + "H " + String(mM) + "M " + String(sS) + "S LEFT TO 2023";
      const char* result = combined.c_str();
      //Serial.println(result);

      strcpy(newMessage, result);
      newMessageAvailable = true;

    } else {
      if (currentMillis - previousMillis > interval1) {
        // save the last time to chaang LED display every second
        previousMillis = currentMillis;
        if (sS > 10) {  // last minute countdown
          if (firstLoop) {
            finalCount = sS;
            firstLoop = false;
          }
          String combined = " " + String(finalCount);
          const char* result = combined.c_str();
          //Serial.println(result);
          effectOut = PA_NO_EFFECT;
          scrollPause = 0;
          scrollSpeed = 59;
          strcpy(newMessage, result);
          newMessageAvailable = true;
          finalCount = finalCount - 1;

        } else {  // last 10 secounds countdown

          String combined = String(finalCount);
          const char* result = combined.c_str();
          effectIn = PA_FADE;
          effectOut = PA_NO_EFFECT;
          scrollAlign = PA_CENTER;
          scrollPause = 0;
          scrollSpeed = 90;
          strcpy(newMessage, result);
          newMessageAvailable = true;
          finalCount = finalCount - 1;
        }
      }
    }
  } else {  //New Year message

    effectIn = PA_SCROLL_LEFT;
    effectOut = PA_NO_EFFECT;
    scrollAlign = PA_LEFT;
    scrollPause = 500;
    scrollSpeed = 100;
    strcpy(newMessage, " Happy New Year 2023!");
    newMessageAvailable = true;
  }
}

void loop() {

  myRTC.updateTime();
  timeCalc();  // Time left calculation

  // Start printing elements as individuals
  //  Serial.print("Current Date / Time: ");
  //  Serial.print(myRTC.dayofmonth); //You can switch between day and month if you're using American system
  //  Serial.print("/");
  //  Serial.print(myRTC.month);
  //  Serial.print("/");
  //  Serial.print(myRTC.year);
  //  Serial.print(" ");
  //  Serial.print(myRTC.hours);
  //  Serial.print(":");
  //  Serial.print(myRTC.minutes);
  //  Serial.print(":");
  //  Serial.println(myRTC.seconds);
  Serial.print("Left");
  Serial.println(hH);
  Serial.print(":");
  Serial.print(mM);
  Serial.print(":");
  Serial.println(sS);

  if (P.displayAnimate()) {
    if (newMessageAvailable) {                                                                     //refresh display
      P.setSpriteData(sprite[numSprite].data, sprite[numSprite].width, sprite[numSprite].frames,   // entry sprite
                      sprite[numSprite].data, sprite[numSprite].width, sprite[numSprite].frames);  // exit sprite
      strcpy(curMessage, newMessage);
      P.displayText(curMessage, scrollAlign, scrollSpeed, scrollPause, effectIn, effectOut);
      newMessageAvailable = false;
    }
    P.displayReset();
  }
}
