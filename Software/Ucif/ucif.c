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


#include <avr/io.h>

#include "Config/AppConfig.h"
#include "./ucif.h"


#define  UCIF_E_DRIVE      (UCIF_E_DIR      |=  (1 << UCIF_E_LINE))     /**< \~English Defines E as output to the UCIF. \~German Definiert E als Ausgang zum UCIF. */
#define  UCIF_RW_DRIVE     (UCIF_RW_DIR     |=  (1 << UCIF_RW_LINE))    /**< \~English Defines RW as output to the UCIF. \~German Definiert RW als Ausgang zum UCIF. */
#define  UCIF_DDR_DRIVE    (UCIF_DDR_DIR    |=  (1 << UCIF_DDR_LINE))   /**< \~English Defines DDR as output to the UCIF. \~German Definiert DDR als Ausgang zum UCIF. */


void ucifBaseInit(void)
{
   UCIF_RW_CLR;
   UCIF_RW_DRIVE;
   UCIF_DATA_PORT = 0;
   UCIF_AS_OUTPUT;
   UCIF_E_CLR;
   UCIF_E_DRIVE;
   UCIF_DDR_CLR;
   UCIF_DDR_DRIVE;
}

/*
void applicationLoop(void)
{
   bool alreadyConnected = false;
   bool read = false;
   bool ddr = false;
   uint8_t buffer[CDC_TXRX_EPSIZE];
   uint8_t rxCount;
// entsprechender HW init ???

   for(;;)
   {
      CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
      USB_USBTask();

/
      if (connected2CDC && !alreadyConnected)
      {
         p(PSTR("'R' to reconfigure\r\n"));
      }
      alreadyConnected = connected2CDC;

      while (CDC_Device_BytesReceived(&VirtualSerial_CDC_Interface) > 0)
      {
         if (CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface) == 'R')
         {
            XilinxReset();
            *cfgKeyPtr = (uint16_t)0x1234;
            return;
         }
      }
/

      rxCount = CDC_Device_BytesReceived(&VirtualSerial_CDC_Interface);
      if (rxCount > 1)
      {
         char packetID = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
         uint8_t byte1 = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
         rxCount -= 2;
         switch (packetID)
         {
            case 'c':
               if (rxCount == 0)
               {
                  CDC_Device_SendByte(&VirtualSerial_CDC_Interface, 'c');
                  CDC_Device_SendByte(&VirtualSerial_CDC_Interface, 0);
// flush?
                  switch (byte1 & 0b00000011)
                  {
                     case 0b00000001:
                        if (UCIF_RW_RET & (1 << UCIF_RW_LINE))
                           read = false;
                        else
                           read = true;
                        break;
                     case 0b00000010:
                        read = false;
                        break;
                     case 0b00000011:
                        read = true;
                        break;
                     default:
                        ;
                  }
                  if (byte1 & 0x10)
                  {
                     if (!read)
                        ddr = true;
                  }
                  else
                  {
                     UCIF_DATA_DIR = 0x00;
                     UCIF_RW_PORT |= (1 << UCIF_RW_LINE);
                     UCIF_E_PORT &= ~(1 << UCIF_E_LINE);
                     ddr = false;
                  }
                  if (read)
                  {
                     UCIF_DATA_DIR = 0x00;
                     UCIF_RW_PORT |= (1 << UCIF_RW_LINE);
                  }
                  else
                  {
                     UCIF_RW_PORT &= ~(1 << UCIF_RW_LINE);
                     UCIF_DATA_DIR = 0xFF;
                  }
                  switch (byte1 & 0b00001100)
                  {
                     case 0b00001000:
                        UCIF_E_PORT &= ~(1 << UCIF_E_LINE);
                        break;
                     case 0b00001100:
                        UCIF_E_PORT |= (1 << UCIF_E_LINE);
                        break;
                     default:
                        ;
                  }
               }
               break;
            case 'd': ;
               uint8_t size = byte1;
               if (size <= CDC_TXRX_EPSIZE)
               {
//             for (uint8_t n = 0; n < byte1; n++)
//                buffer[n] = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
                  if (read)   // read from ucif
// das ist jetzt der DDR modus - SDR fehlt!!
                  {
                     for (uint8_t n = 0; n < size; n++)
                     {
                        buffer[n] = UCIF_DATA_RET;
                        UCIF_E_PORT ^= (1 << UCIF_E_LINE);
                     }
//                   Nachricht an Host senden
// flush?

                  }
                  else        // write to ucif
                  {
                     if ((size & 0x01) == 0)
                     {
                        for (uint8_t n = 0; n < size; n++)
                        {
                           UCIF_DATA_PORT = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
                           UCIF_E_PORT ^= (1 << UCIF_E_LINE);
                        }
                        rxCount = 0;
                     }
                  }
               }
               break;
            default:
               ;
         }
      }
      while(rxCount > 0)
         CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
   }
}
*/