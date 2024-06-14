/**
 * Controller.c
 * Aaron Fredrick — MXEN2003, June 2024
 */

#include "Controller.h"

static volatile uint8_t    g_cam_angles[2] = {90, 90};
static volatile uint8_t    g_auto_state    = 0;
static volatile uint8_t    g_speed_state   = 0;
static volatile DataPacket g_rx_packet;
static volatile uint8_t    g_rx_ready      = 0;

int main(void)
{
    milliseconds_init();
    adc_init();
    serial0_init();
    buttonInit();
    telemetryDisplayInit();
    wifiInit(WIFI_MODE_CONTROLLER);

    sei();

    while (1)
    {
        if (g_rx_ready)
        {
            g_rx_ready = 0;

            DataPacket pkt;
            uint8_t sreg = SREG;
            cli();
            pkt = g_rx_packet;
            SREG = sreg;

            if (pkt.packet_type == PKT_TELEMETRY)
            {
                uint8_t battery_pct  = (uint8_t)((uint32_t)pkt.battery_adc * 100 / 1023);
                float   dist_short   = calibrateShortRange1(pkt.ir_short);
                float   dist_long    = calibrateLongRange(pkt.ir_long);
                telemetryDisplayUpdate(battery_pct, dist_short, dist_long);
            }
        }

        if (milliseconds_now() >= 100)
        {
            milliseconds_reset();

            Motor motor;
            motorManagerComputeSpeeds(&motor, adc_read(ADC_JOY_MOVE_X), adc_read(ADC_JOY_MOVE_Y));
            motorManagerApplySpeedMode(&motor, g_speed_state);
            computeCameraAngles(g_cam_angles);

            DataPacket control;
            buildControlPacket(&control, motor, g_cam_angles);
            wifiSendPacket(&control);
        }
    }

    return 0;
}

/* Speed mode toggle on joystick button */
ISR(INT0_vect)
{
    g_speed_state = !g_speed_state;
}

/* Camera reset to centre on camera joystick button */
ISR(INT1_vect)
{
    g_cam_angles[0] = 90;
    g_cam_angles[1] = 90;
}

/* Autonomous / manual mode toggle on mode switch */
ISR(INT2_vect)
{
    g_auto_state = !g_auto_state;
}

ISR(USART3_RX_vect)
{
    uint8_t byte = UDR3;
    (void)byte;

    DataPacket pkt_out;
    if (wifiReceivePacket(&pkt_out))
    {
        g_rx_packet = pkt_out;
        g_rx_ready  = 1;
    }
}

void buttonInit(void)
{
    DDRD  &= ~((1 << PD0) | (1 << PD1) | (1 << PD2));

    /* Rising edge on INT0, INT1, INT2 */
    EICRA  = (1 << ISC01) | (1 << ISC00)
           | (1 << ISC11) | (1 << ISC10)
           | (1 << ISC21) | (1 << ISC20);

    EIMSK  = (1 << INT0) | (1 << INT1) | (1 << INT2);
}

void computeCameraAngles(volatile uint8_t cam_angles[2])
{
    /* Scale joystick deviation from centre to a max angular delta per cycle */
    float adc_center = 511.5f;
    float adc_range  = 511.5f;

    float dx = (adc_read(ADC_JOY_CAM_X) - adc_center) / adc_range;
    float dy = (adc_read(ADC_JOY_CAM_Y) - adc_center) / adc_range;

    int16_t pan_new  = (int16_t)cam_angles[0] + (int16_t)(dx * CAM_MAX_DELTA_DEG);
    int16_t tilt_new = (int16_t)cam_angles[1] + (int16_t)(dy * CAM_MAX_DELTA_DEG);

    if (pan_new  > 180) pan_new  = 180;
    if (pan_new  < 0)   pan_new  = 0;
    if (tilt_new > 180) tilt_new = 180;
    if (tilt_new < 0)   tilt_new = 0;

    cam_angles[0] = (uint8_t)pan_new;
    cam_angles[1] = (uint8_t)tilt_new;
}

void buildControlPacket(DataPacket *packet, Motor motor, volatile uint8_t cam_angles[2])
{
    /* Pack motor data into joystick fields — mirrors decompression in Robot.c */
    uint32_t motor_raw = ((uint32_t)motor.r_dir << 19)
                       | ((uint32_t)motor.r_val << 10)
                       | ((uint32_t)motor.l_dir << 9)
                       | ((uint32_t)motor.l_val << 0);

    packet->start_byte  = PKT_START_BYTE;
    packet->packet_type = PKT_CONTROL;
    packet->ir_short    = 0;
    packet->ir_long     = 0;
    packet->battery_adc = 0;
    packet->joystick_x  = (uint16_t)(motor_raw & 0xFFFF);
    packet->joystick_y  = (uint16_t)(motor_raw >> 16);
    packet->cam_pan     = cam_angles[0];
    packet->cam_tilt    = cam_angles[1];
    packet->mode        = g_auto_state;
    packet->speed_mode  = g_speed_state;
    packet->checksum    = calculateChecksum((uint8_t *)packet, sizeof(DataPacket) - 1);
}
