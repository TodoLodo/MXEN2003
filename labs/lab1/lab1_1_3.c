/* 
* Code for Lab 1 Task 5
* wokwi: https://wokwi.com/projects/391071142460258305
*/
#include <Controller.h>

int main(void)
{
    int i;

    DDRC = 0xFF; // put PORTC into output mode

    /* 
    * The 8 LEDs are connected to pin 30 to 37
    */
    while (1) // main loop
    {
        for (i=0;i<8;i++) {
            PORTC = 1<<i;  // shifts bits to left for the given iteration count
            _delay_ms(500); // delays by 500ms
        }
    }
    return (1);
} // end main