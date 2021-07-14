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
#include <util/delay.h>


// Defines:

#define  FPGA_DATA_INIT     FPGA_DATA_DIR    =   0xFF                   /**< \~English FPGA data lines get output \~German Definiert FPGA-Datenleitungen als µC Ausgang */
#define  FPGA_DATA_HIZ      FPGA_DATA_DIR    =   0                      /**< \~English FPGA data lines get input \~German Definiert FPGA-Datenleitungen als µC Eingang */

#define  FPGA_CCLK_SET     (FPGA_CCLK_PORT  |=  (1 << FPGA_CCLK_LINE))  /**< \~English Sets FPGA clock to '1' \~German Setzt FPGA-Takteingang auf '1' */
#define  FPGA_CCLK_CLR     (FPGA_CCLK_PORT  &= ~(1 << FPGA_CCLK_LINE))  /**< \~English Clears FPGA clock to '0' \~German Setzt FPGA-Takteingang auf '0' */
#define  FPGA_CCLK_INIT    (FPGA_CCLK_DIR   |=  (1 << FPGA_CCLK_LINE))  /**< \~English FPGA clock gets output \~German Definiert FPGA-Takteingang als µC Ausgang */
#define  FPGA_CCLK_HIZ     (FPGA_CCLK_DIR   &= ~(1 << FPGA_CCLK_LINE))  /**< \~English FPGA clock gets input \~German Definiert FPGA-Takteingang als µC Eingang */

#define  FPGA_nPROG_SET    (FPGA_nPROG_PORT |=  (1 << FPGA_nPROG_LINE)) /**< \~English Sets FPGA config trigger to '1' \~German Setzt Auslöser für die FPGA-Konfiguration auf '1' */
#define  FPGA_nPROG_CLR    (FPGA_nPROG_PORT &= ~(1 << FPGA_nPROG_LINE)) /**< \~English Clears FPGA config trigger to '0' \~German Setzt Auslöser für die FPGA-Konfiguration auf '0' */
#define  FPGA_nPROG_INIT   (FPGA_nPROG_DIR  |=  (1 << FPGA_nPROG_LINE)) /**< \~English FPGA config trigger gets output \~German Definiert Auslöser für die FPGA-Konfiguration als µC Ausgang */
#define  FPGA_nPROG_HIZ    (FPGA_nPROG_DIR  &= ~(1 << FPGA_nPROG_LINE)) /**< \~English FPGA config trigger gets input \~German Definiert FPGA config trigger als µC Eingang */

#define  FPGA_nINIT_SET    (FPGA_nINIT_PORT |=  (1 << FPGA_nINIT_LINE)) /**< \~English Sets FPGA /INIT to '1' \~German Setzt FPGA /INIT auf '1' */
#define  FPGA_nINIT_CLR    (FPGA_nINIT_PORT &= ~(1 << FPGA_nINIT_LINE)) /**< \~English Clears FPGA /INIT to '0' \~German Setzt FPGA /INIT auf '0' */
#define  FPGA_nINIT_INIT   (FPGA_nINIT_DIR  |=  (1 << FPGA_nINIT_LINE)) /**< \~English FPGA /INIT gets output \~German Definiert FPGA /INIT als µC Ausgang */
#define  FPGA_nINIT_HIZ    (FPGA_nINIT_DIR  &= ~(1 << FPGA_nINIT_LINE)) /**< \~English FPGA /INIT gets input \~German Definiert FPGA /INIT als µC Eingang */
#define  FPGA_nINIT_READ   (FPGA_nINIT_RET  &   (1 << FPGA_nINIT_LINE)) /**< \~Reads FPGA /INIT state \~German Liest den /INIT Status des FPGA */

#define  FPGA_DONE_SET     (FPGA_DONE_PORT  |=  (1 << FPGA_DONE_LINE))  /**< \~English Sets FPGA DONE to '1' \~German Setzt FPGA DONE auf '1' */
#define  FPGA_DONE_CLR     (FPGA_DONE_PORT  &= ~(1 << FPGA_DONE_LINE))  /**< \~English Clears FPGA DONE to '0' \~German Setzt FPGA DONE auf '0' */
#define  FPGA_DONE_INIT    (FPGA_DONE_DIR   |=  (1 << FPGA_DONE_LINE))  /**< \~English FPGA DONE gets output \~German Definiert FPGA DONE als µC Ausgang */
#define  FPGA_DONE_HIZ     (FPGA_DONE_DIR   &= ~(1 << FPGA_DONE_LINE))  /**< \~English FPGA DONE gets input \~German Definiert FPGA DONE als µC Eingang */
#define  FPGA_DONE_READ    (FPGA_DONE_RET   &   (1 << FPGA_DONE_LINE))  /**< \~Reads FPGA DONE state \~German Liest den DONE Status des FPGA */


//uint8_t  fieldID;
uint16_t fieldSize;
uint16_t byteInField;
uint8_t  xilinxState;
uint32_t byteCount;
uint32_t streamSize;


void XilinxInitGpioLines(void)
{
   FPGA_CCLK_HIZ;       // set GPIO as input
   FPGA_CCLK_SET;       // enable weak pullup
   FPGA_DATA_HIZ;       // set GPIO as input
   FPGA_DATA_PORT = 0;  // disable pullup
   FPGA_nPROG_HIZ;      // set GPIO as input
   FPGA_nPROG_CLR;      // disable pullup
   FPGA_nINIT_HIZ;      // set GPIO as input
   FPGA_nINIT_CLR;      // disable weak pullup
   FPGA_DONE_HIZ;       // set GPIO as input
   FPGA_DONE_CLR;       // disable weak pullup
}


uint8_t XilinxForceConfig(void)
{
   // emulate open-drain to /PROG - refer to the output preselection
   // xapp176: min 300 ns of /PROGRAM low pulse => 300 ns @ 16 MHz = 5 clocks
   FPGA_nPROG_CLR;   // preselect '0' = GND
   FPGA_nPROG_INIT;  // set GPIO as output
   _delay_us(1);     // give the pulse 1 us duration
   FPGA_nPROG_HIZ;   // set GPIO as input
   _delay_us(1);     // give the pullup 1 us until reading back
   // xapp176: no further delay required (but introduced due to following code)
   return(FPGA_DONE_READ);
}


void XilinxPrepareConfig(void)
{
   byteCount = 0;
   byteInField = 0;
   fieldSize = 0;
   xilinxState = XILINX_STREAM_STATE_HEADER;
   FPGA_CCLK_CLR;
   FPGA_CCLK_INIT;
   FPGA_DATA_INIT;
}


uint8_t XilinxBitstreamHeader(uint8_t byte)
{
   byteCount = byteCount << 8;
   byteCount = byteCount | byte;
   byteInField++;
   if (byteInField == 2)
      fieldSize = (uint16_t) byteCount;
   switch (xilinxState)
   {
      case XILINX_STREAM_STATE_HEADER:
         if ((byteInField == 2) && (fieldSize != 9))
            xilinxState = XILINX_STREAM_STATE_FAIL;
         if (byteInField == (fieldSize + 4))
         {
            if ((uint16_t) byteCount == 0x0001)
            {
               byteInField = 0;
               xilinxState = XILINX_STREAM_STATE_HEADER_1;
            }
            else
               xilinxState = XILINX_STREAM_STATE_FAIL;
         }
         break;
      case XILINX_STREAM_STATE_HEADER_1:
         byteInField = 0;
         switch (byte)
         {
            case 'a':
            case 'b':
            case 'c':
            case 'd':
//             fieldID = byte;
               xilinxState = XILINX_STREAM_STATE_HEADER_2;
               break;
            case 'e':
               xilinxState = XILINX_STREAM_STATE_HEADER_E;
               break;
            default:
               xilinxState = XILINX_STREAM_STATE_FAIL;
         }
         break;
      case XILINX_STREAM_STATE_HEADER_2:
/*
         if (byteInField > 2)
            switch (fieldID)
            {
               case 'a':
                  if (byteInField < (MAX_STRING_LENGTH+1))
                     designName[byteInField-2] = byte;
                  else
                     designName[MAX_STRING_LENGTH] = '\0';
                  break;
               case 'b':
                  // FPGA name is coming in (e.g. "2s200pq208")
                  break;
               case 'c':
                  // Date is coming in
                  break;
               case 'd':
                  // Time is coming in
                  break;
            }
*/
         if (byteInField == (fieldSize + 2))
         {
            if (byte == 0)
               xilinxState = XILINX_STREAM_STATE_HEADER_1;
            else
               xilinxState = XILINX_STREAM_STATE_FAIL;
         }
         break;
      case XILINX_STREAM_STATE_HEADER_E:
         if (byteInField == 4)
         {
            streamSize = byteCount;
            xilinxState = XILINX_STREAM_STATE_BODY;
         }
         break;
      default:
         xilinxState = XILINX_STREAM_STATE_FAIL;
   }
   return(xilinxState);
}


uint8_t XilinxBitstreamBody(uint8_t bytes[], uint8_t bCount)
{
   if (xilinxState == XILINX_STREAM_STATE_BODY)
   {
      if (byteCount < (uint32_t) bCount)
      {
         bCount = byteCount;
         byteCount = 0;
      }
      else
         byteCount = byteCount - bCount;
      if (bCount != 0)
         // ug380, xapp502, xapp176
         for (uint8_t i = 0; i < bCount; i++)
         {
            FPGA_DATA_PORT = bytes[i];
            FPGA_CCLK_SET;
            FPGA_CCLK_CLR;
         }
      if (byteCount == 0)
      {
         for (uint8_t i = 0; i < 8; i++) // 8 sufficient?
         {
            FPGA_CCLK_SET;
            FPGA_CCLK_CLR;
         }
         FPGA_CCLK_HIZ;
         FPGA_CCLK_SET;    // weak pullup
         xilinxState = XILINX_STREAM_STATE_HEADER;
      }
   }
   else
      xilinxState = XILINX_STREAM_STATE_FAIL;
   return(xilinxState);
}


uint8_t XilinxReady(void)
{
   return(FPGA_nINIT_READ);
}


uint8_t XilinxConfigured(void)
{
   return(FPGA_DONE_READ);
}


uint32_t XilinxBitstreamLeft(void)
{
   if (xilinxState == XILINX_STREAM_STATE_BODY)
      return(byteCount);
   else  // xilinxState is XILINX_STREAM_STATE_HEADER
      return(0);
}


uint32_t XilinxBitstreamSize(void)
{
   if (xilinxState == XILINX_STREAM_STATE_BODY)
      return(streamSize);
   else  // xilinxState is XILINX_STREAM_STATE_HEADER
      return(0);
}
