/**
 * comms.h
 * Aaron Fredrick — MXEN2003, June 2024
 */

#ifndef COMMS_H_
#define COMMS_H_

#include <stdint.h>

#define PKT_START_BYTE 0xAA
#define PKT_CONTROL    0x01
#define PKT_TELEMETRY  0x02

/* Cast to uint8_t* for TX/RX — padding must not exist */
typedef struct __attribute__((packed))
{
    uint8_t  start_byte;
    uint8_t  packet_type;
    uint16_t ir_short;
    uint16_t ir_long;
    uint16_t battery_adc;
    uint16_t joystick_x;
    uint16_t joystick_y;
    uint8_t  cam_pan;
    uint8_t  cam_tilt;
    uint8_t  mode;
    uint8_t  speed_mode;
    uint8_t  checksum;
} DataPacket;

uint8_t calculateChecksum(const uint8_t *data, uint8_t length);

#endif /* COMMS_H_ */
