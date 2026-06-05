#pragma once

#include <stdio.h>

#include "lcd.h"

extern uint8_t buffer[];
extern uint8_t buffer_address;
extern display_state_t display_state;

void init_lcd_emulator();

void lcd_handle_command(const command_t* cmd);

void lcd_write_nibble(uint8_t nibble);

void print_lcd_buffer();
