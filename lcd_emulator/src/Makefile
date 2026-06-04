all : flash

TARGET:=main

ADDITIONAL_C_FILES+=lcd_driver.c

EXTRA_CFLAGS+=-Wextra

TARGET_MCU=CH32V003

include ../ch32fun/ch32fun/ch32fun.mk

flash : cv_flash
clean : cv_clean
