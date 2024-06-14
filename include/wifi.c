/**
 * wifi.c
 * Aaron Fredrick — MXEN2003, June 2024
 *
 * Drives the ESP8266 via AT commands over UART3. Each board opens a UDP
 * socket and exchanges DataPacket structs with the peer.
 */

#include "wifi.h"
#include "serial.h"
#include <util/delay.h>
#include <string.h>
#include <stdio.h>

static WifiMode active_mode;

/* Transmit a null-terminated AT command string and wait for the ESP to settle */
static void sendAtCommand(const char *cmd)
{
    serial3_print_string((char *)cmd);
    serial3_write_byte('\r');
    serial3_write_byte('\n');
    _delay_ms(500);
}

void wifiInit(WifiMode mode)
{
    active_mode = mode;
    serial3_init();
    _delay_ms(1000);

    sendAtCommand("AT+RST");
    _delay_ms(2000);
    sendAtCommand("AT+CWMODE=1");
    _delay_ms(500);

    char join_cmd[80];
    snprintf(join_cmd, sizeof(join_cmd), "AT+CWJAP=\"%s\",\"%s\"", WIFI_SSID, WIFI_PASS);
    sendAtCommand(join_cmd);
    _delay_ms(5000);

    sendAtCommand("AT+CIPMUX=0");

    uint16_t local_port  = (mode == WIFI_MODE_ROBOT) ? WIFI_ROBOT_PORT : WIFI_CONTROLLER_PORT;
    char     udp_cmd[80];
    const char *peer_ip   = (mode == WIFI_MODE_ROBOT) ? WIFI_CONTROLLER_IP : WIFI_ROBOT_IP;
    uint16_t peer_port    = (mode == WIFI_MODE_ROBOT) ? WIFI_CONTROLLER_PORT : WIFI_ROBOT_PORT;

    snprintf(udp_cmd, sizeof(udp_cmd),
             "AT+CIPSTART=\"UDP\",\"%s\",%u,%u,2",
             peer_ip, peer_port, local_port);
    sendAtCommand(udp_cmd);
}

void wifiSendPacket(const DataPacket *packet)
{
    char send_cmd[32];
    snprintf(send_cmd, sizeof(send_cmd), "AT+CIPSEND=%u", (unsigned)sizeof(DataPacket));
    sendAtCommand(send_cmd);
    _delay_ms(50);

    const uint8_t *bytes = (const uint8_t *)packet;
    for (uint8_t i = 0; i < sizeof(DataPacket); i++)
    {
        serial3_write_byte(bytes[i]);
    }
}

uint8_t wifiReceivePacket(DataPacket *packet_out)
{
    static uint8_t  rx_buf[sizeof(DataPacket)];
    static uint8_t  rx_count   = 0;
    static uint8_t  collecting = 0;

    /* Poll UART3 data register directly — called from ISR or tight loop */
    if (!(UCSR3A & (1 << RXC3)))
    {
        return 0;
    }

    uint8_t byte = UDR3;

    if (!collecting && byte == PKT_START_BYTE)
    {
        collecting     = 1;
        rx_count       = 0;
        rx_buf[rx_count++] = byte;
        return 0;
    }

    if (collecting)
    {
        rx_buf[rx_count++] = byte;

        if (rx_count == sizeof(DataPacket))
        {
            collecting = 0;
            rx_count   = 0;

            DataPacket *candidate = (DataPacket *)rx_buf;
            uint8_t expected = calculateChecksum(rx_buf, sizeof(DataPacket) - 1);

            if (candidate->checksum == expected)
            {
                memcpy(packet_out, candidate, sizeof(DataPacket));
                return 1;
            }
        }
    }

    return 0;
}
