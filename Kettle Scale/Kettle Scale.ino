

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <soc/rtc.h>
#include <HX711.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Preferences.h>

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 16;
const int LOADCELL_SCK_PIN = 5;
const byte ledPin = 33;

HX711 scale;
int reading;
int lastReading;
//REPLACE WITH YOUR CALIBRATION FACTOR
#define CALIBRATION_FACTOR 451.753

//OLED Display
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

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

Preferences preferences;

void displayWeight(int weight) {
  float cups = (float)weight / 244;
  display.clearDisplay();
  display.setCursor(25, 10);
  display.setTextSize(5);
  display.print(cups, 1);
  display.display();
  if (cups > 1) {
    digitalWrite(ledPin, HIGH);
  } else if (cups < 0.5) {
    digitalWrite(ledPin, HIGH);
    delay(200);
    digitalWrite(ledPin, LOW);
    delay(200);

  } else {
    digitalWrite(ledPin, LOW);
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
  scale.set_scale(453);
  scale.tare();
  Serial.println("Put known weight on ");
    display.setCursor(0, 10);
  display.print("Calibrate");
    display.display();
  delay(5500);
  Serial.print(scale.get_units(10));
  Serial.print(" Divide this value to the weight and insert it in the scale.set_scale() statement");
    display.setCursor(0, 30);
  display.print(scale.get_units(10));
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

void setupSavingCups() {
  preferences.begin("kettleScale", false);
  
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
  scale.tare();
  pinMode(ledPin, OUTPUT);

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

void loop() {
  if (scale.wait_ready_timeout(200)) {
    reading = round(scale.get_units());
    Serial.print("Weight: ");
    Serial.println(reading);
    Serial.print("Cups: ");
    float val = (float)reading / 247;

    Serial.println(val, 1);
    if (reading != lastReading) {
      displayWeight(reading);
    }
    lastReading = reading;

    if (!mqttClient.connected())
      reconnect();

    mqttClient.loop();

    long now = millis();
    if (now - last_time > 10000) {
      sprintf(data, "%f", val);
      mqttClient.publish("kettle/cups",data);
      last_time = now;
    }
  } else {
    Serial.println("HX711 not found.");
  }
}