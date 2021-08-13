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

   #define  XILINX_CFG_SUCCESS             0    /**< \~English The FPGA is configured. \~German Die FPGA-Konfiguration ist abgeschlossen. */
   #define  XILINX_CFG_FAIL              255    /**< \~English The FPGA configuration got aborted. \~German Die FPGA-Konfiguration wurde abgebrochen. */

   #define  XILINX_FIELD_DESIGN          'a'
   #define  XILINX_FIELD_DEVICE          'b'
   #define  XILINX_FIELD_DATE            'c'
   #define  XILINX_FIELD_TIME            'd'
   #define  XILINX_FIELD_DATA            'e'

   #define  XILINX_SIZE_OF_SIZE            4


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
    *  Einstellungen. Geeignet zur Erstinitalisierung oder für die spätere
    *  Nutzung durch die Applikation.
    */


   void XilinxReset(void);
   /**<
    * \~English
    *  forces the FPGA into configuration.
    *
    * \~German
    *  setzt FPGA zurück, Konfiguration wird erwartet.
    */


   void XilinxWriteBlock(uint8_t* bytes, uint16_t bCnt);
   /**<
    * \~English
    *  
    *  @return 
    *
    * \~German
    *  
    *  @return 
    */


   uint8_t XilinxFinishConfig(void);
   /**<
    * \~English
    *  
    *  @return 
    *
    * \~German
    *  
    *  @return 
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
    *  
    *  @return 
    *
    * \~German
    *  
    *  @return 
    */


   uint32_t XilinxExtractBitstreamSize(uint8_t* buffer);
   /**<
    * \~English
    *  
    *  @return 
    *
    * \~German
    *  
    *  @return 
    */


#endif
