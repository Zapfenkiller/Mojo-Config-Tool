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


#ifndef __UCIF_H__
   #define __UCIF_H__


   // Includes:

   #include "./Config/AppConfig.h"
   #include <avr/io.h>


   // Definitions:

   #define  UCIF_E_SET        (UCIF_E_PORT     |=  (1 << UCIF_E_LINE))     /**< E = '1' */
   #define  UCIF_E_CLR        (UCIF_E_PORT     &= ~(1 << UCIF_E_LINE))     /**< E = '0' */

   #define  UCIF_RW_SET       (UCIF_RW_PORT    |=  (1 << UCIF_RW_LINE))    /**< RW = '1' */
   #define  UCIF_RW_CLR       (UCIF_RW_PORT    &= ~(1 << UCIF_RW_LINE))    /**< RW = '0' */
// #define  UCIF_RW_READ      (UCIF_RW_RET & (1 << UCIF_RW_LINE))          /**< \~English Reads UCIF RW state. \~German Liest den RW Status des UCIF. */

   #define  UCIF_DDR_SET      (UCIF_DDR_PORT   |=  (1 << UCIF_DDR_LINE))   /**< RW = '1' */
   #define  UCIF_DDR_CLR      (UCIF_DDR_PORT   &= ~(1 << UCIF_DDR_LINE))   /**< RW = '0' */

   #define  UCIF_AS_INPUT     (UCIF_DATA_DIR = 0)                          /**< \~English The data port gets input. \~German Macht die Datenleitungen zu Eingängen. */
   #define  UCIF_AS_OUTPUT    (UCIF_DATA_DIR = 0xFF)                       /**< \~English The data port gets output. \~German Macht die Datenleitungen zu Ausgängen. */


   // Function Prototypes:

   void ucifBaseInit(void);
   /**<
    * \~English
    *
    * \~German
    */


#endif
