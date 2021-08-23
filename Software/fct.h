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
 *   @brief Header of Mojo Operating System ("Mojo OS").
 *
 *   There are just some function declarations here.
 *
 *  \~German
 *   @brief Header-Datei des Mojo Operating System ("Mojo OS").
 *
 *   Es gibt hier lediglich ein paar Funktionsdeklarationen.
 */


#ifndef __FCT_H__
   #define __FCT_H__


   // Includes:

   #include <avr/io.h>
   #include "Descriptors.h"


   // Function Prototypes:

   int main(void);
   /**<
    * \~English
    *  Main part of the Mojo OS. This function never returns.
    *  In case the FPGA needs its configuration the 'commandLineInterface' gets
    *  called, otherwise the 'applicationLoop' is run.
    *  Any hardware reset (an extra connected pushbutton to the ATmega's RESET
    *  pin) will just restart the microcontroller into the CLI / application
    *  loop. It will not perform a FPGA RESET.
    *
    * \~German
    *  Funktion 'main' des Mojo OS. Die Funktion wird nie verlassen.
    *  Wenn dass FPGA nicht konfiguriert ist, wird das 'commandLineInterface'
    *  gestartet anderenfalls direkt die 'applicationLoop'.
    *  Jeder echte HW-RESET (am Pin des ATmega) startet in den CLI / die
    *  Anwendung. Es wird dabei kein FPGA-RESET ausgelöst.
    */


   void commandLineInterface(void);
   /**<
    * \~English
    *  The interface to handle the FPGA configuration, resembnling a command
    *  line.
    *  In case there is a valid bitstream found in the SPI-FLASH the FPGA gets
    *  automatically configured when the function is called.
    *  If there is no bitstream stored already, the interface awaits manual user
    *  activity via the USB. It emulates a CDC and can get controlled by any
    *  serial terminal emulation running on the host.
    *  To clear an existing bitstream, just erase the FLASH.
    *  To enter the manual mode even with a valid bitstream stored (to change
    *  bitsream file) the FPGA needs to get reset to await configuration and a
    *  "magic key" has to be stored into memory location 0x0802 before this
    *  function get called. The "magic key" required is 0x1234.
    *  This function is immediately left when the FPGA has been configured
    *  successfully.
    *  If the automatic configuration fails the CLI waits for user interaction.
    * \note
    *  The Mojo OS is build to handle Xilinx bitream files (.bit) solely. It
    *  will not accept Xilinx binaries (.bin).
    *
    * \~German
    *  Die Schnittstelle für die Verwaltung der FPGA-Konfiguration, die an eine
    *  Kommandozeile erinnert.
    *  Das FPGA wird sofort konfiguriert falls das seriellen FLASH einen
    *  gültigen Bitstream enthält.
    *  Falls noch kein Bitstream gespeichert ist, wird ein Nutzerauftrag von der
    *  USB-Schnittstelle erwartet. Es wird ein CDC-Gerät emuliert und kann daher
    *  von jedem seriellen Terminal auf dem Host bedient werden.
    *  Um einen gültigen Bitstream zu löschen ist das FLASH zu löschen (Erase).
    *  um unbedingt in den manuellen Modus zu gelangen, muss das FPGA einen
    *  RESET erhalten und die Schlüsselsequenz 0x1234 an der RAM-Adresse 0x0802
    *  abgelegt werden bevor die Funktion aufgerufen wird.
    *  Die Funktion wird sofort verlassen wenn das FPGA erfolgreich konfiguriert
    *  wurde.
    *  Falls die automatische Konfiguration fehlschlägt, wartet die
    *  Kommandozeile auf Anweisungen vom Nutzer.
    * \note
    *  Das Mojo OS akzeptiert ausschließlich Xilinx-Bitstream-Dateien (.bit).
    *  Xilinx-Binär-Dateien (.bin) können nicht genutzt werden.
    */


   void applicationLoop(void);
   /**<
    * \~English
    *  Performs communication to the FPGA application logic after the FPGA got
    *  configured. It uses a simple packet structure to the USB host side and
    *  a parallel interface to the FPGA for highest possible transfer speeds.
    *  This is the place to adjust for your own designs and purposes.
    *
    * \~German
    *  Hier wird die Kommunikation übernommen, sobald das FPGA konfiguriert ist.
    *  Eine einfache Paketstruktur kommt auf der Schnittstelle zum USB-Host zum
    *  Einsatz. Daten werden mit der FPGA-Logik für maximale Datenrate über eine
    *  parallele Schnittstelle ausgetauscht.
    */


   void EVENT_USB_Device_Connect(void);
   /**<
    * \~English
    *  Event handler for the library USB Connection event.
    *
    * \~German
    *  Ereignisverarbeitung der Laufzeitbibliothek für USB Connect.
    */


   void EVENT_USB_Device_Disconnect(void);
   /**<
    * \~English
    *  Event handler for the library USB Disconnection event.
    *
    * \~German
    *  Ereignisverarbeitung der Laufzeitbibliothek für USB Disconnect.
    */


   void EVENT_USB_Device_ConfigurationChanged(void);
   /**<
    * \~English
    *  Event handler for the library USB Configuration Changed event.
    *
    * \~German
    *  Ereignisverarbeitung der Laufzeitbibliothek für USB Configuration Changed.
    */


   void EVENT_USB_Device_ControlRequest(void);
   /**<
    * \~English
    *  Event handler for the library USB Control Request reception event.
    *
    * \~German
    *  Ereignisverarbeitung der Laufzeitbibliothek für USB Control Request.
    */


   void EVENT_CDC_Device_ControLineStateChanged(USB_ClassInfo_CDC_Device_t *const CDCInterfaceInfo);
   /**<
    * \~English
    *  CDC class driver callback function processing changes to the virtual
    *  control lines sent from the host.
    *  \param[in] CDCInterfaceInfo  Pointer to the CDC class interface
    *             configuration structure being referenced.
    *
    * \~German
    *  Funktion des CDC Klassentreibers für die Verarbeitung der virtuellen
    *  Kontrollsignale vom Host.
    *  \param[in] CDCInterfaceInfo  Zeiger auf die angewählte Schnittstelle.
    */


   void EVENT_CDC_Device_LineEncodingChanged(USB_ClassInfo_CDC_Device_t *const CDCInterfaceInfo);
   /**<
    * \~English
    *  CDC class driver callback when the line encoding shall change.
    *  A baudrate change down to 1200 starts the bootloader!
    *  https://blog.fsck.com/2014/08/how-to-reboot-an-arduino-leonardo-micro-into-the-bootloader.html
    *  A baudrate change to 2400 performs a hardware reset into the FPGA loader
    *  command line (CLI).
    *  The baudrate setting does not have any effect on the communication speed,
    *  it is just a way to perform special activities of the device.
    *  \param[in] CDCInterfaceInfo  Pointer to the CDC class interface
    *             configuration structure being referenced
    *
    * \~German
    *  Funktion des CDC Klassentreibers für die Einstellung der virtuellen
    *  Baudrate und weiterer Übertragungsparameter.
    *  Ein Wechsel auf 1200 Baud startet den Bootlader!
    *  https://blog.fsck.com/2014/08/how-to-reboot-an-arduino-leonardo-micro-into-the-bootloader.html
    *  Ein Wechsel auf 2400 Baud erwzingt einen FPGA-RESET und startet die
    *  Kommandozeile (CLI) des Mojo OS.
    *  Die Baudrate hat keinen Einfluss auf die Transferrate des USB. Sie dient
    *  hier als Möglichkeit spezielle 'Kommandos' an das Gerät zu senden.
    *  \param[in] CDCInterfaceInfo  Zeiger auf die angewählte Schnittstelle.
    */


#endif
