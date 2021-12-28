//`timescale 1ns / 1ps
////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    11:31:42 01/Jun/2021
// Design Name:
// Module Name:    syncstage
// Project Name:   generic
// Target Devices: any
// Tool versions:
// Description:    Synchronizes input signals into the logic.
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
