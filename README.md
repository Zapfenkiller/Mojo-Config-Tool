# Mojo Operating System ("Mojo OS")

Alternative to the genuine Mojo software made by Alchitry:

* Directly handles Xilinx bitstream files (.bit).
* Uses parallel interface to the FPGA, allowing for higher transfer speed.
* Introduces as CDC to the USB ("fake modem").
* Control via any terminal program (e. g. Hyperterminal or TeraTerm) on the
  USB host side.

Demo bitstream blinks LED2 once each second if used to configure the Spartan.
Both, native as well as compressed, kinds of bitstreams are processed.

Setting COM to 1200 baud resets the ATmega into its Arduino compliant
Caterina-bootloader. The Mojo OS can get replaced by whatever you like to have.

Setting COM to 2400 baud resets the FPGA and ATmega back into the configuration
mode. In this mode the FPGA is **not** automatically configured from the
SPI-FLASH content. This allows for user operations like changing an already
FLASHed design.

While the application interface is running a "#R" sequence does
the same, just for some convenience.

In case there is no valid bitstream stored to the serial FLASH a simple command
line awaits the user. Otherwise the bistream is configuring the FPGA and the
application function takes over communication.