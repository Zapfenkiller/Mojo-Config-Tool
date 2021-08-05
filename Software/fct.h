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
 *   @brief Header for FPGA Configuration Tool (FCT) main.
 *
 *   ...
 *
 *  \~German
 *   @brief .
 *
 *   ...
 */


#ifndef __FCT_H__
   #define __FCT_H__


   // Includes:

   #include <avr/io.h>
   #include "Descriptors.h"


   // Defines:

   #define  MM_APPLICATION_MODE         0
   #define  MM_HELLO                    1
   #define  MM_PROMPT                   2
   #define  MM_LISTEN                   3
   #define  MM_XILINX_TRIGGER_CONFIG    4
   #define  MM_XILINX_CONFIGURE         5


   // Function Prototypes:

   int   main(void);
   /**<
    * \~English
    *
    * \~German
    */


   void EVENT_USB_Device_Connect(void);
   /**<
    * \~English
    *  Event handler for the library USB Connection event.
    *
    * \~German
    */


   void EVENT_USB_Device_Disconnect(void);
   /**<
    * \~English
    *  Event handler for the library USB Disconnection event.
    *
    * \~German
    */


   void EVENT_USB_Device_ConfigurationChanged(void);
   /**<
    * \~English
    *  Event handler for the library USB Configuration Changed event.
    *
    * \~German
    */


   void EVENT_USB_Device_ControlRequest(void);
   /**<
    * \~English
    *  Event handler for the library USB Control Request reception event.
    *
    * \~German
    */


   void EVENT_CDC_Device_ControLineStateChanged(USB_ClassInfo_CDC_Device_t *const CDCInterfaceInfo);
   /**<
    * \~English
    *  CDC class driver callback function processing changes to the virtual
    *  control lines sent from the host.
    *  \param[in] CDCInterfaceInfo  Pointer to the CDC class interface
    *             configuration structure being referenced
    *
    * \~German
    */


   void EVENT_CDC_Device_LineEncodingChanged(USB_ClassInfo_CDC_Device_t *const CDCInterfaceInfo);
   /**<
    * \~English
    *  CDC class driver callback when the line encoding shall change.
    *  A baudrate change down to 1200 starts the bootloader!
    *  https://blog.fsck.com/2014/08/how-to-reboot-an-arduino-leonardo-micro-into-the-bootloader.html
    *  \param[in] CDCInterfaceInfo  Pointer to the CDC class interface
    *             configuration structure being referenced
    *
    * \~German
    */


#endif
