#include <Arduino.h>
#include <FastLED.h>
#include <string>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>


#define LED_PIN 4
#define NUM_LEDS 116
//#define LED_DIGITS 4
//#define LED_PER_DIGITS_STRIP 56
#define CHIPSET WS2812B
#define COLOR_ORDER GRB

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "cz.pool.ntp.org", 3600, 60000);
String formattedDate;
String dayStamp;
String timeStamp;

//const char * ssid = "ESPNet";
//const char * password = "";
const char * ssid = "TP-Link_7632";
const char * password = "97261261";
//const char * ssid = "www.computerparts.cz";
//const char * password = "algoritmus";
//const char * ssid = "SSPUOpava";
//const char * password = "";
const char * hostName = "ESPWS_8621_WS_DC";
const char * http_username = "admin";
const char * http_password = "admin";
const char * PARAM_MESSAGE = "message";
CRGB leds[NUM_LEDS];
uint8_t hue = 0;
int alfa = 255;

void setup(){
  Serial.begin(115200);
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
  timeClient.setTimeOffset(3600);

  //int hod = timeClient.getHours();
  //int min = timeClient.getMinutes();

  
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  //FastLED.setBrightness(alfa); /* MAX: 255 */
  /*while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  dvojteckaON();
  displayTime(hod,min); */

}

/* Jednotky minut */
int digitMin1[10][28] = {
{ 4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,-1,-1,-1,-1 },       //0
{ 12,13,14,15,16,17,18,19,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 }, //1
{ 0,1,2,3,8,9,10,11,12,13,14,15,20,21,22,23,24,25,26,27,-1,-1,-1,-1,-1,-1,-1,-1 },       //2
{ 0,1,2,3,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,-1,-1,-1,-1,-1,-1,-1,-1 },       //3
{ 0,1,2,3,4,5,6,7,12,13,14,15,16,17,18,19,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 },         //4
{ 0,1,2,3,4,5,6,7,8,9,10,11,16,17,18,19,20,21,22,23,-1,-1,-1,-1,-1,-1,-1,-1 },           //5
{ 0,1,2,3,4,5,6,7,8,9,10,11,16,17,18,19,20,21,22,23,24,25,26,27,-1,-1,-1,-1 },           //6
{ 8,9,10,11,12,13,14,15,16,17,18,19,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 },   //7
{ 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27 },           //8
{ 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,-1,-1,-1,-1 }            //9
};

/* Desítky minut */
int digitMin2[10][28] = {
{ 28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1 }, //0
{ 28,29,30,31,48,49,50,51,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 }, //1
{ 32,33,34,35,36,37,38,39,44,45,46,47,48,49,50,51,52,53,54,55,-1,-1,-1,-1,-1,-1,-1,-1 }, //2
{ 28,29,30,31,32,33,34,35,44,45,46,47,48,49,50,51,52,53,54,55,-1,-1,-1,-1,-1,-1,-1,-1 }, //3
{ 28,29,30,31,40,41,42,43,48,49,50,51,52,53,54,55,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 }, //4
{ 28,29,30,31,32,33,34,35,40,41,42,43,44,45,46,47,52,53,54,55,-1,-1,-1,-1,-1,-1,-1,-1 }, //5
{ 28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,52,53,54,55,-1,-1,-1,-1 }, //6
{ 28,29,30,31,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 }, //7
{ 28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55 }, //8
{ 28,29,30,31,32,33,34,35,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,-1,-1,-1,-1 }  //9
};

/* Jednotky hodin */
int digitHod1[10][28] = {
{ 64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,-1,-1,-1,-1 }, //0
{ 72,73,74,75,76,77,78,79,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 }, //1
{ 60,61,62,63,68,69,70,71,72,73,74,75,80,81,82,83,84,85,86,87,-1,-1,-1,-1,-1,-1,-1,-1 }, //2
{ 60,61,62,63,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,-1,-1,-1,-1,-1,-1,-1,-1 }, //3
{ 60,61,62,63,64,65,66,67,72,73,74,75,76,77,78,79,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 }, //4
{ 60,61,62,63,64,65,66,67,68,69,70,71,76,77,78,79,80,81,82,83,-1,-1,-1,-1,-1,-1,-1,-1 }, //5
{ 60,61,62,63,64,65,66,67,68,69,70,71,76,77,78,79,80,81,82,83,84,85,86,87,-1,-1,-1,-1 }, //6
{ 68,69,70,71,72,73,74,75,76,77,78,79,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 }, //7
{ 60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87 }, //8
{ 60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,-1,-1,-1,-1 }  //9
};

/* Desítky hodin */
int digitHod2[10][28] = {
{ 88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,-1,-1,-1,-1 },      //0
{ 88,89,90,91,108,109,110,111,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 },              //1
{ 92,93,94,95,96,97,98,99,104,105,106,107,108,109,110,111,112,113,114,115,-1,-1,-1,-1,-1,-1,-1,-1 },      //2
{ 88,89,90,91,92,93,94,95,104,105,106,107,108,109,110,111,112,113,114,115,-1,-1,-1,-1,-1,-1,-1,-1 },      //3
{ 88,89,90,91,100,101,102,103,108,109,110,111,112,113,114,115,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 },      //4
{ 88,89,90,91,92,93,94,95,100,101,102,103,104,105,106,107,112,113,114,115,-1,-1,-1,-1,-1,-1,-1,-1 },      //5
{ 88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,112,113,114,115,-1,-1,-1,-1 },      //6
{ 88,89,90,91,104,105,106,107,108,109,110,111,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 },          //7
{ 88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115 },  //8
{ 88,89,90,91,92,93,94,95,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,-1,-1,-1,-1 }   //9
};

int dots[] = {56,57,58,59};

/* Jednotky minuty */
void displayMin1(int cislo){
  for(int i = 0; i < 28; i++){
    leds[digitMin1[cislo][i]] = CHSV(hue+(i*10),255,255);
  }
 hue++; 
}

/* Desítky minut */
void displayMin2(int cislo){
  for(int i = 0; i < 28; i++){
    leds[digitMin2[cislo][i]] = CHSV(hue+(i*10),255,255);
  }
  hue++;
}

/* Dvojtečka */
void dvojteckaON(){
  for(int z = 0; z < 4; z++){
    leds[dots[z]] = CHSV(hue+(z*10),255,255);
  }
  hue++;
}

void dvojteckaOFF(){
  for(int z = 0; z < 4; z++){
    leds[dots[z]] = CRGB::Black;
  }
  hue++;
}

/* Jednotky hodin */
void displayHod1(int cislo){
  for(int i = 0; i < 28; i++){
    leds[digitHod1[cislo][i]] = CHSV(hue+(i*10),255,255);
  }
  hue++;
}

/* Desítky hodin */
void displayHod2(int cislo){
  for(int i = 0; i < 28; i++){
    leds[digitHod2[cislo][i]] = CHSV(hue+(i*10),255,255);
  }
  hue++;
}

/* Vymazání celého displeje */
void displayBlack(){
  for(int i = 0; i < NUM_LEDS; i++){
    leds[i] = CRGB::Black;
  }
  
}

void displayTime(int h, int m){
    displayMin1(m % 10);
    displayMin2(m / 10);
    displayHod1(h % 10);
    displayHod2(h / 10);
}

void loop(){
  
  formattedDate = timeClient.getFormattedTime();
  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(0, splitT);
  timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
  Serial.println(formattedDate);
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  int h = timeClient.getHours();
  int m = timeClient.getMinutes();

  //dvojteckaON();
  //  dvojteckaON();
    unsigned long currentMillis = millis();
    unsigned long previousMillis = 0;
    if (currentMillis - previousMillis >= 1000) {
      // save the last time you blinked the LED
    previousMillis = currentMillis;
      displayBlack();
      displayTime(h,m);
      dvojteckaON();
    } 
  FastLED.show();
  //delay(1000);
}