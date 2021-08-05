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
 *   @brief Implements access to SPI-FLASH of the Alchitry Mojo v3.
 *
 *  \~German
 *   @brief Implementiert die Funktionen für SPI-FLASH Zugriff passend für das
 *          Alchitry Mojo v3.
 */


#include "flash.h"
#include "Config/AppConfig.h"
#include <avr/io.h>


// Defines:

#define  CMD_WRITE_COMMAND        0x01 /**< \~English  \~German  */
#define  CMD_WRITE_MEM_BYTE       0x02 /**< \~English  \~German  */
#define  CMD_READ_MEM_BYTE        0x03 /**< \~English  \~German  */
#define  CMD_WRITE_DISABLE        0x04 /**< \~English  \~German  */
#define  CMD_READ_STATUS          0x05 /**< \~English  \~German  */
#define  CMD_WRITE_ENABLE         0x06 /**< \~English  \~German  */
#define  CMD_BULK_ERASE           0x60 /**< \~English  \~German  */
#define  CMD_EBSY                 0x70 /**< \~English  \~German  */
#define  CMD_DBSY                 0x80 /**< \~English  \~German  */
#define  CMD_JEDEC_READ_ID        0x9F /**< \~English Command to read the chip ID \~German Kommando um die Chip ID zu lesen */
#define  CMD_AUTOINC_ENABLE       0xAD /**< \~English  \~German  */

#define  ID_ADESTO                0x1F /**< \~English ID of Adesto \~German Identifikation Adesto */
#define  ID_MICROCHIP             0xBF /**< \~English ID of Microchip \~German Identifikation Microchip */

#define  DESELECT_FLASH    (FLASH_CS_PORT   |=  (1 << FLASH_CS_LINE))   /**< \~English Sets CS '1' \~German Setzt CS auf '1' */
#define  SELECT_FLASH      (FLASH_CS_PORT   &= ~(1 << FLASH_CS_LINE))   /**< \~English Clears CS to '0' \~German Setzt CS auf '0' */
#define  FLASH_CS_DRIVE    (FLASH_CS_DIR    |=  (1 << FLASH_CS_LINE))   /**< \~English CS gets output \~German Definiert CS als µC Ausgang */


void setupSpiAsMaster(void)
{
   SPI_CORE_DIR = (1 << SPI_MOSI_LINE) | (1 << SPI_SCK_LINE) | (1 << SPI_SS_LINE);
   SPSR = (1 << SPI2X);
   SPCR = (0 << SPIE) | (1 << SPE) | (0 << DORD) | (1 << MSTR) | (0 << CPOL) | (0 << CPHA) | (0 << SPR1) | (1 << SPR0);
}


uint8_t xfer(uint8_t byte)
{
   SPDR = byte;
   while(!(SPSR & (1 << SPIF)))
      ;
   return(SPDR);
}


void spiBaseInitHw(void)
{
   DESELECT_FLASH;
   FLASH_CS_DRIVE;
   SPI_CORE_DIR = 0x00;
   SPI_CORE_PORT = 0xFF;
}


void spiReleaseHw(void)
{
   DESELECT_FLASH;
   SPI_CORE_DIR = 0x00;
   SPCR = 0;
}


uint8_t getFlashChipID(void)
{
   uint8_t retVal;

   setupSpiAsMaster();
   SELECT_FLASH;
   xfer(CMD_JEDEC_READ_ID);
   retVal = xfer(0);
   DESELECT_FLASH;
   spiReleaseHw();
   return(retVal);
}
