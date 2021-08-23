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
 *   \note These library functions only work all for Microchip SPI-FLASH.
 *         Instead of the genuine Alchitry code implication the Adesto (Dialog
 *         Semiconductor) support is incomplete. It needs retrofitting if there
 *         is any Mojo V3 with Adesto or other SPI-FLASHES around. This port
 *         from Arduino (.ino) to native avr-gcc omits non-Microchip artifacts.
 *         As it turns out, the genuine Alchitry code does not fully support an
 *         Adesto FLASH.
 *
 *  \~German
 *   @brief Stellt die Hardware-Abstraktions-Schicht dar.
 *
 *   Die bereitgestellten Funktionen steuern die Kommunikation mit dem
 *   SPI-FLASH.
 *   \note Der originale Alchitry Code erweckt den Eindruck, dass ein SPI-FLASH
 *         von Microchip oder von Adesto (Dialog Semiconductor) installiert sein
 *         könnte. Tatsächlich funktionieren einige Routinen nur mit Microchip
 *         und nicht mit Adesto! Falls ein Mojo V3 mit Adesto oder anderem
 *         SPI-FLASH auftaucht, müssen die entsprechenden Codeteile nachgerüstet
 *         werden! Diese Portierung von Arduino (.ino) zum avr-gcc verwirft
 *         die nicht-Microchip Artefakte.
 *         Wie eine tiefere Analyse aufzeigt, unterstützt bereits der
 *         Originalcode von Alchitry nur Microchip-FLASH Speicher.
 */


#ifndef __FLASH_H__
   #define __FLASH_H__


   // Includes:

   #include <avr/io.h>
   #include "Descriptors.h"


   // Definitions:

// #define  ID_ADESTO      0x1F
   #define  ID_MICROCHIP   0xBF /**< \~English chip-ID of Microchip \~German Chip-ID von Microchip */


   // Function Prototypes:

   void spiBaseInitHw(void);
   /**<
    * \~English
    *  Prepares IO lines used by SPI-FLASH to defaults, works for all SPI-FLASH.
    *
    * \~German
    *  Grundeinstellung der vom SPI verwendeten IO-Leitungen, für alle SPI-FLASH.
    */


   uint8_t getFlashChipID(void);
   /**<
    * \~English
    *  Reads the chip ID. JEDEC-standard works for all SPI-FLASH.
    *  @return the ID code.
    *
    * \~German
    *  Liest die Chip ID.
    *  @return den ID Code. JEDEC-Standard funktioniert für alle SPI-FLASH.
    */


   void readFlash(volatile uint8_t* buffer, uint32_t address, uint16_t size);
   /**<
    * \~English
    *  Reads one block of \code size \encode from the FLASH memory to a given
    *  \code buffer \endcode location; Microchip only.
    *  @param[in] pointer to the output data buffer.
    *  @param[in] address of first byte to read.
    *  @param[in] count of bytes to read.
    *
    * \~German
    *  Liest einen Block der Größe \code size \endcode vom FLASH zu einem
    *  Pufferspeicher, auf den \code buffer \endcode zeigt; nur für Microchip.
    *  SPI-FLASH funktionieren.
    *  @param[in] Zeiger auf den Ausgabepuffer.
    *  @param[in] Adresse des ersten zu lesenden Bytes.
    *  @param[in] Anzahl der zu lesenden Bytes.
    */


   void eraseFlash(void);
   /**<
    * \~English
    *  Erases the entire FLASH memory; Microchip only.
    *
    * \~German
    *  Löscht den gesamten FLASH-Inhalt; nur für Microchip.
    */


   void writeFlash(uint8_t* buffer, uint32_t address, uint16_t size);
   /**<
    * \~English
    *  Writes one block of \code size \encode from a given \code buffer \endcode
    *  location to the FLASH memory; Microchip only.
    *  @param[in] pointer to the input data buffer.
    *  @param[in] address of first byte to write.
    *  @param[in] count of bytes to write.
    *
    * \~German
    *  Schreibt einen Block der Größe \code size \endcode aus einem
    *  Pufferspeicher, auf den \code buffer \endcode zeigt, zum FLASH; nur für
    *  Microchip.
    *  @param[in] Zeiger auf den Eingabepuffer.
    *  @param[in] Adresse des ersten zu schreibenden Bytes.
    *  @param[in] Anzahl der zu schreibenden Bytes.
    */


#endif