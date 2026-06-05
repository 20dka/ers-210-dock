all : flash

TARGET:=main

ADDITIONAL_C_FILES+=lcd_emulator.c

EXTRA_CFLAGS+=-Wextra -I..

TARGET_MCU=CH32V003

include ../ch32fun/ch32fun/ch32fun.mk

flash : cv_flash
clean : cv_clean
