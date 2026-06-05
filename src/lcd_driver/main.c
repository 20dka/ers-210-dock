#include "ch32fun.h"
#include "ch32v003_GPIO_branchless.h"

#include <stdio.h>

#include "lcd_driver.h"

#define PIN_DEBUG PC4

#define BLIP() ({ \
	funDigitalWrite( PIN_DEBUG, FUN_HIGH ); \
	funDigitalWrite( PIN_DEBUG, FUN_LOW ); \
})



void print_packet(const packet_t* packet) {
	printf("%c ", packet->is_command ? 'C' : 'D');

	if (packet->is_command) {
		command_t cmd = {.command = (packet->data) >> 5, .data = (packet->data &0b111)};

		printf("%x %x", cmd.command, cmd.data);
	} else {
		printf("%x", packet->data);
		if (packet->data) {
			uint8_t v = packet->data;
			uint8_t c;
			for (c = 0; v; c++) v &= v-1;

			printf("\t(%d)", c);
		}
	}

	printf("\n");
}

int main() {
	SystemInit();

	GPIO_port_enable(GPIO_port_C);
	funPinMode(PIN_DEBUG, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP);


	//BLIP();

	Delay_Ms(50);

	printf("ERS-210 dock LCD controller.\r\nInitialized GPIO\n");

	init_lcd_driver();

	while (1) {
		for(int i = 0;i<32;i++) {
			uint8_t nibble = 1;
			for (size_t f = 0; f < 4; f++)
			{
				send_command(LCD_COMMAND_ADDRESS_SET, i);
				send_command(LCD_COMMAND_REWRITE_DDRAM, nibble);
				nibble<<=1;
				nibble|=1;
				//Delay_Ms(100);
			}
			
			
			//Delay_Ms(300);
			Delay_Ms(30);
		}

		Delay_Ms(500);

		for(int i = 31;i>=0;i--) {
			uint8_t nibble = 0b1111;
			for (size_t f = 0; f < 5; f++)
			{
				send_command(LCD_COMMAND_ADDRESS_SET, i);
				send_command(LCD_COMMAND_REWRITE_DDRAM, nibble);
				nibble>>=1;
				//Delay_Ms(100);
			}
			
			
			//Delay_Ms(300);
			Delay_Ms(30);
		}

		send_command(LCD_COMMAND_ADDRESS_SET, 0);

		for (size_t i = 0; i < 16; i++)
		{
			send_data(0);
		}

		Delay_Ms(1000);
	}
}
void blinky(){ {
		send_command(LCD_COMMAND_ADDRESS_SET, 0);
		
		send_data(0);
		send_data(0);
		send_data(0);
		send_data(0);
		send_data(0);
		send_data(0);

		Delay_Ms(250);
		
		send_command(LCD_COMMAND_ADDRESS_SET, 0);

		send_data(0);
		send_data(0x08);
		
		Delay_Ms(250);
		send_data(0);
		send_data(0x08);

		Delay_Ms(250);
		send_data(0x80);
		send_data(0);

		Delay_Ms(250);
	}
}
