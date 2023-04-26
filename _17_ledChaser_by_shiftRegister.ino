int latchPin = 5;  //  ST_CP (12) pin of Shift register
int clockPin = 6;  //  SH_CP (11) pin of Shift register
int dataPin = 4;  //   DS (14)  pin of shift register



void setup() {
  // All pin initialize as OUTPUT
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  
}

void loop() {

  updateShiftRegister(B10000000);  // first led will on
  delay(200);
  updateShiftRegister(B01000000);  // second led will on
  delay(200);
  updateShiftRegister(B00100000);  // third led will on
  delay(200);
  updateShiftRegister(B00010000);  // fourth led will on
  delay(200);
  updateShiftRegister(B00001000);  // fifth led will on
  delay(200);
  updateShiftRegister(B00000100);  // sixth led will on
  delay(200);
  updateShiftRegister(B00000010);  // Seventh led will on
  delay(200);
  updateShiftRegister(B00000001);  // Eighth led will on
  delay(200);
  updateShiftRegister(B00000010);  // 7th led will on
  delay(200);
  updateShiftRegister(B00000100);  // 6th led will on
  delay(200);
  updateShiftRegister(B00001000);  // 5th led will on
  delay(200);
  updateShiftRegister(B00010000);  // 4th led will on
  delay(200);
  updateShiftRegister(B00100000);  // 3rd led will on
  delay(200);
  updateShiftRegister(B01000000);  // 2nd led will on
  delay(200);
 
  

}


void updateShiftRegister(byte leds)  //  this is function to update shift register in which we have to pass output of shift register in binary form
{
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, leds);  // LSBFIRST means LSB bit arrive first
  digitalWrite(latchPin, HIGH);
  
}



