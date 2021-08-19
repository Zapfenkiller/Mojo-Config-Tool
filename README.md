# Spartan Configurator

Alternative to the genuine Mojo software:

* Directly handles Xilinx bitstream files (.bit).
* Uses parallel interface to the FPGA, allowing for higher transfer speed.
* Control via any terminal program (e. g. Hyperterminal or TeraTerm).

Demo bitstream blinks LED2 once each second if used to configure the Spartan.
Both, native as well as compressed, kinds of bitstreams are processed.

Setting COM to 1200 baud resets the ATmega into its Arduino compliant
Caterina-bootloader.

Setting COM to 2400 baud resets the FPGA and ATmega back into the configuration
mode. In this mode the FPGA is **not** automatically configured from the
SPI-FLASH content. This allows for user operations like changing an already
FLASHed design. While the application interface is running a "#R" sequence does
the same, just for some convenience.
