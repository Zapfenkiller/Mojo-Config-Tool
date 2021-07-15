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

ATTENTION:
  Any license expressly does **not** include the VID/PID used.
  This has been donated to me, René Trapp, by Microchip Technology Inc.
  for sole use within my experiments. Your modifications either stay fully
  unpublished on your lab bench, or you get your own VID/PID
  combination before publishing / selling your contraption.
*/


/** @file
 *  \~English
 *   @brief Here are the USB-Userport device descriptors.
 *
 *  USB Device Descriptors, for LUFA use when in USB device mode.
 *  Descriptors are special computer-readable structures. The host
 *  requests those upon device enumeration, to determine the
 *  device's capabilities and functions.
 *
 *  \~German
 *   @brief Hier befinden sich die USB-Device-Deskriptoren des
 *   USB-Userport.
 *
 *  Die LUFA-Bibliothek und der Host benötigen die
 *  Device-Deskriptoren zur Definition des USB-Gerätes.
 *  Deskriptoren sind spezielle maschinenlesbare Strukturen. Der
 *  Host fordert sie an wenn er das Gerät enumeriert, um dessen
 *  Eigenschaften und Funktionen zu bestimmen.
 */


#include "Descriptors.h"
#include <avr/pgmspace.h>


const USB_Descriptor_Device_t PROGMEM DeviceDescriptor =
{
   .Header                 = {.Size = sizeof(USB_Descriptor_Device_t), .Type = DTYPE_Device},

   .USBSpecification       = VERSION_BCD(1,1,0),
   .Class                  = CDC_CSCP_CDCClass,
	.SubClass               = CDC_CSCP_NoSpecificSubclass,   // 0x00  'None'
	.Protocol               = CDC_CSCP_NoSpecificProtocol,   // 0x00  'None'

   .Endpoint0Size          = FIXED_CONTROL_ENDPOINT_SIZE,   // ./Config/LUFAConfig.h

	.VendorID               = 0x2341,                        // Atmel Corp.?
	.ProductID              = 0x8036,                        // Arduino Leonardo (application)
   .ReleaseNumber          = VERSION_BCD(                   // ./Config/AppConfig.h
                              RELEASE_MAJOR,                // ./Config/AppConfig.h
                              RELEASE_MINOR,                // ./Config/AppConfig.h
                              RELEASE_REVISION              // ./Config/AppConfig.h
                             ),

   .ManufacturerStrIndex   = STRING_ID_Manufacturer,        // 0x01 ./Descriptors.h
   .ProductStrIndex        = STRING_ID_Product,             // 0x02 ./Descriptors.h
   .SerialNumStrIndex      = STRING_ID_SerialNo,            // 0x03 ./Descriptors.h

   .NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS       // ./Config/LUFAConfig.h
};
/**<
 * \~ Device Descriptor structure.
 *
 * \~English
 *  This descriptor, located in FLASH memory, describes the overall
 *  device characteristics, including the supported USB version,
 *  control endpoint size and the number of device configurations.
 *  The descriptor is read out by the USB host when the enumeration
 *  process begins. See also
 *  [USBIF HID Specification](https://www.usb.org/sites/default/files/documents/hid1_11.pdf),
 *  page 76.
 *
 * \~German
 *  Der Device Deskriptor beschriebt die globalen Eigneschaften des
 *  USB-Gerätes, unter anderem die unterstützte USB-Version, Größe
 *  des Kontrollendpunktes und Anzahl der Geräte-Konfigurationen.
 *  Der Deskriptor wird zu Beginn der Enumeration vom Host
 *  angefordert. Siehe auch die
 *  [USBIF HID Specification](https://www.usb.org/sites/default/files/documents/hid1_11.pdf),
 *  Seite 76.
 *  Der Deskriptor liegt im FLASH des Controllers.
 */


const USB_Descriptor_Configuration_t PROGMEM ConfigurationDescriptor =
{
   .Config =
      {
         .Header                 = {.Size = sizeof(USB_Descriptor_Configuration_Header_t), .Type = DTYPE_Configuration},

         .TotalConfigurationSize = sizeof(USB_Descriptor_Configuration_t),
         .TotalInterfaces        = 2,

         .ConfigurationNumber    = 1,
         .ConfigurationStrIndex  = NO_DESCRIPTOR,

         .ConfigAttributes       = (USB_CONFIG_ATTR_RESERVED | USB_CONFIG_ATTR_SELFPOWERED),

         .MaxPowerConsumption    = USB_CONFIG_POWER_MA(MAX_CURRENT_DRAW) // ./Config/AppConfig.h
      },

   .CDC_CCI_Interface =
      {
         .Header                 = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

         .InterfaceNumber        = INTERFACE_ID_CDC_CCI,
         .AlternateSetting       = 0,

         .TotalEndpoints         = 1,

         .Class                  = CDC_CSCP_CDCClass,
         .SubClass               = CDC_CSCP_ACMSubclass,
         .Protocol               = CDC_CSCP_ATCommandProtocol,

         .InterfaceStrIndex      = NO_DESCRIPTOR
      },

   .CDC_Functional_Header =
      {
         .Header                 = {.Size = sizeof(USB_CDC_Descriptor_FunctionalHeader_t), .Type = DTYPE_CSInterface},
         .Subtype                = CDC_DSUBTYPE_CSInterface_Header,

         .CDCSpecification       = VERSION_BCD(1,1,0),
      },

   .CDC_Functional_ACM =
      {
         .Header                 = {.Size = sizeof(USB_CDC_Descriptor_FunctionalACM_t), .Type = DTYPE_CSInterface},
         .Subtype                = CDC_DSUBTYPE_CSInterface_ACM, // 0x02
                                   // = Abstract Control Management Functional Descriptor

         .Capabilities           = 0x06, // bmCapabilities
                                   //= Device supports the request Send_Break,
                                   // Device supports the request combination of
                                   //  Set_Line_Coding,
                                   //  Set_Control_Line_State,
                                   //  Get_Line_Coding, and the
                                   //  notification Serial_State
      },

   .CDC_Functional_Union =
      {
         .Header                 = {.Size = sizeof(USB_CDC_Descriptor_FunctionalUnion_t), .Type = DTYPE_CSInterface},
         .Subtype                = CDC_DSUBTYPE_CSInterface_Union,

         .MasterInterfaceNumber  = INTERFACE_ID_CDC_CCI,
         .SlaveInterfaceNumber   = INTERFACE_ID_CDC_DCI,
      },

   .CDC_NotificationEndpoint =
      {
         .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

         .EndpointAddress        = CDC_NOTIFICATION_EPADDR,
         .Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
         .EndpointSize           = CDC_NOTIFICATION_EPSIZE, // ./Descriptors.h
         .PollingIntervalMS      = 0xFF
      },

   .CDC_DCI_Interface =
      {
         .Header                 = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

         .InterfaceNumber        = INTERFACE_ID_CDC_DCI,
         .AlternateSetting       = 0,

         .TotalEndpoints         = 2,

         .Class                  = CDC_CSCP_CDCDataClass,
         .SubClass               = CDC_CSCP_NoDataSubclass,
         .Protocol               = CDC_CSCP_NoDataProtocol,

         .InterfaceStrIndex      = NO_DESCRIPTOR
      },

   .CDC_DataOutEndpoint =
      {
         .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

         .EndpointAddress        = CDC_RX_EPADDR,
         .Attributes             = (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
         .EndpointSize           = CDC_TXRX_EPSIZE, // ./Descriptors.h
         .PollingIntervalMS      = POLLING_INTERVAL // ./Config/AppConfig.h
      },

   .CDC_DataInEndpoint =
      {
         .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

         .EndpointAddress        = CDC_TX_EPADDR,
         .Attributes             = (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
         .EndpointSize           = CDC_TXRX_EPSIZE, // ./Descriptors.h
         .PollingIntervalMS      = POLLING_INTERVAL // ./Config/AppConfig.h
      }
};
/**<
 * \~ Configuration Descriptor structure.
 *
 * \~English
 *  This descriptor, located in FLASH memory, describes the usage
 *  of the device in one of its supported configurations, including
 *  information about any device interfaces and endpoints. The
 *  descriptor is read out by the USB host during the enumeration
 *  process when selecting a configuration so that the host may
 *  correctly communicate with the USB device.
 *
 * \~German
 *  Dieser Deskriptor beschreibt eine der unterstützen
 *  Konfigurationen des Gerätes. Dazu gehören die Interfaces und
 *  weitere Endpunkte. Der Deskriptor wird vom Host während der
 *  Enummerierung angefordert. Die Kommunikation zwischen Host und
 *  Gerät basiert auf der gewählten Konfiguration.
 *  Der Deskriptor liegt im FLASH des Controllers.
 */


const USB_Descriptor_String_t PROGMEM LanguageString = USB_STRING_DESCRIPTOR_ARRAY(LANGUAGE_ID_ENG);
/**<
 * \~ Language Descriptor structure.
 *
 * \~English
 *  This descriptor, located in FLASH memory, is returned when the
 *  host requests the string descriptor with index 0 (the first
 *  index). It is actually an array of 16-bit integers, which
 *  indicate what languages the device supports for its string
 *  descriptors.
 *  The language ID table (USB_LANGIDs.pdf) is no longer available
 *  at USB.org, unfortunately.
 *
 * \~German
 *  Wenn der Host den String mit Index 0 anfordert, wird dieser
 *  Deskriptor gesendet. Es ist ein Array aus 16-Bit Werten, die
 *  anzeigen welche Sprachen das Gerät mit seinen weiteren String
 *  Deskriptoren unterstützt. Die Tabelle der Sprach-IDs ist bei
 *  USB.org leider nicht mehr verfügbar (USB_LANGIDs.pdf).
 *  Der Deskriptor liegt im FLASH des Controllers.
 */


const USB_Descriptor_String_t PROGMEM ManufacturerString = USB_STRING_DESCRIPTOR(MANUFACTURER_DESCR_STRING);
// ./Config/AppConfig.h
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
 *  Dieser Unicode-String enthält den menschenlesbaren Namen des
 *  Geräte-Herstellers.
 *  Der Text wird dem Host auf entsprechende Anforderung gesendet.
 *  Die zugehörige String-ID ist im Device Deskriptor abgelegt.
 *  Der Text liegt im FLASH des Controllers.
 */


const USB_Descriptor_String_t PROGMEM ProductString = USB_STRING_DESCRIPTOR(PRODUCT_DESCR_STRING);
// ./Config/AppConfig.h
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
 *  Dieser Unicode-String enthält menschenlesbare Angaben zum
 *  Produkt. Meist ist es nur der Gerätename.
 *  Der Text wird dem Host auf entsprechende Anforderung gesendet.
 *  Die zugehörige String-ID ist im Device Deskriptor abgelegt.
 *  Der Text liegt im FLASH des Controllers.
 */


const USB_Descriptor_String_t PROGMEM SerialString = USB_STRING_DESCRIPTOR(DEVICE_SERIAL_NUMBER);
// ./Config/AppConfig.h
/**<
 * \~ Serial number descriptor string.
 *
 * \~English
 *  This is a Unicode string containing the serial number in human
 *  readable form, and is read out upon request by the host when
 *  the appropriate string ID is requested, listed in the Device
 *  Descriptor.
 *
 * \~German
 *  Dieser Unicode-String enthält die menschenlesbare Seriennummer
 *  des Gerätes.
 *  Der Text wird dem Host auf entsprechende Anforderung gesendet.
 *  Die zugehörige String-ID ist im Device Deskriptor abgelegt.
 *  Der Text liegt im FLASH des Controllers.
 */


uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
                                    const uint16_t wIndex,
                                    const void** const DescriptorAddress)
{
   const uint8_t  DescriptorType   = (wValue >> 8);
   const uint8_t  DescriptorNumber = (wValue & 0xFF);

   const void* Address = NULL;
   uint16_t    Size    = NO_DESCRIPTOR;

   switch (DescriptorType)
   {
      case DTYPE_Device:
         Address = &DeviceDescriptor;
         Size    = sizeof(USB_Descriptor_Device_t);
         break;
      case DTYPE_Configuration:
         Address = &ConfigurationDescriptor;
         Size    = sizeof(USB_Descriptor_Configuration_t);
         break;
      case DTYPE_String:
         switch (DescriptorNumber)
         {
            case STRING_ID_Language:
               Address = &LanguageString;
               Size    = pgm_read_byte(&LanguageString.Header.Size);
               break;
            case STRING_ID_Manufacturer:
               Address = &ManufacturerString;
               Size    = pgm_read_byte(&ManufacturerString.Header.Size);
               break;
            case STRING_ID_Product:
               Address = &ProductString;
               Size    = pgm_read_byte(&ProductString.Header.Size);
               break;
            case STRING_ID_SerialNo:
               Address = &SerialString;
               Size    = pgm_read_byte(&SerialString.Header.Size);
               break;
         }
         break;
   }
   *DescriptorAddress = Address;
   return Size;
}
/**<
 * \~English
 *  is called by the LUFA library to get size and address of the
 *  desired descriptor in case the host issued a Get Descriptor
 *  request. See library "USB Descriptors" documentation.
 *
 * \~German
 *  wird von der LUFA-Bibliothek benötigt um dem anfragenden Host
 *  den passenden Deskriptor zurückzuliefern. Dazu ist jeweils die
 *  Adresse und die Größe des Deskriptors an das LUFA zurück zu
 *  geben. Siehe auch die LUFA-Dokumentation zu "USB Descriptors".
 */
