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
 *   @brief Header file for Descriptors.c.
 *
 *  The header gets included also by the application.
 *
 *  \~German
 *   @brief Datei mit dem Kopfteil zu Descriptors.c.
 *
 *  Diese Datei wird ebenfalls von der Applikation benutzt.
 */


#ifndef _DESCRIPTORS_H_
   #define _DESCRIPTORS_H_

   // Includes:

   #include <LUFA/Drivers/USB/USB.h>
   #include "Config/AppConfig.h"


   // Macros:

   /** Endpoint address of the CDC device-to-host notification IN endpoint. */
   #define CDC_NOTIFICATION_EPADDR        (ENDPOINT_DIR_IN  | 2)

   /** Endpoint address of the CDC device-to-host data IN endpoint. */
   #define CDC_TX_EPADDR                  (ENDPOINT_DIR_IN  | 3)

   /** Endpoint address of the CDC host-to-device data OUT endpoint. */
   #define CDC_RX_EPADDR                  (ENDPOINT_DIR_OUT | 4)

   /** Size in bytes of the CDC device-to-host notification IN endpoint. */
   #define CDC_NOTIFICATION_EPSIZE        8

   /** Size in bytes of the CDC data IN and OUT endpoints. */
   #define CDC_TXRX_EPSIZE                64 // max. 128 or needs 2 banks?


   // Type Defines:

   /**
    * \~English
    *  Type define for the device configuration descriptor structure.
    *  This must be defined in the application code, as the configuration
    *  descriptor contains several sub-descriptors which vary between
    *  devices, and which describe the device's usage to the host.
    *
    *  \~German
    *  Typdefinition der Device Configuration Descriptor Struktur.
    *  Sie muss im Anwendungsteil definiert werden, da sie einige weitere
    *  Deskriptoren enthält die sich zwischen den Geräten unterscheiden können.
    *  Diese Deskriptoren benötigt der USB Host.
    */
   typedef struct
   {
      USB_Descriptor_Configuration_Header_t    Config;

      // CDC Control Interface
      USB_Descriptor_Interface_t               CDC_CCI_Interface;
      USB_CDC_Descriptor_FunctionalHeader_t    CDC_Functional_Header;
      USB_CDC_Descriptor_FunctionalACM_t       CDC_Functional_ACM;
      USB_CDC_Descriptor_FunctionalUnion_t     CDC_Functional_Union;
      USB_Descriptor_Endpoint_t                CDC_NotificationEndpoint;

      // CDC Data Interface
      USB_Descriptor_Interface_t               CDC_DCI_Interface;
      USB_Descriptor_Endpoint_t                CDC_DataOutEndpoint;
      USB_Descriptor_Endpoint_t                CDC_DataInEndpoint;
   } USB_Descriptor_Configuration_t;


   /**
    * \~English
    *  Enum for the device interface descriptor IDs within the device.
    *  Each interface descriptor should have a unique ID index associated
    *  with it, which can be used to refer to the interface from other
    *  descriptors.
    *
    *  \~German
    *  Typdefinition der Device Interface Descriptor Struktur.
    *  Jeder Deskriptor sollte eine eineindeutige ID erhalten um ein bestimmtes
    *  Gerät (Interface) gezielt anzusprechen.
    */
   enum InterfaceDescriptors_t
   {
      INTERFACE_ID_CDC_CCI = 0, /**< CDC CCI interface descriptor ID */
      INTERFACE_ID_CDC_DCI = 1, /**< CDC DCI interface descriptor ID */
   };


   /**
    * \~English
    *  Enum for the device string descriptor IDs within the
    *  device. Each string descriptor shall have a unique ID
    *  index associated with it, for referencing purposes.
    *
    * \~German
    *  Aufzählung der String Descriptor IDs des Gerätes. Jeder
    *  String Deskriptor muss eine einmalige ID haben mit der
    *  er referenziert wird.
    */
   enum StringDescriptors_t
   {
      STRING_ID_Language     = 0, /**< Supported Languages string descriptor ID (must be zero) */
      STRING_ID_Manufacturer = 1, /**< Manufacturer string ID */
      STRING_ID_Product      = 2, /**< Product string ID */
      STRING_ID_SerialNo     = 3, /**< Serial number string ID */
   };


   // Function Prototypes:

   /**
    * \~English
    *  Defines a callback to the framework so that any query from the USB host
    *  will return the approriate answer.
    *
    * \~German
    *  Stellt eine Funktion für das LUFA dar, so dass eine Anfrage vom USB-Host
    *  die gewünschte Antwort liefert.
    */
   uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
                                       const uint16_t wIndex,
                                       const void** const DescriptorAddress)
                                       ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(3);


#endif
