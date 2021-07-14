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


   * LUFA Library *
   Copyright 2017  Dean Camera (dean [at] fourwalledcubicle [dot] com)
   License: "The MIT license"
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
 *  Im Anschluß werden die Einstellmöglichkeiten für jeden Parameter
 *  beschrieben.
 */


#ifndef _APP_CONFIG_H_
   #define _APP_CONFIG_H_


   // Port connections forming Xilinx serial configuration interface:

   #define  FPGA_DATA_PORT    PORTB /**< \~English Port register of FPGA serial data \~German Portregister für seriellen FPGA-Dateneingang */
   #define  FPGA_DATA_RET     PINB  /**< \~English Port register of FPGA DIN readback \~German Portregister für FPGA-DIN Rücklesung */
   #define  FPGA_DATA_DIR     DDRB  /**< \~English Port direction of FPGA serial data \~German Richtungsregister für seriellen FPGA-Dateneingang */

   #define  FPGA_CCLK_PORT    PORTD /**< \~English Port register of FPGA clock \~German Portregister für FPGA-Takteingang */
   #define  FPGA_CCLK_RET     PIND  /**< \~English Port register of FPGA clock readback \~German Portregister für FPGA-Takteingang Rücklesung */
   #define  FPGA_CCLK_DIR     DDRD  /**< \~English Port direction of FPGA clock \~German Richtungsregister für FPGA-Takteingang */
   #define  FPGA_CCLK_LINE    0     /**< \~English Port line of FPGA clock \~German Portleitung für FPGA-Takteingang */

   #define  FPGA_nPROG_PORT   PORTC /**< \~English Port register of FPGA config trigger \~German Portregister für Auslöser zur FPGA-Konfiguration */
   #define  FPGA_nPROG_DIR    DDRC  /**< \~English Port direction of FPGA config trigger \~German Richtungsregister für Auslöser zur FPGA-Konfiguration */
   #define  FPGA_nPROG_LINE   7     /**< \~English Port line of FPGA config trigger \~German Portleitung für Auslöser zur FPGA-Konfiguration */

   #define  FPGA_nINIT_PORT   PORTD /**< \~English Port register of FPGA /INIT \~German Portregister für FPGA-/INIT */
   #define  FPGA_nINIT_RET    PIND  /**< \~English Port register of FPGA /INIT readback \~German Portregister für FPGA-/INIT Rücklesung */
   #define  FPGA_nINIT_DIR    DDRD  /**< \~English Port direction of FPGA /INIT \~German Richtungsregister für FPGA-/INIT */
   #define  FPGA_nINIT_LINE   5     /**< \~English Port line of FPGA /INIT \~German Portleitung für FPGA-/INIT */

   #define  FPGA_DONE_PORT    PORTC /**< \~English Port register of FPGA DONE \~German Portregister für FPGA-DONE */
   #define  FPGA_DONE_RET     PINC  /**< \~English Port register of FPGA DONE readback \~German Portregister für FPGA-DONE Rücklesung */
   #define  FPGA_DONE_DIR     DDRC  /**< \~English Port direction of FPGA DONE \~German Richtungsregister für FPGA-DONE */
   #define  FPGA_DONE_LINE    6     /**< \~English Port line of FPGA DONE \~German Portleitung für FPGA-DONE */


   // Port connections forming SPI-FLASH interface:

   #define  FLASH_MOSI_PORT   PORTB /**< \~English Port register of FLASH MOSI \~German Portregister für FLASH-MOSI */
   #define  FLASH_MOSI_DIR    DDRB  /**< \~English Port direction of FLASH MOSI \~German Richtungsregister für FLASH-MOSI */
   #define  FLASH_MOSI_LINE   2     /**< \~English Port line of FLASH MOSI \~German Portleitung für FLASH-MOSI */

   #define  FLASH_MISO_PORT   PORTB /**< \~English Port register of FLASH MISO \~German Portregister für FLASH-MISO */
// #define  FLASH_MISO_RET    PINB  /**< \~English Port register of FLASH MISO readback \~German Portregister für FLASH-MISO Rücklesung */
   #define  FLASH_MISO_DIR    DDRB  /**< \~English Port direction of FLASH MISO \~German Richtungsregister für FLASH-MISO */
   #define  FLASH_MISO_LINE   3     /**< \~English Port line of FLASH MISO \~German Portleitung für FLASH-MISO */

   #define  FLASH_SCK_PORT    PORTB /**< \~English Port register of FLASH SCK \~German Portregister für FLASH-SCK */
   #define  FLASH_SCK_DIR     DDRB  /**< \~English Port direction of FLASH SCK \~German Richtungsregister für FLASH-SCK */
   #define  FLASH_SCK_LINE    1     /**< \~English Port line of FLASH SCK \~German Portleitung für FLASH-SCK */

   #define  FLASH_CS_PORT     PORTD /**< \~English Port register of FLASH CS \~German Portregister für FLASH-CS */
   #define  FLASH_CS_DIR      DDRD  /**< \~English Port direction of FLASH CS \~German Richtungsregister für FLASH-CS */
   #define  FLASH_CS_LINE     1     /**< \~English Port line of FLASH CS \~German Portleitung für FLASH-CS */

   #define  SPI_SS_PORT       PORTB /**< \~English Port register of SPI core SS \~German Portregister für SPI-SS */
   #define  SPI_SS_DIR        DDRB  /**< \~English Port direction of SPI core SS \~German Richtungsregister für SPI-SS */
   #define  SPI_SS_LINE       0     /**< \~English Port line of SPI core SS \~German Portleitung für SPI-SS */


   // USB properties:

   #define MAX_CURRENT_DRAW             500  // Milliampères
   /**<
    * \~English
    *  defines the topmost current draw of the device. The USB host
    *  monitors this limit and might turn off the USB power supply to
    *  the device if exceeded. For the Mojo we give "all in".
    * \~German
    *  stellt die maximal auftretende Stromaufnahme des Gerätes ein.
    *  Der Host überwacht diesen Grenzwert und kann bei dessen
    *  Überschreitung den USB-Anschluss ausschalten. Für das Mojo gibt
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
    *  gibt dem Host das Abfrageintervall für die Interrupt In Pipe
    *  vor. Der Wert ist in Millisekunden anzugeben, der erlaubte
    *  Bereich ist 1 bis 255. Der Vorgabewert ist 5.
    *  @note Die Dokumentation des jeweiligen Betriebssystems ist
    *  sorgfältig zu prüfen, sonst könnte es sehr unliebsame
    *  Überraschungen mit 'eigenwilligen' USB-Hosts geben.
    *  Zum Beispiel:
    *  https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/usbspec/ns-usbspec-_usb_endpoint_descriptor
    */


   #define DEVICE_SERIAL_NUMBER           L"01"
   /**<
    * \~English
    *  The device serial number. The default value is "01".
    * \~German
    *  Die Seriennummer des Gerätes. Der Startwert ist "01".
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
    * \~German  gibt den Überarbeitungsstand des Produktes an.
    */


#endif
