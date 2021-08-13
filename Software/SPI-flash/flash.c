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


#include <avr/io.h>
#include <util/delay.h>

#include "Config/AppConfig.h"
#include "./flash.h"


// This table is partially specific to Microchip and invalid for Adesto!
#define  CMD_WRITE_STATUS         0x01 /**< \~English Microchip: Command to write FLASH status register. \~German Microchip: Kommando zum Schreiben ins FLASH Status-Register. */
#define  CMD_WRITE_MEM_BYTE       0x02 /**< \~English Microchip: Command to write one FLASH memory byte. \~German Microchip: Kommando zum Schreiben eines Bytes in den FLASH-Speicher. */
#define  CMD_READ_MEM_BYTE        0x03 /**< \~English Microchip: Command to read one FLASH memory byte. \~German Microchip: Kommando zum Lesen eines Bytes vom FLASH-Speicher. */
#define  CMD_WRITE_DISABLE        0x04 /**< \~English Microchip: Command to disable writing. \~German Microchip: Kommando zum Sperren der Schreibzugriffe. */
#define  CMD_READ_STATUS          0x05 /**< \~English Microchip: Command to read FLASH status register. \~German Microchip: Kommando zum Lesen vom FLASH Status-Register. */
#define  CMD_WRITE_ENABLE         0x06 /**< \~English Microchip: Command to enable writing. \~German Microchip: Kommando zum Erlauben der Schreibzugriffe. */
#define  CMD_BULK_ERASE           0x60 /**< \~English Microchip: Command to erase the entire FLASH memory. \~German Microchip: Kommando zum Löschen des gesamten FLASH-Speichers. */
#define  CMD_EBSY                 0x70 /**< \~English Microchip: Command to turn on HW busy indication. \~German Microchip: Kommando zum Einschalten des HW-Busy. */
#define  CMD_DBSY                 0x80 /**< \~English Microchip: Command to turn off HW busy indication. \~German Microchip: Kommando zum Ausschalten des HW-Busy. */
#define  CMD_JEDEC_READ_ID        0x9F /**< \~English All: Command to read the chip ID. \~German Alle: Kommando zum Lesen der Chip-ID. */
#define  CMD_AUTOINC_WRITE_WORD   0xAD /**< \~English Microchip: Command to write a pair of bytes to the FLASH memory. \~German Microchip: Kommando zum paarweisen Schreiben von Bytes in den FLASH-Speicher. */

#define  DESELECT_FLASH    (FLASH_CS_PORT   |=  (1 << FLASH_CS_LINE))   /**< CS = '1' */
#define  SELECT_FLASH      (FLASH_CS_PORT   &= ~(1 << FLASH_CS_LINE))   /**< CS = '0' */
#define  FLASH_CS_DRIVE    (FLASH_CS_DIR    |=  (1 << FLASH_CS_LINE))   /**< \~English Defines CS as output to the SPI-FLASH. \~German Definiert CS zum SPI-FLASH als Ausgang. */
#define  SPI_MISO_READ     (SPI_CORE_RET    &   (1 << SPI_MISO_LINE))   /**< \~English Reads SPI MISO state. \~German Liest den MISO Status der SPI. */
#define  SPI_SS_SET        (SPI_CORE_PORT   |=  (1 << SPI_SS_LINE))     /**< SS = '1' */


void setupSpiAsMaster(void)
{
   SPI_SS_SET; // In any case set to '1' before reversing the direction to
               // output! If not done any '0' reading still is stored somewhere
               // inside the SPI logic and immediately turns off the SPI master
               // when SPI gets enabled. No hint to this in any ATMEL datasheet!
               // You have to come up with that first!
               // A very big :( for this explicit lack of information.
   SPI_CORE_DIR = (1 << SPI_MOSI_LINE) | (1 << SPI_SCK_LINE) | (1 << SPI_SS_LINE);
   SPCR = (0 << SPIE) | (1 << SPE) | (0 << DORD) | (1 << MSTR) | (0 << CPOL) | (0 << CPHA) | (0 << SPR1) | (0 << SPR0);
   SPSR = (1 << SPI2X);
}


void spiReleaseHw(void)
{
   DESELECT_FLASH;
   SPI_CORE_DIR = 0;
   SPCR = 0;
}


uint8_t xfer(uint8_t byte)
{
   SPDR = byte;
   while(!(SPSR & (1 << SPIF)))
      ;
   return(SPDR);
}


void waitWhileBusy(void)
{
   SELECT_FLASH;
   xfer(CMD_READ_STATUS);
   while (xfer(0) & 0x01)
      ;
   DESELECT_FLASH;
}


void waitWhileHwBusy(void)
{
   SELECT_FLASH;
   _delay_us(1);
   while (SPI_MISO_READ == 0)
      ;
   DESELECT_FLASH;
}


void spiBaseInitHw(void)
{
   DESELECT_FLASH;
   FLASH_CS_DRIVE;
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


void readFlash(volatile uint8_t* buffer, uint32_t address, uint16_t size)
{
   setupSpiAsMaster();

   SELECT_FLASH;
   xfer(CMD_READ_MEM_BYTE);
   xfer(address >> 16);
   xfer(address >> 8);
   xfer(address);

   for (uint16_t s = size; s > 0; s--)
      *buffer++ = xfer(0);

   DESELECT_FLASH;
   spiReleaseHw();
}


void eraseFlash(void)
{
   setupSpiAsMaster();

   // Disable write protection
   SELECT_FLASH;
   xfer(CMD_WRITE_ENABLE);
   DESELECT_FLASH;

   SELECT_FLASH;
   xfer(CMD_WRITE_STATUS);
   xfer(0);
   DESELECT_FLASH;

   // Erase entire chip
   SELECT_FLASH;
   xfer(CMD_WRITE_ENABLE);
   DESELECT_FLASH;

   SELECT_FLASH;
   xfer(CMD_BULK_ERASE);
   DESELECT_FLASH;

   waitWhileBusy();
   spiReleaseHw();
}


void writeFlash(uint8_t* buffer, uint32_t address, uint16_t size)
{
   if (size == 0)
      return;

   setupSpiAsMaster();

   if ((address % 2) != 0)    // Word align by writing one single byte
   {
      SELECT_FLASH;
      xfer(CMD_WRITE_ENABLE);
      DESELECT_FLASH;

      SELECT_FLASH;
      xfer(CMD_WRITE_MEM_BYTE);
      xfer(address >> 16);
      xfer(address >> 8);
      xfer(address);
      xfer(*buffer++);
      DESELECT_FLASH;
      address += 1;
      size -= 1;
      waitWhileBusy();
   }

   if (size > 1)              // At least one pair of bytes is left
   {
      SELECT_FLASH;
      xfer(CMD_EBSY);         // Enable HW BUSY indication
      DESELECT_FLASH;

      SELECT_FLASH;
      xfer(CMD_WRITE_ENABLE);
      DESELECT_FLASH;

      SELECT_FLASH;
      xfer(CMD_AUTOINC_WRITE_WORD);
      xfer(address >> 16);
      xfer(address >> 8);
      xfer(address);
      xfer(*buffer++);
      xfer(*buffer++);
      DESELECT_FLASH;
      address += 2;
      size -= 2;
      waitWhileHwBusy();

      while (size > 1)
      {
         SELECT_FLASH;
         xfer(CMD_AUTOINC_WRITE_WORD);
         xfer(*buffer++);
         xfer(*buffer++);
         DESELECT_FLASH;
         address += 2;
         size -= 2;
         waitWhileHwBusy();
      }

      SELECT_FLASH;
      xfer(CMD_WRITE_DISABLE);
      DESELECT_FLASH;

      SELECT_FLASH;
      xfer(CMD_DBSY);         // Disable HW BUSY indication
      DESELECT_FLASH;
   }

   if (size != 0)             // It might still be just one byte left over
   {
      SELECT_FLASH;
      xfer(CMD_WRITE_ENABLE);
      DESELECT_FLASH;

      SELECT_FLASH;
      xfer(CMD_WRITE_MEM_BYTE);
      xfer(address >> 16);
      xfer(address >> 8);
      xfer(address);
      xfer(*buffer++);
      DESELECT_FLASH;
      waitWhileBusy();
   }

   spiReleaseHw();
}
