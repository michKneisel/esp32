#include "types.h"

/*
 * init function
 * writeByte
 */

void SR595N_init( Shift_Register_595N * const sr )
{
    pinMode( sr->SER, OUTPUT);
    pinMode( sr->RCLK, OUTPUT);
    pinMode( sr->SCLK, OUTPUT);
}


/* Byte value, and write LSB to data pin , pulse SCLK, shift value by >> 1, repeat 8x. Pulse OUT REG
 * 0b11011011
 */
void SR595N_writeByte ( const Shift_Register_595N * const sr , byte val )
{
    uint8_t counter;
    byte shiftVal;


https://forum.arduino.cc/t/i-have-daisy-chained-11-74hc595n-shift-registers-but-they-are-acting-strangely/683038/7
byte dataArray[11]; // referenced as dataArray[0] to dataArray[10]

// manipulate data as needed
dataArray[0] = some value, however you get there, same for the others

// send it out
digitalWrite (latchPin, LOW);
for (x = 0; x <11; x = x+1){
SPI.transfer ( dataArray[x] );
}
digitalWrite (latchPin, HIGH); // all outputs update on this rising edge


https://forum.arduino.cc/t/controlling-16-leds-with-two-74hc595-shift-register/696419/2
//                XWVUTSRQPONMLKJIHGFEDCBA
uint32_t LED1 = 0b111111111111111111111111;

shiftOut(dataPin, clockPin, MSBFIRST, LED1 >> 16); // send bits X-Q
shiftOut(dataPin, clockPin, MSBFIRST, LED1 >> 8);  // send bits P-I
shiftOut(dataPin, clockPin, MSBFIRST, LED1);       // send bits H-A


    for ( counter = 0; counter < NUM_BITS_IN_BYTE ; counter++ )
    {
        shiftVal = ( val & 0x01 );
        digitalWrite ( sr->SER, shiftVal );
        digitalWrite ( sr->SCLK, HIGH);
        digitalWrite ( sr->SCLK, LOW);
        val = val >> 1;
    }
    digitalWrite ( sr->RCLK, HIGH);
    digitalWrite ( sr->RCLK, LOW);
}

Shift_Register_595N sr1 = 
{
    .SER = 9,
    .RCLK = 4,
    .SCLK = 19
};


void setup()
{
    SR595N_init ( &sr1 );
    Serial.begin(115200);
}

uint8_t readVal = 0;
void loop()
{
    SR595N_writeByte ( &sr1, 0b10010010);
    delay(1000);
    SR595N_writeByte ( &sr1, 0b11011011);
    delay(1000);
    SR595N_writeByte ( &sr1, 0b10110110);
    delay(1000);
    SR595N_writeByte ( &sr1, 0b01001001);
    delay(1000);
    SR595N_writeByte ( &sr1, 0b00100100);
    delay(1000);
    SR595N_writeByte ( &sr1, 0b11111111);
    delay(1000);
    SR595N_writeByte ( &sr1, 0b00000000);
    delay(1000);
}
