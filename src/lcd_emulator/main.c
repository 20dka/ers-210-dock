#include "ch32fun.h"
#include "ch32v003_GPIO_branchless.h"

#include <stdio.h>

#include "lcd_emulator.h"

#define PIN_DEBUG PC4

#define BLIP() ({ \
	funDigitalWrite( PIN_DEBUG, FUN_HIGH ); \
	funDigitalWrite( PIN_DEBUG, FUN_LOW ); \
})

#define BIT_CD  (1<<1)
#define BIT_CS  (1<<2)
#define BIT_SDA (1<<5)
#define BIT_SCK (1<<7)

#define CHIPSELECT_HIGH(port) (port & BIT_CS)
#define CHIPSELECT_LOW(port) (!(port & BIT_CS))

#define CLOCK_HIGH(port) (port & BIT_SCK)
#define CLOCK_LOW(port) (!(port & BIT_SCK))

#define PORT_CHAR(p, b) ((port & (1<<b)) ? '1' : '0')

#define HALF_MS_TIME (DELAY_MS_TIME/2)
#define TIMEOUT_FUNC(start, active) if (active && TimeElapsed32(funSysTick32(), start) > DELAY_MS_TIME) return 1;

void printport(uint32_t port) {
	printf("%c %c %c %c %c %c %c %c\n", PORT_CHAR(p, 7),PORT_CHAR(p, 6),PORT_CHAR(p, 5),PORT_CHAR(p, 4),PORT_CHAR(p, 3),PORT_CHAR(p, 2),PORT_CHAR(p, 1),PORT_CHAR(p, 0));
}

int wait_for_clock_rise() {
	uint8_t bitmask = BIT_SCK;
	uint32_t port = GPIO_port_digitalRead(GPIO_port_C);
	uint32_t start = funSysTick32();

	while((port & bitmask)) {
		Delay_Tiny(1);
		port = GPIO_port_digitalRead(GPIO_port_C);
		TIMEOUT_FUNC(start, 1);
	}

	while(!(port & bitmask)) {
		Delay_Tiny(1);
		port = GPIO_port_digitalRead(GPIO_port_C);
		TIMEOUT_FUNC(start, 1);
	}

	return 0;
}

int wait_for_chipselect_fall() {
	uint8_t bitmask = BIT_CS;
	uint32_t port = GPIO_port_digitalRead(GPIO_port_C);
	uint32_t start = funSysTick32();

	while(!(port & bitmask)) {
		Delay_Tiny(1);
		port = GPIO_port_digitalRead(GPIO_port_C);
		TIMEOUT_FUNC(start, 1);
	}

	while((port & bitmask)) {
		Delay_Tiny(1);
		port = GPIO_port_digitalRead(GPIO_port_C);
		TIMEOUT_FUNC(start, 1);
	}

	return 0;
}

int get_packet(packet_t* packet) {
	memset(packet, 0, sizeof(packet_t));

	uint32_t port = GPIO_port_digitalRead(GPIO_port_C);
	uint8_t bit_count = 0;

	if (wait_for_chipselect_fall()) return 2;
	// packet START

	uint32_t time_since_last_bit = 0;
	uint32_t last_bit = 0;

	while (bit_count < 8 && time_since_last_bit < HALF_MS_TIME) {
		if (wait_for_clock_rise()) return 3;
		port = GPIO_port_digitalRead(GPIO_port_C);
		//data valid

		uint32_t now = funSysTick32();
		packet->data <<= 1;
		packet->data |= !!(port & BIT_SDA);
		bit_count++;


		if (last_bit > 0)
			time_since_last_bit = TimeElapsed32(now,  last_bit);

		last_bit = now;
	}

	if (time_since_last_bit >= HALF_MS_TIME) {
		printf("%d\n", time_since_last_bit);
		return 1;
	}

	packet->is_command = (port & BIT_CD);
	packet->valid = 1;

	return 0;
}

void process_packet(const packet_t* packet) {
	if (packet->is_command) {
		command_t cmd = {.command = (packet->data) >> 5, .data = (packet->data &0b111)};
		lcd_handle_command(&cmd);
	} else {
		// we assume 8-bit mode
		lcd_write_nibble(packet->data >> 4);
		lcd_write_nibble(packet->data & 0b1111);
	}
}

void print_packet(const packet_t* packet) {
	if (packet->valid) {
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
	} else {
		printf("ERR\n");
	}
}

void print_packet_verbose(packet_t packet) {
	if (packet.valid) {
		printf("%s 0x%X\n", packet.is_command ? "Command" : "Data", packet.data);
	} else {
		printf("ERR\n");
	}
}

int main() {
	SystemInit();

	GPIO_port_enable(GPIO_port_C);
	GPIO_port_pinMode(GPIO_port_C, GPIO_pinMode_I_floating, GPIO_Speed_In);
	funPinMode( PIN_DEBUG, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP );


BLIP();

	Delay_Ms(50);

	printf("ERS-210 dock LCD controller.\r\nInitialized GPIO\n");
	printf("%d", sizeof(command_t));

	init_lcd_emulator();

	size_t max = 12 * 7;
	packet_t buf[max];
	size_t idx = 0;

	uint8_t packets_since = 0;

	while (1) {
		int ret = get_packet(&buf[idx]);
BLIP();
		switch (ret)
		{
		case 0: {
			packets_since++;
BLIP();
			process_packet(&buf[idx]);
			idx++;
			break;
			}
		case 2:
		case 3: {
			if (packets_since > 10) {
				funDigitalWrite( PIN_DEBUG, FUN_HIGH );
				//print_lcd_buffer();
				printf("\t\t\t\t\t\t\t%d\n", packets_since);

				for (size_t i = 0; i < packets_since; i++)
				{
					printf("%3d: ", i+1);
					print_packet(&buf[i]);
				}

				idx = 0;
				packets_since = 0;

				funDigitalWrite( PIN_DEBUG, FUN_LOW );
			}
			break;
			}
		default:
			printf("packet error\n");
			break;
		}

		if (idx == max) {

			idx = 0;
		}
	}
}
