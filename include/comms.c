/**
 * comms.c
 * Aaron Fredrick — MXEN2003, June 2024
 */

#include "comms.h"

uint8_t calculateChecksum(const uint8_t *data, uint8_t length)
{
    uint8_t checksum = 0;
    for (uint8_t i = 0; i < length; i++)
    {
        checksum ^= data[i];
    }
    return checksum;
}
