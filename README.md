# MXEN2003 — Microcontroller Project

**Author:** Aaron Fredrick  
**Unit:** MXEN2003 — Microcontroller Project  
**Curtin University | June 2024**

---

## Overview

A two-unit wireless robot system built on the **Arduino Mega WiFi (ATmega2560 + ESP8266)**. A handheld controller streams joystick and mode commands over WiFi UDP. The robot responds with live sensor telemetry. The robot autonomously navigates a maze using IR sensors and a left-wall-following algorithm when switched into autonomous mode.

---

## System Architecture

```
┌─────────────────────────────────────────────────────────┐
│                     CONTROLLER                          │
│  ATmega2560                                             │
│  ┌──────────────┐  ┌────────────┐  ┌─────────────────┐ │
│  │ Move Joystick│  │ Cam Joy    │  │ LCD (HD44780)   │ │
│  │ ADC0 / ADC1  │  │ ADC2/ADC3  │  │ Telemetry Read  │ │
│  └──────┬───────┘  └─────┬──────┘  └────────▲────────┘ │
│         │                │                   │          │
│  ┌──────▼───────────────▼───────────────────┴────────┐ │
│  │              ATmega2560 Main Loop (100 ms)          │ │
│  │  motor_manager → DataPacket (PKT_CONTROL) → UART3  │ │
│  └──────────────────────────────────────────────────┬─┘ │
│  INT0=Speed  INT1=CamReset  INT2=Mode              │    │
│                                                    ▼    │
│                                              ESP8266     │
└────────────────────────────────────────────────┬────────┘
                                                 │ WiFi UDP
                                                 ▼
┌────────────────────────────────────────────────┴────────┐
│                      ROBOT                              │
│                                              ESP8266     │
│  ┌──────────────────────────────────────────┬─────────┐ │
│  │              ATmega2560 Main Loop (100 ms)│ UART3   │ │
│  │  DataPacket (PKT_TELEMETRY) ◄─────────── │         │ │
│  └─────────┬──────────┬──────────┬──────────┘         │ │
│            │          │          │                     │ │
│  ┌─────────▼──┐ ┌─────▼──┐ ┌───▼──────────────┐      │ │
│  │ L298 Motor │ │ Servos │ │ IR Sensors + ADC │      │ │
│  │ Driver     │ │ Timer3 │ │ Battery Monitor  │      │ │
│  └────────────┘ └────────┘ └──────────────────┘      │ │
└─────────────────────────────────────────────────────────┘
```

---

## Hardware Components

### Robot
| Component | Description |
|---|---|
| Arduino Mega WiFi | ATmega2560 + ESP8266 co-processor |
| L298 Motor Driver | Dual H-bridge, PWM speed + direction |
| DC Motors × 2 | Left and right drive wheels |
| Sharp GP2Y0A21 | Short-range IR, 10–80 cm |
| Sharp GP2Y0A02 | Long-range IR, 20–150 cm |
| Pan-Tilt Servos × 2 | 180° rotation, camera mount |
| LiPo Battery | Voltage monitored via ADC divider |

### Controller
| Component | Description |
|---|---|
| Arduino Mega WiFi | ATmega2560 + ESP8266 co-processor |
| Joystick × 2 | Movement (X/Y) and Camera (Pan/Tilt) |
| HD44780 LCD | 16×2 telemetry display |
| Push Button × 3 | Speed mode, camera reset, mode toggle |
| Mode Switch | Manual / Autonomous selector |

---

## Pin Configuration

### Robot (ATmega2560)
| Signal | Pin | Notes |
|---|---|---|
| Motor Left PWM | PB5 (OC1A) | Timer1, 500 Hz |
| Motor Right PWM | PB6 (OC1B) | Timer1, 500 Hz |
| L298 IN1 (L fwd) | PL2 | Left motor direction |
| L298 IN2 (L rev) | PL3 | Left motor direction |
| L298 IN3 (R fwd) | PL4 | Right motor direction |
| L298 IN4 (R rev) | PL5 | Right motor direction |
| Servo Pan | PE3 (OC3A) | Timer3, 50 Hz |
| Servo Tilt | PE4 (OC3B) | Timer3, 50 Hz |
| IR Short (ADC) | ADC0 | GP2Y0A21 |
| IR Long (ADC) | ADC1 | GP2Y0A02 |
| Battery (ADC) | ADC2 | Voltage divider |
| WiFi (TX3 / RX3) | PJ1 / PJ0 | UART3 → ESP8266 |
| Status LEDs | PA0–PA5 | Mode, speed, RX/TX |

### Controller (ATmega2560)
| Signal | Pin | Notes |
|---|---|---|
| Move Joystick X | ADC0 | |
| Move Joystick Y | ADC1 | |
| Camera Joystick X | ADC2 | |
| Camera Joystick Y | ADC3 | |
| Speed Button | PD0 (INT0) | Rising edge |
| Camera Reset | PD1 (INT1) | Rising edge |
| Mode Toggle | PD2 (INT2) | Rising edge |
| WiFi (TX3 / RX3) | PJ1 / PJ0 | UART3 → ESP8266 |

---

## Communication Protocol

Both subsystems exchange `DataPacket` structs over **WiFi UDP** via the on-board ESP8266 (UART3, 9600 baud). The struct is declared `__attribute__((packed))` and cast directly to `uint8_t*` for transmission.

```
 Byte  Field          Direction
 ────  ─────────────  ──────────────────────
  0    start_byte     0xAA
  1    packet_type    0x01=Control, 0x02=Telemetry
  2-3  ir_short       Robot→Controller (raw ADC)
  4-5  ir_long        Robot→Controller (raw ADC)
  6-7  battery_adc    Robot→Controller (raw ADC)
  8-9  joystick_x     Controller→Robot (motor packed low 16)
 10-11 joystick_y     Controller→Robot (motor packed high 16)
 12    cam_pan        Controller→Robot (0–180°)
 13    cam_tilt       Controller→Robot (0–180°)
 14    mode           Both directions (0=manual, 1=auto)
 15    speed_mode     Both directions (0=full, 1=half)
 16    checksum       XOR of bytes 0–15
```

### WiFi Setup
- SSID: `MXEN_GRP5`
- Protocol: UDP
- Robot IP: `192.168.4.2` — port `5000`
- Controller IP: `192.168.4.1` — port `5001`
- Both boards join the same AP on startup via AT commands.

---

## Build & Upload

### Prerequisites
- [PlatformIO IDE](https://platformio.org/) or PlatformIO CLI
- AVR toolchain (installed automatically by PlatformIO)

### Commands
```bash
# Build robot firmware
pio run -e Robot_atmega

# Build controller firmware
pio run -e Controller_atmega

# Upload (set upload_port in platformio.ini first)
pio run -e Robot_atmega --target upload
pio run -e Controller_atmega --target upload
```

---

## Usage

1. Power both boards. The robot LED bank flashes all on for 1 second then goes dark.
2. Both ESP8266 modules connect to `MXEN_GRP5` on startup (~7 s).
3. Push the **mode switch** on the controller to enter **Manual** mode.
4. Use the **Move joystick** to drive; push **Speed button** to toggle half-speed.
5. Use the **Camera joystick** to pan/tilt; push it to **reset to 90°**.
6. Push the **mode switch** again to enter **Autonomous** mode.
7. LCD displays live battery %, short-range, and long-range IR distances.

---

## Sensor Calibration

| Sensor | Model | Range | Equation |
|---|---|---|---|
| Short-range (model 1) | Sharp GP2Y0A21 | 10–80 cm | `4800 / (adc - 20)` |
| Short-range (model 2) | Alternate | 10–80 cm | `4500 / (adc - 20)` |
| Long-range | Sharp GP2Y0A02 | 20–150 cm | `9462 / (adc - 16.92)` |

All outputs are clamped to the valid physical range to prevent runaway values on sensor fault.

---

## Autonomous Navigation Algorithm

The robot uses a **left-wall-following** finite state machine:

| State | Condition | Action |
|---|---|---|
| `FORWARD` | Left wall detected, front clear | Drive straight |
| `TURN_RIGHT` | Front wall < 25 cm | Rotate right until clear |
| `TURN_LEFT` | Left wall lost (> 18 cm) | Rotate left until wall regained |
| `REVERSE` | Fallback from deadlock | Reverse then transition to TURN_RIGHT |

The FSM is driven by `navigationStep()` called every 100 ms from the robot main loop.

---

## License

MIT License — see [LICENSE](LICENSE).

---

## Author

**Aaron Fredrick**  
MXEN2003 — Microcontroller Project  
Curtin University, June 2024
