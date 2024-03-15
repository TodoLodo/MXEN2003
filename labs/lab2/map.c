
#include "map.h"

uint8_t map(uint16_t value, uint16_t fromLow, uint16_t fromHigh, uint8_t toLow, uint8_t toHigh)
{
  return (uint8_t)((value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow);
}