/*
  ESP8266 Blink by Simon Peter
  Blink the blue LED on the ESP-01 module
  This example code is in the public domain

  The blue LED on the ESP-01 module is connected to GPIO1
  (which is also the TXD pin; so we cannot use Serial.print() at the same time)

  Note that this sketch uses LED_BUILTIN to find the pin with the internal LED
*/

#include "SSD1306Wire.h"
#include "OLEDDisplayUi.h"
#include "Wire.h"

#include "secrets.h"
#include "wifi_helper.h"

// Display Settings
const int I2C_DISPLAY_ADDRESS = 0x3c;
#if defined(ESP8266)
const int SDA_PIN = 2;
const int SDC_PIN = 0;
#else
const int SDA_PIN = 5; //D3;
const int SDC_PIN = 4; //D4;
#endif


WifiHelper wh; // use wh(123) to set EEPROM offset

 SSD1306Wire     display(I2C_DISPLAY_ADDRESS, SDA_PIN, SDC_PIN);
 OLEDDisplayUi   ui( &display );

void setup() {
      // initialize dispaly
  display.init();
  display.clear();
  display.display();

    display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setContrast(255);

  wh.setup();
  Serial.begin(115200);
  delay(1500); // just for debugging
  Serial.println("\nStarting.");
  
  display.print("Starting to connect to wifi");
    display.display();
  uint32_t ts_start = millis();
  bool res = wh.connect(WIFI_SSID, WIFI_AUTH);
  Serial.print("Timing connect(): "); Serial.print(millis()-ts_start); Serial.println("ms");
  Serial.print("Result="); Serial.println(res);

  


if (res) {
  display.print("Connected");
    display.display();
}
  

}

void loop() {
  delay(1000); // wait, reboot, try again. 
  Serial.println("Rebooting.");
  ESP.restart();
}
