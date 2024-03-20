#include <Controller.h>
#include <stdbool.h>

int main(void)
{

    // set A0 as output
    DDRA = 1;

    // Set high for A0
    PORTA = 1;

    // Main loop
    while (1)
    {
        PORTA = !PORTA;

        _delay_ms(500);
    }

    return 0;
}
