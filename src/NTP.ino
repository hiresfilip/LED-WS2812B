#include <Arduino.h>
#include <FastLED.h>
/*#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>*/


#define LED_PIN 4
#define NUM_LEDS 116
#define LED_DIGITS 4
#define LED_PER_DIGITS_STRIP 56
#define CHIPSET WS2812B
#define COLOR_ORDER GRB

/*WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "cz.pool.ntp.org", 3600, 60000);
String formattedDate;
String dayStamp;
String timeStamp;

const char * ssid = "TP-Link_7632";
const char * password = "97261261";
const char * hostName = "ESPWS_8621_WS_DC";
const char * http_username = "admin";
const char * http_password = "admin";*/
//const char * PARAM_MESSAGE = "message";
int r_val = 255;
int g_val = 0;
int b_val = 0;
CRGB leds[NUM_LEDS];
CRGB color = CRGB(r_val, g_val, b_val);
byte brightness = 200;

byte segGroups1[14][4] = {
  {  8,  9, 10, 11 },             // top, a
  { 12, 13, 14, 15 },             // top right, b
  { 16, 17, 18, 19 },             // bottom right, c
  { 20, 21, 22, 23 },             // bottom, d
  { 24, 25, 26, 27 },             // bottom left, e
  {  4,  5, 6, 7 },               // top left, f
  {  0,  1, 2, 3 },               // center, g
  // left (seen from front) digit
  { 44, 45, 46, 47 },             // top, a
  { 48, 49, 50, 51 },             // top right, b
  { 28, 29, 30, 31 },             // bottom right, c
  { 32, 33, 34, 35 },             // bottom, d
  { 36, 37, 38, 39 },             // bottom left, e
  { 40, 41, 42, 43 },             // top left, f
  { 52, 53, 54, 55 }              // center, g
};

int segGroups2[14][4] = {
  { 68, 69, 70, 71 },             // top, a
  { 72, 73, 74, 75 },             // top right, b
  { 76, 77, 78, 79 },             // bottom right, c
  { 80, 81, 82, 83 },             // bottom, d
  { 84, 85, 86, 87 },             // bottom left, e
  { 64, 65, 66, 67 },             // top left, f
  { 60, 61, 62, 63 },             // center, g
  // left (seen from front) digit
  { 104, 105, 106, 107 },             // top, a
  { 108, 109, 110, 111 },             // top right, b
  { 88, 89, 90, 91 },             // bottom right, c
  { 92, 93, 94, 95 },             // bottom, d
  { 96, 97, 98, 99 },             // bottom left, e
  { 100, 101, 102, 103 },             // top left, f
  { 112, 113, 114, 115 }              // center, g
};

// Note: The first number always has to be the lower one as they're subtracted later on... (fix by using abs()? ^^)

// Using above arrays it's very easy to "talk" to the segments. Simply use 0-6 for the first 7 segments, add 7 (7-13) for the following ones per strip/two digits
byte digits[10][7] = {                    // Lets define 10 numbers (0-9) with 7 segments each, 1 = segment is on, 0 = segment is off
  {   1,   1,   1,   1,   1,   1,   0 },  // 0 -> Show segments a - f, don't show g (center one)
  {   0,   1,   1,   0,   0,   0,   0 },  // 1 -> Show segments b + c (top and bottom right), nothing else
  {   1,   1,   0,   1,   1,   0,   1 },  // 2 -> and so on...
  {   1,   1,   1,   1,   0,   0,   1 },  // 3
  {   0,   1,   1,   0,   0,   1,   1 },  // 4
  {   1,   0,   1,   1,   0,   1,   1 },  // 5
  {   1,   0,   1,   1,   1,   1,   1 },  // 6
  {   1,   1,   1,   0,   0,   0,   0 },  // 7
  {   1,   1,   1,   1,   1,   1,   1 },  // 8
  {   1,   1,   1,   1,   0,   1,   1 }   // 9
};

void setup(){
  /*Serial.begin(115200);
  Serial.setDebugOutput(true);
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(hostName);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.printf("STA: Failed!\n");
    WiFi.disconnect(false);
    delay(1000);
    WiFi.begin(ssid, password);
  }
  timeClient.begin();
  timeClient.setTimeOffset(3600);*/

  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(200); /* MAX: 255 */
}

/*void displayTime(time_t t, byte color, byte colorSpacing) {
  byte posOffset = 0;                                                                     // this offset will be used to move hours and minutes...
  if ( LED_DIGITS / 2 > 2) posOffset = 2;                                                 // ... to the left so we have room for the seconds when there's 6 digits available
  if ( displayMode == 0 ) {                                                               // if 12h mode is selected...
    if ( hourFormat12(t) >= 10 ) showDigit(1, color + colorSpacing * 2, 3 + posOffset);   // ...and hour > 10, display 1 at position 3
    showDigit((hourFormat12(t) % 10), color + colorSpacing * 3, 2  + posOffset);          // display 2nd digit of HH anyways
  } else if ( displayMode == 1 ) {                                                        // if 24h mode is selected...
    if ( hour(t) > 9 ) showDigit(hour(t) / 10, color + colorSpacing * 2, 3 + posOffset);  // ...and hour > 9, show 1st digit at position 3 (this is to avoid a leading 0 from 0:00 - 9:00 in 24h mode)
    showDigit(hour(t) % 10, color + colorSpacing * 3, 2 + posOffset);                     // again, display 2nd digit of HH anyways
  }
  showDigit((minute(t) / 10), color + colorSpacing * 4, 1 + posOffset);                   // minutes thankfully don't differ between 12h/24h, so this can be outside the above if/else
  showDigit((minute(t) % 10), color + colorSpacing * 5, 0 + posOffset);                   // each digit is drawn with an increasing color (*2, *3, *4, *5) (*6 and *7 for seconds only in HH:MM:SS)
  if ( posOffset > 0 ) {
    showDigit((second(t) / 10), color + colorSpacing * 6, 1);
    showDigit((second(t) % 10), color + colorSpacing * 7, 0);
  }
  if ( second(t) % 2 == 0 ) showDots(2, second(t) * 4.25);                                // show : between hours and minutes on even seconds with the color cycling through the palette once per minute
  lastSecond = second(t);
}*/

  /*void showSegment(byte segment, byte color, byte segDisplay) {
  // This shows the segments from top of the sketch on a given position (segDisplay).
  // pos 0 is the most right one (seen from the front) where data in is connected to the arduino
  byte leds_per_segment = 1 + abs( segGroups1[segment][1] - segGroups1[segment][0] );            // get difference between 2nd and 1st value in array to get led count for this segment
  if (segDisplay % 2 != 0 ) segment += 7;                                                  // if segDisplay/position is odd add 7 to segment
  for (byte i = 0; i < leds_per_segment; i++) {                                             // fill all leds inside current segment with color
    leds[(segGroups1[segment][0] + ( segDisplay / 2 ) * ( LED_PER_DIGITS_STRIP) ) + i];
  }
}

  void showDigit(byte digit, byte color, byte pos) {
  // This draws numbers using the according segments as defined on top of the sketch (0 - 9)
  for (byte i = 0; i < 7; i++) {
    if (digits[digit][i] != 0) showSegment(i, color, pos);
  }
}*/

void num(int cislo){
/* 0 */
    if(cislo == 0){
      for(int i = 0; i < 4; i++){
      leds[segGroups1[0][i]] = CRGB::Green;    //a
      leds[segGroups1[1][i]] = CRGB::Red;      //b
      leds[segGroups1[2][i]] = CRGB::Blue;     //c
      leds[segGroups1[3][i]] = CRGB::Yellow;   //d
      leds[segGroups1[4][i]] = CRGB::Violet;   //e
      leds[segGroups1[5][i]] = CRGB::Orange;   //f
      }
    FastLED.show();
  }

  /* 1 */
  else if(cislo == 1){
    for(int i = 0; i < 4; i++){
      leds[segGroups1[1][i]] = CRGB::Red;
      leds[segGroups1[2][i]] = CRGB::Blue;
    }
  FastLED.show();
  }

  /* 2 */
  else if(cislo == 2){
    for(int i = 0; i < 4; i++){
      leds[segGroups1[0][i]] = CRGB::Red;      //a
      leds[segGroups1[1][i]] = CRGB::Blue;     //b
      leds[segGroups1[6][i]] = CRGB::Violet;   //g
      leds[segGroups1[4][i]] = CRGB::Yellow;   //e
      leds[segGroups1[3][i]] = CRGB::Green;    //d
    }
  FastLED.show();
  }

    /* 3 */
  else if(cislo == 3){
    for(int i = 0; i < 4; i++){
      leds[segGroups1[0][i]] = CRGB::Red;      //a
      leds[segGroups1[1][i]] = CRGB::Blue;     //b
      leds[segGroups1[6][i]] = CRGB::Violet;   //g
      leds[segGroups1[2][i]] = CRGB::Yellow;   //c
      leds[segGroups1[3][i]] = CRGB::Green;    //d
    }
  FastLED.show();
  }

    /* 4 */
    else if(cislo == 4){
      for(int i = 0; i < 4; i++){
        leds[segGroups1[1][i]] = CRGB::Blue;     //b
        leds[segGroups1[6][i]] = CRGB::Violet;   //g
        leds[segGroups1[2][i]] = CRGB::Yellow;   //c
        leds[segGroups1[5][i]] = CRGB::Green;    //f
      }
    FastLED.show();
    }

    /* 5 */
    else if(cislo == 5){
      for(int i = 0; i < 4; i++){
        leds[segGroups1[0][i]] = CRGB::Red;      //a
        leds[segGroups1[5][i]] = CRGB::Green;    //f
        leds[segGroups1[6][i]] = CRGB::Violet;   //g
        leds[segGroups1[2][i]] = CRGB::Yellow;   //c
        leds[segGroups1[3][i]] = CRGB::Blue;     //d
        }
    FastLED.show();
    }

  /* 6 */
    else if(cislo == 6){
      for(int i = 0; i < 4; i++){
        leds[segGroups1[0][i]] = CRGB::Red;      //a
        leds[segGroups1[5][i]] = CRGB::Green;    //f
        leds[segGroups1[4][i]] = CRGB::Yellow;   //e
        leds[segGroups1[3][i]] = CRGB::Blue;     //d
        leds[segGroups1[2][i]] = CRGB::Yellow;   //c
        leds[segGroups1[6][i]] = CRGB::Violet;   //g
      }
    FastLED.show();
    }

    /* 7 */
    else if(cislo == 7){
      for(int i = 0; i < 4; i++){
        leds[segGroups1[0][i]] = CRGB::Green;    //a
        leds[segGroups1[1][i]] = CRGB::Red;      //b
        leds[segGroups1[2][i]] = CRGB::Blue;     //c
      }
    FastLED.show();
    }

    /* 8 */
    else if(cislo == 8){
      for(int i = 0; i < 4; i++){
        leds[segGroups1[0][i]] = CRGB::Green;    //a
        leds[segGroups1[1][i]] = CRGB::Red;      //b
        leds[segGroups1[2][i]] = CRGB::Blue;     //c
        leds[segGroups1[3][i]] = CRGB::Yellow;   //d
        leds[segGroups1[4][i]] = CRGB::Violet;   //e
        leds[segGroups1[5][i]] = CRGB::Orange;   //f
        leds[segGroups1[6][i]] = CRGB::Pink;     //g
      }
    FastLED.show();
    }

    /* 9 */
    else{
      for(int i = 0; i < 4; i++){
        for(int i = 0; i < 4; i++){
          leds[segGroups1[0][i]] = CRGB::Green;    //a
          leds[segGroups1[1][i]] = CRGB::Red;      //b
          leds[segGroups1[2][i]] = CRGB::Blue;     //c
          leds[segGroups1[3][i]] = CRGB::Yellow;   //d
          leds[segGroups1[5][i]] = CRGB::Orange;   //f
          leds[segGroups1[6][i]] = CRGB::Pink;     //g
      }
      FastLED.show();
    }
  }
}

/* ################################################################################################################################################# */
void vypisMinuty(int cislo){
  /* 0 */
    if(cislo == 0){
      for(int i = 0; i < 4; i++){
        for(int j = 0; j < 6; j++){
          leds[segGroups1[j][i]] = color;
        }
      }
    FastLED.show();
  }

  /* 1 */
  else if(cislo == 1){
    for(int i = 0; i < 4; i++){
      for(int j = 1; j < 3; j++){
        //leds[segGroups1[j][i]] = CRGB::Red;
        leds[segGroups1[j][i]] = color;
      }
    }
  FastLED.show();
  }

  /* 2 */
  else if(cislo == 2){
    for(int i = 0; i < 4; i++){
      leds[segGroups1[0][i]] = color;
      leds[segGroups1[1][i]] = color;
      leds[segGroups1[6][i]] = color;
      leds[segGroups1[4][i]] = color;
      leds[segGroups1[3][i]] = color;
    }
  FastLED.show();
    }

    /* 3 */
    else if(cislo == 3){
      for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
          leds[segGroups1[j][i]] = color;
          leds[segGroups1[6][i]] = color;
        }
      }
    FastLED.show();
    }

    /* 4 */
    else if(cislo == 4){
      for(int i = 0; i < 4; i++){
        leds[segGroups1[1][i]] = color;
        leds[segGroups1[6][i]] = color;
        leds[segGroups1[2][i]] = color;
        leds[segGroups1[5][i]] = color;
      }
    FastLED.show();
    }

    /* 5 */
    else if(cislo == 5){
      for(int i = 0; i < 4; i++){
        leds[segGroups1[0][i]] = color;
        leds[segGroups1[5][i]] = color;
        leds[segGroups1[6][i]] = color;
        leds[segGroups1[2][i]] = color;
        leds[segGroups1[3][i]] = color;
      }
    FastLED.show();
    }

  /* 6 */
    else if(cislo == 6){
      for(int i = 0; i < 4; i++){
        for(int j = 2; j < 7; j++){
          leds[segGroups1[j][i]] = color;
          leds[segGroups1[0][i]] = color;
        }
      }
    FastLED.show();
    }

    /* 7 */
    else if(cislo == 7){
      for(int i = 0; i < 4; i++){
        for(int j = 0; j < 3; j++){
          leds[segGroups1[j][i]] = color;
        }
      }
    FastLED.show();
    }

    /* 8 */
    else if(cislo == 8){
      for(int i = 0; i < 4; i++){
        for(int j = 0; j < 7; j++){
          leds[segGroups1[j][i]] = color;
        }
      }
    FastLED.show();
    }

    /* 9 */
    else if(cislo == 9){
      for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
          for(int k = 5; k < 7; k++){
            leds[segGroups1[j][i]] = color;
            leds[segGroups1[k][i]] = color;
          }
        }
      }
      FastLED.show();
    }

/* ----------------------------------------------------------------------------------------------------- */
    /* 10 */
    else if(cislo == 10){
      for(int i = 0; i < 4; i++){
        for(int k = 8; k < 10; k++){
          leds[segGroups1[k][i]] = color;
        }
        for(int j = 0; j < 6; j++){
          leds[segGroups1[j][i]] = color;
        }
      }
      FastLED.show();
    }

    /* 11 */
    else if(cislo == 11){
      for(int i = 0; i < 4; i++){
        for(int k = 8; k < 10; k++){
          leds[segGroups1[k][i]] = color;
          for(int j = 1; j < 3; j++){
            leds[segGroups1[j][i]] = color;
          }
        } 
      }
      FastLED.show();
    }

    /* 12 */
    else if(cislo == 12){
      for(int i = 0; i < 4; i++){
        for(int k = 8; k < 10; k++){
          leds[segGroups1[k][i]] = color;
          }
          leds[segGroups1[0][i]] = color;
          leds[segGroups1[1][i]] = color;
          leds[segGroups1[6][i]] = color;
          leds[segGroups1[4][i]] = color;
          leds[segGroups1[3][i]] = color;
        }
      FastLED.show();
    }

    /* 13 */
    else if(cislo == 13){
      for (int i = 0; i < 4; i++){
        for(int k = 8; k < 10; k++){
          leds[segGroups1[k][i]] = color;
        }
        for(int j = 0; j < 4; j++){
          leds[segGroups1[j][i]] = color;
          leds[segGroups1[6][i]] = color;
        }
      }
      FastLED.show();
    }

    /* 14 */
    else if(cislo == 14){
      for(int i = 0; i < 4; i++){
        for(int k = 8; k < 10; k++){
          leds[segGroups1[k][i]] = color;
        }
        leds[segGroups1[1][i]] = color;
        leds[segGroups1[6][i]] = color;
        leds[segGroups1[2][i]] = color;
        leds[segGroups1[5][i]] = color;
      }
      FastLED.show();
    }

    /* 15 */
    else if(cislo == 15){
      for(int i = 0; i < 4; i++){
        for(int k = 8; k < 10; k++){
          leds[segGroups1[k][i]] = color;
        }
        leds[segGroups1[0][i]] = color;
        leds[segGroups1[5][i]] = color;
        leds[segGroups1[6][i]] = color;
        leds[segGroups1[2][i]] = color;
        leds[segGroups1[3][i]] = color;
      }
      FastLED.show();
    }

    /* 16 */
    else if(cislo == 16){
      for(int i = 0; i < 4; i++){
       for(int k = 8; k < 10; k++){
          leds[segGroups1[k][i]] = color;
        }
        for(int j = 2; j < 7; j++){
          leds[segGroups1[j][i]] = color;
          leds[segGroups1[0][i]] = color;
        }
      }
      FastLED.show();
    }

    /* 17 */
    else if(cislo == 17){
      for(int i = 0; i < 4; i++){
       for(int k = 8; k < 10; k++){
          leds[segGroups1[k][i]] = color;
        }
        for(int j = 0; j < 3; j++){
          leds[segGroups1[j][i]] = color;
        }
      }
      FastLED.show();
    }

    /* 18 */
    else if(cislo == 18){
      for(int i = 0; i < 4; i++){
       for(int k = 8; k < 10; k++){
          leds[segGroups1[k][i]] = color;
        }
        for(int j = 0; j < 7; j++){
          leds[segGroups1[j][i]] = color;
        }
      }
      FastLED.show();
    }

    /* 19 */
    else if(cislo == 19){
      for(int i = 0; i < 4; i++){
       for(int k = 8; k < 10; k++){
          leds[segGroups1[k][i]] = color;
        }
        for(int j = 0; j < 4; j++){
          for(int m = 5; m < 7; m++){
            leds[segGroups1[j][i]] = color;
            leds[segGroups1[m][i]] = color;
          }
        }
      }
      FastLED.show();
    }

    /* ----------------------------------------------------------------------------------------------------- */
    /* 20 */
    else if(cislo == 20){
      for(int i = 0; i < 4; i++){
        leds[segGroups1[7][i]] = color;
        leds[segGroups1[8][i]] = color;
        leds[segGroups1[10][i]] = color;
        leds[segGroups1[11][i]] = color;
        leds[segGroups1[13][i]] = color;
      for(int j = 0; j < 6; j++){
          leds[segGroups1[j][i]] = color;
        }
      }
      FastLED.show();
    }

    /* 21 */
    else if(cislo == 21){
      for(int i = 0; i < 4; i++){
        leds[segGroups1[7][i]] = color;
        leds[segGroups1[8][i]] = color;
        leds[segGroups1[10][i]] = color;
        leds[segGroups1[11][i]] = color;
        leds[segGroups1[13][i]] = color;
        for(int j = 1; j < 3; j++){
          leds[segGroups1[j][i]] = color;
        }
      }
      FastLED.show();
    }

    /* 22 */
    else if(cislo == 22){
      for(int i = 0; i < 4; i++){
        leds[segGroups1[7][i]] = color;
        leds[segGroups1[8][i]] = color;
        leds[segGroups1[10][i]] = color;
        leds[segGroups1[11][i]] = color;
        leds[segGroups1[13][i]] = color;

        leds[segGroups1[0][i]] = color;
        leds[segGroups1[1][i]] = color;
        leds[segGroups1[6][i]] = color;
        leds[segGroups1[4][i]] = color;
        leds[segGroups1[3][i]] = color;
      }
      FastLED.show();
    }

  /* 23 */
    else if(cislo == 23){
      for(int i = 0; i < 4; i++){
        leds[segGroups1[7][i]] = color;
        leds[segGroups1[8][i]] = color;
        leds[segGroups1[10][i]] = color;
        leds[segGroups1[11][i]] = color;
        leds[segGroups1[13][i]] = color;
        for(int j = 0; j < 4; j++){
          leds[segGroups1[j][i]] = color;
          leds[segGroups1[6][i]] = color;
        }
      }
      FastLED.show();
    }

    /* 24 */
    else if(cislo == 24){
      for(int i = 0; i < 4; i++){
        leds[segGroups1[7][i]] = color;
        leds[segGroups1[8][i]] = color;
        leds[segGroups1[10][i]] = color;
        leds[segGroups1[11][i]] = color;
        leds[segGroups1[13][i]] = color;
        
        leds[segGroups1[1][i]] = color;
        leds[segGroups1[6][i]] = color;
        leds[segGroups1[2][i]] = color;
        leds[segGroups1[5][i]] = color;
      }
      FastLED.show();
    }

    /* 25 */
    else if(cislo == 25){
      for(int i = 0; i < 4; i++){
        leds[segGroups1[7][i]] = color;
        leds[segGroups1[8][i]] = color;
        leds[segGroups1[10][i]] = color;
        leds[segGroups1[11][i]] = color;
        leds[segGroups1[13][i]] = color;
        
        leds[segGroups1[0][i]] = color;
        leds[segGroups1[5][i]] = color;
        leds[segGroups1[6][i]] = color;
        leds[segGroups1[2][i]] = color;
        leds[segGroups1[3][i]] = color;
      }
      FastLED.show();
    }

    /* 26 */
    else if(cislo == 26){
      for(int i = 0; i < 4; i++){
        leds[segGroups1[7][i]] = color;
        leds[segGroups1[8][i]] = color;
        leds[segGroups1[10][i]] = color;
        leds[segGroups1[11][i]] = color;
        leds[segGroups1[13][i]] = color;
        for(int j = 2; j < 7; j++){
          leds[segGroups1[j][i]] = color;
          leds[segGroups1[0][i]] = color;
        }
      }
      FastLED.show();
    }

    /* 27 */
    else if(cislo == 27){
      for(int i = 0; i < 4; i++){
        leds[segGroups1[7][i]] = color;
        leds[segGroups1[8][i]] = color;
        leds[segGroups1[10][i]] = color;
        leds[segGroups1[11][i]] = color;
        leds[segGroups1[13][i]] = color;
        for(int j = 0; j < 3; j++){
          leds[segGroups1[j][i]] = color;
        }
      }
      FastLED.show();
    }


    /* 28 */
    else if(cislo == 28){
      for(int i = 0; i < 4; i++){
        leds[segGroups1[7][i]] = color;
        leds[segGroups1[8][i]] = color;
        leds[segGroups1[10][i]] = color;
        leds[segGroups1[11][i]] = color;
        leds[segGroups1[13][i]] = color;
        for(int j = 0; j < 7; j++){
          leds[segGroups1[j][i]] = color;
        }
      }
      FastLED.show();
    }
    
    /* 29 */
    else if(cislo == 29){
      for(int i = 0; i < 4; i++){
        leds[segGroups1[7][i]] = color;
        leds[segGroups1[8][i]] = color;
        leds[segGroups1[10][i]] = color;
        leds[segGroups1[11][i]] = color;
        leds[segGroups1[13][i]] = color;
        for(int j = 0; j < 4; j++){
          for(int m = 5; m < 7; m++){
            leds[segGroups1[j][i]] = color;
            leds[segGroups1[m][i]] = color;
          }
        }
      }
      FastLED.show();
    }

/* ----------------------------------------------------------------------------------------------------- */
  /* 30 */
    else if(cislo == 30){
      for(int i = 0; i < 4; i++){
        for(int j = 7; j < 11; j++){
          leds[segGroups1[j][i]] = color;
          leds[segGroups1[13][i]] = color;
          for(int k = 0; k < 6; k++){
            leds[segGroups1[k][i]] = color;
          }
        }
      }
      FastLED.show();
    }

  /* 31 */
    else if(cislo == 31){
      for(int i = 0; i < 4; i++){
        for(int j = 7; j < 11; j++){
          leds[segGroups1[j][i]] = color;
          leds[segGroups1[13][i]] = color;
          for(int k = 1; k < 3; k++){
            leds[segGroups1[k][i]] = color;
          }
        }
      }
      FastLED.show();
    }

    
  /* 32 */
    else if(cislo == 32){
      for(int i = 0; i < 4; i++){
        for(int j = 7; j < 11; j++){
          leds[segGroups1[j][i]] = color;
          leds[segGroups1[13][i]] = color;
        }
          leds[segGroups1[0][i]] = color;
          leds[segGroups1[1][i]] = color;
          leds[segGroups1[6][i]] = color;
          leds[segGroups1[4][i]] = color;
          leds[segGroups1[3][i]] = color;
      }
      FastLED.show();
    }

    
  /* 33 */
    else if(cislo == 33){
      for(int i = 0; i < 4; i++){
        for(int j = 7; j < 11; j++){
          leds[segGroups1[j][i]] = color;
          leds[segGroups1[13][i]] = color;
        }
          for(int k = 0; k < 4; k++){
          leds[segGroups1[k][i]] = color;
          leds[segGroups1[6][i]] = color;
        }
      }
      FastLED.show();
    }

  /* 34 */
    else if(cislo == 34){
      for(int i = 0; i < 4; i++){
        for(int j = 7; j < 11; j++){
          leds[segGroups1[j][i]] = color;
          leds[segGroups1[13][i]] = color;
        }
        leds[segGroups1[1][i]] = color;
        leds[segGroups1[6][i]] = color;
        leds[segGroups1[2][i]] = color;
        leds[segGroups1[5][i]] = color;
      }
      FastLED.show();
    }

    
  /* 35 */
    else if(cislo == 35){
      for(int i = 0; i < 4; i++){
        for(int j = 7; j < 11; j++){
          leds[segGroups1[j][i]] = color;
          leds[segGroups1[13][i]] = color;
        }
        leds[segGroups1[0][i]] = color;
        leds[segGroups1[5][i]] = color;
        leds[segGroups1[6][i]] = color;
        leds[segGroups1[2][i]] = color;
        leds[segGroups1[3][i]] = color;
      }
      FastLED.show();
    }

    
  /* 36 */
    else if(cislo == 36){
      for(int i = 0; i < 4; i++){
        for(int j = 7; j < 11; j++){
          leds[segGroups1[j][i]] = color;
          leds[segGroups1[13][i]] = color;
        }
        for(int k = 2; k < 7; k++){
          leds[segGroups1[k][i]] = color;
          leds[segGroups1[0][i]] = color;
        }
      }
      FastLED.show();
    }

        
  /* 37 */
    else if(cislo == 37){
      for(int i = 0; i < 4; i++){
        for(int j = 7; j < 11; j++){
          leds[segGroups1[j][i]] = color;
          leds[segGroups1[13][i]] = color;
        }
        for(int k = 0; k < 3; k++){
          leds[segGroups1[k][i]] = color;
        }
      }
      FastLED.show();
    }

            
  /* 38 */
    else if(cislo == 38){
      for(int i = 0; i < 4; i++){
        for(int j = 7; j < 11; j++){
          leds[segGroups1[j][i]] = color;
          leds[segGroups1[13][i]] = color;
        }
        for(int k = 0; k < 7; k++){
          leds[segGroups1[k][i]] = color;
        }
      }
      FastLED.show();
    }
            
  /* 39 */
    else if(cislo == 39){
      for(int i = 0; i < 4; i++){
        for(int j = 7; j < 11; j++){
          leds[segGroups1[j][i]] = color;
          leds[segGroups1[13][i]] = color;
        }
        for(int k = 0; k < 4; k++){
          for(int m = 5; m < 7; m++){
            leds[segGroups1[k][i]] = color;
            leds[segGroups1[m][i]] = color;
          }
        }
      }
      FastLED.show();
    }

/* ----------------------------------------------------------------------------------------------------- */
  /* 40 */
  else if(cislo == 40){
    for(int i = 0; i< 4; i++){
      leds[segGroups1[8][i]] = color;
      leds[segGroups1[9][i]] = color;
      leds[segGroups1[12][i]] = color;
      leds[segGroups1[13][i]] = color;
      for(int j = 0; j < 6; j++){
          leds[segGroups1[j][i]] = color;
      }
    }
    FastLED.show();
  }

  /* 41 */
  else if(cislo == 41){
    for(int i = 0; i< 4; i++){
      leds[segGroups1[8][i]] = color;
      leds[segGroups1[9][i]] = color;
      leds[segGroups1[12][i]] = color;
      leds[segGroups1[13][i]] = color;
      for(int j = 1; j < 3; j++){
        leds[segGroups1[j][i]] = color;
      }
    }
    FastLED.show();
  }


  /* 42 */
  else if(cislo == 42){
    for(int i = 0; i< 4; i++){
      leds[segGroups1[8][i]] = color;
      leds[segGroups1[9][i]] = color;
      leds[segGroups1[12][i]] = color;
      leds[segGroups1[13][i]] = color;

      leds[segGroups1[0][i]] = color;
      leds[segGroups1[1][i]] = color;
      leds[segGroups1[6][i]] = color;
      leds[segGroups1[4][i]] = color;
      leds[segGroups1[3][i]] = color;
    }
    FastLED.show();
  }


  /* 43 */
  else if(cislo == 43){
    for(int i = 0; i< 4; i++){
      leds[segGroups1[8][i]] = color;
      leds[segGroups1[9][i]] = color;
      leds[segGroups1[12][i]] = color;
      leds[segGroups1[13][i]] = color;
      for(int j = 0; j < 4; j++){
        leds[segGroups1[j][i]] = color;
        leds[segGroups1[6][i]] = color;
      }
    }
    FastLED.show();
  }


  /* 44 */
  else if(cislo == 44){
    for(int i = 0; i< 4; i++){
      leds[segGroups1[8][i]] = color;
      leds[segGroups1[9][i]] = color;
      leds[segGroups1[12][i]] = color;
      leds[segGroups1[13][i]] = color;
      
      leds[segGroups1[1][i]] = color;
      leds[segGroups1[6][i]] = color;
      leds[segGroups1[2][i]] = color;
      leds[segGroups1[5][i]] = color;
    }
    FastLED.show();
  }


  /* 45 */
  else if(cislo == 45){
    for(int i = 0; i< 4; i++){
      leds[segGroups1[8][i]] = color;
      leds[segGroups1[9][i]] = color;
      leds[segGroups1[12][i]] = color;
      leds[segGroups1[13][i]] = color;
      
      leds[segGroups1[0][i]] = color;
      leds[segGroups1[5][i]] = color;
      leds[segGroups1[6][i]] = color;
      leds[segGroups1[2][i]] = color;
      leds[segGroups1[3][i]] = color;
    }
    FastLED.show();
  }


  /* 46 */
  else if(cislo == 46){
    for(int i = 0; i< 4; i++){
      leds[segGroups1[8][i]] = color;
      leds[segGroups1[9][i]] = color;
      leds[segGroups1[12][i]] = color;
      leds[segGroups1[13][i]] = color;
      
      for(int j = 2; j < 7; j++){
      leds[segGroups1[j][i]] = color;
      leds[segGroups1[0][i]] = color;
      }
    }
    FastLED.show();
  }


  /* 47 */
  else if(cislo == 47){
    for(int i = 0; i< 4; i++){
      leds[segGroups1[8][i]] = color;
      leds[segGroups1[9][i]] = color;
      leds[segGroups1[12][i]] = color;
      leds[segGroups1[13][i]] = color;
      
      for(int j = 0; j < 3; j++){
      leds[segGroups1[j][i]] = color;
      }
    }
    FastLED.show();
  }


  /* 48 */
  else if(cislo == 48){
    for(int i = 0; i< 4; i++){
      leds[segGroups1[8][i]] = color;
      leds[segGroups1[9][i]] = color;
      leds[segGroups1[12][i]] = color;
      leds[segGroups1[13][i]] = color;
      
      for(int j = 0; j < 7; j++){
        leds[segGroups1[j][i]] = color;
      }
    }
    FastLED.show();
  }


  /* 49 */
  else if(cislo == 49){
    for(int i = 0; i< 4; i++){
      leds[segGroups1[8][i]] = color;
      leds[segGroups1[9][i]] = color;
      leds[segGroups1[12][i]] = color;
      leds[segGroups1[13][i]] = color;
      
      for(int j = 0; j < 4; j++){
        for(int k = 5; k < 7; k++){
          leds[segGroups1[j][i]] = color;
          leds[segGroups1[k][i]] = color;
        }
      }
    }
    FastLED.show();
  }

/* ----------------------------------------------------------------------------------------------------- */
  /* 50 */
  else if(cislo == 50){
    for(int i = 0; i < 4; i++){
      leds[segGroups1[7][i]] = color;
      leds[segGroups1[9][i]] = color;
      leds[segGroups1[10][i]] = color;
      leds[segGroups1[12][i]] = color;
      leds[segGroups1[13][i]] = color;
      for(int j = 0; j < 6; j++){
        leds[segGroups1[j][i]] = color;
      }
    }
    FastLED.show();
  }

  /* 51 */
  else if(cislo == 51){
    for(int i = 0; i < 4; i++){
      leds[segGroups1[7][i]] = color;
      leds[segGroups1[9][i]] = color;
      leds[segGroups1[10][i]] = color;
      leds[segGroups1[12][i]] = color;
      leds[segGroups1[13][i]] = color;
      for(int j = 1; j < 3; j++){
        leds[segGroups1[j][i]] = color;
      }
    }
    FastLED.show();
  }

  /* 52 */
  else if(cislo == 52){
    for(int i = 0; i < 4; i++){
      leds[segGroups1[7][i]] = color;
      leds[segGroups1[9][i]] = color;
      leds[segGroups1[10][i]] = color;
      leds[segGroups1[12][i]] = color;
      leds[segGroups1[13][i]] = color;
      
      leds[segGroups1[0][i]] = color;
      leds[segGroups1[1][i]] = color;
      leds[segGroups1[6][i]] = color;
      leds[segGroups1[4][i]] = color;
      leds[segGroups1[3][i]] = color;
    }
    FastLED.show();
  }

  /* 53 */
  else if(cislo == 53){
    for(int i = 0; i < 4; i++){
      leds[segGroups1[7][i]] = color;
      leds[segGroups1[9][i]] = color;
      leds[segGroups1[10][i]] = color;
      leds[segGroups1[12][i]] = color;
      leds[segGroups1[13][i]] = color;
      
      for(int j = 0; j < 4; j++){
        leds[segGroups1[j][i]] = color;
        leds[segGroups1[6][i]] = color;
      }
    }
    FastLED.show();
  }

  /* 54 */
  else if(cislo == 54){
    for(int i = 0; i < 4; i++){
      leds[segGroups1[7][i]] = color;
      leds[segGroups1[9][i]] = color;
      leds[segGroups1[10][i]] = color;
      leds[segGroups1[12][i]] = color;
      leds[segGroups1[13][i]] = color;
      
      leds[segGroups1[1][i]] = color;
      leds[segGroups1[6][i]] = color;
      leds[segGroups1[2][i]] = color;
      leds[segGroups1[5][i]] = color;
    }
    FastLED.show();
  }
  
  /* 55 */
  else if(cislo == 55){
    for(int i = 0; i < 4; i++){
      leds[segGroups1[7][i]] = color;
      leds[segGroups1[9][i]] = color;
      leds[segGroups1[10][i]] = color;
      leds[segGroups1[12][i]] = color;
      leds[segGroups1[13][i]] = color;

      leds[segGroups1[0][i]] = color;
      leds[segGroups1[5][i]] = color;
      leds[segGroups1[6][i]] = color;
      leds[segGroups1[2][i]] = color;
      leds[segGroups1[3][i]] = color;
    }
    FastLED.show();
  }

  
  /* 56 */
  else if(cislo == 56){
    for(int i = 0; i < 4; i++){
      leds[segGroups1[7][i]] = color;
      leds[segGroups1[9][i]] = color;
      leds[segGroups1[10][i]] = color;
      leds[segGroups1[12][i]] = color;
      leds[segGroups1[13][i]] = color;

      for(int j = 2; j < 7; j++){
        leds[segGroups1[j][i]] = color;
        leds[segGroups1[0][i]] = color;
      }
    }
    FastLED.show();
  }

  
  /* 57 */
  else if(cislo == 57){
    for(int i = 0; i < 4; i++){
      leds[segGroups1[7][i]] = color;
      leds[segGroups1[9][i]] = color;
      leds[segGroups1[10][i]] = color;
      leds[segGroups1[12][i]] = color;
      leds[segGroups1[13][i]] = color;

      for(int j = 0; j < 3; j++){
        leds[segGroups1[j][i]] = color;
      }
    }
    FastLED.show();
  }

  
  /* 58 */
  else if(cislo == 58){
    for(int i = 0; i < 4; i++){
      leds[segGroups1[7][i]] = color;
      leds[segGroups1[9][i]] = color;
      leds[segGroups1[10][i]] = color;
      leds[segGroups1[12][i]] = color;
      leds[segGroups1[13][i]] = color;

      for(int j = 0; j < 7; j++){
        leds[segGroups1[j][i]] = color;
      }
    }
    FastLED.show();
  }
  
  /* 59 */
  else if(cislo == 59){
    for(int i = 0; i < 4; i++){
      leds[segGroups1[7][i]] = color;
      leds[segGroups1[9][i]] = color;
      leds[segGroups1[10][i]] = color;
      leds[segGroups1[12][i]] = color;
      leds[segGroups1[13][i]] = color;

      for(int j = 0; j < 4; j++){
        for(int k = 5; k < 7; k++){
          leds[segGroups1[j][i]] = color;
          leds[segGroups1[k][i]] = color;
        }
      }
    }
    FastLED.show();
  }
}

void dvojtecka(){
  for(int z = 56; z <= 59; z++){
    leds[z] = color;
  }
  FastLED.show();
}
/* ################################################################################################################################################# */

void vypisHodiny(int cislo){
  /* 0 */
  if(cislo == 0){
    for(int i = 0; i < 4; i++){
      for(int j = 0; j < 6; j++){
       leds[segGroups2[j][i]] = color;
      }
    }
    FastLED.show();
  }
  
  /* 1 */
  else if(cislo == 1){
    for(int i = 0; i < 4; i++){
      for(int j = 1; j < 3; j++){
        leds[segGroups2[j][i]] = color;
      }
    }
  FastLED.show();
  }

  /* 2 */
  else if(cislo == 2){
    for(int i = 0; i < 4; i++){
      leds[segGroups2[0][i]] = color;
      leds[segGroups2[1][i]] = color;
      leds[segGroups2[6][i]] = color;
      leds[segGroups2[4][i]] = color;
      leds[segGroups2[3][i]] = color;
    }
  FastLED.show();
    }

    /* 3 */
    else if(cislo == 3){
      for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
          leds[segGroups2[j][i]] = color;
          leds[segGroups2[6][i]] = color;
        }
      }
    FastLED.show();
    }

    /* 4 */
    else if(cislo == 4){
      for(int i = 0; i < 4; i++){
        leds[segGroups2[1][i]] = color;
        leds[segGroups2[6][i]] = color;
        leds[segGroups2[2][i]] = color;
        leds[segGroups2[5][i]] = color;
      }
    FastLED.show();
    }

    /* 5 */
    else if(cislo == 5){
      for(int i = 0; i < 4; i++){
        leds[segGroups2[0][i]] = color;
        leds[segGroups2[5][i]] = color;
        leds[segGroups2[6][i]] = color;
        leds[segGroups2[2][i]] = color;
        leds[segGroups2[3][i]] = color;
      }
    FastLED.show();
    }

  /* 6 */
    else if(cislo == 6){
      for(int i = 0; i < 4; i++){
        for(int j = 2; j < 7; j++){
          leds[segGroups2[j][i]] = color;
          leds[segGroups2[0][i]] = color;
        }
      }
    FastLED.show();
    }

    /* 7 */
    else if(cislo == 7){
      for(int i = 0; i < 4; i++){
        for(int j = 0; j < 3; j++){
          leds[segGroups2[j][i]] = color;
        }
      }
    FastLED.show();
    }

    /* 8 */
    else if(cislo == 8){
      for(int i = 0; i < 4; i++){
        for(int j = 0; j < 7; j++){
          leds[segGroups2[j][i]] = color;
        }
      }
    FastLED.show();
    }

    /* 9 */
    else if(cislo == 9){
      for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
          for(int k = 5; k < 7; k++){
            leds[segGroups2[j][i]] = color;
            leds[segGroups2[k][i]] = color;
          }
        }
      }
      FastLED.show();
    }
}

/*void showNum(int cislo){
  if(cislo >= 0 && cislo <= 9){
    nums(cislo);
  }
  if(cislo >= 10 && cislo < 20){

  }
}*/

/*byte digit1[10][28] = {
{ 0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 }, //0
{ 0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0 }, //1
{ 1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1 }, //2
{ 1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0 }, //3
{ 1,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0 }, //4
{ 1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0 }, //5
{ 1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1 }, //6
{ 0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0 }, //7
{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 }, //8
{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0 }  //9
};*/

/*byte digit2[10][28] = {
  { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0 }, //0
  { 1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0 }, //1
  { 0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1 }, //2
  { 1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1 }, //3
  { 1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1 }, //4
  { 1,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1 }, //5
  { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1 }, //6
  { 1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0 }, //7
  { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 }, //8
  { 1,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 }  //9
};*/
/*
byte digit1[10][28] = {
{ 4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27 }, //0
{ 12,13,14,15,16,17,18,19 }, //1
{ 0,1,2,3,8,9,10,11,12,13,14,15,20,21,22,23,24,25,26,27 }, //2
{ 0,1,2,3,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23 }, //3
{ 0,1,2,3,4,5,6,7,12,13,14,15,16,17,18,19 }, //4
{ 0,1,2,3,4,5,6,7,8,9,10,11,16,17,18,19,20,21,22,23 }, //5
{ 0,1,2,3,4,5,6,7,8,9,10,11,16,17,18,19,20,21,22,23,24,25,26,27 }, //6
{ 8,9,10,11,12,13,14,15,16,17,18,19 }, //7
{ 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27 }, //8
{ 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23 }  //9
};*/

/*void vypis(int cislo){
  for(int i = 0; i < 28; i++){
    leds[digit1[cislo][i]] = CRGB::Red;
  }
  FastLED.show();
}*/

void loop(){
//vypis(5);
//delay(100);
  int g = 0;
  while(g >= 0 && g <= 59){
  vypisMinuty(g);
  dvojtecka();
  delay(500);
    for(int i = 0; i < 29; i++){
      for(int j = 0; j < 11; j++){
        leds[segGroups1[j][i]] = CRGB::Black;
        //leds[digit1[j][i]] = CRGB::Black;
      }
      FastLED.show();
      delay(10);
    }
    g++;
  }
//vypisHodiny(1);
}

  /*for(int i = 0; i < NUM_LEDS; i++){
    leds[i] = CRGB::Red;
    delay(100);
    FastLED.show();
  }

  for(int i = 0; i < NUM_LEDS; i++){
    leds[i] = CRGB::Black;
    delay(100);
    FastLED.show();
  }
}*/

/* 0 */
  /*for(int i = 0; i < 4; i++){
  leds[segGroups1[0][i]] = CRGB::Green;    //a
  leds[segGroups1[1][i]] = CRGB::Red;      //b
  leds[segGroups1[2][i]] = CRGB::Blue;     //c
  leds[segGroups1[3][i]] = CRGB::Yellow;   //d
  leds[segGroups1[4][i]] = CRGB::Violet;   //e
  leds[segGroups1[5][i]] = CRGB::Orange;   //f
  delay(100);
  }
  FastLED.show();
}*/


/* 1 */
  /*for(int i = 0; i < 4; i++){
  leds[segGroups1[1][i]] = CRGB::Red;
  leds[segGroups1[2][i]] = CRGB::Blue;
  delay(500);
}
FastLED.show();*/

/* 2 */
/*for(int i = 0; i < 4; i++){
  leds[segGroups1[0][i]] = CRGB::Red;      //a
  leds[segGroups1[1][i]] = CRGB::Blue;     //b
  leds[segGroups1[6][i]] = CRGB::Violet;   //g
  leds[segGroups1[4][i]] = CRGB::Yellow;   //e
  leds[segGroups1[3][i]] = CRGB::Green;    //d
  delay(500);
}*/


/* 3 */
/*for(int i = 0; i < 4; i++){
  leds[segGroups1[0][i]] = CRGB::Red;      //a
  leds[segGroups1[1][i]] = CRGB::Blue;     //b
  leds[segGroups1[6][i]] = CRGB::Violet;   //g
  leds[segGroups1[2][i]] = CRGB::Yellow;   //c
  leds[segGroups1[3][i]] = CRGB::Green;    //d
  delay(500);
}*/

/* 4 */
/*for(int i = 0; i < 4; i++){
  leds[segGroups1[1][i]] = CRGB::Blue;     //b
  leds[segGroups1[6][i]] = CRGB::Violet;   //g
  leds[segGroups1[2][i]] = CRGB::Yellow;   //c
  leds[segGroups1[5][i]] = CRGB::Green;    //f
  delay(500);
  }*/

/* 5 */
/*for(int i = 0; i < 4; i++){
  leds[segGroups1[0][i]] = CRGB::Red;      //a
  leds[segGroups1[5][i]] = CRGB::Green;    //f
  leds[segGroups1[6][i]] = CRGB::Violet;   //g
  leds[segGroups1[2][i]] = CRGB::Yellow;   //c
  leds[segGroups1[3][i]] = CRGB::Blue;     //d
  delay(500);
  }*/

  /* 6 */
  /*for(int i = 0; i < 4; i++){
  leds[segGroups1[0][i]] = CRGB::Red;      //a
  leds[segGroups1[5][i]] = CRGB::Green;    //f
  leds[segGroups1[4][i]] = CRGB::Yellow;   //e
  leds[segGroups1[3][i]] = CRGB::Blue;     //d
  leds[segGroups1[2][i]] = CRGB::Yellow;   //c
  leds[segGroups1[6][i]] = CRGB::Violet;   //g
  delay(500);
  }*/

  /* 7 */
  /*for(int i = 0; i < 4; i++){
  leds[segGroups1[0][i]] = CRGB::Green;    //a
  leds[segGroups1[1][i]] = CRGB::Red;      //b
  leds[segGroups1[2][i]] = CRGB::Blue;     //c
  delay(500);
  }*/

  /* 8 */
  /* 1. možnost */
  /*for(int i = 0; i < 4; i++){
  leds[segGroups1[0][i]] = CRGB::Green;    //a
  leds[segGroups1[1][i]] = CRGB::Red;      //b
  leds[segGroups1[2][i]] = CRGB::Blue;     //c
  leds[segGroups1[3][i]] = CRGB::Yellow;   //d
  leds[segGroups1[4][i]] = CRGB::Violet;   //e
  leds[segGroups1[5][i]] = CRGB::Orange;   //f
  leds[segGroups1[6][i]] = CRGB::Pink;     //g
  delay(100);
  }
  FastLED.show();*/

  /* 2. možnost */
  /*for(int i = 0; i < 4; i++){
    for(int j = 0; j < 7; j++){
      leds[segGroups1[j][i]] = CRGB::White;
    }
  delay(100);
  }
  FastLED.show();*/

  /* 9 */
  /*for(int i = 0; i < 4; i++){
  leds[segGroups1[0][i]] = CRGB::Green;    //a
  leds[segGroups1[1][i]] = CRGB::Red;      //b
  leds[segGroups1[2][i]] = CRGB::Blue;     //c
  leds[segGroups1[3][i]] = CRGB::Yellow;   //d
  leds[segGroups1[5][i]] = CRGB::Orange;   //f
  leds[segGroups1[6][i]] = CRGB::Pink;     //g
  delay(100);
  }
  FastLED.show();
}*/


/*
leds[segGroups1[13][0]] = CRGB::Red;
leds[segGroups1[13][1]] = CRGB::Green;
leds[segGroups1[13][2]] = CRGB::Blue;
leds[segGroups1[13][3]] = CRGB::Violet;
delay(20);

leds[segGroups1[6][0]] = CRGB::Red;
leds[segGroups1[6][1]] = CRGB::Green;
leds[segGroups1[6][2]] = CRGB::Blue;
leds[segGroups1[6][3]] = CRGB::Violet;
delay(20);

for(int i = 0; i<= 4; i++){
leds[55 + i] = CRGB::Yellow;
}

FastLED.show();
*/


/*leds[0] = CRGB::Red;
FastLED.show();
delay(500);

leds[0] = CRGB::Black;
FastLED.show();
delay(500);

leds[1] = CRGB::Green;
FastLED.show();
delay(500);

leds[1] = CRGB::Black;
FastLED.show();
delay(500);

leds[2] = CRGB::Blue;
FastLED.show();
delay(500);

leds[2] = CRGB::Black;
FastLED.show();
delay(500);

leds[110] = CRGB::Honeydew;
FastLED.show();
delay(500);

leds[110] = CRGB::Black;
FastLED.show();
delay(500);*/

  /*while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  formattedDate = timeClient.getFormattedTime();
  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(0, splitT);
  timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
  Serial.println(formattedDate);
  delay(1000);*/