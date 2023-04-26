

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <soc/rtc.h>
#include <HX711.h>
#include <WiFi.h>
#include <PubSubClient.h>

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 16;
const int LOADCELL_SCK_PIN = 5;

HX711 scale;
int reading;
int lastReading;
//REPLACE WITH YOUR CALIBRATION FACTOR
#define CALIBRATION_FACTOR 478.579

//OLED Display
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
// SCL pin -> 22
// SDA pin -> 21

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET -1  // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//WIFI
WiFiClient espWifiClient;
PubSubClient mqttClient(espWifiClient);
char* mqttServer = "197.242.149.64";
int mqttPort = 1883;

long last_time = 0;
char data[100];
char raw[100];
float prev_cups = 0.0f;

long currentOffset;

void displayWeight(float cups) {
  display.clearDisplay();
  display.setCursor(25, 10);
  display.setTextSize(5);
  display.print(cups, 1);
  display.display();
}

float calculateCups(int weight) {
  if (weight > 500 ) {
    return (float)weight / 244;
  } else {
    return 0.0f;
  }

}

void initWifi() {
  WiFi.begin("TP-LINK_75C5", "99650988");

  // Wi-Fi
  Serial.println("Connecting to WiFi..");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to the WiFi network");
}

void calibrate() {
  float factor = 478.579;
  scale.set_scale(factor);
   // set tare and save value
    scale.tare();
    currentOffset = scale.get_offset();
     Serial.print("Current Offset: ");
    Serial.println(currentOffset);  
  Serial.println("Put known weight on ");
  display.setCursor(0, 10);
  display.print("Calibrate");
  display.display();
  Serial.print(scale.get_units(10));
  delay(5500);

  Serial.print(" ... ");
  display.setCursor(0, 30);
  display.print(scale.get_units(10));
  Serial.println(scale.get_units(10));
  char text[100];
  sprintf(text, "%.3f", factor);
  Serial.print("factor = ");
  Serial.println(text);
  display.display();

  while(1==1);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Callback - ");
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
}

void setupMQTT() {
  mqttClient.setServer(mqttServer, mqttPort);
  // set the callback function
  mqttClient.setCallback(callback);
}

void setup() {
  Serial.begin(115200);

  // rtc_clk_cpu_freq_set(RTC_CPU_FREQ_80M);
  // initialize with the I2C addr 0x3C
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  delay(2000);
  display.clearDisplay();
  display.setTextColor(WHITE);

  initWifi();

  Serial.println("Initializing the scale");
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  // uncomment if you want to calibrate the bowl
  // calibrate();

  scale.set_scale(CALIBRATION_FACTOR);  // this value is obtained by calibrating the scale with known weights; see the README for details
  // scale.tare();
  scale.set_offset(1080911);

  setupMQTT();
}

void reconnect() {
  Serial.println("Connecting to MQTT Broker...");
  while (!mqttClient.connected()) {
    Serial.println("Reconnecting to MQTT Broker..");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);

    if (mqttClient.connect(clientId.c_str())) {
      Serial.println("Connected.");
    }
  }
}

boolean shouldPublishCups(float cups, int reading) {
  long now = millis();
  if(last_time == 0) {
    return true;
  }
  // Serial.print("cups ");
  // Serial.print(cups);
  // Serial.print(" (cups - prev_cups) ");
  // Serial.print(cups - prev_cups);
  // Serial.print(" now - last_time = ");
  // int time = now - last_time;
  // Serial.print(time);
  // Serial.print(" 15 seconds = ");
  // Serial.print(15 * 1000);
  // float cupDifference = fabsf(cups - prev_cups);
  // char text[100];
  // sprintf(text, " %.1f ", cupDifference);
  // Serial.println(text);

  if ((fabsf(cups - prev_cups) > 0.1f) && ((now - last_time) > (15 * 1000)) && reading > 0) { //30 seconds
    return true;
  }
  if ((fabsf(cups - prev_cups) < 0.1f || fabsf(cups - prev_cups) > 0.1f) && (last_time - now) > (24 * 60 * 60 * 1000)) { // 2hours
    return true;
  }
  return false;
}

void loop() {
  if (scale.wait_ready_timeout(200)) {
    reading = round(scale.get_units());
    // Serial.print("Weight: ");
    // Serial.println(reading);
    // Serial.print("Cups: ");
    float cups = calculateCups(reading);
    // Serial.print("Published at: ");
    // Serial.print(last_time);
    // Serial.print(" prev cups = ");
    // Serial.println(prev_cups);
    // Serial.println(cups);

    if (reading != lastReading) {
      displayWeight(cups);
    }
    lastReading = reading;

    if (!mqttClient.connected())
      reconnect();

    mqttClient.loop();

    long now = millis();

    // if cups - prev_cups diff is > 0.1
      // publish cups
    

    // if (now - last_time > 10000) {
    if (shouldPublishCups(cups, reading)) {
      sprintf(data, "%.1f", cups);
      sprintf(raw, "%d", lastReading);
      mqttClient.publish("kettle/cups",data);
      Serial.print("publish data ");
      Serial.println(data);
      mqttClient.publish("kettle/raw",raw);
      prev_cups = cups;
      last_time = now;
    }
  } else {
    Serial.println("HX711 not found.");
  }
}