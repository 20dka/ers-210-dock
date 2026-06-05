#pragma once

#include <stdio.h>
#include "lcd.h"

void init_lcd_driver();

void send_packet(const packet_t* packet);

void send_command(lcd_command_t cmd, uint8_t data);
void send_data(uint8_t data);

void flush_display();
