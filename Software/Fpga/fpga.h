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
 *   These functions control the configuration of the FPGA.
 *
 *  \~German
 *   @brief Stellt die Hardware-Abstraktions-Schicht dar.
 *
 *   Die bereitgestellten Funktionen steuern die Konfiguration des FPGA.
 */


#ifndef __FPGA_H__
   #define __FPGA_H__


   // Includes:

   #include <avr/io.h>


   // Defines:

   #define  XILINX_CFG_SUCCESS             0    /**< \~English Return value: The FPGA is configured. \~German Rückgabewert: Die FPGA-Konfiguration ist abgeschlossen. */
   #define  XILINX_CFG_FAIL              255    /**< \~English Return value: The FPGA configuration got aborted. \~German Rückgabewert: Die FPGA-Konfiguration wurde abgebrochen. */

   #define  XILINX_FIELD_DESIGN          'a'    /**< \~English ID of the 'Design' data field. \~German ID des Datenfeldes 'Design'. */
   #define  XILINX_FIELD_DEVICE          'b'    /**< \~English ID of the 'Device' data field. \~German ID des Datenfeldes 'Device'. */
   #define  XILINX_FIELD_DATE            'c'    /**< \~English ID of the 'Date' data field. \~German ID des Datenfeldes 'Datum'. */
   #define  XILINX_FIELD_TIME            'd'    /**< \~English ID of the 'Time' data field. \~German ID des Datenfeldes 'Uhrzeit'. */
   #define  XILINX_FIELD_DATA            'e'    /**< \~English ID of the 'Bitstream' data field. \~German ID des Datenfeldes 'Bitstream'. */

   #define  XILINX_SIZE_OF_SIZE            4    /**< \~English Byte size of the bitstream size subfield. \~German Größe des Feldes der Bitstream-Größe, in Bytes. */


   // Function Prototypes:

   void XilinxPreparePorts(void);
   /**<
    * \~English
    *  initializes the microcontroller GPIO hardware to regular (non-critical)
    *  settings. First initialization during boot or preparation of application
    *  usage.
    *
    * \~German
    *  initialisiert die GPIOs des Mikrokontrollers auf unkritische
    *  Einstellungen. Geeignet zur Erstinitalisierung und für die spätere
    *  Nutzung durch die Applikation.
    */


   void XilinxReset(void);
   /**<
    * \~English
    *  forces the FPGA into configuration.
    *
    * \~German
    *  setzt das FPGA zurück, die Konfiguration kann beginnen.
    */


   void XilinxWriteBlock(uint8_t* bytes, uint16_t bCnt);
   /**<
    * \~English
    *  writes one data packet of size \code bCnt \endcode bytes to the FPGA.
    *  @param[in] pointer to the input stream (buffer).
    *  @param[in] count of bytes ready.
    *
    * \~German
    *  schreibt ein Datenpaket der Größe \code bCnt \endcode Bytes zum FPGA.
    *  @param[in] Zeiger auf den Datenstrom (Puffer).
    *  @param[in] Anzahl der bereitstehenden Bytes.
    */


   uint8_t XilinxFinishConfig(void);
   /**<
    * \~English
    *  finishes the configuration.
    *  The code respects waiting for potential PLL lock in.
    *  @return XILINX_CFG_SUCCESS,
    *          XILINX_CFG_FAIL.
    *
    * \~German
    *  beendet die Konfiguration.
    *  Der Code berücksichtigt die möglicherweise nötige Wartezeit für das
    *  Einrasten von PLLs.
    *  @return XILINX_CFG_SUCCESS,
    *          XILINX_CFG_FAIL.
    */


   uint8_t XilinxConfigured(void);
   /**<
    * \~English
    *  reports state of FPGA configuration.
    *  @return !'0' (true) in case DONE is released to VCC,
    *          '0' (false) in case DONE still is pulled to GND.
    *
    * \~German
    *  liefert den Status der FPGA Konfiguration.
    *  @return !'0' (true) falls DONE auf VCC liegt,
    *          '0' (false) falls DONE auf GND gezogen bleibt.
    */


   char* XilinxGetHeaderField(uint8_t* buffer, uint8_t FieldID);
   /**<
    * \~English
    *  Gets the pointer to a certain header field of the bitstream.
    *  @param[in] pointer to the input stream (buffer).
    *  @param[in] ID of header field to retrive.
    *  @return pointer to the first character of the field,
    *          0 if the field is not found.
    *  \note The full header has to be contained in the refered memory.
    *        With ISE Webpack 14.7 the header always seems to be less than 128
    *        Bytes.
    *
    * \~German
    *  Ermittelt den Zeiger auf ein bestimmtes Datenfeld im Kopf des Bitstreams.
    *  @param[in] Zeiger auf den Datenstrom (Puffer).
    *  @param[in] ID des gewünschten Datenfeldes.
    *  @return Zeiger auf das erst Zeichen im Feld,
    *          0 falls das Feld nicht gefunden wurde.
    *  \note Im referenzierten Speicherbereich muss der vollständige Header
    *        enthalten sein.
    *        Mit ISE Webpack 14.7 scheint der Kopfteil stets weniger als 128
    *        Bytes zu umfassen.
    */


   uint32_t XilinxExtractBitstreamSize(uint8_t* buffer);
   /**<
    * \~English
    *  Calculates the size of the bitstream itself.
    *  @param[in] Pointer to the first byte of the bitstream field.
    *  @return size of bitstream in bytes.
    *
    * \~German
    *  Berechnet die Größe des eigentlichen Bitstreams.
    *  @param[in] Zeiger auf das erste Byte des Bitstreams-Feldes.
    *  @return Anzahl der Bytes im Bitstream.
    */


#endif
