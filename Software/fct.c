/*
   * Spartan Configurator *

   Copyright 2021  René Trapp (rene [dot] trapp (-at-) web [dot] de)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/


/** @file
 *  \~English
 *   @brief FPGA Configuration Tool core.
 *
 *  \~German
 *   @brief Das FPGA-Configuration-Tool.
 */


#include "Config/AppConfig.h"
#include "Descriptors.h"
#include "Fct.h"
#include "Fpga/fpga.h"
#include "stdio.h"
#include <LUFA/Drivers/USB/USB.h>
#include <LUFA/Platform/Platform.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/power.h>
#include <avr/wdt.h>


/**
 *  \~English
 *   LUFA CDC Class driver interface configuration and state information. This structure is
 *   passed to all CDC Class driver functions, so that multiple instances of the same class
 *   within a device can be differentiated from one another.
 *
 *  \~German
 */
USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface =
   {
      .Config =
         {
            .ControlInterfaceNumber = INTERFACE_ID_CDC_CCI,
            .DataINEndpoint         =
               {
                  .Address          = CDC_TX_EPADDR,
                  .Size             = CDC_TXRX_EPSIZE,
                  .Banks            = 1,
               },
            .DataOUTEndpoint =
               {
                  .Address          = CDC_RX_EPADDR,
                  .Size             = CDC_TXRX_EPSIZE,
                  .Banks            = 1,
               },
            .NotificationEndpoint =
               {
                  .Address          = CDC_NOTIFICATION_EPADDR,
                  .Size             = CDC_NOTIFICATION_EPSIZE,
                  .Banks            = 1,
               },
         },
   };


/**
 *  \~English
 *   Standard file stream for the CDC interface when set up, so that the
 *   virtual CDC COM port can be used like any regular character stream
 *   in the C APIs.
 *
 *  \~German
 */
static FILE USBSerialStream;


const char PROGMEM greetStr[]    = "\r\n\n\n* Spartan Configurator *\n\r(c) 2021, René Trapp\n";
const char PROGMEM promptStr[]   = "\r\n> ";
const char PROGMEM unknownStr[]  = " <-?";
const char PROGMEM needStr[]     = "\r\nFPGA configuration ready, awaiting bitstream (.bit) file.\r\n";
const char PROGMEM successStr[]  = "** FPGA successfully configured. **";
const char PROGMEM failStr[]     = "** FPGA configuration failed! **";
const char PROGMEM helpStr[]     = "\r\nAccepted commands:\r\n U: USB-Configure (configure FPGA from file)\r\n ?: This 'manpage'\n";

uint8_t  mainMachine;

uint8_t  rBuffer[CDC_TXRX_EPSIZE]; // Minimum one EP size, more when SPI-FLASH is getting used.
uint16_t rCount;


int main(void)
{
   /* Disable watchdog if enabled by bootloader/fuses */
   MCUSR &= ~(1 << WDRF);
   wdt_disable();

   clock_prescale_set(clock_div_1);
   XilinxInitConfig();

   mainMachine = MM_OFFLINE_MODE;

   /* Create a regular character stream for the interface so that it can be used with the stdio.h functions */
   CDC_Device_CreateStream(&VirtualSerial_CDC_Interface, &USBSerialStream);

   USB_Init();
   GlobalInterruptEnable();

   for (;;)
   {

      // Check how many bytes are ready, move EP content into buffer.
      rCount = CDC_Device_BytesReceived(&VirtualSerial_CDC_Interface);
      if (rCount)
      {
         // Free the EP as fast as possible for the next USB packet.
         for (uint8_t n = 0; n < rCount; n++)
            rBuffer[n] = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
      }

// [x]   '?' = Hilfetext
// [x]   'U' = USB-Configure (Bitstream vom USB zum FPGA)
// [ ]   'W' = Schreibe Bitstream ins Flash (Bitstream vom USB ins serielle Flash)
// [ ]   'F' = FLASH-Configure (Bitstream vom seriellen FLASH zum FPGA)
// [ ]   'i' = Header-Info aus dem FLASH (nur Infofelder in Klarschrift)
// [ ]   'r' = Lies Bitstream vom FLASH (Bitstream vom Flash zum USB)
// [ ]   'h' = Lies Bitstream vom FLASH in ASCII-Hex-Darstellung

      switch (mainMachine)
      {
         case MM_OFFLINE_MODE:
            if (!XilinxConfigured())
               mainMachine = MM_HELLO;
            break;
         case MM_HELLO:
            fputs_P(greetStr, &USBSerialStream);
            fputs_P(helpStr, &USBSerialStream);
            // There is intentionally no `break;` here!
         case MM_PROMPT:
            fputs_P(promptStr, &USBSerialStream);
            mainMachine = MM_LISTEN;
            // There is intentionally no `break;` here!
         case MM_LISTEN:
            if (rCount == 1)
            {
               CDC_Device_SendByte(&VirtualSerial_CDC_Interface, rBuffer[0]);
               mainMachine = MM_PROMPT;
               switch (rBuffer[0])
               {
                  case 'U':
                     mainMachine = MM_XILINX_TRIGGER_CONFIG;
                     break;
                  case '?':
                     mainMachine = MM_HELLO;
                     break;
                  case '\r':
                  case '\n':
                     break;
                  default:
                     fputs_P(unknownStr, &USBSerialStream);
               }
            }
            break;
         case MM_XILINX_TRIGGER_CONFIG:
            XilinxStartConfig();
            fputs_P(needStr, &USBSerialStream);
            mainMachine = MM_XILINX_CONFIGURE;
            break;
         case MM_XILINX_CONFIGURE:
            switch (XilinxDoConfig(rBuffer, rCount))
            {
               case XILINX_CFG_ONGOING:
                  if (rCount > 0)
                  {
                     fprintf(&USBSerialStream, "%7lu /", XilinxBitstreamLeft());
                     fprintf(&USBSerialStream, "%7lu\r", XilinxBitstreamSize());
                  }
                  break;
               case XILINX_CFG_SUCCESS:
                  fputs_P(successStr, &USBSerialStream);
                  mainMachine = MM_PROMPT;
                  break;
               case XILINX_CFG_FAIL:
                  fputs_P(failStr, &USBSerialStream);
                  mainMachine = MM_PROMPT;
                  break;
               default:
                  ;
            }
            break;
         default:
            ;
      }

      CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
      USB_USBTask();

   }
}


void EVENT_USB_Device_Connect(void)
{
   mainMachine = MM_HELLO;
}


void EVENT_USB_Device_Disconnect(void)
{
   mainMachine = MM_OFFLINE_MODE;
}


void EVENT_USB_Device_ConfigurationChanged(void)
{
   bool ConfigSuccess = true;

   ConfigSuccess &= CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC_Interface);
}


void EVENT_USB_Device_ControlRequest(void)
{
   CDC_Device_ProcessControlRequest(&VirtualSerial_CDC_Interface);
}


void EVENT_CDC_Device_ControLineStateChanged(USB_ClassInfo_CDC_Device_t *const CDCInterfaceInfo)
{
}


void EVENT_CDC_Device_LineEncodingChanged(USB_ClassInfo_CDC_Device_t *const CDCInterfaceInfo)
{
   switch (CDCInterfaceInfo->State.LineEncoding.BaudRateBPS)
   {
      case 1200:
         // Activate the Arduino bootloader (Caterina)
         // Detach USB
         USB_Disable();
         // Disable all interrupts
         cli();
         // Stash the magic key
         uint16_t bootKey = 0x7777;
         uint16_t *const bootKeyPtr = (uint16_t *)0x0800;
         *bootKeyPtr = bootKey;
         // Let the WDT do a full HW reset
         // 250 ms is for the USB host to really catch the disconnect
         wdt_enable(WDTO_250MS);
         for (;;);
         break; // just for the convenient look, RESET strikes in the for-loop
      case 2400:
         // Turn on command line response to handle FPGA configuration
         mainMachine = MM_HELLO;
         break;
      default: ;
   }
}
