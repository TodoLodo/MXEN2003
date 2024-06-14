# System Architecture — MXEN2003

## Subsystem Overview

Two Arduino Mega WiFi boards communicate over the local WiFi network `MXEN_GRP5`. The ESP8266 co-processor on each board handles all WiFi/UDP I/O via AT commands over UART3. The ATmega2560 on each board handles all real-time control logic.

---

## Packet Wire Format (17 bytes)

```
Offset  Size   Field         Notes
──────  ────   ───────────   ────────────────────────────────────────
  0      1     start_byte    0xAA — framing marker
  1      1     packet_type   0x01 = PKT_CONTROL, 0x02 = PKT_TELEMETRY
  2      2     ir_short      Raw ADC from short-range IR sensor
  4      2     ir_long       Raw ADC from long-range IR sensor
  6      2     battery_adc   Raw ADC from battery voltage divider
  8      2     joystick_x    Motor data packed low 16 bits (see below)
 10      2     joystick_y    Motor data packed high 16 bits
 12      1     cam_pan       Pan angle 0–180°
 13      1     cam_tilt      Tilt angle 0–180°
 14      1     mode          0 = manual, 1 = autonomous
 15      1     speed_mode    0 = full speed, 1 = half speed
 16      1     checksum      XOR of bytes 0–15
```

### Motor Data Packing (20-bit field in joystick_x / joystick_y)

```
Bit 19   18..10   9       8..0
──────   ──────   ──────  ──────
r_dir    r_val    l_dir   l_val
  1       9-bit     1      9-bit
```

---

## Navigation FSM

```
            ┌─────────────────────────────┐
            │           FORWARD           │◄─────────────────┐
            │  l_dir=1 r_dir=1            │                  │
            └─────┬──────────┬────────────┘                  │
                  │          │                                │
       front<25cm │          │ left>18cm                     │
                  ▼          ▼                                │
          ┌────────────┐  ┌───────────┐              front>=25cm
          │ TURN_RIGHT │  │ TURN_LEFT │         left<=18cm   │
          │ l_dir=1    │  │ l_dir=0   │                      │
          │ r_dir=0    │  │ r_dir=1   │──────────────────────┘
          └─────┬──────┘  └───────────┘
                │
          fault / deadlock
                ▼
          ┌──────────┐
          │ REVERSE  │──────► TURN_RIGHT
          └──────────┘
```

---

## Interrupt Mapping — Controller

| Vector | Pin | Trigger | Action |
|---|---|---|---|
| INT0_vect | PD0 | Rising edge | Toggle `g_speed_state` |
| INT1_vect | PD1 | Rising edge | Reset camera to 90°/90° |
| INT2_vect | PD2 | Rising edge | Toggle `g_auto_state` |
| USART3_RX_vect | — | UART3 RX complete | Feed byte to `wifiReceivePacket()` |

## Interrupt Mapping — Robot

| Vector | Pin | Trigger | Action |
|---|---|---|---|
| USART3_RX_vect | — | UART3 RX complete | Feed byte to `wifiReceivePacket()` |

---

## Timer Usage

| Timer | Peripheral | Mode | Frequency |
|---|---|---|---|
| Timer1 | Motor PWM (OC1A/OC1B) | Fast PWM mode 14 | 500 Hz |
| Timer3 | Servo PWM (OC3A/OC3B) | Fast PWM mode 14 | 50 Hz |
| Timer5 | Milliseconds counter | CTC | 1 kHz |
