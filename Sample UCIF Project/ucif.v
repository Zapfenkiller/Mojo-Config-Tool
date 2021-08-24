//`timescale 1ns / 1ps
////////////////////////////////////////////////////////////////////////////////
// Company:
// Engineer:
// 
// Create Date:    20:09:37 30/Jun/2021
// Design Name:
// Module Name:    ucif
// Project Name:   Mojo OS
// Target Devices: XC6SLX9-TQG144
// Tool versions:  Xilinx ISE Webpack 14.7 (any supporting the Xc6SLX9)
// Description:    Microcontroller Interface (µC-IF => ucif)
//
// License:
// --------
// Copyright 2021  René Trapp (rene [dot] trapp (-at-) web [dot] de)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
// Dependencies:
//
// Additional Comments:
//
// Cycle Catalogue:
// ----------------
//                Set Address      WR Data        RD DATA
//                                                _______  
// rw             ___________    ___________    _/       \_
//                      _____    _____          __       __
// e              _____/              \_____    __*******__
//                    ___            ___                   
// data[7:0] µC   ---<___>---    ---<___>---    -----------
//                     :              :           _______  
// data[7:0] FPGA -----------    -----------    -<_______>-
//                _____:_____    ___________    ___________
// address        _____X_____    ___________    ___________
//                ___________    _____:_____    ___________
// dataIn[7:0]    ___________    _____X_____    ___________
//                                     _
// wr_data        ___________    _____/ \___    ___________
//
// While RW is '0' a rising edge on E latches an address, a falling edge latches
// data. While RW is '1' edges on E are ignored.
//
//
// Double Data Rate Cycle Catalogue:
// ---------------------------------
//                  WR Data        WR Data
//
// rw             ___________    ___________
//                      _____    _____      
// e              _____/              \_____
//                    ___            ___    
// data[7:0] µC   ---<___>---    ---<___>---
//                _____:_____    _____:_____
// dataIn[7:0]    _____X_____    _____X_____
//                     _              _
// wr_data        ____/ \____    ____/ \____
//
// Any edge on E latches data while RW is '0'.
//
//
// Basic Cyle Combinations:
// ------------------------
//                   Register WR          Register RD        Register RD + WR
//                                                ____             ___
// rw             _________________    __________/    \_    ______/   \______
//                      _______              _______            _________
// e              _____/       \___    _____/       \___    ___/         \___
//                  ____ ________        ____                 ___       ___
// data[7:0] µC   -<____X________>-    -<____>----------    -<___>-----<___>-
//                      :      :            :     ____         :   ___     :
// data[7:0] FPGA -----------------    ----------<____>-    ------<___>------
//                ______:__________    _____:___________    ___:_____________
// address        ______X__________    _____X___________    ___X_____________
//                _____________:___    _________________    _____________:___
// dataIn[7:0]    _____________X___    _________________    _____________X___
//                              _                                         _
// wr_data        _____________/ \_    _________________    _____________/ \_
//
//
// Advanced Cyle Combination (Example):
// ------------------------------------
//                   Sequential Random Register WR
//
// rw             ___________________________________
//                    ___     ___     ___     ___
// e              ___/   \___/   \___/   \___/   \___
//                  ___ ___ ___ ___ ___ ___ ___ ___
// data[7:0] µC   -<___X___X___X___X___X___X___X___>-
//                   :   :   :   :   :   :   :   :
// data[7:0] FPGA -----------------------------------
//                ___:_______:_______:_______:_______
// address        ___X_______X_______X_______X_______
//                _______:_______:_______:_______:___
// dataIn[7:0]    _______X_______X_______X_______X___
//                        _       _       _       _
// wr_data        _______/ \_____/ \_____/ \_____/ \_
//
//
// Double Data Rate Write Access:
// ------------------------------
//
// rw             ___________________________________
//                    ___     ___     ___     ___
// e              ___/   \___/   \___/   \___/   \___
//                  ___ ___ ___ ___ ___ ___ ___ ___
// data[7:0] µC   -<___X___X___X___X___X___X___X___>-
//                   :   :   :   :   :   :   :   :
// data[7:0] FPGA -----------------------------------
//                ___:___:___:___:___:___:___:___:___
// dataIn[7:0]    ___X___X___X___X___X___X___X___X___
//                    _   _   _   _   _   _   _   _
// wr_data        ___/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_
//
//
// Timing Constraints (Clock = 50 MHz):
// ------------------------------------
//
// All figures are worst case, assuming one full clock loss. Best case is
// immediately in front of the next clock edge and thus 20 ns less. Still this
// estimation is conservative, the FPGA setup and hold times are less than
// my 20 ns assumption. Still with these figures any 20 MHz AVR should
// interface to the ucif.
//
//   Parameter    ! Symbol ! Min/ns !  Max/ns
// ---------------+--------+--------+---------
// Setup time     ! tsu    !  -20   ! infinite
// Hold time      ! th     !   60   ! infinite
// RW setup time  ! trw    !   20   ! infinite
// RW hold time   ! t1     !   20   ! infinite
// Response time  ! trsp   !    0   !    10
// Release time   ! trls   !    0   !    10
// Delay time     ! tdly   !   90   !    *)
// Pulse time     ! tp     !   40   ! infinite
//
// *) Maximum Delay time for addressed register data to arrive on the outputs
// depends on the inner structure of the register file and the pipelined logic.
// Currently assumed to take one additional clock after address arrives in
// dedicated register, add 20 ns per extra clock.
//
// Register WR:
// ------------
//                            _____________________
// e              ___________/                     \_____________________
//                           |<------- tp -------->|
//                           :                     :
// rw             _______________________________________________________
//                           :                     :
//                  __________________    __________________
// data[7:0] µC   -<______Address_____XXXX_______Data_______>------------
//                 |<- tsu ->|<- th ->|  |<- tsu ->|<- th ->|
//
// Register RD:
// ------------
//                            _____________________
// e              ___________/                     \_____________________
//                           |<------- tp -------->|
//                           :           |<----- tp ------->|
//                           :            __________________
// rw             _______________________/         :        \____________
//                 |<- tsu ->|<- th ->|  |<- trw ->|<- t1 ->|
//                  __________________   :                  :
// data[7:0] µC   -<______Address_____>----------------------------------
//                 |<- tsu ->|<- th ->|  :                  :
//                                       :           __________________
// data[7:0] FPGA ----------------------------------<___X____Data______>-
//                                       |<- trsp ->|   :   |<- trls ->|
//                           |<--------- tdly --------->|
//
// Failure in timing of RW pulse with respect to E pulse leads to unintended
// data written to the addressed location.
//
// An AVR could achieve 1 register access per 13 CPU clocks. This leads to
// roughly 4923 kbit/s (615 kByte/s) when running off 8 MHz. A sequence of 256
// register addresses could get written in one burst transfer from a buffer,
// taking approximately 416 µs.
//
//
// DDR WR cycle:
// -------------
//                            _____________________
// e              ___________/                     \_____________________
//                           |<------- tp -------->|
//                           :                     :
// rw             _______________________________________________________
//                           :                     :
//                  __________________    __________________
// data[7:0] µC   -<______Address_____XXXX_______Data_______>------------
//                 |<- tsu ->|<- th ->|  |<- tsu ->|<- th ->|
//
// An AVR could achieve 2 Bytes DDR transfer per 13 CPU clocks. This leads to
// roughly 9846 kbit/s (1230 kByte/s) when running off 8 MHz. A sequence of 512
// bytes could get written in one burst transfer from a buffer, taking
// approximately 416 µs.
//
// Things scale linear with the AVR clock speed. The ucif, running off 50 MHz,
// supports AVR up to 20 MHz. Thus the DDR peak throughput can get as high as
// 24615 kbit/s. Since the AVR has to transfer the data between two terminal
// points the effective peak throughput will get no more than just half of this
// figure within a completed application - as a rule of thumb.
//
// By the way:
// Topmost SPI speed is cut to half of the CPU clock frequency. With the
// on-board ATmega running off 8 MHz the SPI throughput is 4000 kbit/s.
//
////////////////////////////////////////////////////////////////////////////////


module ucif
#(
   parameter   dataSize          = 8,
   parameter   addrSize          = 2
)
(
   input                         clock,
   input                         ddr,
   input                         rw,
   input                         e,
   input       [(dataSize-1):0]  dataIn,

   output reg  [(addrSize-1):0]  address = 0,
   output reg  [(dataSize-1):0]  dataToLogic = 0,
   output reg                    wrPulse = 0
);


   reg         e_old = 0;


   // For the edge detection
   always @(posedge clock)
      e_old <= e;


   // Set address
   always @(posedge clock)
      if (e & ~e_old & ~rw & ~ddr)
         address <= dataIn[(addrSize-1):0];


   // WR data
   always @(posedge clock)
   begin
      wrPulse <= 1'b0;
      if (~ddr)
      begin
         if (~e & e_old & ~rw)
         begin
            dataToLogic <= dataIn[(dataSize-1):0];
            wrPulse <= 1'b1;
         end
      end
      else
         if ((e ^ e_old) & ~rw)
         begin
            dataToLogic <= dataIn[(dataSize-1):0];
            wrPulse <= 1'b1;
         end
   end


endmodule
