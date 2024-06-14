/**
 * Robot.c
 * Aaron Fredrick — MXEN2003, June 2024
 */

#include "Robot.h"

static volatile Motor        g_motor       = {0, 0, 0, 0};
static volatile uint8_t      g_cam_pan     = 90;
static volatile uint8_t      g_cam_tilt    = 90;
static volatile uint8_t      g_auto_state  = 0;
static volatile uint8_t      g_speed_state = 0;
static volatile DataPacket   g_rx_packet;
static volatile uint8_t      g_rx_ready    = 0;

int main(void)
{
    milliseconds_init();
    adc_init();
    serial0_init();
    motorDriverInit();
    servoInit();
    ledInit();
    navigationInit(ADC_IR_SHORT, ADC_IR_LONG);
    wifiInit(WIFI_MODE_ROBOT);

    sei();

    while (1)
    {
        if (g_rx_ready)
        {
            g_rx_ready = 0;

            DataPacket pkt;
            /* Copy volatile packet under no-interrupt window */
            uint8_t sreg = SREG;
            cli();
            pkt = g_rx_packet;
            SREG = sreg;

            if (pkt.packet_type == PKT_CONTROL)
            {
                g_auto_state  = pkt.mode;
                g_speed_state = pkt.speed_mode;

                if (!g_auto_state)
                {
                    Motor m;
                    /* Reconstruct motor from packed joystick fields — decompress 20-bit layout */
                    uint32_t raw = (uint32_t)pkt.joystick_x | ((uint32_t)pkt.joystick_y << 16);
                    m.l_val = (raw >> 0)  & 0x1FF;
                    m.l_dir = (raw >> 9)  & 0x01;
                    m.r_val = (raw >> 10) & 0x1FF;
                    m.r_dir = (raw >> 19) & 0x01;

                    if (g_speed_state)
                    {
                        m.l_val /= 2;
                        m.r_val /= 2;
                    }

                    uint8_t s = SREG;
                    cli();
                    g_motor = m;
                    SREG = s;
                }

                g_cam_pan  = pkt.cam_pan;
                g_cam_tilt = pkt.cam_tilt;
            }
        }

        if (milliseconds_now() >= 100)
        {
            milliseconds_reset();

            if (g_auto_state)
            {
                Motor nav_motor = {0, 0, 0, 0};
                navigationStep(&nav_motor);
                uint8_t s = SREG;
                cli();
                g_motor = nav_motor;
                SREG = s;
            }

            Motor current_motor;
            uint8_t s = SREG;
            cli();
            current_motor = g_motor;
            SREG = s;

            motorDriverSet(current_motor);
            servoSetAngles(g_cam_pan, g_cam_tilt);
            ledUpdate(g_auto_state, g_speed_state);

            DataPacket telemetry;
            buildTelemetryPacket(&telemetry);
            wifiSendPacket(&telemetry);
        }
    }

    return 0;
}

ISR(USART3_RX_vect)
{
    uint8_t byte = UDR3;
    DataPacket pkt_out;

    /* wifiReceivePacket polls UDR3 internally — feed the byte through the framer */
    if (wifiReceivePacket(&pkt_out))
    {
        g_rx_packet = pkt_out;
        g_rx_ready  = 1;
    }
}

void ledInit(void)
{
    /* PA0=manual, PA1=auto, PA2=speed0, PA3=speed1, PA4=rx, PA5=tx */
    DDRA |= (1 << PA0) | (1 << PA1) | (1 << PA2) | (1 << PA3) | (1 << PA4) | (1 << PA5);
    PORTA = (1 << PA0) | (1 << PA1) | (1 << PA2) | (1 << PA3) | (1 << PA4) | (1 << PA5);
    _delay_ms(1000);
    PORTA = 0;
}

void ledUpdate(uint8_t auto_state, uint8_t speed_state)
{
    PORTA = (1 << (auto_state & 1)) | (1 << (2 + (speed_state & 1)));
}

void servoInit(void)
{
    /* Timer3 fast PWM mode 14, 50 Hz for servo control */
    TCCR3A = (1 << WGM31) | (1 << COM3A1) | (1 << COM3B1);
    TCCR3B = (1 << WGM33) | (1 << WGM32) | (1 << CS31);

    ICR3  = 39999; /* 50 Hz at 16 MHz with prescaler 8 */
    OCR3A = 3000;  /* 90 degrees centre */
    OCR3B = 3000;

    DDRE |= (1 << PE3) | (1 << PE4);
}

void servoSetAngles(uint8_t pan_deg, uint8_t tilt_deg)
{
    OCR3A = (uint16_t)(SERVO_PAN_MIN_OCR  + ((uint32_t)pan_deg  * (SERVO_PAN_MAX_OCR  - SERVO_PAN_MIN_OCR)  / 180));
    OCR3B = (uint16_t)(SERVO_TILT_MIN_OCR + ((uint32_t)tilt_deg * (SERVO_TILT_MAX_OCR - SERVO_TILT_MIN_OCR) / 180));
}

void buildTelemetryPacket(DataPacket *packet)
{
    packet->start_byte  = PKT_START_BYTE;
    packet->packet_type = PKT_TELEMETRY;
    packet->ir_short    = adc_read(ADC_IR_SHORT);
    packet->ir_long     = adc_read(ADC_IR_LONG);
    packet->battery_adc = adc_read(ADC_BATTERY);
    packet->joystick_x  = 0;
    packet->joystick_y  = 0;
    packet->cam_pan     = 0;
    packet->cam_tilt    = 0;
    packet->mode        = g_auto_state;
    packet->speed_mode  = g_speed_state;
    packet->checksum    = calculateChecksum((uint8_t *)packet, sizeof(DataPacket) - 1);
}