# Spartan Configurator

Alternative to the genuine Mojo software:

* Directly handles Xilinx bitstream files (.bit).
* Uses different interface to FPGA and USB-Host.
* Control via any terminal program (e. g. Hyperterminal or TeraTerm).

Demo bitstream blinks LED2 once each second if used to configure the Spartan.

Setting COM to 1200 baud resets the ATmega into its Arduino compliant
Caterina-bootloader.

Setting COM to 2400 baud resets the FPGA and ATmega back into the configuration
mode.
