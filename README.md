# Spartan Configurator

Some while ago I had the idea of recycling a Trenz TE-0115 applying a Spartan-II
for my experiments. Unfortunately this board lacks the pricey (and meanwhile
out of stock) XC18V02 configuration FLASH. So there was the idea to use some
cheap and available components (some Arduino and some SPI-FLASH) to replace the
XC18V02. After finishing lots of the project it turns out that the Spartan-II
was damaged and the board just not more than scrap. So all the effort to load
the FPGA-config from a PC via USB seems to be waste.

By chance I got an Alchitry Mojo, featuring a Spartan-6 and an ATmega32U4. Well,
this is more than just some lucky coincidence. All the stuff already packed onto
one small PCB and ready to rumble? Yes, and no. One of the drawbacks is the Mojo
as delivered takes just .bin files, not .bit files. Okay, just a few clicks and
the ISE webpack also delivers .bin files. But there is more to it. The original
configuration degrades the µC just down to a configuration device, enhanced ADC
and UART bridge. All this works, but needs dedicated extra effort to get it work
inside the FPGA logic. Also the data rate via UART might be quite low. High
speed data exchange from FPGA to USB thus is not feasible or at least restricted
heavily.

Since the Mojo setup already features a parallel loading of the FPGA-config data,
it is possible to apply a parallel data interface to the FPGA enabling the µC to
drive data at a transfer rate beyond the reach of the common designs. Of course,
this needs also some logic inside the FPGA but could be way simpler and well
tuned to the ATmega32U4 capabilities when it comes to bit-bang control signals.


## Features

1. Non permanent configuration of Spartan-6 via USB, dedicated to experimental
   setups.
2. Permanent configuration of Spartan-6 via USB and by onboard SPI-FLASH.
3. Direct use of a Xilinx Bitstream file (.bit). Conserves information on the
   project that is currently loaded into the FLASH.
4. Highest speed parallel interface between µC and FPGA. Peak throughput can
   reach up to 9846 kbit/s (1230 kB/s) with the ATmega32U4 running off 8 MHz.
   Some assembly tuning could push the limit a little bit higher, in case this
   is needed. USB full-speed mode theoretically gives 12000 kbit/s.
5. No installation required. Just connect to USB and start your application
   program. Loading a configuration into the FPGA needs a terminal program
   allowing file transfers.
6. Your FPGA design can be used as a PC peripheral, the ATmega32U4 serves as
   USB-bridge.


## Current Status

Revision 0.0.1:
*  Volatile FPGA configuration via terminal file transfer using USB-CDC class.
   File format supported is Xilinx bitstream (.bit).


## Hardware

An Alchitry Mojo V3, featuring a Xilinx Spartan-6 XC6SLX9 FPGA running off
50 MHz, an Atmel ATmega32U4 running off 8 MHz and some SPI-FLASH for storing
non-volatile FLASH configuration data. The Mojo uses 3V3 interface standard.


## Software

The Spartan-Configurator explains itself as serial communication device
(USB-CDC class) to the host. Volatile configuration of a FPGA project is
possible as well as non-volatile storage of a certain configuration into the
onboard SPI-FLASH. A Xilinx bitstream file is downloaded from the host by a
capable terminal program.

After the FPGA is configured the host can communicate the the FPGA application
by a dedicated interface, capable of highest achievable transfer speed. This
feature reuses most of the wires that perform the FPGA configuration loading.


## Credits, links and further readings

The first credits go to Dean Camera for his really fine  
[LUFA](http://www.fourwalledcubicle.com/LUFA.php)
project. See there for lots of examples on LUFA usage and How-Tos.

Next comes the VBA code from  
[Jan Axelson's](http://janaxelson.com/hidpage.htm)  
[Usbhidio2](http://janaxelson.com/files/usbhidio2.zip)
application. It gave me a first insight how to use the µ$ HID API
from excel macros. This I found useful for the host side of the
USB-Userport.

The  
[USB-Userport](https://github.com/Zapfenkiller/USB-Userport)
might serve as a kind of how-to when it comes to dealing with certain setups
using other than the USB-CDC classes for communication, especially the generic
USB-HID class.

µ$ documentation is somewhat useful when writing host applications
based on windows:  
[Introduction to Human Interface Devices (HID)](https://docs.microsoft.com/en-us/windows-hardware/drivers/hid/)
[Human Interface Devices (HID)](https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/_hid/)  
[hidsdi.h header](https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/hidsdi/)  
For Linux you could probably start at  
[HID I/O Transport Drivers](https://www.kernel.org/doc/html/latest/hid/hid-transport.html)

Give a visit to the  
[Alchitry Mojo v3 page](https://alchitry.com/products/mojo-v3?_pos=7&_sid=45c7220f0&_ss=r).
Check the "More Information" section there to find additional information like
the  
[Mojo schematic](http://cdn.embeddedmicro.com/mojo/v3-sch.pdf).
Also you might find some useful information when  
[searching their page](https://alchitry.com/search?q=mojo+v3).

The most complex part is the Spartan-6 FPGA, made by  
[Xilix](https://www.xilinx.com/search/site-keyword-search.html#q=spartan-6).
One of the important documents with this project is the  
[UG380 - Spartan-6 FPGA Configuration User Guide (v2.11) ](https://www.xilinx.com/support/documentation/user_guides/ug380.pdf)
and the older  
[Configuration and Readback of the Spartan-II and Spartan-IIE FPGA Families](https://www.xilinx.com/support/documentation/application_notes/xapp176.pdf)
which still is valid for the bitstream format.


And, of course, the datasheet of the  
[ATmega32U4](http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7766-8-bit-AVR-ATmega16U4-32U4_Datasheet.pdf)
is essential to understand the properties offered when you want to
enhance the thing to your needs. Please keep in mind that the Mojo runs the
ATmega32U4 just off a 3V3 supply and thus it is limited to 8MHz.

For those willing to learn a little bit more on the USB itself,
the most recent specifications are found at the
[USB-IF site](https://www.usb.org).
The information found there also could help with some aspects
of the generic HID device firmware. You might consider the
[Device Class Definition for Human Interface Devices (HID)](https://www.usb.org/sites/default/files/documents/hid1_11.pdf).
There are
[more related documents](https://usb.org/documents?search=HID&items_per_page=50)
around there.
And also the
[USB 2.0 specification](https://www.usb.org/sites/default/files/usb_20_20190524.zip).
