/*
   * Spartan Configurator *

   Copyright 2021  Ren� Trapp (rene [dot] trapp (-at-) web [dot] de)

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
#include "SPI-flash/flash.h"
#include "stdio.h"
#include <LUFA/Drivers/USB/USB.h>
#include <LUFA/Platform/Platform.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <util/delay.h>


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


/**
 *  \~English
 *   Some clever code to get the amount of currently available heap space is
 *   found at https://www.avrfreaks.net/forum/free-ram-available
 *   This information is needed to get optimum buffer size.
 *   The code works fine, but has also one drawback: It causes the compiler to
 *   assert
 *   \code fct.c:114: warning: function returns address of local variable \endcode
 *   This particular warning can be safely ignored.
 *
 *  \~German
 *   Siehe https://www.avrfreaks.net/forum/free-ram-available
 *   Berechnet den aktuell freien Platz im Heap. Wird ben�tigt um die optimale
 *   Puffergr��e zu ermitteln.
 *   Der Code funktioniert, hat aber eine Nebenwirkung: Er veranlasst den
 *   Compiler zur Beschwerde
 *   \code fct.c:114: warning: function returns address of local variable \endcode
 *   Diese spezielle Warnung kann getrost ignoriert werden.
 */
int availRAM(void)
{
   extern int __heap_start, *__brkval;
   int v;
   return ((int) &v - (__brkval == 0 ? (int) &__heap_start: (int) __brkval));
}


#define  MM_APPLICATION_MODE         0
#define  MM_HELLO                    1
#define  MM_FLASH_INFO               2
#define  MM_PROMPT                   3
#define  MM_LISTEN                   4
#define  MM_XILINX_TRIGGER_CONFIG    5
#define  MM_XILINX_CONFIGURE         6
#define  MM_STORE_BITSTREAM_INTRO    7
#define  MM_STORE_BITSTREAM_BODY     8
#define  MM_VERIFY                   9


#define  CFG_SRC_USB                 'u'
#define  CFG_SRC_SPI                 's'


const char PROGMEM greetStr[]    = "\r\n\n* Mojo OS *\r\n" \
                                   "(c) 2021, R. Trapp\n";
const char PROGMEM promptStr[]   = "\r\n> ";
const char PROGMEM unknownStr[]  = " <- ?";
const char PROGMEM needStr[]     = "\r\nAwaiting data\r";
const char PROGMEM successStr[]  = "FPGA config success";
const char PROGMEM failStr[]     = "FPGA config FAIL";
const char PROGMEM emptyStr[]    = "\r\nConfig FLASH is empty";
const char PROGMEM wrongStr[]    = "\r\nNot a Microchip FLASH";
const char PROGMEM invalidStr[]  = "\r\nInvalid bitstream";
const char PROGMEM helpStr[]     = "\r\nCommands:\r\n" \
                                   " V: Volatile Config\r\n" \
                                   " W: Write to FLASH\r\n" \
                                   " C: Config from FLASH\r\n" \
                                   " i: Info about FLASH\r\n" \
                                   " ?: Help";


uint8_t  mainMachine;
uint8_t  cfgSrc = 0;
uint32_t flashAddr = 0;
uint32_t fileSize = 0;

uint8_t  equal = 0;


int main(void)
{
   // Disable watchdog if enabled by bootloader/fuses, only works if WDRF is
   // cleared. Ensures some board response, even if BOOTRST is unprogrammed!
   // With BOOTRST unprogrammed any attempt to start the Bootloader just
   // restarts the application instead.
   MCUSR &= ~(1 << WDRF);
   wdt_disable();

   clock_prescale_set(clock_div_1);
   XilinxPreparePorts();
   spiBaseInitHw();

   // Create a regular character stream for the interface so that it can be used with the stdio.h functions
   CDC_Device_CreateStream(&VirtualSerial_CDC_Interface, &USBSerialStream);

   mainMachine = MM_APPLICATION_MODE;

   USB_Init();
   GlobalInterruptEnable();

   for (;;)
   {
      uint8_t aBuffer[4*CDC_TXRX_EPSIZE];

      switch (mainMachine)
      {
         case MM_APPLICATION_MODE:
            if (XilinxConfigured())
//             user application runs from here.
               ;
            else
            {
               _delay_ms(500);
               mainMachine = MM_HELLO;
            }
            break;
         case MM_HELLO:
            fputs_P(greetStr, &USBSerialStream);
            fputs_P(helpStr, &USBSerialStream);
            // There is intentionally no `break;` here!
         case MM_FLASH_INFO: ;
            {
               uint8_t  buffer[200]; // Should do, usually header is around 100 bytes.
               uint8_t* ptr = 0;

               if (getFlashChipID() != ID_MICROCHIP)
                  fputs_P(wrongStr, &USBSerialStream);
               readFlash(buffer, 0, sizeof(buffer));
               ptr = XilinxGetHeaderField(buffer, XILINX_FIELD_DESIGN);
               if (ptr != 0)
                  fprintf_P(&USBSerialStream, PSTR("\r\n%s"), ptr);
               else
                  fputs_P(emptyStr, &USBSerialStream);
            }
            // There is intentionally no `break;` here!
         case MM_PROMPT:
            fputs_P(promptStr, &USBSerialStream);
            mainMachine = MM_LISTEN;
            // There is intentionally no `break;` here!
         case MM_LISTEN:
            {
               uint8_t rxCount = CDC_Device_BytesReceived(&VirtualSerial_CDC_Interface);
               if (rxCount == 1)
               {
                  mainMachine = MM_PROMPT;
                  uint8_t cmdChar = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
                  CDC_Device_SendByte(&VirtualSerial_CDC_Interface, cmdChar);
                  switch (cmdChar)
                  {
                     case '\r':
                     case '\n':
                        break;
                     case '?':   // 'manpage'
                        mainMachine = MM_HELLO;
                        break;
                     case 'i':   // return bitstream header info from FLASH
                        mainMachine = MM_FLASH_INFO;
                        break;
                     case 'V':   // feed bitstream volatile into FPGA
                        cfgSrc = CFG_SRC_USB;
                        fputs_P(needStr, &USBSerialStream);
                        mainMachine = MM_XILINX_TRIGGER_CONFIG;
                        break;
                     case 'W':   // store bitstream non-volatile into SPI-FLASH
                        eraseFlash();
                        fputs_P(needStr, &USBSerialStream);
                        flashAddr = 0;
                        mainMachine = MM_STORE_BITSTREAM_INTRO;
                        break;
                     case 'C':   // configure from recent SPI-FLASH content
                        cfgSrc = CFG_SRC_SPI;
                        fputs_P(PSTR("\r\n"), &USBSerialStream);
                        mainMachine = MM_XILINX_TRIGGER_CONFIG;
                        break;

/*
case 'v':   // verify FLASH data
   fputs_P(needStr, &USBSerialStream);
   flashAddr = 0;
   equal = 0xFF;
   mainMachine = MM_VERIFY;
   break;
case 'e':
   eraseFlash();
   break;
case '0':
   flashAddr = 0;
   break;
case '1':
   flashAddr = 340512;
   break;
case ' ':
   ;
   uint8_t storage[16];
   for (uint8_t i = 16; i > 0; i--)
   {
      readFlash(storage, flashAddr, sizeof(storage));
      fprintf_P(&USBSerialStream, PSTR("\r\n%8lx  "), flashAddr);
      for (int n = 0; n < sizeof(storage); n++)
         fprintf_P(&USBSerialStream, PSTR(" %2x"), storage[n]);
      flashAddr += sizeof(storage);
   }
   break;
*/
                     default:
                        fputs_P(unknownStr, &USBSerialStream);
                  }
               }
               else
                  for (uint8_t n = rxCount; n > 0; n--)
                     CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
            }
            break;
         case MM_XILINX_TRIGGER_CONFIG:
            XilinxStartConfig();
            mainMachine = MM_XILINX_CONFIGURE;
            flashAddr = 0;
            break;
         case MM_XILINX_CONFIGURE: ;
            {
//             uint8_t  rxBuffer[availRAM() - 400]; // at least CDC_TXRX_EPSIZE
               uint8_t  rxBuffer[CDC_TXRX_EPSIZE]; // at least CDC_TXRX_EPSIZE
               uint16_t rxCount = 0;
               switch (cfgSrc)
               {
                  case CFG_SRC_USB:
                     // Check how many bytes are ready, move EP content into buffer.
                     rxCount = CDC_Device_BytesReceived(&VirtualSerial_CDC_Interface);
                     if (rxCount)
                     {
                        // Free the EP as fast as possible for the next USB packet
                        // to drop in in the background. Hope this is how LUFA
                        // works otherwise this is waste.
                        for (uint8_t n = 0; n < rxCount; n++)
                           rxBuffer[n] = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
                     }
                     break;
                  case CFG_SRC_SPI:
                     rxCount = sizeof(rxBuffer);
//fprintf_P(&USBSerialStream, PSTR("\r\n%p, %4x\r\n"), rxBuffer, rxCount);
                     readFlash(rxBuffer, flashAddr, rxCount);
//fprintf_P(&USBSerialStream, PSTR("%6lx  "), flashAddr);
//for (int n = 0; n < rxCount; n++)
//   fprintf_P(&USBSerialStream, PSTR(" %2x"), rxBuffer[n]);
                     flashAddr += (uint32_t)rxCount;
// bis hierher kommen nachweislich die richtigen Daten aus dem FLASH.
// der gleiche �rger wie unten mit den bootkey pointern bis sie volatile wurden?
                     break;
                  default:
                     mainMachine = MM_PROMPT;
               }
               switch (XilinxDoConfig(rxBuffer, rxCount))
               {
                  case XILINX_CFG_SUCCESS:
                     fputs_P(successStr, &USBSerialStream);
                     mainMachine = MM_APPLICATION_MODE;
                     break;
                  case XILINX_CFG_FAIL:
                     fputs_P(failStr, &USBSerialStream);
                     if (cfgSrc == CFG_SRC_SPI)
                        mainMachine = MM_FLASH_INFO;
                     else
                        mainMachine = MM_PROMPT;
                     break;
                  default:
                     ;
               }
            }
            break;
         case MM_STORE_BITSTREAM_INTRO: ;
            {
               uint8_t* ptrToSize = 0;
               uint16_t rxCount = CDC_Device_BytesReceived(&VirtualSerial_CDC_Interface);
               if (rxCount)
               {
                  for (uint8_t n = 0; n < rxCount; n++)
                     aBuffer[flashAddr++] = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
               }
               if (flashAddr > (sizeof(aBuffer) - CDC_TXRX_EPSIZE))
               {
                  fileSize = XilinxGetSize(aBuffer);
                  if (fileSize > 0)
                  {
                     ptrToSize = XilinxGetHeaderField(aBuffer, XILINX_FIELD_DATA);
                     fileSize += (uint32_t)(ptrToSize - aBuffer);
                     writeFlash(aBuffer, 0, (uint16_t) flashAddr);
                     mainMachine = MM_STORE_BITSTREAM_BODY;
                  }
                  else
                  {
                     fputs_P(invalidStr, &USBSerialStream);
                     mainMachine = MM_PROMPT;
                  }
               }
            }
            break;
         case MM_STORE_BITSTREAM_BODY: ;
            {
               uint8_t  rxBuffer[CDC_TXRX_EPSIZE];
               uint16_t rxCount = CDC_Device_BytesReceived(&VirtualSerial_CDC_Interface);
               if (rxCount > 0)
               {
                  for (uint8_t n = 0; n < rxCount; n++)
                     rxBuffer[n] = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
                  writeFlash(rxBuffer, flashAddr, rxCount);
                  flashAddr += rxCount;
               }
               if (flashAddr >= fileSize)
               {
                  cfgSrc = CFG_SRC_SPI;
                  mainMachine = MM_XILINX_TRIGGER_CONFIG;
               }
            }
            break;
/*
case MM_VERIFY:
   {
      uint8_t  flBuffer[CDC_TXRX_EPSIZE];
      uint8_t  rxBuffer[CDC_TXRX_EPSIZE];
      uint16_t rxCount = CDC_Device_BytesReceived(&VirtualSerial_CDC_Interface);
      if (rxCount > 0)
      {
         for (uint8_t n = 0; n < rxCount; n++)
            rxBuffer[n] = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
         readFlash(flBuffer, flashAddr, rxCount);
         for (uint16_t n = 0; n < rxCount; n++)
         {
            if (rxBuffer[n] != flBuffer[n])
            {
               fprintf_P(&USBSerialStream, PSTR("%6lx: spi %2x <> usb %2x\r\n"), flashAddr, flBuffer[n], rxBuffer[n]);
               equal = 0;
            }
            flashAddr++;
         }
      }
   }
   break;
*/
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
   mainMachine = MM_APPLICATION_MODE;
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
   volatile uint16_t *const bootKeyPtr = (volatile uint16_t *)0x0800;

   switch (CDCInterfaceInfo->State.LineEncoding.BaudRateBPS)
   {
      case 1200:  // Activate the Arduino bootloader (Caterina)
         // Detach USB
         USB_Disable();
         // Disable all interrupts
         cli();
         // Stash the magic key
         *bootKeyPtr = (uint16_t)0x7777;
         // Let the WDT do a full HW reset
         // 250 ms is for the USB host to really catch the disconnect
         wdt_enable(WDTO_250MS);
         for (;;);
         break; // just for the convenient look, RESET strikes in the for-loop
      case 2400:  // Turn on command line response to handle FPGA (re)configuration
         XilinxReset();
         USB_Disable();
         cli();
         *bootKeyPtr = (uint16_t)0x8888; // just any value != 0x7777 will do
         wdt_enable(WDTO_250MS);
         for (;;);
         break;
      default:
         // No particular action necessary.
         ;
   }
}
