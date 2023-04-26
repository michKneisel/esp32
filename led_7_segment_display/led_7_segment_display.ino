// for ESP32 microcontroller
int clockPin = 7;//22; //green SHCP
int latchPin = 6;//21; // brown STCP
int dataPin = 4;//23; //blue DS

int DS_pin = 4;//23;
int STCP_pin = 6;//21;
int SHCP_pin = 7;//22 ;
                    // 0   1  2    3  4  5  6  7  8  9
int dec_digits   [10] {126,48,109,121,51,91,95,112,127,115};

void setup() {

  pinMode(DS_pin,   OUTPUT);
  pinMode(STCP_pin, OUTPUT);
  pinMode(SHCP_pin, OUTPUT);
}


void   loop() { 
 
// for (int j=0;j<10;j++){  
for (int i=0;i<10;i++){

  digitalWrite(STCP_pin,LOW);
  shiftOut(DS_pin, SHCP_pin, LSBFIRST,dec_digits[i]);

  // shiftOut(DS_pin, SHCP_pin, LSBFIRST,dec_digits[j]);
  digitalWrite(STCP_pin, HIGH);
  delay(3000);
}
}
// } 
 