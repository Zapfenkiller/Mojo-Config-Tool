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
 *   @brief Mojo Operating System ("Mojo OS").
 *
 *  \~English
 *   My approach to use any terminal to perform the base activities, configure
 *   the FPGA from a bitstream file (.bit) and allow for topspeed exchange of
 *   data with the FPGA application logic by a dedicated parallel interface.
 *
 *  \~German
 *   Mein Ansatz um ein beliebiges Terminalprogramm für die fundamentalen
 *   Aktivitäten zu nutzen, das FPGA aus einer Bitstream-Datei (.bit) zu
 *   konfigurieren und durch eine angepasste parallele Schnittstelle die
 *   schnellste Datenübertragung von und zur Anwendungslogik im FPGA zu
 *   erzielen.
 */


#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include "stdio.h"
#include <LUFA/Drivers/USB/USB.h>
#include <LUFA/Drivers/Misc/RingBuffer.h>
#include <LUFA/Platform/Platform.h>

#include "./Fct.h"
#include "./Fpga/fpga.h"
#include "./SPI-flash/flash.h"
#include "./Ucif/ucif.h"
#include "./Config/AppConfig.h"
#include "./Descriptors.h"


/**
 *  \~English
 *   LUFA CDC Class driver interface configuration and state information.
 *   This structure is passed to all CDC Class driver functions, so that
 *   multiple instances of the same class within a device can be differentiated
 *   from one another.
 *
 *  \~German
 *   Schnittstelleneinstellungen und Statusinformationen des LUFA
 *   CDC-Klassentreibers. Die Datenstruktur wird jeder Funktion des
 *   CDC-Klassentreibers übergeben, so dass im Gerät verschiedene Instanzen
 *   dieser Klasse unterscheidbar sind.
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
 *   Standard Datei Datenstrom für die CDC Schnittstelle; sofern eingestellt.
 *   Hierdurch kann der virtuelle CDC Anschluß in den C APIs wie jeder andere
 *   Zeichenstrom benutzt werden.
 */
static FILE USBSerialStream;


/**
 *  \~English
 *   Some clever code to get the amount of currently available heap space is
 *   found at https://www.avrfreaks.net/forum/free-ram-available
 *   This information is needed to get optimum buffer size.
 *   The code works fine, but has also two drawbacks: It causes the compiler to
 *   assert
 *   \code fct.c:134: warning: function returns address of local variable \endcode
 *   This particular warning can be safely ignored.
 *   Secondly it causes the code to get roughly 510 bytes larger if used.
 *
 *  \~German
 *   Siehe https://www.avrfreaks.net/forum/free-ram-available
 *   Berechnet den aktuell freien Platz im Heap. Wird benötigt um die optimale
 *   Puffergröße zu ermitteln.
 *   Der Code funktioniert, hat aber zwei Nebenwirkungen: Er veranlasst den
 *   Compiler zur Beschwerde
 *   \code fct.c:134: warning: function returns address of local variable \endcode
 *   Diese spezielle Warnung kann getrost ignoriert werden.
 *   Zweitens wird das Kompilat rund 510 Bytes größer wenn die Funktion benutzt
 *   wird.
 */
int availRAM(void)
{
   extern int __heap_start, *__brkval;
   int v;
   return ((int) &v - (__brkval == 0 ? (int) &__heap_start: (int) __brkval));
}


#define  CLI_WAIT_FOR_CONNECT        0 /**< \~English Wait for Terminal connection. \~German Wartet auf die Verbindungsanfrage vom Terminal. */
#define  CLI_HELP                    1 /**< \~English Send welcome message. \~German Begrüßungsmeldung senden. */
#define  CLI_FLASH_INFO              2 /**< \~English Send information on FLASH (content, ID mismatch). \~German Information über das FLASH senden (Inhalt, nicht unterstützter Chip). */
#define  CLI_PROMPT                  3 /**< \~English Send the command prompt. \~German Das Prompt der Konsole senden. */
#define  CLI_LISTEN                  4 /**< \~English Wait for user input and process it. \~German Nutzereingabe erwarten und verarbeiten. */
#define  CLI_XILINX_TRIGGER_CONFIG   5 /**< \~English Start FPGA configuration. \~German FPGA-Konfiguration starten. */
#define  CLI_XILINX_CONFIGURE_INTRO  6 /**< \~English Process bitstream header. \~German Kopfteil des Bitstreams verarbeiten. */
#define  CLI_XILINX_CONFIGURE_BODY   7 /**< \~English Configure FPGA from data stream. \~German FPGA aus dem Datenstrom konfigurieren. */
#define  CLI_XILINX_FINISH           8 /**< \~English Finish FPGA configuration. \~German Die FPGA-Konfiguration abschliessen. */
#define  CLI_STORE_BITSTREAM_INTRO   9 /**< \~English Process bitstream header, store to flash. \~German Kopfteil des Bitstreams verarbeiten, ins FLASH speichern. */
#define  CLI_STORE_BITSTREAM_BODY   10 /**< \~English Store data stream. \~German Datenstrom ins FLASH speichern. */
#define  CLI_VERIFY_FLASH           11

#define  CFG_SRC_USB               'u' /**< \~English Bitstream source is USB. \~German Der Datenstrom kommt vom USB. */
#define  CFG_SRC_SPI               's' /**< \~English Bitstream source is FLASH. \~German Der Datenstrom kommt aus dem FLASH. */

#define  APP_WAIT_FOR_PACKET_ID      0
#define  APP_WAIT_FOR_PACKET_SIZE    1
#define  APP_UCIF_SDR_WR             2
#define  APP_UCIF_DDR_WR             3
#define  APP_UCIF_SDR_RD             4
#define  APP_UCIF_DDR_RD             5


const char PROGMEM greetStr[]    = "\r\n\n* Mojo OS *\r\n" \
                                   "(c) 2021, R. Trapp\n";
const char PROGMEM promptStr[]   = "\r\n> ";
const char PROGMEM unknownStr[]  = " <- ?";
const char PROGMEM needStr[]     = "\r\nAwaiting data";
const char PROGMEM successStr[]  = "\r\nSuccess\r\n";
const char PROGMEM failStr[]     = "\r\nFAIL";
const char PROGMEM emptyStr[]    = "\r\nConfig FLASH is empty";
const char PROGMEM wrongStr[]    = "\r\nNot a Microchip FLASH";
const char PROGMEM invalidStr[]  = "\r\nInvalid bitstream";
const char PROGMEM helpStr[]     = "\r\nCommands:\r\n" \
                                   " V: Volatile Config\r\n" \
                                   " E: Erase FLASH\r\n" \
                                   " W: Write to FLASH\r\n" \
                                   " C: Config from FLASH\r\n" \
                                   " i: Info about FLASH\r\n" \
                                   " ?: Help\r\n";


void p(const char *str)
{
   fputs_P(str, &USBSerialStream);
}


volatile uint16_t *const bootKeyPtr = (volatile uint16_t*)0x0800;
volatile uint16_t *const cfgKeyPtr = (volatile uint16_t*)0x0802;
bool connected2CDC;


int main(void)
{
   // Disable watchdog if enabled by bootloader/fuses, only works if WDRF is
   // cleared. Ensures some board response, even if BOOTRST is unprogrammed!
   // Otherwise the board will seem bricked until a HW-RESET or power cycle.
   // With BOOTRST unprogrammed any attempt to start the Bootloader now just
   // restarts the application instead. With some Mojo clones apply a hard
   // reset via ISP probe pads to get into the booloader.
   MCUSR &= ~(1 << WDRF);
   wdt_disable();

   clock_prescale_set(clock_div_1);
   ucifBaseInit();
   XilinxPreparePorts();
   spiBaseInitHw();

   connected2CDC = false;

   // Create a regular character stream for the interface so that it can be
   // used with the stdio.h functions
   CDC_Device_CreateStream(&VirtualSerial_CDC_Interface, &USBSerialStream);

   USB_Init();
   GlobalInterruptEnable();

   for(;;)
   {
      if (!XilinxConfigured())
         commandLineInterface();
      else
         applicationLoop();
   }
}


void applicationLoop(void)
{
   uint8_t        buffermemory[2*CDC_TXRX_EPSIZE];
   RingBuffer_t   inBuffer;
   uint8_t        id = 0;
   uint8_t        size = 0;
   uint8_t        appState = APP_WAIT_FOR_PACKET_ID;

   ucifBaseInit();
   RingBuffer_InitBuffer(&inBuffer, buffermemory, sizeof(buffermemory));

   for(;;)
   {

      CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
      USB_USBTask();

      uint16_t rxCount = CDC_Device_BytesReceived(&VirtualSerial_CDC_Interface);
      uint16_t freeCount = RingBuffer_GetFreeCount(&inBuffer);
      uint16_t count = (freeCount > rxCount) ? rxCount : freeCount;
      for (; count > 0; count--)
         RingBuffer_Insert(&inBuffer, CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface));

      switch(appState)
      {
         case APP_WAIT_FOR_PACKET_ID:
            if (RingBuffer_GetCount(&inBuffer) > 0)
            {
               id = RingBuffer_Remove(&inBuffer);
               appState = APP_WAIT_FOR_PACKET_SIZE;
            }
            break;
         case APP_WAIT_FOR_PACKET_SIZE:
            if (RingBuffer_GetCount(&inBuffer) > 0)
            {
               size = RingBuffer_Remove(&inBuffer);
               CDC_Device_SendByte(&VirtualSerial_CDC_Interface, id);
               CDC_Device_SendByte(&VirtualSerial_CDC_Interface, size);
               switch(id)
               {
                  case 'w':   // SDR-WR packet
                     UCIF_DDR_CLR;
                     appState = APP_UCIF_SDR_WR;
                     break;
                  case 'W':   // DDR-WR packet
                     UCIF_DDR_SET;
                     appState = APP_UCIF_DDR_WR;
                     break;
                  case 'r':   // SDR-RD packet
                     UCIF_DDR_CLR;
                     appState = APP_UCIF_SDR_RD;
                     break;
                  case 'R':   // DDR-RD packet
                     UCIF_DDR_SET;
                     appState = APP_UCIF_DDR_RD;
                     break;
                  case '#':   // Return to reconfiguration
                     if (size == 'R')
                     {
                        XilinxReset();
                        *cfgKeyPtr = (uint16_t)0x1234;
                        return;
                     }
                  default:
                     appState = APP_WAIT_FOR_PACKET_ID;
               }
            }
            break;
         case APP_UCIF_SDR_WR:
         case APP_UCIF_DDR_WR:
            {
               UCIF_RW_CLR;
               UCIF_AS_OUTPUT;
               uint16_t ready = RingBuffer_GetCount(&inBuffer);
               while ((ready > 1) && (size > 0))
               {
                  UCIF_DATA_PORT = RingBuffer_Remove(&inBuffer);
                  UCIF_E_SET;
                  UCIF_DATA_PORT = RingBuffer_Remove(&inBuffer);
                  UCIF_E_CLR;
                  size--;
               }
               if (size == 0)
                  appState = APP_WAIT_FOR_PACKET_ID;
            }
            break;
         case APP_UCIF_SDR_RD:
            {
               uint16_t ready = RingBuffer_GetCount(&inBuffer);
               while ((ready > 0) && (size > 0))
               {
                  UCIF_RW_CLR;
                  UCIF_AS_OUTPUT;
                  UCIF_DATA_PORT = RingBuffer_Remove(&inBuffer);
                  UCIF_E_SET;
                  UCIF_AS_INPUT;
                  UCIF_RW_SET;
                  UCIF_E_CLR;
                  CDC_Device_SendByte(&VirtualSerial_CDC_Interface, UCIF_DATA_RET);
                  ready--;
                  size--;
               }
               if (size == 0)
                  appState = APP_WAIT_FOR_PACKET_ID;
            }
            break;
         case APP_UCIF_DDR_RD:
            {
               UCIF_AS_INPUT;
               UCIF_RW_SET;
               while (size > 0)
               {
                  CDC_Device_SendByte(&VirtualSerial_CDC_Interface, UCIF_DATA_RET);
                  UCIF_E_SET;
                  CDC_Device_SendByte(&VirtualSerial_CDC_Interface, UCIF_DATA_RET);
                  UCIF_E_CLR;
                  size--;
               }
               if (size == 0)
                  appState = APP_WAIT_FOR_PACKET_ID;
            }
            break;
         default:
            ;
      }
      CDC_Device_Flush(&VirtualSerial_CDC_Interface);
   }
}


void commandLineInterface(void)
{
   uint8_t  cliState;
   uint8_t  cfgSrc = 0;
   uint32_t flashAddr = 0;
   uint32_t fileSize = 0;
   bool alreadyConnected = false;
// bool     equal = false;
   uint8_t  aBuffer[1800]; // at least max(4*CDC_TXRX_EPSIZE, 256)

   if (*cfgKeyPtr != 0x1234)
   {
      cfgSrc = CFG_SRC_SPI;
      cliState = CLI_XILINX_TRIGGER_CONFIG;
   }
   else
      cliState = CLI_WAIT_FOR_CONNECT;

   for (;;)
   {
      CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
      USB_USBTask();
      switch (cliState)
      {
         case CLI_WAIT_FOR_CONNECT:
            if (CDC_Device_BytesReceived(&VirtualSerial_CDC_Interface) != 0)
            {
               CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
               connected2CDC = true;
            }
            if (connected2CDC)
            {
               if (!alreadyConnected)
               {
                  p(greetStr);
                  cliState = CLI_HELP;
               }
               else
                  cliState = CLI_PROMPT;
            }
            alreadyConnected = connected2CDC;
            break;
         case CLI_HELP:
            p(helpStr);
            // There is intentionally no `break;` here!
         case CLI_FLASH_INFO: ;
            {
               char* ptr = 0;

               if (getFlashChipID() != ID_MICROCHIP)
                  p(wrongStr);
               readFlash(aBuffer, 0, sizeof(aBuffer));
               ptr = XilinxGetHeaderField(aBuffer, XILINX_FIELD_DESIGN);
               if (ptr != 0)
               {
                  p(PSTR("\r\n"));
                  fputs(ptr, &USBSerialStream);
               }
               else
                  p(emptyStr);
            }
            // There is intentionally no `break;` here!
         case CLI_PROMPT:
            p(promptStr);
            cliState = CLI_LISTEN;
            // There is intentionally no `break;` here!
         case CLI_LISTEN:
            {
               uint8_t rxCount = CDC_Device_BytesReceived(&VirtualSerial_CDC_Interface);
               if (rxCount == 1)
               {
                  cliState = CLI_WAIT_FOR_CONNECT;
                  uint8_t cmdChar = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
                  CDC_Device_SendByte(&VirtualSerial_CDC_Interface, cmdChar);
                  switch (cmdChar)
                  {
                     case '\r':
                     case '\n':
                        break;
                     case '?':   // 'manpage'
                        cliState = CLI_HELP;
                        break;
                     case 'i':   // return bitstream header info from FLASH
                        cliState = CLI_FLASH_INFO;
                        break;
                     case 'V':   // feed bitstream volatile into FPGA
                        cfgSrc = CFG_SRC_USB;
                        p(needStr);
                        cliState = CLI_XILINX_TRIGGER_CONFIG;
                        break;
                     case 'C':   // configure from recent SPI-FLASH content
                        cfgSrc = CFG_SRC_SPI;
                        p(PSTR("\r\n"));
                        cliState = CLI_XILINX_TRIGGER_CONFIG;
                        break;
                     case 'W':   // store bitstream non-volatile into SPI-FLASH
                        eraseFlash();
                        p(needStr);
                        flashAddr = 0;
                        fileSize = 0;
                        cliState = CLI_STORE_BITSTREAM_INTRO;
                        break;
                     /*
                     case 'v':   // verify FLASH data
                        p(needStr);
                        flashAddr = 0;
                        equal = 0xFF;
                        cliState = CLI_VERIFY_FLASH;
                        break;
                     */
                     case 'E':   // erase FLASH
                        eraseFlash();
                        break;
                     default:
                        p(unknownStr);
                  }
               }
               else
                  for (uint8_t n = rxCount; n > 0; n--)
                     CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
            }
            break;
         case CLI_XILINX_TRIGGER_CONFIG:
            flashAddr = 0;
            fileSize = 0;
            cliState = CLI_XILINX_CONFIGURE_INTRO;
            // There is intentionally no `break;` here!
         case CLI_XILINX_CONFIGURE_INTRO:
            switch (cfgSrc)
            {
               case CFG_SRC_USB:
                  {
                     // Free the EP as fast as possible for the next USB packet
                     // to drop in in the background. Hope this is how LUFA
                     // works otherwise this is waste.
                     uint16_t rxCount = CDC_Device_BytesReceived(&VirtualSerial_CDC_Interface);
                     for (uint16_t n = 0; n < rxCount; n++)
                        aBuffer[flashAddr++] = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
                  }
                  break;
               case CFG_SRC_SPI:
                  {
                     // Get as many bytes from FLASH as possible.
                     readFlash(aBuffer, flashAddr, sizeof(aBuffer));
                     flashAddr += sizeof(aBuffer);
                  }
                  break;
               default:
                  cliState = CLI_WAIT_FOR_CONNECT;
            }
            if (flashAddr > (sizeof(aBuffer) - CDC_TXRX_EPSIZE))
            {
               uint8_t* ptrToSize = (uint8_t*)XilinxGetHeaderField(aBuffer, XILINX_FIELD_DATA);
               if (ptrToSize != 0)
               {
                  fileSize = XilinxExtractBitstreamSize(ptrToSize);
                  uint8_t* ptrToData = ptrToSize + XILINX_SIZE_OF_SIZE;
                  uint16_t remaining = (uint16_t)(aBuffer + flashAddr - ptrToData );
                  fileSize -= remaining;
                  XilinxReset();
                  XilinxWriteBlock(ptrToData, remaining);
                  cliState = CLI_XILINX_CONFIGURE_BODY;
               }
               else
               {
                  p(invalidStr);
                  cliState = CLI_WAIT_FOR_CONNECT;
               }
            }
            break;
         case CLI_XILINX_CONFIGURE_BODY: ;
            {
               uint16_t rxCount = 0;
               if (cfgSrc == CFG_SRC_USB)
               {
                  rxCount = CDC_Device_BytesReceived(&VirtualSerial_CDC_Interface);
                  for (uint16_t n = 0; n < rxCount; n++)
                     aBuffer[n] = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
               }
               else // CFG_SRC_SPI
               {
                  rxCount = sizeof(aBuffer);
                  readFlash(aBuffer, flashAddr, rxCount);
                  flashAddr += rxCount;
               }
               if (fileSize < (uint32_t)rxCount)
               {
                  rxCount = (uint16_t)fileSize;
                  fileSize = 0;
               }
               else
                  fileSize -= rxCount;
               XilinxWriteBlock(aBuffer, rxCount);
               if (fileSize == 0)
                  cliState = CLI_XILINX_FINISH;
            }
            break;
         case CLI_XILINX_FINISH:
            {
               if (XilinxFinishConfig() == XILINX_CFG_SUCCESS)
               {
                  p(successStr);
                  return;  // it is time to start the user application code
               }
               else
               {
                  p(failStr);
                  cliState = CLI_WAIT_FOR_CONNECT;
               }
            }
            break;
         case CLI_STORE_BITSTREAM_INTRO: ;
            {
               uint16_t rxCount = CDC_Device_BytesReceived(&VirtualSerial_CDC_Interface);
               for (uint16_t n = 0; n < rxCount; n++)
                  aBuffer[flashAddr++] = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
               if (flashAddr > (sizeof(aBuffer) - CDC_TXRX_EPSIZE))
               {
                  uint8_t* ptrToSize = (uint8_t*)XilinxGetHeaderField(aBuffer, XILINX_FIELD_DATA);
                  if (ptrToSize != 0)
                  {
                     fileSize = XilinxExtractBitstreamSize(ptrToSize);
                     fileSize += (uint32_t)(ptrToSize - aBuffer);
                     writeFlash(aBuffer, 0, (uint16_t) flashAddr);
                     cliState = CLI_STORE_BITSTREAM_BODY;
                  }
                  else
                  {
                     p(invalidStr);
                     cliState = CLI_WAIT_FOR_CONNECT;
                  }
               }
            }
            break;
         case CLI_STORE_BITSTREAM_BODY: ;
            {
               uint16_t rxCount = CDC_Device_BytesReceived(&VirtualSerial_CDC_Interface);
               if (rxCount > 0)
               {
                  for (uint16_t n = 0; n < rxCount; n++)
                     aBuffer[n] = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
                  writeFlash(aBuffer, flashAddr, rxCount);
                  flashAddr += rxCount;
               }
               if (flashAddr >= fileSize)
               {
                  cfgSrc = CFG_SRC_SPI;
                  cliState = CLI_XILINX_TRIGGER_CONFIG;
               }
            }
            break;
         /*
         case CLI_VERIFY_FLASH:
            {
               uint8_t  flBuffer[CDC_TXRX_EPSIZE];
               uint8_t  rxBuffer[CDC_TXRX_EPSIZE];
               uint16_t rxCount = CDC_Device_BytesReceived(&VirtualSerial_CDC_Interface);
               equal = true;
               if (rxCount > 0)
               {
                  for (uint8_t n = 0; n < rxCount; n++)
                     aBuffer[n] = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
                  readFlash(flBuffer, flashAddr, rxCount);
                  for (uint16_t n = 0; n < rxCount; n++)
                  {
                     if (aBuffer[n] != flBuffer[n])
                     {
                        fprintf_P(&USBSerialStream, PSTR("%6lx: spi %2x <> usb %2x\r\n"), flashAddr, flBuffer[n], aBuffer[n]);
                        equal = false;
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
   }
}


void EVENT_USB_Device_Connect(void)
{
}


void EVENT_USB_Device_Disconnect(void)
{
   // Does not cover all events, but at least USB host plug off.
   connected2CDC = false;
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
   bool CurrentDTRState  = (CDCInterfaceInfo->State.ControlLineStates.HostToDevice & CDC_CONTROL_LINE_OUT_DTR);

   // DTR == active => assume host connects to COM port
   if (CurrentDTRState)
      connected2CDC = true;
   else
      connected2CDC = false;
}


void EVENT_CDC_Device_LineEncodingChanged(USB_ClassInfo_CDC_Device_t *const CDCInterfaceInfo)
{
   switch (CDCInterfaceInfo->State.LineEncoding.BaudRateBPS)
   {
      case 1200:  // Activate the Arduino bootloader (Caterina)
         // Do not block FPGA configuration from FLASH after a reset.
         *cfgKeyPtr = (uint16_t)0; // just any value != 0x1234 will do
         // Detach USB
         USB_Disable();
         // Disable all interrupts
         cli();
         // Stash the magic keys
         *bootKeyPtr = (uint16_t)0x7777;
         // Let the WDT do a full HW reset
         // 250 ms is for the USB host to really catch the disconnect
         wdt_enable(WDTO_250MS);
         for (;;);
         break; // just for the convenient look, RESET strikes in the for-loop
      case 2400:  // Turn on command line response to handle FPGA (re)configuration
         // Block FPGA configuration from FLASH after a reset.
         *cfgKeyPtr = (uint16_t)0x1234;
         XilinxReset();
         // Detach USB and so on (see above)
         USB_Disable();
         cli();
         *bootKeyPtr = (uint16_t)0; // just any value != 0x7777 will do here
         wdt_enable(WDTO_250MS);
         for (;;);
         break;
      default:
         // Active baud rate setting => assume host connects to COM port
         connected2CDC = true;
         ;
   }
}
