/**
 * telemetry_display.c
 * Aaron Fredrick — MXEN2003, June 2024
 */

#include "telemetry_display.h"
#include "hd44780.h"
#include <stdio.h>

void telemetryDisplayInit(void)
{
    lcd_init(LCD_DISP_ON);
    lcd_clrscr();
}

void telemetryDisplayUpdate(uint8_t battery_pct, float dist_short_cm, float dist_long_cm)
{
    char line[17];

    lcd_clrscr();

    snprintf(line, sizeof(line), "BAT:%3u%% S:%4.1fcm", battery_pct, dist_short_cm);
    lcd_gotoxy(0, 0);
    lcd_puts(line);

    snprintf(line, sizeof(line), "LONG: %6.1f cm", dist_long_cm);
    lcd_gotoxy(0, 1);
    lcd_puts(line);
}
