#include "ch32fun.h"

#include "lcd_driver.h"


#define PIN_DEBUG PC4

#define BLIP() ({ \
	funDigitalWrite( PIN_DEBUG, FUN_HIGH ); \
	funDigitalWrite( PIN_DEBUG, FUN_LOW ); \
})


uint8_t buffer[DDRAM_LEN];
uint8_t buffer_address;
display_state_t display_state;

void init_lcd_driver() {
    display_state = DISPLAY_STATE_OFF;
    memset(buffer, 0, DDRAM_LEN);
    buffer_address = 0;
}

void lcd_handle_command(const command_t* cmd) {
BLIP();
    if (cmd->command != 0) {
BLIP();
BLIP();
        printf("command: %x data: %x\n", cmd->command, cmd->data);
    }

	switch (cmd->command)
    {
    case LCD_COMMAND_ADDRESS_SET: {
        buffer_address = cmd->data;
        //BLIP();
        break;
    }
    case LCD_COMMAND_DISPLAY_ON: {
        display_state = DISPLAY_STATE_ON;
        break;
    }
    case LCD_COMMAND_DISPLAY_OFF: {
        display_state = DISPLAY_STATE_OFF;
        break;
    }
    case LCD_COMMAND_DISPLAY_START: {
        display_state = DISPLAY_STATE_NORMAL;
        break;
    }
    case LCD_COMMAND_REWRITE_DDRAM: {
        lcd_write_nibble(cmd->data & 0b1111);
        break;
    }
    case LCD_COMMAND_RESET: {
        init_lcd_driver();
        break;
    }
    default:
        printf("Unknown command: 0x%X\n", cmd->command);
        break;
    }
}

void lcd_write_nibble(uint8_t nibble) {
    buffer[buffer_address++] = nibble;
    if (buffer_address >= DDRAM_LEN) {
        printf("\t\t\t\tbuffer overrun\n");
        buffer_address = 0;
    }
}

void print_lcd_buffer() {
    //printf("DDRAM contents:\n ");
    printf(" ");
    for (size_t i = 0; i < DDRAM_LEN; i++) printf(".");
    printf("\n");
    for (size_t i = 0; i < 4; i++)
    {
        printf("%d\t", i);
        for (size_t f = 0; f < DDRAM_LEN; f++)
        {
            
            printf("%c", !!(buffer[f] & (1<<i)) ? 'X' : ' ');
        }
        printf("\n");
        
    }
    
}