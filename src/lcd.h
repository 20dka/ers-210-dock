#pragma once

#include <stdio.h>

#define LCD_COMMAND_TYPE_MASK (0b11100000)
#define LCD_COMMAND_DATA_MASK (0b00011111)

typedef enum lcd_command_t {
	LCD_COMMAND_ADDRESS_SET = 0b000,
	LCD_COMMAND_DISPLAY_ON = 0b001,
	LCD_COMMAND_DISPLAY_OFF = 0b010,
	LCD_COMMAND_DISPLAY_START = 0b011,
	LCD_COMMAND_REWRITE_DDRAM = 0b100,
	LCD_COMMAND_RESET = 0b110,
} lcd_command_t;

typedef enum display_state_t {
	DISPLAY_STATE_OFF,
	DISPLAY_STATE_ON,
	DISPLAY_STATE_NORMAL
} display_state_t;

typedef struct command_t {
    lcd_command_t command : 3;
    uint8_t data : 5;
} command_t;

typedef struct packet_t {
	uint8_t is_command;
	uint8_t valid;
	uint8_t data;
} packet_t;


#define DDRAM_LEN (32)
