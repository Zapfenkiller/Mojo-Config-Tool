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
 *   @brief Serves as hardware abstraction layer (HAL).
 *
 *   These functions control the communication to the SPI-FLASH.
 *
 *  \~German
 *   @brief Stellt die Hardware-Abstraktions-Schicht dar.
 *
 *   Die bereitgestellten Funktionen steuern die Kommunikation mit dem
 *   SPI-FLASH.
 */


#ifndef __FLASH_H__
   #define __FLASH_H__


   // Includes:

   #include <avr/io.h>
   #include "Descriptors.h"


   // Function Prototypes:

   void spiBaseInitHw(void);
   /**<
    * \~English
    *
    * \~German
    */


   void spiReleaseHw(void);
   /**<
    * \~English
    *
    * \~German
    */


   uint8_t getFlashChipID(void);
   /**<
    * \~English
    *  Reads the chip ID.
    *  @return the ID code.
    *
    * \~German
    *  Liest die Chip ID.
    *  @return den ID Code.
    */


// uint8_t XilinxDoConfig(uint8_t *bytes, uint16_t bCnt);
   /**<
    * \~English
    *  processes one data packet of size \code bCnt \endcode bytes.
    *  @param[in] pointer to the input stream.
    *  @param[in] count of bytes ready.
    *  @return XILINX_CFG_READY,
    *          XILINX_CFG_ONGOING,
    *          XILINX_CFG_FINISHING,
    *          XILINX_CFG_SUCCESS,
    *          XILINX_CFG_FAIL.
    *
    * \~German
    *  verarbeitet ein Datenpaket der Größe \code bCnt \endcode Bytes.
    *  @param[in] Zeiger auf den Datenstrom.
    *  @param[in] Anzahl der bereitstehenden Bytes.
    *  @return XILINX_CFG_READY,
    *          XILINX_CFG_ONGOING,
    *          XILINX_CFG_FINISHING,
    *          XILINX_CFG_SUCCESS,
    *          XILINX_CFG_FAIL.
    */


#endif