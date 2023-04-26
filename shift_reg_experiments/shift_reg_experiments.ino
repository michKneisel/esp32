/*
  74HC595 Shift Register with 7-segment LED display
  74hc595-7segdisplay.ino
  Count in hex from 0-F and display on 7-segment Common Cathode LED display

  DroneBot Workshop 2020
  https://dronebotworkshop.com
*/

// Define Connections to 74HC595

// ST_CP pin 12 RCLK
const int latchPin = 4;
// SH_CP pin 11 SRCLK
const int clockPin = 19;
// DS pin 14 SER
const int dataPin = 9;

// Patterns for characters 0,1,2,3,4,5,6,7,8,9,A,b,C,d,E,F
// int datArray[6] = {B10010010, B11111111, B10010010, B01001001, B10010010, B11111111};
//                  white                   blue                red                green
int datArray[8] = {B11111111, B11111111, B00100100, B00100100, B10010010, B10010010, B01001001, B01001001};
// int datArray[5] = {B10010010, B11111111, B00000000, B01001001, B00100100};//, B01100110, B10110110, B10111110, B11100000, B11111110, B11110110, B11101110, B00111110, B10011100, B01111010, B10011110, B10001110};
int offArray[4] = {B00000000, B00000000, B00000000, B00000000};

int red[2] ={B01001001, B01001001};

void setup ()
{
  // Setup pins as Outputs
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  Serial.begin(115200);
}
void loop()
{
  // Count from 0 to 15
  for (int num = 0; num < 8; num++)
  {
    Serial.print("num: ");
    Serial.print(num);
    // ST_CP LOW to keep LEDs from changing while reading serial data
    digitalWrite(latchPin, LOW);

    Serial.print(" datArray: ");
    Serial.println(datArray[num]);

    // Shift out the bits
    shiftOut(dataPin, clockPin, LSBFIRST, datArray[num]);
    Serial.println("shifted ");

    // ST_CP HIGH change LEDs
    digitalWrite(latchPin, HIGH);
        Serial.println("written ");

    delay(1000);
  }
}