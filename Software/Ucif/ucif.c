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
 *   @brief Mojo Operating System ("Mojo OS").
 *
 *  \~English
 *   My approach to use any terminal to perform the base activities, configure
 *   the FPGA from a bitstream file (.bit) and allow for topspeed exchange of
 *   data with the FPGA application logic by a dedicated parallel interface.
 *
 *  \~German
 *   Mein Ansatz um ein beliebiges Terminalprogramm für die fundamentalen
 *   Aktivitäten zu nutzen, das FPGA aus einer Bitstream-Datei (.bit) zu
 *   konfigurieren und durch eine angepasste parallele Schnittstelle die
 *   schnellste Datenübertragung von und zur Anwendungslogik im FPGA zu
 *   erzielen.
 */


#include <avr/io.h>

#include "Config/AppConfig.h"
#include "./ucif.h"


#define  UCIF_E_DRIVE      (UCIF_E_DIR      |=  (1 << UCIF_E_LINE))     /**< \~English Defines E as output to the UCIF. \~German Definiert E als Ausgang zum UCIF. */
#define  UCIF_RW_DRIVE     (UCIF_RW_DIR     |=  (1 << UCIF_RW_LINE))    /**< \~English Defines RW as output to the UCIF. \~German Definiert RW als Ausgang zum UCIF. */
#define  UCIF_DDR_DRIVE    (UCIF_DDR_DIR    |=  (1 << UCIF_DDR_LINE))   /**< \~English Defines DDR as output to the UCIF. \~German Definiert DDR als Ausgang zum UCIF. */


void ucifBaseInit(void)
{
   UCIF_RW_CLR;
   UCIF_RW_DRIVE;
   UCIF_DATA_PORT = 0;
   UCIF_AS_OUTPUT;
   UCIF_E_CLR;
   UCIF_E_DRIVE;
   UCIF_DDR_CLR;
   UCIF_DDR_DRIVE;
}
