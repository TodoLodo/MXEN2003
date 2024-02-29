#include <Controller.h>

int main(void)
{
    DDRA = 0xFF; // put PORTA into output mode

    /* 
    * The two LEDs are connected to pin 22 and 23
    * These pins are controlled by manupilation the first 2 LSBs of PORTA
    * To set HIGH and LOW alternatively PORTA should alternate between 0b00000001 and 0b00000010 (1 and 2) 
    */
    while (1) // main loop
    {
        _delay_ms(500);      // 500 millisecond delay
        PORTA = 1; // setting pin 22 HIGH and others LOW in PORTA 
        _delay_ms(500);
        PORTA = 2; // setting pin 23 HIGH and others LOW in PORTA 
    }
    return (1);
} // end main