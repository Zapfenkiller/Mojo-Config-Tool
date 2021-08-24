//`timescale 1ns / 1ps
////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    11:31:42 01/Jun/2021
// Design Name:
// Module Name:    syncstage
// Project Name:   generic
// Target Devices: XC6SLX9-TQG144
// Tool versions:  Xilinx ISE Webpack 14.7 (any supporting the Xc6SLX9)
// Description:    Synchronizes input signals into the logic
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
// Generic syncstage, 1 clock delay + 1 clock of uncertainty.
// Divided into separate stages to allow for dedicated design constraints.
//
//            _   _   _   _   _   _   _   _   _   _  
// clock  ___/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_
//             __________________
// async  ____/                  \___________________
//                    ___________________
// sync   ___________/                   \___________
//
////////////////////////////////////////////////////////////////////////////////


module syncstage
(
   input                         clock,
   input                         async,

   output reg                    sync = 0
);

   reg         sync_1st = 0;


   always @(posedge clock)
      sync_1st <= async;


   always @(posedge clock)
      sync <= sync_1st;


endmodule
