`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    06:50:29 08/20/2021
// Design Name:
// Module Name:    led_ctrl
// Project Name:   Mojo OS
// Target Devices: XC6SLX9-TQG144
// Tool versions:  Xilinx ISE Webpack 14.7 (any supporting the Xc6SLX9)
// Description:    Controls on board LEDs
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
// Register Model:
// ---------------
//
// Addr !   7   !   6   !   5   !   4   !   3   !   2   !   1   !   0   ! Function
// -----+-------+-------+-------+-------+-------+-------+-------+-------+------------------
//   0  !   0   !   0   !   0   !   0   !   0   !   0   !   M1  !   M0  ! LED Mode
//   1  !  POF7 !  POF6 !  POF5 !  POF4 !  POF3 !  POF2 !  POF1 !  POF0 ! PWM when LED Off
//   2  !  PON7 !  PON6 !  PON5 !  PON4 !  PON3 !  PON2 !  PON1 !  PON0 ! PWM when LED On
//   3  !   0   !   0   !   0   !   0   !   BL3 !   BL2 !   BL1 !   BL0 ! Blink Frequency
// All bits default to '0'.
//
// LED Mode:
// ---------
//
//  M1  !  M0 !
// -----!-----!------------------------------------------------
//   0  !   0 ! Totally Dark
//   0  !   1 ! Dim to PWM Off
//   1  !   0 ! Blinking between PWM Off and PWM On brightness
//   1  !   1 ! Dim to PWM On
// The blink frequency is controlled by BL3:0.
//
// PWM Values:
// -----------
//
// 0..255, where 0 means totally dark and 255 means fully bright. The control is
// linear; keep in mind the human eye exhibits non-linear response.
//
// Blink Frequency:
// ----------------
//
// Value range is 0 to 15 giving 0.5 to 8 Hz
// f = 0.5 Hz * (BL + 1)
//
//////////////////////////////////////////////////////////////////////////////////


module led_ctrl
#(
   parameter   WIDTH             =  4
)
(
   input                         clk,
   input                         blinkClkPulse,
   input       [1:0]             address,
   input       [(WIDTH-1):0]     data,
   input                         wr,
   input                         sel,
   input       [(WIDTH-1):0]     ramp,

   output reg  [(WIDTH-1):0]     read = 0,
   output reg                    drive = 0
);


   reg         [1:0]             ledMode = 0;
   reg         [(WIDTH-1):0]     pwmOnVal = 0;
   reg         [(WIDTH-1):0]     pwmOffVal = 0;
   reg         [3:0]             blinkFreq = 0;
   reg         [(WIDTH-1):0]     pwmIn = 0;
   reg         [5:0]             blink = 0;


   always @(posedge clk)
      if (wr & sel)
         case (address)
            0: ledMode <= data[1:0];
            1: pwmOffVal <= data[(WIDTH-1):0];
            2: pwmOnVal <= data[(WIDTH-1):0];
            3: blinkFreq <= data[3:0];
         endcase


   always @(*)
      case (address)
         0: read = {{(WIDTH-2){1'b0}}, ledMode};
         1: read = pwmOffVal;
         2: read = pwmOnVal;
         3: read = {{(WIDTH-4){1'b0}}, blinkFreq};
      endcase


   always @(posedge clk)
      if (blinkClkPulse)
         blink <= blink + blinkFreq + 1'b1;


   always @(posedge clk)
      case (ledMode)
         2'b00:   pwmIn <= 0;
         2'b01:   pwmIn <= pwmOffVal;
         2'b10:   pwmIn <= blink[5] ? pwmOnVal : pwmOffVal;
         2'b11:   pwmIn <= pwmOnVal;
      endcase


   always @(posedge clk)
      if (pwmIn > ramp)
         drive <= 1'b1;
      else
         drive <= 1'b0;


endmodule
