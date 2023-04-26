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
    SR595N_writeByte ( &sr1, 0x3);
    Serial.println(readVal);
    delay(100);
}
