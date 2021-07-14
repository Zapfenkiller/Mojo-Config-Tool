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
 *   @brief ...
 */


#include "Config/AppConfig.h"
#include "Descriptors.h"
#include "Fct.h"
#include "Fpga/fpga.h"
#include <LUFA/Drivers/USB/USB.h>
#include <LUFA/Platform/Platform.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/power.h>
#include <avr/wdt.h>


/** LUFA CDC Class driver interface configuration and state information. This structure is
 *  passed to all CDC Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
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
const char PROGMEM promptStr[]   = "\r\n>";
const char PROGMEM unknownStr[]  = " <-?";
const char PROGMEM needStr[]     = "\r\nFPGA-Konfiguration erforderlich, bitte Bitstream-Datei (*.bit) senden.";
const char PROGMEM sizeStr[]     = "\r\nBitstream = %lu Bytes";
const char PROGMEM successStr[]  = "\r\nFPGA erfolgreich konfiguriert.";
const char PROGMEM failStr[]     = "\r\nFehler bei der FPGA-Konfiguration!";
const char PROGMEM noprogStr[]   = "\r\n/PROG nicht angeschlossen!";
const char PROGMEM helpStr[]     = "\r\nKommandos:\r\n C: Trigger FPGA to configure from file\r\n ?: This 'manpage'\n";
const char PROGMEM noBitStr[]    = "\r\nNot a valid bitstream! Wrong file or not in BINARY transfer mode.\r\nYou can safely abort the transfer.\r\nPress <RETURN>";
//const char PROGMEM doneLineStr[] = "\r\n DONE: %c";
//const char PROGMEM initLineStr[] = "\r\n/INIT: %c";

uint8_t  mainMachine;

uint8_t  rBuffer[CDC_TXRX_EPSIZE]; // Just one EP size is perfect.
uint16_t rCount;


int main(void)
{
   clock_prescale_set(clock_div_1);
   XilinxInitGpioLines();

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

      // Handle the configuration request(s).
      switch (mainMachine)
      {
         case MM_HELLO:
            fputs_P(greetStr, &USBSerialStream);
            fputs_P(helpStr, &USBSerialStream);
            // There is intentionally no `break;` here!
         case MM_PROMPT:
            fputs_P(promptStr, &USBSerialStream);
            mainMachine = MM_LISTEN;
            // There is intentionally no `break;` here!
         case MM_LISTEN:
// [ ] prüfen ob DONE != '1' und automatische Config starten
//     aber nur wenn der Bitstream lokal verfügbar ist => 3te Ausbaustufe
// [x] prüfen ob DONE != '1' und per Terminal nach dem Bitstream fragen
//     => 1te Ausbaustufe
            if (!XilinxConfigured()) // && XilinxPowered())
               mainMachine = MM_XILINX_RESPOND_TRIGGER;
            if (rCount == 1)
            {
               CDC_Device_SendByte(&VirtualSerial_CDC_Interface, rBuffer[0]);
               switch (rBuffer[0])
               {
//                case 's':
//                   fprintf_P(&USBSerialStream, doneLineStr, XilinxConfigured() ? '1' : '0');
//                   fprintf_P(&USBSerialStream, initLineStr, XilinxReady() ? '1' : '0');
//                   break;
// [x] Config per nPROG erzwingen => 2te Ausbaustufe mit Extradraht.
                  case 'C':
                     mainMachine = MM_XILINX_TRIGGER_CONFIG;
                     break;
//                case 'F':
//                   
//                   break;
                  case '?':
                     mainMachine = MM_HELLO;
                     break;
                  default:
                     fputs_P(unknownStr, &USBSerialStream);
               }
               if (mainMachine == MM_LISTEN)
                  mainMachine = MM_PROMPT;
            }
            break;
         case MM_OFFLINE_MODE:
//          if (!XilinxConfigured())
//          {
//             mainMachine = MM_XILINX_RESPOND_TRIGGER;
//          }
            if (rCount)
               mainMachine = MM_HELLO;
            break;
         case MM_XILINX_TRIGGER_CONFIG:
            if (XilinxForceConfig())
               mainMachine = MM_XILINX_RESPOND_TRIGGER;
            else
            {
               mainMachine = MM_PROMPT;
               fputs_P(noprogStr, &USBSerialStream);
            }
            break;
         case MM_XILINX_RESPOND_TRIGGER:
            if (XilinxReady())
            {
               XilinxPrepareConfig();
               mainMachine = MM_XILINX_SCAN_HEADER;
               fputs_P(needStr, &USBSerialStream);
            }
            break;
         case MM_XILINX_SCAN_HEADER:
            for (uint8_t n = 0; (n < rCount); n++)
            {
               switch (XilinxBitstreamHeader(rBuffer[n]))
               {
                  case XILINX_STREAM_STATE_BODY:
                     fprintf_P(&USBSerialStream, sizeStr, XilinxBitstreamSize());
                     XilinxBitstreamBody(&rBuffer[n+1], rCount - n);
                     mainMachine = MM_XILINX_CONFIGURE;
                     n = rCount;
                     break;
                  case XILINX_STREAM_STATE_FAIL:
                     fputs_P(noBitStr, &USBSerialStream);
                     mainMachine = MM_XILINX_FAILED;
                     n = rCount;
                     break;
                  default:
                     ;
               }
            }
            break;
         case MM_XILINX_CONFIGURE:
            if (rCount)
               if (XilinxBitstreamBody(rBuffer, rCount) == XILINX_STREAM_STATE_HEADER)
                  mainMachine = MM_XILINX_CHECK_SUCCESS;
            break;
         case MM_XILINX_CHECK_SUCCESS:
            if (XilinxConfigured())
               fputs_P(successStr, &USBSerialStream);
            else
               fputs_P(failStr, &USBSerialStream);
            mainMachine = MM_PROMPT;
            break;
         case MM_XILINX_FAILED:
            if (rCount == 1)
               if (rBuffer[0] == 0x0d)
                  mainMachine = MM_PROMPT;
            break;
         default:
            ;
      }

      CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
      USB_USBTask();
   }
}

// 's' = Status DONE und /INIT? Sinnlos, weil !DONE die Konfig auch startet.
// 'C' = Configure (Bitstream vom USB zum FPGA)
// 'F' = Schreibe Bitstream ins Flash (Bitstream vom USB ins serielle Flash)
// 'i' = Header-Info aus dem Flash (nur Infofelder in Klarschrift)
// 'r' = Hole Bitstream vom Flash (Bitstream vom Flash zum USB)
// '?' = Hilfetext
// Flash-Version: If !DONE && Terminal dran => woher: USB / Lokal?


void EVENT_USB_Device_Connect(void)
{
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
   if (CDCInterfaceInfo->State.LineEncoding.BaudRateBPS == 1200)
   {
      // If USB is used, detach from the bus and reset it
      USB_Disable();
      // Disable all interrupts
      cli();
      // Stash the magic key
      uint16_t bootKey = 0x7777;
      uint16_t *const bootKeyPtr = (uint16_t *)0x0800;
      *bootKeyPtr = bootKey;
      // Let the WDT do a full HW reset
      wdt_enable(WDTO_250MS);
      for (;;);
   }
}
