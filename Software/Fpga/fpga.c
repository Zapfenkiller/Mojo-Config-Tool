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
 *   @brief Implements FPGA config port dedicated to the Alchitry Mojo v3.
 *          This set of functions could also do for other Xilinx FPGAs, check
 *          the documentation. ug380, xapp502 (xapp176)
 *
 *  \~German
 *   @brief Implementiert die Funktionen zur FPGA-Konfiguration passend
 *          für das Alchitry Mojo v3. Die Funktionen können auch für andere
 *          Xilinx FPGAs verwendet werden, man prüfe die zugehörige
 *          Dokumentation. ug380, xapp502 (xapp176)
 */


#include "fpga.h"
#include "Config/AppConfig.h"
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>


// Defines:

#define  FPGA_DATA_DRIVE    FPGA_DATA_DIR    =   0xFF                   /**< \~English FPGA data lines get output \~German Definiert FPGA-Datenleitungen als µC Ausgang */
#define  FPGA_DATA_HIZ      FPGA_DATA_DIR    =   0x00                   /**< \~English FPGA data lines get input \~German Definiert FPGA-Datenleitungen als µC Eingang */

#define  FPGA_CCLK_SET     (FPGA_CCLK_PORT  |=  (1 << FPGA_CCLK_LINE))  /**< \~English Sets FPGA clock to '1' \~German Setzt FPGA-Takteingang auf '1' */
#define  FPGA_CCLK_CLR     (FPGA_CCLK_PORT  &= ~(1 << FPGA_CCLK_LINE))  /**< \~English Clears FPGA clock to '0' \~German Setzt FPGA-Takteingang auf '0' */
#define  FPGA_CCLK_DRIVE   (FPGA_CCLK_DIR   |=  (1 << FPGA_CCLK_LINE))  /**< \~English FPGA clock gets output \~German Definiert FPGA-Takteingang als µC Ausgang */
#define  FPGA_CCLK_HIZ     (FPGA_CCLK_DIR   &= ~(1 << FPGA_CCLK_LINE))  /**< \~English FPGA clock gets input \~German Definiert FPGA-Takteingang als µC Eingang */

#define  FPGA_nPROG_SET    (FPGA_nPROG_PORT |=  (1 << FPGA_nPROG_LINE)) /**< \~English Sets FPGA config trigger to '1' \~German Setzt Auslöser für die FPGA-Konfiguration auf '1' */
#define  FPGA_nPROG_CLR    (FPGA_nPROG_PORT &= ~(1 << FPGA_nPROG_LINE)) /**< \~English Clears FPGA config trigger to '0' \~German Setzt Auslöser für die FPGA-Konfiguration auf '0' */
#define  FPGA_nPROG_DRIVE  (FPGA_nPROG_DIR  |=  (1 << FPGA_nPROG_LINE)) /**< \~English FPGA config trigger gets output \~German Definiert Auslöser für die FPGA-Konfiguration als µC Ausgang */
#define  FPGA_nPROG_HIZ    (FPGA_nPROG_DIR  &= ~(1 << FPGA_nPROG_LINE)) /**< \~English FPGA config trigger gets input \~German Definiert FPGA config trigger als µC Eingang */

#define  FPGA_nINIT_SET    (FPGA_nINIT_PORT |=  (1 << FPGA_nINIT_LINE)) /**< \~English Sets FPGA /INIT to '1' \~German Setzt FPGA /INIT auf '1' */
#define  FPGA_nINIT_CLR    (FPGA_nINIT_PORT &= ~(1 << FPGA_nINIT_LINE)) /**< \~English Clears FPGA /INIT to '0' \~German Setzt FPGA /INIT auf '0' */
#define  FPGA_nINIT_DRIVE  (FPGA_nINIT_DIR  |=  (1 << FPGA_nINIT_LINE)) /**< \~English FPGA /INIT gets output \~German Definiert FPGA /INIT als µC Ausgang */
#define  FPGA_nINIT_HIZ    (FPGA_nINIT_DIR  &= ~(1 << FPGA_nINIT_LINE)) /**< \~English FPGA /INIT gets input \~German Definiert FPGA /INIT als µC Eingang */
#define  FPGA_nINIT_READ   (FPGA_nINIT_RET  &   (1 << FPGA_nINIT_LINE)) /**< \~English Reads FPGA /INIT state \~German Liest den /INIT Status des FPGA */

#define  FPGA_DONE_SET     (FPGA_DONE_PORT  |=  (1 << FPGA_DONE_LINE))  /**< \~English Sets FPGA DONE to '1' \~German Setzt FPGA DONE auf '1' */
#define  FPGA_DONE_CLR     (FPGA_DONE_PORT  &= ~(1 << FPGA_DONE_LINE))  /**< \~English Clears FPGA DONE to '0' \~German Setzt FPGA DONE auf '0' */
#define  FPGA_DONE_DRIVE   (FPGA_DONE_DIR   |=  (1 << FPGA_DONE_LINE))  /**< \~English FPGA DONE gets output \~German Definiert FPGA DONE als µC Ausgang */
#define  FPGA_DONE_HIZ     (FPGA_DONE_DIR   &= ~(1 << FPGA_DONE_LINE))  /**< \~English FPGA DONE gets input \~German Definiert FPGA DONE als µC Eingang */
#define  FPGA_DONE_READ    (FPGA_DONE_RET   &   (1 << FPGA_DONE_LINE))  /**< \~English Reads FPGA DONE state \~German Liest den DONE Status des FPGA */


uint32_t streamSize;


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
}


void XilinxReset(void)
{
   // ds162:   min 500 ns of nPROG low pulse
   FPGA_nPROG_CLR;            // reset the FPGA
   _delay_us(1);              // allow the FPGA to respond, 1000 ns
   FPGA_CCLK_CLR;             // preselect '0' = GND
   FPGA_CCLK_DRIVE;           // set GPIO as output
   FPGA_nPROG_SET;            // release reset to FPGA
   while (!FPGA_nINIT_READ)   // wait until FPGA is ready for configuration
      ;                       // t_{PL} <= 4 ms
   // xapp176: no further delay required
   // ug380: pullup on INIT_B hosted by FPGA already
}


void XilinxWriteBlock(uint8_t* bytes, uint16_t bCnt)
{
   FPGA_DATA_DRIVE;
   // ug380, xapp502, xapp176
   for (uint16_t n = bCnt; n > 0; n--)
   {
      FPGA_DATA_PORT = *bytes++;
      FPGA_CCLK_SET;
      FPGA_CCLK_CLR;
   }
}


uint8_t XilinxFinishConfig()
{
   // ug380 p. 90 vs. 103
   FPGA_DATA_HIZ;       // GPIOs just as pull-ups to "drive" '1', avoid
                        // conflicts with user logic when FPGA takes over.
   FPGA_DATA_PORT = 0xFF;
   _delay_us(3);        // allow the lines are pulled-up to '1'
   // in case some PLLs need to lock
   // ds162 max. 1 ms req'd
   // code allows roughly 10 ms @ 8 MHz
   for (uint16_t i = 10000; (i > 0) && !(FPGA_DONE_READ); i--)
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
      return(XILINX_CFG_SUCCESS);
   }
   else
      return(XILINX_CFG_FAIL);
}


uint8_t XilinxConfigured(void)
{
   return(FPGA_DONE_READ);
}


char* XilinxGetHeaderField(uint8_t* buffer, uint8_t FieldID)
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


uint32_t XilinxExtractBitstreamSize(uint8_t* buffer)
{
   streamSize = 0;

   if (buffer != 0)
      for (uint8_t n = XILINX_SIZE_OF_SIZE; n > 0; n--)
         streamSize = (streamSize << 8) | *buffer++;

   return(streamSize);
}