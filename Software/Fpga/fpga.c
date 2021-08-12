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
 *   @brief Implements FPGA config port dedicated to the Alchitry Mojo v3.
 *          This set of functions could also do for other Xilinx FPGAs, check
 *          the documentation. ug380, xapp502 (xapp176)
 *
 *  \~German
 *   @brief Implementiert die Funktionen zur FPGA-Konfiguration passend
 *          f�r das Alchitry Mojo v3. Die Funktionen k�nnen auch f�r andere
 *          Xilinx FPGAs verwendet werden, man pr�fe die zugeh�rige
 *          Dokumentation. ug380, xapp502 (xapp176)
 */


#include "fpga.h"
#include "Config/AppConfig.h"
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>


// Defines:

#define  FPGA_DATA_DRIVE    FPGA_DATA_DIR    =   0xFF                   /**< \~English FPGA data lines get output \~German Definiert FPGA-Datenleitungen als �C Ausgang */
#define  FPGA_DATA_HIZ      FPGA_DATA_DIR    =   0x00                   /**< \~English FPGA data lines get input \~German Definiert FPGA-Datenleitungen als �C Eingang */

#define  FPGA_CCLK_SET     (FPGA_CCLK_PORT  |=  (1 << FPGA_CCLK_LINE))  /**< \~English Sets FPGA clock to '1' \~German Setzt FPGA-Takteingang auf '1' */
#define  FPGA_CCLK_CLR     (FPGA_CCLK_PORT  &= ~(1 << FPGA_CCLK_LINE))  /**< \~English Clears FPGA clock to '0' \~German Setzt FPGA-Takteingang auf '0' */
#define  FPGA_CCLK_DRIVE   (FPGA_CCLK_DIR   |=  (1 << FPGA_CCLK_LINE))  /**< \~English FPGA clock gets output \~German Definiert FPGA-Takteingang als �C Ausgang */
#define  FPGA_CCLK_HIZ     (FPGA_CCLK_DIR   &= ~(1 << FPGA_CCLK_LINE))  /**< \~English FPGA clock gets input \~German Definiert FPGA-Takteingang als �C Eingang */

#define  FPGA_nPROG_SET    (FPGA_nPROG_PORT |=  (1 << FPGA_nPROG_LINE)) /**< \~English Sets FPGA config trigger to '1' \~German Setzt Ausl�ser f�r die FPGA-Konfiguration auf '1' */
#define  FPGA_nPROG_CLR    (FPGA_nPROG_PORT &= ~(1 << FPGA_nPROG_LINE)) /**< \~English Clears FPGA config trigger to '0' \~German Setzt Ausl�ser f�r die FPGA-Konfiguration auf '0' */
#define  FPGA_nPROG_DRIVE  (FPGA_nPROG_DIR  |=  (1 << FPGA_nPROG_LINE)) /**< \~English FPGA config trigger gets output \~German Definiert Ausl�ser f�r die FPGA-Konfiguration als �C Ausgang */
#define  FPGA_nPROG_HIZ    (FPGA_nPROG_DIR  &= ~(1 << FPGA_nPROG_LINE)) /**< \~English FPGA config trigger gets input \~German Definiert FPGA config trigger als �C Eingang */

#define  FPGA_nINIT_SET    (FPGA_nINIT_PORT |=  (1 << FPGA_nINIT_LINE)) /**< \~English Sets FPGA /INIT to '1' \~German Setzt FPGA /INIT auf '1' */
#define  FPGA_nINIT_CLR    (FPGA_nINIT_PORT &= ~(1 << FPGA_nINIT_LINE)) /**< \~English Clears FPGA /INIT to '0' \~German Setzt FPGA /INIT auf '0' */
#define  FPGA_nINIT_DRIVE  (FPGA_nINIT_DIR  |=  (1 << FPGA_nINIT_LINE)) /**< \~English FPGA /INIT gets output \~German Definiert FPGA /INIT als �C Ausgang */
#define  FPGA_nINIT_HIZ    (FPGA_nINIT_DIR  &= ~(1 << FPGA_nINIT_LINE)) /**< \~English FPGA /INIT gets input \~German Definiert FPGA /INIT als �C Eingang */
#define  FPGA_nINIT_READ   (FPGA_nINIT_RET  &   (1 << FPGA_nINIT_LINE)) /**< \~English Reads FPGA /INIT state \~German Liest den /INIT Status des FPGA */

#define  FPGA_DONE_SET     (FPGA_DONE_PORT  |=  (1 << FPGA_DONE_LINE))  /**< \~English Sets FPGA DONE to '1' \~German Setzt FPGA DONE auf '1' */
#define  FPGA_DONE_CLR     (FPGA_DONE_PORT  &= ~(1 << FPGA_DONE_LINE))  /**< \~English Clears FPGA DONE to '0' \~German Setzt FPGA DONE auf '0' */
#define  FPGA_DONE_DRIVE   (FPGA_DONE_DIR   |=  (1 << FPGA_DONE_LINE))  /**< \~English FPGA DONE gets output \~German Definiert FPGA DONE als �C Ausgang */
#define  FPGA_DONE_HIZ     (FPGA_DONE_DIR   &= ~(1 << FPGA_DONE_LINE))  /**< \~English FPGA DONE gets input \~German Definiert FPGA DONE als �C Eingang */
#define  FPGA_DONE_READ    (FPGA_DONE_RET   &   (1 << FPGA_DONE_LINE))  /**< \~English Reads FPGA DONE state \~German Liest den DONE Status des FPGA */

#define  XILINX_STREAM_STATE_STANDBY    0    /**< \~English Waiting to get startet. \~German Warten auf das Startsignal. */
#define  XILINX_STREAM_STATE_HEADER     1    /**< \~English Scanning the header. \~German Der Header wird gelesen. */
#define  XILINX_STREAM_STATE_PREPARE    2    /**< \~English Preparing FPGA for configuration. \~German Das FPGA wird auf Konfigurations-Modus geschaltet. */
#define  XILINX_STREAM_STATE_BODY       3    /**< \~English Submit the bitstream. \~German Der Bitstream wird �bertragen. */
#define  XILINX_STREAM_STATE_STARTUP    4    /**< \~English Handles start-up sequencing. \~German K�mmert sich um FPGA-Applikation Startsequenz. */
#define  XILINX_STREAM_STATE_FAIL      99    /**< \~English State if failure occured. \~German Fehlerzustand w�hrend der FPGA-Konfiguration. */

#define  XILINX_HEADER_STATE_INTRO      0    /**< \~English Checking header intro. \~German Pr�ft auf g�ltigen Headeranfang. */
#define  XILINX_HEADER_STATE_FID        1    /**< \~English Checking field type identifier. \~German Der Feldtyp wird ausgewertet. */
#define  XILINX_HEADER_STATE_2          2    /**< \~English Reading field string data. \~German Ein Textfeld wird eingelesen. */
#define  XILINX_HEADER_STATE_SIZE       3    /**< \~English Reading bitstream size. \~German Die Gr��enangabe des Bitstream wird gelesen. */


uint16_t fieldSize;
uint16_t byteInField;
uint8_t  xilinxState;
uint8_t  xilinxHstat;
uint32_t byteCount;
uint32_t streamSize;
uint32_t fourBytes;


void XilinxPreparePorts(void)
{
   FPGA_nPROG_SET;            // prepare '1' to get driven
   FPGA_nPROG_DRIVE;          // set GPIO as output
   FPGA_DONE_HIZ;             // set GPIO as input
   FPGA_DONE_SET;             // enable weak pullup on dedicated input pin
   FPGA_CCLK_HIZ;             // set GPIO as input
   FPGA_CCLK_CLR;             // disable weak pullup on user I/O
   FPGA_nINIT_HIZ;            // set GPIO as input
   FPGA_nINIT_CLR;            // disable weak pullup on user I/O
   FPGA_DATA_HIZ;             // set GPIO as input
   FPGA_DATA_PORT = 0;        // disable weak pullup on user I/O
   xilinxState = XILINX_STREAM_STATE_STANDBY;
}


void XilinxStartConfig(void)
{
   fourBytes = 0;
   byteCount = 0;
   byteInField = 0;
   fieldSize = 0;
   streamSize = 0;
   xilinxState = XILINX_STREAM_STATE_HEADER;
   xilinxHstat = XILINX_HEADER_STATE_INTRO;
}


void XilinxReset(void)
{
   // emulate open-drain to nPROG - refer to the output preselection
   // xapp176: min 300 ns of nPROG low pulse
   // ug380:   min 500 ns of nPROG low pulse
   FPGA_nPROG_CLR;            // reset the FPGA
   _delay_us(1);              // allow the FPGA to respond, 1000 ns
   FPGA_CCLK_CLR;             // preselect '0' = GND
   FPGA_CCLK_DRIVE;           // set GPIO as output
   FPGA_nPROG_SET;            // release reset to FPGA
   while (!FPGA_nINIT_READ)   // wait until FPGA is ready for configuration
      ;
   // xapp176: no further delay required
   // ug380: pullup on INIT_B hosted by FPGA already
}


uint8_t XilinxDoConfig(uint8_t *bytes, uint16_t bCnt)
{
   uint16_t n = 0;

   for (;;)
   {
      switch (xilinxState)
      {
         case XILINX_STREAM_STATE_STANDBY:
            return(XILINX_CFG_READY);
            break;
         case XILINX_STREAM_STATE_HEADER:
            for (n = bCnt; (n > 0) && (xilinxState == XILINX_STREAM_STATE_HEADER); n--)
            {
               uint8_t byte = *bytes++;
               fourBytes = fourBytes << 8;
               fourBytes = fourBytes | byte;
               byteInField++;
               if (byteInField == 2)
               {
                  fieldSize = (uint16_t) fourBytes;
                  if (fieldSize > 1000)
                     xilinxState = XILINX_STREAM_STATE_FAIL;
               }
               switch (xilinxHstat)
               {
                  case XILINX_HEADER_STATE_INTRO:
                     switch (byteInField)
                     {
                        case  2:
                           if ((uint16_t) fourBytes != 0x0009)
                              xilinxState = XILINX_STREAM_STATE_FAIL;
                           break;
                        case  6:
                        case 10:
                           if (fourBytes != 0x0FF00FF0)
                              xilinxState = XILINX_STREAM_STATE_FAIL;
                           break;
                        case 13:
                           if (fourBytes != 0xF0000001)
                              xilinxState = XILINX_STREAM_STATE_FAIL;
                           else
                              xilinxHstat = XILINX_HEADER_STATE_FID;
                           break;
                        default:
                           ;
                     }
                     break;
                  case XILINX_HEADER_STATE_FID:
                     byteInField = 0;
                     switch (byte)
                     {
                        case 'a':
                        case 'b':
                        case 'c':
                        case 'd':
                           xilinxHstat = XILINX_HEADER_STATE_2;
                           break;
                        case 'e':
                           xilinxHstat = XILINX_HEADER_STATE_SIZE;
                           break;
                        default:
                           xilinxState = XILINX_STREAM_STATE_FAIL;
                     }
                     break;
                  case XILINX_HEADER_STATE_2:
                     if (byteInField == (fieldSize + 2))
                     {
                        if (byte == 0x00)
                           xilinxHstat = XILINX_HEADER_STATE_FID;
                        else
                           xilinxState = XILINX_STREAM_STATE_FAIL;
                     }
                     break;
                  case XILINX_HEADER_STATE_SIZE:
                     if (byteInField == 4)
                     {
                        streamSize = fourBytes;
                        if (streamSize == 0)
                           xilinxState = XILINX_STREAM_STATE_FAIL;
                        else
                           xilinxState = XILINX_STREAM_STATE_PREPARE;
                     }
                     break;
                  default:
                     xilinxState = XILINX_STREAM_STATE_FAIL;
               }
            }
            if (n == 0)
               return(XILINX_CFG_ONGOING);
            break;
         case XILINX_STREAM_STATE_PREPARE:
            XilinxReset();
            byteCount = streamSize;
            xilinxState = XILINX_STREAM_STATE_BODY;
            // There is intentionally no `break;` here!
         case XILINX_STREAM_STATE_BODY:
            FPGA_DATA_DRIVE;       // set GPIOs as output
            // ug380, xapp502, xapp176
            for (n = bCnt; (n > 0) && (byteCount > 0); n--)
            {
               FPGA_DATA_PORT = *bytes++;
//FPGA_DATA_PORT = *bytes;
//*bytes = PINB; // wrong data applies here (ATmega32U4 datasheet, p70)
//*bytes = PINB; // now get the right readback
//bytes++;
               FPGA_CCLK_SET;
               byteCount--;
               FPGA_CCLK_CLR;
            }
            if (byteCount == 0)
            {
               xilinxState = XILINX_STREAM_STATE_STARTUP;
               return(XILINX_CFG_FINISHING);
            }
            else
               return(XILINX_CFG_ONGOING);
            break;
         case XILINX_STREAM_STATE_STARTUP:
            // ug380 p. 90
            FPGA_DATA_HIZ;       // GPIOs just as pull-ups to "drive" '1'
            FPGA_DATA_PORT = 0xFF;
            _delay_us(3);        // allow the lines are pulled-up to '1'
            // in case some PLLs need to lock
            // ds162 max. 1 ms req'd
            for (uint16_t i = 20000; (i > 0) && !(FPGA_DONE_READ); i--)
            {
               FPGA_CCLK_SET;
               FPGA_CCLK_CLR;
            }
            if (FPGA_DONE_READ)
            {
               for (uint8_t i = 8; i > 0; i--)
               {
                  FPGA_CCLK_SET;
                  FPGA_CCLK_CLR;
               }
               XilinxPreparePorts();
               xilinxState = XILINX_STREAM_STATE_STANDBY;
               return(XILINX_CFG_SUCCESS);
            }
            else
               xilinxState = XILINX_STREAM_STATE_FAIL;
            break;
         case XILINX_STREAM_STATE_FAIL:
            // There is intentionally no `break;` here!
         default:
            xilinxState = XILINX_STREAM_STATE_STANDBY;
            return(XILINX_CFG_FAIL);
      }
   }
}


uint8_t XilinxConfigured(void)
{
   return(FPGA_DONE_READ);
}


uint32_t XilinxBitstreamLeft(void)
{
   if ((xilinxState == XILINX_STREAM_STATE_BODY) || (xilinxState == XILINX_STREAM_STATE_STARTUP))
      return(byteCount);
   else
      return(0);
}


uint32_t XilinxBitstreamSize(void)
{
   if ((xilinxState == XILINX_STREAM_STATE_BODY) || (xilinxState == XILINX_STREAM_STATE_STARTUP))
      return(streamSize);
   else
      return(0);
}


char* XilinxGetHeaderField(uint8_t *buffer, uint8_t FieldID)
{
   static const uint8_t PROGMEM preamble[] = {0x00, 0x09, 0x0f, 0xf0, 0x0f, 0xf0, 0x0f, 0xf0, 0x0f, 0xf0, 0x00, 0x00, 0x01};

   for (uint8_t n = 0; n < sizeof(preamble); n++)
   {
      if (*buffer != pgm_read_byte(&preamble[n]))
         return(0);
      buffer++;
   }

   for (uint8_t n = 6; n > 0; n--)
   {
      int ofs = 0;
      if (*buffer++ == FieldID)
      {
         if (FieldID != XILINX_FIELD_DATA)
            buffer += 2;
         return((char*)buffer);
      }
      ofs = *buffer++;
      ofs = (ofs << 8) | *buffer++;
      buffer += ofs;
   }
   return(0);
}


uint32_t XilinxGetSize(uint8_t *buffer)
{
   streamSize = 0;
   uint8_t* ptr = XilinxGetHeaderField(buffer, XILINX_FIELD_DATA);

   if (ptr != 0)
      for (uint8_t n = 4; n > 0; n--)
         streamSize = (streamSize << 8) | *ptr++;

   return(streamSize);
}