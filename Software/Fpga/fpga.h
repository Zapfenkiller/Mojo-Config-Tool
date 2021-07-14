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
 *   These functions control the configuration lines to the FPGA.
 *
 *  \~German
 *   @brief Stellt die Hardware-Abstraktions-Schicht dar.
 *
 *   Die bereitgestellten Funktionen steuern die Konfigurationsanschlüsse des
 *   FPGA wie gewünscht.
 */


#ifndef __FPGA_H__
   #define __FPGA_H__


   // Includes:

   #include <avr/io.h>


   // Defines:

   #define  XILINX_STREAM_STATE_HEADER     0    /**< \~English Scanning the header intro. \~German Das Header-Intro wird gelesen. */
   #define  XILINX_STREAM_STATE_HEADER_1   1    /**< \~English Checking field type qualifier. \~German Der Feldtyp wird ausgewertet. */
   #define  XILINX_STREAM_STATE_HEADER_2   2    /**< \~English Reading field string data. \~German Ein Textfeld wird eingelesen. */
   #define  XILINX_STREAM_STATE_HEADER_E   6    /**< \~English Reading bitstream size. \~German Die Größenangabe des Bitstream wird gelesen. */
   #define  XILINX_STREAM_STATE_BODY      12    /**< \~English Shifting the bitstream \~German Der Bitstream wird verschoben. */
   #define  XILINX_STREAM_STATE_FAIL      13    /**< \~English State if header failure occured \~German Fehlerzustand während Header-Auswertung. */


   // Function Prototypes:

   void XilinxInitGpioLines(void);
   /**<
    * \~English
    *  initializes the microcontroller GPIO hardware to serve a Spartan II
    *  serial slave mode configuration.
    * \~German
    *  initialisiert die GPIOs des Mikrokontrollers um einen Spartan II im
    *  'serial slave mode' zu konfigurieren.
    */


   uint8_t XilinxForceConfig(void);
   /**<
    * \~English
    *  forces the FPGA into configuration. \code /PROG \endcode needs to be
    *  connected to the FPGA. \code /PROG \endcode emulates an open drain stage.
    *  @return '0' (false) in case the DONE is pulled to GND,
    *          !'0' (true) in case DONE still is released to VCC.
    *
    * \~German
    *  bringt das FPGA in den Konfigurationsmodus. \code /PROG \endcode muss
    *  am FPGA angeschlossen sein. \code /PROG \endcode verhält sich wie ein
    *  Open-Kollektor Ausgang.
    *  @return '0' (false) falls DONE auf GND gezogen bleibt,
    *          !'0' (true) falls DONE auf VCC liegt.
    */


   void XilinxPrepareConfig(void);
   /**<
    * \~English
    *  initializes the Xilinx bitstream handler.
    *
    * \~German
    *  initialisiert die Verarbeitung eines Xilinx bitstreams.
    */


   uint8_t XilinxReady(void);
   /**<
    * \~English
    *  reports if FPGA is ready to configure.
    *  @return !'0' (true) in case the /INIT line is released to VCC,
    *          '0' (false) in case /INIT still is pulled to GND.
    *
    * \~German
    *  sagt aus, ob das FPGA zur Konfiguration bereit ist.
    *  @return !'0' (true) falls /INIT auf VCC liegt,
    *          '0' (false) falls /INIT auf GND gezogen bleibt.
    */


   uint8_t XilinxBitstreamHeader(uint8_t byte);
   /**<
    * \~English
    *  scans header to get size and start of bitstream (xapp176.pdf).
    *  @param[in] one data byte from bitstream header.
    *  @return XILINX_STREAM_STATE_HEADER,
    *          XILINX_STREAM_STATE_HEADER_1,
    *          XILINX_STREAM_STATE_HEADER_2,
    *          XILINX_STREAM_STATE_HEADER_E,
    *          XILINX_STREAM_STATE_BODY,
    *          XILINX_STREAM_FAIL.
    *
    * \~German
    *  durchsucht den Header nach Größe und Beginn des Bitstream (xapp176.pdf).
    *  @param[in] ein Byte des Bitstream-Headers.
    *  @return XILINX_STREAM_STATE_HEADER,
    *          XILINX_STREAM_STATE_HEADER_1,
    *          XILINX_STREAM_STATE_HEADER_2,
    *          XILINX_STREAM_STATE_HEADER_E,
    *          XILINX_STREAM_STATE_BODY,
    *          XILINX_STREAM_FAIL.
    */


   uint8_t XilinxBitstreamBody(uint8_t bytes[], uint8_t bCount);
   /**<
    * \~English
    *  transfers a Xilinx bitstream (*.bit) into a Spartan-II FPGA.
    *  @param[in] pointer to the input stream.
    *  @param[in] count of bytes ready.
    *  @return XILINX_STREAM_STATE_HEADER,
    *          XILINX_STREAM_STATE_BODY.
    *
    * \~German
    *  überträgt einen Xilinx Bitstream  (*.bit) zum Spartan-II FPGA.
    *  @param[in] Zeiger auf den Datenstrom.
    *  @param[in] Anzahl der bereitstehenden Bytes.
    *  @return XILINX_STREAM_STATE_HEADER,
    *          XILINX_STREAM_STATE_BODY.
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


   uint32_t XilinxBitstreamLeft(void);
   /**<
    * \~English
    *  reports count of bytes still needing to be transferred to the FPGA.
    *  @return count.
    *
    * \~German
    *  liefert die Anzahl der Bytes die noch ins FPGA geschoben werden müssen.
    *  @return Anzahl.
    */


   uint32_t XilinxBitstreamSize(void);
   /**<
    * \~English
    *  reports count of bytes forming the bitstream.
    *  @return count.
    *
    * \~German
    *  liefert die Anzahl der Bytes des gesamten Bitstreams.
    *  @return Anzahl.
    */


#endif
