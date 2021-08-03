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

   #define  XILINX_CFG_READY               0    /**< \~English The FPGA configuration waits to get triggered. \~German Die FPGA-Konfiguration wartet auf ihren Einsatz. */
   #define  XILINX_CFG_ONGOING             1    /**< \~English The FPGA configuration is ongoing, need more data. \~German Die FPGA-Konfiguration ist noch nicht abgeschlossen, benötigt mehr Daten. */
   #define  XILINX_CFG_FINISHING           2    /**< \~English The FPGA configuration is in the start-up sequence. \~German Die FPGA-Konfiguration ist in der Start-Up Phase. */
   #define  XILINX_CFG_SUCCESS             3    /**< \~English The FPGA is configured. \~German Die FPGA-Konfiguration ist abgeschlossen. */
   #define  XILINX_CFG_FAIL              255    /**< \~English The FPGA configuration got aborted. \~German Die FPGA-Konfiguration wurde abgebrochen. */


   // Function Prototypes:

   void XilinxInitConfig(void);
   /**<
    * \~English
    *  initializes the microcontroller GPIO hardware to regular (non-critical)
    *  settings. First initialization during boot or preparation of application
    *  usage.
    *
    * \~German
    *  initialisiert die GPIOs des Mikrokontrollers auf unkritische
    *  Einstellungen. Geeignet zur Erstinitalisierung oder für die spätere
    *  Nutzung durch die Applikation.
    */


   void XilinxStartConfig(void);
   /**<
    * \~English
    *  prepares the FPGA configuration logic.
    *
    * \~German
    *  bereitet die Konfigurationslogik vor.
    */


   uint8_t XilinxDoConfig(uint8_t *bytes, uint16_t bCnt);
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
