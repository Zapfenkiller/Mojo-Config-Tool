/*
   * Spartan Configurator *

   Copyright 2021  Ren� Trapp (rene [dot] trapp (-at-) web [dot] de)

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


   * LUFA Library *
   Copyright 2017  Dean Camera (dean [at] fourwalledcubicle [dot] com)
   License: "The MIT license"

   ATTENTION:
     Any license granted expressly **excludes** the VID/PID combination given.
*/


/** @file
 *  \~English
 *   @brief Application Configuration Header File
 *
 *  This is a header file which is used to configure some of
 *  the application's compile time options, as an alternative to
 *  specifying the compile time constants supplied through a
 *  makefile or build system.
 *
 *  Please see the descriptions below for information how to adapt
 *  the settings.
 *
 *  \~German
 *   @brief Applikationseinstellungen (Header File)
 *
 *  In dieser Datei sind zahlreiche Konfigurationseinstellungen
 *  zentral zusammengefasst. Dadurch sind diese Konstanten nicht
 *  in Makefile-Optionen versteckt.
 *
 *  Im Anschlu� werden die Einstellm�glichkeiten f�r jeden Parameter
 *  beschrieben.
 */


#ifndef _APP_CONFIG_H_
   #define _APP_CONFIG_H_


   // Port connections forming Xilinx serial configuration interface:

   #define  FPGA_DATA_PORT    PORTB /**< \~English Port register of FPGA data \~German Portregister f�r FPGA-Dateneingang */
// #define  FPGA_DATA_RET     PINB  /**< \~English Port register of FPGA DIN readback \~German Portregister f�r FPGA-DIN R�cklesung */
   #define  FPGA_DATA_DIR     DDRB  /**< \~English Port direction of FPGA data \~German Richtungsregister f�r FPGA-Dateneingang */

   #define  FPGA_CCLK_PORT    PORTD /**< \~English Port register of FPGA clock \~German Portregister f�r FPGA-Takteingang */
// #define  FPGA_CCLK_RET     PIND  /**< \~English Port register of FPGA clock readback \~German Portregister f�r FPGA-Takteingang R�cklesung */
   #define  FPGA_CCLK_DIR     DDRD  /**< \~English Port direction of FPGA clock \~German Richtungsregister f�r FPGA-Takteingang */
   #define  FPGA_CCLK_LINE    0     /**< \~English Port line of FPGA clock \~German Portleitung f�r FPGA-Takteingang */

   #define  FPGA_nPROG_PORT   PORTC /**< \~English Port register of FPGA config trigger \~German Portregister f�r Ausl�ser zur FPGA-Konfiguration */
   #define  FPGA_nPROG_DIR    DDRC  /**< \~English Port direction of FPGA config trigger \~German Richtungsregister f�r Ausl�ser zur FPGA-Konfiguration */
   #define  FPGA_nPROG_LINE   7     /**< \~English Port line of FPGA config trigger \~German Portleitung f�r Ausl�ser zur FPGA-Konfiguration */

   #define  FPGA_nINIT_PORT   PORTD /**< \~English Port register of FPGA /INIT \~German Portregister f�r FPGA-/INIT */
   #define  FPGA_nINIT_RET    PIND  /**< \~English Port register of FPGA /INIT readback \~German Portregister f�r FPGA-/INIT R�cklesung */
   #define  FPGA_nINIT_DIR    DDRD  /**< \~English Port direction of FPGA /INIT \~German Richtungsregister f�r FPGA-/INIT */
   #define  FPGA_nINIT_LINE   5     /**< \~English Port line of FPGA /INIT \~German Portleitung f�r FPGA-/INIT */

   #define  FPGA_DONE_PORT    PORTC /**< \~English Port register of FPGA DONE \~German Portregister f�r FPGA-DONE */
   #define  FPGA_DONE_RET     PINC  /**< \~English Port register of FPGA DONE readback \~German Portregister f�r FPGA-DONE R�cklesung */
   #define  FPGA_DONE_DIR     DDRC  /**< \~English Port direction of FPGA DONE \~German Richtungsregister f�r FPGA-DONE */
   #define  FPGA_DONE_LINE    6     /**< \~English Port line of FPGA DONE \~German Portleitung f�r FPGA-DONE */


   // Port connections forming SPI-FLASH interface:

   #define  SPI_CORE_PORT     PORTB /**< \~English Port register of SPI core IOs \~German Portregister f�r IOs der SPI */
   #define  SPI_CORE_DIR      DDRB  /**< \~English Port direction of FLASH MOSI \~German Richtungsregister f�r FLASH-MOSI */
   #define  SPI_MISO_LINE     3     /**< \~English Port line of FLASH MISO \~German Portleitung f�r FLASH-MISO */
   #define  SPI_MOSI_LINE     2     /**< \~English Port line of FLASH MOSI \~German Portleitung f�r FLASH-MOSI */
   #define  SPI_SCK_LINE      1     /**< \~English Port line of FLASH SCK \~German Portleitung f�r FLASH-SCK */
   #define  SPI_SS_LINE       0     /**< \~English Port line of SPI core SS \~German Portleitung f�r SPI-SS */

   #define  FLASH_CS_PORT     PORTD /**< \~English Port register of FLASH CS \~German Portregister f�r FLASH-CS */
   #define  FLASH_CS_DIR      DDRD  /**< \~English Port direction of FLASH CS \~German Richtungsregister f�r FLASH-CS */
   #define  FLASH_CS_LINE     1     /**< \~English Port line of FLASH CS \~German Portleitung f�r FLASH-CS */


   // Port connections forming bytewide UCIF:

   #define  UCIF_DATA_PORT    PORTB /**< \~English Port register of UCIF output data \~German Portregister f�r Ausgabedaten zum UCIF */
   #define  UCIF_DATA_RET     PINB  /**< \~English Port register of UCIF input data \~German Portregister f�r Eingabedaten vom UCIF */
   #define  UCIF_DATA_DIR     DDRB  /**< \~English Port direction of UCIF data \~German Richtungsregister f�r UCIF Daten */

   #define  UCIF_E_PORT       PORTD /**< \~English Port register of UCIF-E line \~German Portregister f�r UCIF-E Signal */
   #define  UCIF_E_DIR        DDRD  /**< \~English Port direction of UCIF-E line \~German Richtungsregister f�r UCIF-E Signal */
   #define  UCIF_E_LINE       3     /**< \~English Port line of UCIF-E line \~German Portleitung f�r UCIF-E Signal */

   #define  UCIF_RW_PORT      PORTD /**< \~English Port register of UCIF-RW line \~German Portregister f�r UCIF-RW Signal */
   #define  UCIF_RW_DIR       DDRD  /**< \~English Port direction of UCIF-RW line \~German Richtungsregister f�r UCIF-RW Signal */
   #define  UCIF_RW_LINE      2     /**< \~English Port line of UCIF-RW line \~German Portleitung f�r UCIF-RW Signal */

   // Possible auxiliary ports for UCIF:
   // PD0, PD5, PB0


   // USB properties:

   #define VENDOR_ID                      0x29DD   // Alchitry Mojo v3
   #define PRODUCT_ID                     0x8001


   #define PRODUCT_DESCR_STRING           L"Alchitry Mojo V3 Config Tool, ATmega32U4 @ 8 MHz"
   /**<
    * \~ Product descriptor string.
    *
    * \~English
    *  This is a Unicode string containing the product's details in
    *  human readable form, and is read out upon request by the host
    *  when the appropriate string ID is requested, listed in the
    *  Device Descriptor.
    *
    * \~German
    *  Dieser Unicode-String enth�lt menschenlesbare Angaben zum
    *  Produkt. Meist ist es nur der Ger�tename.
    *  Der Text wird dem Host auf entsprechende Anforderung gesendet.
    *  Die zugeh�rige String-ID ist im Device Deskriptor abgelegt.
    *  Der Text liegt im FLASH des Controllers.
    */


   #define MANUFACTURER_DESCR_STRING      L"Embedded Micro, Rene Trapp"
   /**<
    * \~ Manufacturer Descriptor string.
    *
    * \~English
    *  This is a Unicode string containing the manufacturer's details
    *  in human readable form, and is read out upon request by the
    *  host when the appropriate string ID is requested, listed in
    *  the Device Descriptor.
    *
    * \~German
    *  Dieser Unicode-String enth�lt den menschenlesbaren Namen des
    *  Ger�te-Herstellers.
    *  Der Text wird dem Host auf entsprechende Anforderung gesendet.
    *  Die zugeh�rige String-ID ist im Device Deskriptor abgelegt.
    *  Der Text liegt im FLASH des Controllers.
    */


   #define MAX_CURRENT_DRAW             500  // Milliamp�res
   /**<
    * \~English
    *  defines the topmost current draw of the device. The USB host
    *  monitors this limit and might turn off the USB power supply to
    *  the device if exceeded. For the Mojo we give "all in".
    * \~German
    *  stellt die maximal auftretende Stromaufnahme des Ger�tes ein.
    *  Der Host �berwacht diesen Grenzwert und kann bei dessen
    *  �berschreitung den USB-Anschluss ausschalten. F�r das Mojo gibt
    *  es alles was geht.
    */


   #define POLLING_INTERVAL               5  // Milliseconds for low-/full-speed devices
   /**<
    * \~English
    *  defines the host polling interval. This is how often the
    *  host shall check the asynchronous event queue (Interrupt
    *  In pipe) for updates.
    *  The value shall be given as milliseconds from 1 to 255 for
    *  low- or full-speed devices. The default is 5.
    *  @note Carefully check your OS documentation, e. g.
    *  https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/usbspec/ns-usbspec-_usb_endpoint_descriptor
    *  or you might experience some unwelcome suprises by sloppy
    *  USB-host implementations.
    * \~German
    *  gibt dem Host das Abfrageintervall f�r die Interrupt In Pipe
    *  vor. Der Wert ist in Millisekunden anzugeben, der erlaubte
    *  Bereich ist 1 bis 255. Der Vorgabewert ist 5.
    *  @note Die Dokumentation des jeweiligen Betriebssystems ist
    *  sorgf�ltig zu pr�fen, sonst k�nnte es sehr unliebsame
    *  �berraschungen mit 'eigenwilligen' USB-Hosts geben.
    *  Zum Beispiel:
    *  https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/usbspec/ns-usbspec-_usb_endpoint_descriptor
    */


   #define DEVICE_SERIAL_NUMBER           L"01"
   /**<
    * \~English
    *  The device serial number. The default value is "01".
    * \~German
    *  Die Seriennummer des Ger�tes. Der Startwert ist "01".
    */


   #define RELEASE_MAJOR                  0
   /**<
    * \~English defines the MAJOR device release level.
    * \~German  gibt die MAJOR Freigabestufe des Produktes an.
    */


   #define RELEASE_MINOR                  0
   /**<
    * \~English defines the MINOR device release level.
    * \~German  gibt die MINOR Freigabestufe des Produktes an.
    */


   #define RELEASE_REVISION               1
   /**<
    * \~English defines the device revision.
    * \~German  gibt den �berarbeitungsstand des Produktes an.
    */


#endif
