#ifndef CONFIGURATION_H
#define CONFIGURATION_H

//example keys to encrypt internal storage
#define ENCRYPTION_KEY { 0x7b, 0x9f, 0x6d, 0x8d, 0xb8, 0xc3, 0x9f, 0xce, 0xc2, 0x4c, 0xd8, 0x94, 0x7e, 0xad, 0x75, 0x5c, 0xde, 0x34, 0xf3, 0x95, 0xea, 0xda, 0x03, 0xf0, 0x2d, 0xd5, 0x50, 0x8a, 0x17, 0xba, 0xaf, 0x0e }
#define ENCRYPTION_TWEAK { 0x4d, 0xa1, 0x15, 0x09, 0xd1, 0x12, 0x01, 0xae, 0x58, 0xb1, 0x68, 0x0e, 0x1b, 0xd3, 0x55, 0x35 }

#define SERIAL_PORT Serial
#define SERIAL_PORT_SPEED 38400
#define SERIAL_RX_PIN 0
#define RTC_POWER_PIN 4
#define LED_SYNC LED_BUILTIN

#define DEFAULT_IDLE_TIMEOUT 10000
#define DEFAULT_CODE_TIMEOUT 30000

#define BUTTON_NEXT_PIN 2
#define BUTTON_SELECT_PIN 3
#define BUTTON_DELAY_MS 100

#define DISPLAY_ADDR 0x78
#define DISPLAY_POWER_PIN 7

#define MAIN_SCREEN_DATE_FONT u8g2_font_logisoso16_tr
#define MAIN_SCREEN_TIME_FONT u8g2_font_logisoso38_tn

#define MAIN_SCREEN_DATE_MIN_POS_X 0
#define MAIN_SCREEN_DATE_MAX_POS_X 8
#define MAIN_SCREEN_DATE_MIN_POS_Y 16
#define MAIN_SCREEN_DATE_MAX_POS_Y 22

#define MAIN_SCREEN_TIME_MIN_POS_X 0
#define MAIN_SCREEN_TIME_MAX_POS_X 8
#define MAIN_SCREEN_TIME_MIN_POS_Y 60
#define MAIN_SCREEN_TIME_MAX_POS_Y 64

#define MAIN_SCREEN_DAYS_OF_THE_WEEK {"mon","tue","wed","thu","fri","sat","sun"}
#define MAIN_SCREEN_DAY_LEN 4

#define DEBUG_SERIAL_PORT SERIAL_PORT

#endif
