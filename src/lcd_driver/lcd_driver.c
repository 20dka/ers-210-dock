#include "ch32fun.h"

#include "lcd_driver.h"


#define PIN_DEBUG PC4

#define PIN_CD PC1
#define PIN_CS PC2
#define PIN_SDA PC5
#define PIN_SCK PC7



#define BLIP() ({ \
	funDigitalWrite( PIN_DEBUG, FUN_HIGH ); \
	funDigitalWrite( PIN_DEBUG, FUN_LOW ); \
})

#define CD_LOW() { funDigitalWrite( PIN_CD, FUN_LOW ); }
#define CD_HIGH() { funDigitalWrite( PIN_CD, FUN_HIGH ); }

#define CS_LOW() { funDigitalWrite( PIN_CS, FUN_LOW ); }
#define CS_HIGH() { funDigitalWrite( PIN_CS, FUN_HIGH ); }

#define SDA_LOW() { funDigitalWrite( PIN_SDA, FUN_LOW ); }
#define SDA_HIGH() { funDigitalWrite( PIN_SDA, FUN_HIGH ); }

#define SCK_LOW() { funDigitalWrite( PIN_SCK, FUN_LOW ); }
#define SCK_HIGH() { funDigitalWrite( PIN_SCK, FUN_HIGH ); }

uint8_t buffer[DDRAM_LEN];
uint8_t buffer_address;
display_state_t display_state;

void init_lcd_driver() {
	funPinMode(PIN_CD, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP);
	funPinMode(PIN_CS, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP);
	funPinMode(PIN_SDA, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP);
	funPinMode(PIN_SCK, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP);


    send_command(LCD_COMMAND_RESET, 0);

flush_display();

    Delay_Ms(100);

    send_command(LCD_COMMAND_DISPLAY_START, 0);

    Delay_Ms(100);
}


void send_packet(const packet_t* packet) {
    CS_LOW();
    funDigitalWrite( PIN_CD, packet->is_command );

    uint8_t data = packet->data;

    for (size_t i = 0; i < 8; i++)
    {
        SCK_LOW();

    Delay_Us(4);

        funDigitalWrite( PIN_SDA, !!(data & (1<<7)) );
        data <<= 1;

    Delay_Us(5);

        SCK_HIGH();
    
    Delay_Us(4);
    }

    CS_HIGH();
    Delay_Us(50);
}

void send_command(lcd_command_t cmd, uint8_t data) {
    packet_t packet = { .is_command = 1 };
    packet.data = ((cmd & 0b111) << 5) | (data & 0b11111);

    send_packet(&packet);
}

void send_data(uint8_t data) {
    packet_t packet = { .is_command = 0 };
    packet.data = data;

    send_packet(&packet);
}

void flush_display() {  
    send_command(LCD_COMMAND_ADDRESS_SET, 0);

    for (size_t i = 0; i < 16; i++)
    {
        send_data(0);
    }
}