#include <Controller.h>

int main(void)
{
    int i;

    DDRA = 0xFF; // put PORTA into output mode

    /* 
    * The 8 LEDs are connected to pin 22 to 29
    */
    while (1) // main loop
    {
        for (i=0;i<8;i++) {
            PORTA = 1<<i;  // shifts bits to left for the given iteration count
            _delay_ms(500); // delays by 500ms
        }
    }
    return (1);
} // end main