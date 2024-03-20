#include <stdint.h>
#include <stdbool.h>
#include <Controller.h>
#include "map.h"

int main()
{
  static uint8_t channel = 0;
  bool lastButtonState = false;

  DDRA = 0xFF; // put PORTA into output mode
  DDRC = 254;  // put PORTC into input mode for pin 0 only (0b11111110)

  PORTC = 1; // enable the internal pull-up resistor on C1

  adc_init(); // initialse ADC
  _delay_ms(20);

  while (1)
  {
    if (PINC & (1)) // if PINC0 is high
    {
      lastButtonState = true;
    }
    else if (lastButtonState) // if PINC0 is low (when the button is pressed in the active low configuration)
    {
      // PORTL = 1;
      channel = (channel == 1) ? 0 : 1;
      lastButtonState = false;
    }
    
    PORTA = 1 << map(adc_read(channel), 0, 1023, 0, 7);
  }

  return (1);
}