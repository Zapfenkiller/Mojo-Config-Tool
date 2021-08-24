`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    17:15:14 08/19/2021
// Design Name:
// Module Name:    ucif_demo (top)
// Project Name:   Mojo OS
// Target Devices: XC6SLX9-TQG144
// Tool versions:  Xilinx ISE Webpack 14.7 (any supporting the Xc6SLX9)
// Description:    Sample implementation of ucif plus on-board LED control
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
// Alchitry "Mojo v3":
// -------------------
//
// Board:
//  i_CLOCK       P56  (CLK, 50 MHz)   LVCMOS33, no pull
//  i_BUTTON      P38  (L65N_2)        LVCMOS33, no pull
//  o_LEDS9       P123 (L37N_0)        LVCMOS33, slow, 24 mA
//  o_LEDS8       P124 (L37P_0)        LVCMOS33, slow, 24 mA
//  o_LEDS7       P126 (L36N_0)        LVCMOS33, slow, 24 mA
//  o_LEDS6       P127 (L36P_0)        LVCMOS33, slow, 24 mA
//  o_LEDS5       P131 (L35N_0)        LVCMOS33, slow, 24 mA
//  o_LEDS4       P132 (L35P_0)        LVCMOS33, slow, 24 mA
//  o_LEDS3       P133 (L34N_0)        LVCMOS33, slow, 24 mA
//  o_LEDS2       P134 (L34P_0)        LVCMOS33, slow, 24 mA
//
// ATmega32U4 ucif:
//  io_DATA[7]    P65  (D0)            LVCMOS33, slow, 4 mA, no pull
//  io_DATA[6]    P62  (D1)            LVCMOS33, slow, 4 mA, no pull
//  io_DATA[5]    P61  (D2)            LVCMOS33, slow, 4 mA, no pull
//  io_DATA[4]    P46  (D3)            LVCMOS33, slow, 4 mA, no pull
//  io_DATA[3]    P45  (MISO)          LVCMOS33, slow, 4 mA, no pull
//  io_DATA[2]    P44  (MOSI)          LVCMOS33, slow, 4 mA, no pull
//  io_DATA[1]    P43  (SCK)           LVCMOS33, slow, 4 mA, no pull
//  io_DATA[0]    P48  (SS)            LVCMOS33, slow, 4 mA, no pull
//  i_E           P55  (AVR_TX)        LVCMOS33, no pull
//  i_RW          P59  (AVR_RX)        LVCMOS33, no pull
//  i_DDR         P70  (CCLK)          LVCMOS33, no pull
//                P39  (INIT_B)        LVCMOS33
//
////////////////////////////////////////////////////////////////////////////////


module ucif_demo
(
   input          i_CLOCK,    // On board oscillator, 50 MHz <=> 20 ns
   input          i_BUTTON,   // On board pushbutton, '0' = pushed
   input          i_E,        // µC-Interface (ucif) data strobe
   input          i_RW,       // ucif RD- / WR-control, '0' = WR
   input          i_DDR,      // ucif Double Data Rate, '1' = enable
   inout   [7:0]  io_DATA,    // ucif data lines
   output         o_LED2,     // On board LED, '1' = light
   output         o_LED3,     // On board LED, '1' = light
   output         o_LED4,     // On board LED, '1' = light
   output         o_LED5,     // On board LED, '1' = light
   output         o_LED6,     // On board LED, '1' = light
   output         o_LED7,     // On board LED, '1' = light
   output         o_LED8,     // On board LED, '1' = light
   output         o_LED9      // On board LED, '1' = light
);


   localparam     REGCNT   = 33;       // Register
   localparam     REGSIZE  = 8;        // Bit
   localparam     OSCFREQ  = 50000000; // Hz
   localparam     PWMFREQ  = 100;      // Hz
   localparam     BLINKFRQ = 32;       // Hz

   localparam     PWMSTEPS = 2**REGSIZE;
   localparam     PRESCAL1 = (OSCFREQ / (PWMSTEPS * PWMFREQ)) - 1;
   localparam     PRESCAL2 = (OSCFREQ / (PRESCAL1 * BLINKFRQ)) - 1;
   localparam     PRESIZE1 = clog2(PRESCAL1);
   localparam     PRESIZE2 = clog2(PRESCAL2);
   localparam     ADDRSIZE = clog2(REGCNT);


   wire  [(ADDRSIZE-1):0]  address;
   wire  [(REGSIZE-1):0]   dataToLogic;
   wire                    wrPulse;

   reg   [8:0]             regWrSel = 0;

   reg   [(PRESIZE1-1):0]  prescal1Reg = 0;
   reg   [(PRESIZE2-1):0]  prescal2Reg = 0;
   reg                     clkPulse25k6Hz = 0;
   reg                     clkPulse32Hz = 0;
   reg   [(REGSIZE-1):0]   pwmCounter = 0;

   wire  [(REGSIZE-1):0]   regLed2;
   wire  [(REGSIZE-1):0]   regLed3;
   wire  [(REGSIZE-1):0]   regLed4;
   wire  [(REGSIZE-1):0]   regLed5;
   wire  [(REGSIZE-1):0]   regLed6;
   wire  [(REGSIZE-1):0]   regLed7;
   wire  [(REGSIZE-1):0]   regLed8;
   wire  [(REGSIZE-1):0]   regLed9;

   reg   [(REGSIZE-1):0]   dataFromLogic = 0;


   assign io_DATA = i_RW ? dataFromLogic : {8'bz};


   syncstage e
   (
      .clock      (i_CLOCK),
      .async      (i_E),
      .sync       (e_sync)
   );


   syncstage rw
   (
      .clock      (i_CLOCK),
      .async      (i_RW),
      .sync       (rw_sync)
   );


   syncstage ddr
   (
      .clock      (i_CLOCK),
      .async      (i_DDR),
      .sync       (ddr_sync)
   );


   // Interface to on-board µC
   ucif
   #(
      .dataSize         (REGSIZE),
      .addrSize         (ADDRSIZE)
   )
   ucif1
   (
      // Inputs
      .clock            (i_CLOCK),
      .ddr              (ddr_sync),
      .rw               (rw_sync),
      .e                (e_sync),
      .dataIn           (io_DATA),
      // Outputs
      .address          (address),
      .dataToLogic      (dataToLogic),
      .wrPulse          (wrPulse)
   );


   // Synchronize the button input
   syncstage syncButton
   (
      .clock      (i_CLOCK),
      .async      (i_BUTTON),
      .sync       (button)
   );


   // Clock enable pulse for PWM generator
   always @(posedge i_CLOCK)
      if (prescal1Reg >= PRESCAL1)
      begin
         prescal1Reg <= 0;
         clkPulse25k6Hz <= 1'b1;
      end
      else
      begin
         prescal1Reg <= prescal1Reg + 1'b1;
         clkPulse25k6Hz <= 1'b0;
      end


   // PWM ramp
   always @(posedge i_CLOCK)
      if (clkPulse25k6Hz)
         pwmCounter <= pwmCounter + 1'b1;


   // Clock enable pulse for blink generator
   always @(posedge i_CLOCK)
   begin
      clkPulse32Hz <= 1'b0;
      if (clkPulse25k6Hz)
      begin
         if (prescal2Reg >= PRESCAL2)
         begin
            prescal2Reg <= 0;
            clkPulse32Hz <= 1'b1;
         end
         else
            prescal2Reg <= prescal2Reg + 1'b1;
      end
   end


   // Register write selection
   always @(posedge i_CLOCK)
   begin
      case (address[(ADDRSIZE-1):0])
          0: regWrSel <= 9'b0_0000_0001;
          1: regWrSel <= 9'b0_0000_0001;
          2: regWrSel <= 9'b0_0000_0001;
          3: regWrSel <= 9'b0_0000_0001;
          4: regWrSel <= 9'b0_0000_0010;
          5: regWrSel <= 9'b0_0000_0010;
          6: regWrSel <= 9'b0_0000_0010;
          7: regWrSel <= 9'b0_0000_0010;
          8: regWrSel <= 9'b0_0000_0100;
          9: regWrSel <= 9'b0_0000_0100;
         10: regWrSel <= 9'b0_0000_0100;
         11: regWrSel <= 9'b0_0000_0100;
         12: regWrSel <= 9'b0_0000_1000;
         13: regWrSel <= 9'b0_0000_1000;
         14: regWrSel <= 9'b0_0000_1000;
         15: regWrSel <= 9'b0_0000_1000;
         16: regWrSel <= 9'b0_0001_0000;
         17: regWrSel <= 9'b0_0001_0000;
         18: regWrSel <= 9'b0_0001_0000;
         19: regWrSel <= 9'b0_0001_0000;
         20: regWrSel <= 9'b0_0010_0000;
         21: regWrSel <= 9'b0_0010_0000;
         22: regWrSel <= 9'b0_0010_0000;
         23: regWrSel <= 9'b0_0010_0000;
         24: regWrSel <= 9'b0_0100_0000;
         25: regWrSel <= 9'b0_0100_0000;
         26: regWrSel <= 9'b0_0100_0000;
         27: regWrSel <= 9'b0_0100_0000;
         28: regWrSel <= 9'b0_1000_0000;
         29: regWrSel <= 9'b0_1000_0000;
         30: regWrSel <= 9'b0_1000_0000;
         31: regWrSel <= 9'b0_1000_0000;
         default:
             regWrSel <= 9'b0_0000_0000;
      endcase
   end


   // Register readback
   always @(posedge i_CLOCK)
      case (address[(ADDRSIZE-1):0])
          0: dataFromLogic <= regLed2;
          1: dataFromLogic <= regLed2;
          2: dataFromLogic <= regLed2;
          3: dataFromLogic <= regLed2;
          4: dataFromLogic <= regLed3;
          5: dataFromLogic <= regLed3;
          6: dataFromLogic <= regLed3;
          7: dataFromLogic <= regLed3;
          8: dataFromLogic <= regLed4;
          9: dataFromLogic <= regLed4;
         10: dataFromLogic <= regLed4;
         11: dataFromLogic <= regLed4;
         12: dataFromLogic <= regLed5;
         13: dataFromLogic <= regLed5;
         14: dataFromLogic <= regLed5;
         15: dataFromLogic <= regLed5;
         16: dataFromLogic <= regLed6;
         17: dataFromLogic <= regLed6;
         18: dataFromLogic <= regLed6;
         19: dataFromLogic <= regLed6;
         20: dataFromLogic <= regLed7;
         21: dataFromLogic <= regLed7;
         22: dataFromLogic <= regLed7;
         23: dataFromLogic <= regLed7;
         24: dataFromLogic <= regLed8;
         25: dataFromLogic <= regLed8;
         26: dataFromLogic <= regLed8;
         27: dataFromLogic <= regLed8;
         28: dataFromLogic <= regLed9;
         29: dataFromLogic <= regLed9;
         30: dataFromLogic <= regLed9;
         31: dataFromLogic <= regLed9;
         32: dataFromLogic <= {7'b0, ~button};
         33: dataFromLogic <= pwmCounter;
         default:
             dataFromLogic <= 0;
      endcase


   // Control one LED
   led_ctrl
   #(
      .WIDTH         (8)
   )
   led2
   (
      // Inputs
      .clk           (i_CLOCK),
      .blinkClkPulse (clkPulse32Hz),
      .address       (address[1:0]),
      .data          (dataToLogic),
      .wr            (wrPulse),
      .sel           (regWrSel[0]),
      .ramp          (pwmCounter),
      // Outputs
      .read          (regLed2),
      .drive         (o_LED2)
   );


   led_ctrl
   #(
      .WIDTH         (8)
   )
   led3
   (
      // Inputs
      .clk           (i_CLOCK),
      .blinkClkPulse (clkPulse32Hz),
      .address       (address[1:0]),
      .data          (dataToLogic),
      .wr            (wrPulse),
      .sel           (regWrSel[1]),
      .ramp          (pwmCounter),
      // Outputs
      .read          (regLed3),
      .drive         (o_LED3)
   );


   led_ctrl
   #(
      .WIDTH         (8)
   )
   led4
   (
      // Inputs
      .clk           (i_CLOCK),
      .blinkClkPulse (clkPulse32Hz),
      .address       (address[1:0]),
      .data          (dataToLogic),
      .wr            (wrPulse),
      .sel           (regWrSel[2]),
      .ramp          (pwmCounter),
      // Outputs
      .read          (regLed4),
      .drive         (o_LED4)
   );


   led_ctrl
   #(
      .WIDTH         (8)
   )
   led5
   (
      // Inputs
      .clk           (i_CLOCK),
      .blinkClkPulse (clkPulse32Hz),
      .address       (address[1:0]),
      .data          (dataToLogic),
      .wr            (wrPulse),
      .sel           (regWrSel[3]),
      .ramp          (pwmCounter),
      // Outputs
      .read          (regLed5),
      .drive         (o_LED5)
   );


   led_ctrl
   #(
      .WIDTH         (8)
   )
   led6
   (
      // Inputs
      .clk           (i_CLOCK),
      .blinkClkPulse (clkPulse32Hz),
      .address       (address[1:0]),
      .data          (dataToLogic),
      .wr            (wrPulse),
      .sel           (regWrSel[4]),
      .ramp          (pwmCounter),
      // Outputs
      .read          (regLed6),
      .drive         (o_LED6)
   );


   led_ctrl
   #(
      .WIDTH         (8)
   )
   led7
   (
      // Inputs
      .clk           (i_CLOCK),
      .blinkClkPulse (clkPulse32Hz),
      .address       (address[1:0]),
      .data          (dataToLogic),
      .wr            (wrPulse),
      .sel           (regWrSel[5]),
      .ramp          (pwmCounter),
      // Outputs
      .read          (regLed7),
      .drive         (o_LED7)
   );


   led_ctrl
   #(
      .WIDTH         (8)
   )
   led8
   (
      // Inputs
      .clk           (i_CLOCK),
      .blinkClkPulse (clkPulse32Hz),
      .address       (address[1:0]),
      .data          (dataToLogic),
      .wr            (wrPulse),
      .sel           (regWrSel[6]),
      .ramp          (pwmCounter),
      // Outputs
      .read          (regLed8),
      .drive         (o_LED8)
   );


   led_ctrl
   #(
      .WIDTH         (8)
   )
   led9
   (
      // Inputs
      .clk           (i_CLOCK),
      .blinkClkPulse (clkPulse32Hz),
      .address       (address[1:0]),
      .data          (dataToLogic),
      .wr            (wrPulse),
      .sel           (regWrSel[7]),
      .ramp          (pwmCounter),
      // Outputs
      .read          (regLed9),
      .drive         (o_LED9)
   );


   // Conveninece function
   function integer clog2;
      input integer argument;
      begin
         argument = argument - 1;
         for (clog2 = 0; argument > 0; clog2 = clog2 + 1)
            argument = argument >> 1;
      end
   endfunction


endmodule
