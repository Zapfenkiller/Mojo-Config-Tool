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


## Demo Bitstreams

There are 3 files provided for some "hands on" experience. Those can get readily
used for volatile as well as non-volatile configuration of the FPGA.

1. LED2_1Hz.bit
2. LED2_1Hz_c.bit
3. UCIF_Demo.bit


### LED2_1Hz.bit

This is a regular (expressly non-compressed) Xilinx bitstream representation of
a rather simple blink example. It just flashes LED2 once a second with 50 % of
duty cycle. That's it, nothing else.


### LED2_1Hz_c.bit

This is a compressed Xilinx bitstream representation of the exact same logic
flashing LED2 once a second. Its sole usage is to demonstrate the Mojo OS also
can handle compressed bitstreams.


### UCIF_Demo.bit

The UCIF demo is a more complex logic that needs a host control to send command
packets to the Mojo - after the FPGA is configured with this logic.

The command packets control a set of registers inside the FPGA logic. Those
registers in turn control brightness and flashing of all on-board LEDs. One
simple packet sent once could unleash some impressive light show.

The packet format is rather simple but powerful in terms of transfer speed.
Every packet consists of a header word and up to 255 data words thereafter.
Each word consists of two bytes. The structure is as follows:  
`<ID> <Length> [<byte> <byte> <byte> <byte]`  
The `<Length>` parameter covers the range from 0 up to 255 and counts words (not
bytes).

1. SDR-WR  
   <ID> = 0x77 = 'w'  
   SDR stands for Single Data Rate and the WR for WRite. SDR here means that
   every word encodes a register address plus the data byte written to this
   register. It is perfect for random writes accessing one or more registers.
   A complete set of up to 255 accesses can get done with one packet of 256
   words (512 bytes) in length. The exact meaning depends on the UCIF variant
   inside the FPGA logic. The `DDR` line is held at '0' when this kind of packet
   is received.  
   Example (writing 2 words = 4 bytes):  
   `'w' 2 0x00 0x03 0x02 0x77`  
   Which means write 0x03 into register 0x00 and 0x77 into register 0x02.
2. DDR-WR  
   <ID> = 0x57 = 'W'  
   DDR stands for Double Data Rate and the WR for WRite. DDR here means that
   every word encodes just two bytes of data. It is perfect for continous writes
   to a certain register (e. g. a write buffer entry) or to a file of subsequent
   register addresses. The exact usage depends on the FPGA logic implementing
   the UCIF variant.
   A complete set of up to 255 words (510 bytes) can get done with one packet of
   256 words (512 bytes) in length. The `DDR` line is held at '1' when this kind
   of packet is received.  
   Example (writing 3 words = 6 bytes):  
   `'w' 3 0x03 0x77 0xA0 0x00 0xFF 0x42`
3. SDR-RD  
   <ID> = 0x72 = 'r'  
   The SDR you already know about. The RD, you guessed it right, is for ReaD.
   The packet content differ in that the `Length` counts bytes (!) and each byte
   send is interpreted as an register address to read one byte of data from.
   It is perfect when it comes to random sequence read outs.
   A complete set of up to 255 accesses can get done with one packet of 256
   bytes in length.
   The exact meaning depends on the UCIF variant inside the FPGA logic.
   The `DDR` line is held at '0' when this kind of packet is received.  
   Example (reading 2 bytes):  
   'r' 2 0x03 0x04  
   The Mojos responds to this packet by sending back a similar answer. The
   payload carries the exact same amount of bytes from the requested address(es).
   Example (Mojo answer):  
   'r' 2 0x47 0xBD
4. DDR-RD  
   <ID> = 0x52 = 'R'  
   This is the Double Data Read request. This is perfect to catch a sequence of
   words as fast as possible. The exact source is determined by the UCIF logic
   implementation.
   A complete set of up to 255 words (510 bytes) can get done with one packet of
   256 words (512 bytes) in length. The `DDR` line is held at '1' when this kind
   of packet is received.
   As a very special property this packet doe not contain any payload even if
   the `Length` field tells else. The `Length` field is maesuring words again.
   Example (reading 4 words):  
   'R' 2  
   The Mojos responds to this packet by sending back its answer. The payload
   carries the exact same amount of words from the requested address(es).
   Example (Mojo answer):  
   'R' 2 0x47 0xBD 0x59 0x88
5. Reset FPGA Request  
   <ID> = 0x23 = '#'  
   <LENGTH> = 0x52 = 'R'
   When the Mojo receives exact packet it leaves the UCIF mode and awaits the
   user control at the command line.

Now that it is obvious what to send to the Mojo and expect back from it the
demonstration logic needs some documentation. A set of 32 registers controls the
8 LEDS on the Mojo. Each LED takes 4 register addresses. Well, one bit per LED
and just one register is enough, you might think. Yes, for just on/off control
this yields true. But here the focus is on some fancy demo and so each LED gets
its own control on brightness and some blinking. No timed control transfers are
necessary.

Each LED receives some controls spread across 4 registers. This excerpt is taken
directly from the logic code documentation:

````
Addr !   7   !   6   !   5   !   4   !   3   !   2   !   1   !   0   ! Function
-----+-------+-------+-------+-------+-------+-------+-------+-------+------------------
  0  !   0   !   0   !   0   !   0   !   0   !   0   !   M1  !   M0  ! LED Mode
  1  !  POF7 !  POF6 !  POF5 !  POF4 !  POF3 !  POF2 !  POF1 !  POF0 ! PWM when LED Off
  2  !  PON7 !  PON6 !  PON5 !  PON4 !  PON3 !  PON2 !  PON1 !  PON0 ! PWM when LED On
  3  !   0   !   0   !   0   !   0   !   BL3 !   BL2 !   BL1 !   BL0 ! Blink Frequency
All bits default to '0'.

LED Mode:
---------

 M1  !  M0 !
-----!-----!------------------------------------------------
  0  !   0 ! Totally Dark
  0  !   1 ! Dim to PWM Off
  1  !   0 ! Blinking between PWM Off and PWM On brightness
  1  !   1 ! Dim to PWM On
The blink frequency is controlled by BL3:0.

PWM Values:
-----------

0..255, where 0 means totally dark and 255 means fully bright. The control is
linear; keep in mind the human eye exhibits non-linear response.

Blink Frequency:
----------------

Value range is 0 to 15 giving 0.5 to 8 Hz
f = 0.5 Hz * (BL + 1)
````
This `Addr` parameter is an offset to add to each LEDs base address:
````
 LED  ! Base
------!------
 LED2 !   0
 LED3 !   4
 LED4 !   8
 LED5 !  12
 LED6 !  16
 LED7 !  20
 LED8 !  24
 LED9 !  28
````

Each address can also get read back to see the recent data it contains.

Reading register address 32 returns the Mojo button status ('1' = pushed, '0' =
released) at bit 0. Reading register 33 returns an internal counter running at a
25600 Hz clock.

The host application just sending out a SDR-WR packet containing 32 words will
control all 8 LEDs instantly. DDR-packets do not make sense on the LEDs at all,
DDR reading register 33 might give some insight on the timing achievable by the
control packets.
