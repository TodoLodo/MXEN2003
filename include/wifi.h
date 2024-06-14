/**
 * wifi.h
 * Aaron Fredrick — MXEN2003, June 2024
 *
 * ESP8266 co-processor on UART3. Both boards join the same AP and exchange
 * DataPacket structs over UDP.
 */

#ifndef WIFI_H_
#define WIFI_H_

#include <stdint.h>
#include "comms.h"
#include "ESP.h"

#define WIFI_ROBOT_PORT      5000
#define WIFI_CONTROLLER_PORT 5001
#define WIFI_ROBOT_IP        "192.168.4.2"
#define WIFI_CONTROLLER_IP   "192.168.4.1"

typedef enum
{
    WIFI_MODE_ROBOT      = 0,
    WIFI_MODE_CONTROLLER = 1
} WifiMode;

void    wifiInit(WifiMode mode);
void    wifiSendPacket(const DataPacket *packet);
uint8_t wifiReceivePacket(DataPacket *packet_out);

#endif /* WIFI_H_ */
