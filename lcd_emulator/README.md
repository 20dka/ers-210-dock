# lcd_emulator

This folder contains source code for a CH32Vxxx microcontroller to sniff packets between the main and LCD PCBs. It is not very pretty. It could easily be adapted to other targets, like an Arduino Uno

To use, pull the required submodule ([ch32fun SDK](https://github.com/cnlohr/ch32fun)), follow its getting started guide, and finally call `make all monitor`.

## Basic overview

The LCD uses a BU9728A series LCD Driver, and the dock talks to it using its serial protocol described in the datasheet `doc/BU9728AKV - lcd driver.pdf`.

After gathering enough packets (timeout of 1ms), the current RAM state is printed to the terminal, along with the # of packets since the last readout.
