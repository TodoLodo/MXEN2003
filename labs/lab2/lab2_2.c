#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <Controller.h>
#include "map.h"

int main()
{
  DDRA = 0xFF; // put PORTA into output mode

  adc_init(); // initialse ADC
  _delay_ms(20);

  while (1)
  {
    PORTA = 1 << map(adc_read(0), 0, 1023, 0, 7);
  }

  return (1);
}