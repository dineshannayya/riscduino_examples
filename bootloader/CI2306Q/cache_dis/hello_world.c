/*
 * SPDX-FileCopyrightText: 2020 Efabless Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * SPDX-License-Identifier: Apache-2.0
 */
/***********************************************************************
    Author: Dinesh Annayya
    This Caravel C code will be used to wakeup the Riscduino core.
    Caravel CPU does following action
       1. Drive the Configuration for Uart Master for 57600 Baud Rate
       2. Set the System Strap with Cache Bypass Mode, 
       3. Flash SPI in Quad Bit Mode
       4. SRAM SPI in Single Bit Mode
 
    On Power Up, it will wait for 20 Second to see any external programming will
    be configuring the riscduino. It will check reg_mprj_wbhost_ctrl  bit [31] =1; 
    If there is no indication on this, then caravel cpu will remove the riscduino cpu.


   Revision:
    0.1   - 1 Jun 2023 , Dinesh A
            Initial Verson
    0.2  - 18 Aug 2023, Dinesh A
           As Keypad test, need input pull row, we are using caravel function to configure the pads
           digital_io[16:13] are declared as input with pull-up
    0.3 -  16 Sept 2023, Dinesh A
           A. system strap set to cache disable 
           B. Quad SPI mode enable for Flash and SRAM I/F

**************************************************************************/
/************************************************************
system strap decoding
     bit[1:0] - System Clock Source Selection for wbs
                 00 - User clock1 
                 01 - User clock2 
                 10 - Internal PLL
                 11 - Xtal
     bit[3:2] - Clock Division for wbs
                 00 - 0 Div
                 01 - 2 Div
                 10 - 4 Div
                 11 - 8 Div
     bit[5:4] - System Clock Source Selection for riscv
                 00 - User clock1 
                 01 - User clock2 
                 10 - Internal PLL
                 11 - Xtal
     bit[7:6] - Clock Division for riscv
                 00 - 0 Div
                 01 - 2 Div
                 10 - 4 Div
                 11 - 8 Div
     bit [8]   - uart master config control
                 0   - load from LA
                 1   - constant value based on system clock selection
     bit [9]   - QSPI SRAM Mode Selection CS#2
                 1'b0 - Single
                 1'b1 - Quad
     bit [11:10] - QSPI FLASH Mode Selection CS#0
                 2'b00 - Single
                 2'b01 - Double
                 2'b10 - Quad
                 2'b11 - QDDR
     bit [12]  - Riscv Reset control
                 0 - Keep Riscv on Reset
                 1 - Removed Riscv on Power On Reset
     bit [13]   - Riscv Cache Bypass
                 0 - Cache Enable
                 1 - Bypass cache
     bit [14]  - Riscv SRAM clock edge selection
                 0 - Normal
                 1 - Invert
     bit [15]    -  Soft Reboot Request
     bit [17:16] -  cfg_cska_wi Skew selection      
     bit [19:18] -  cfg_cska_wh Skew selection        
     bit [21:20] -  cfg_cska_riscv Skew selection      
     bit [23:22] -  cfg_cska_qspi  Skew selection      
     bit [25:24] -  cfg_cska_uart  Skew selection       
     bit [27:26] -  cfg_cska_pinmux Skew selection     
     bit [29:28] -  cfg_cska_qspi_co Skew selection    
********************************************************/

// This include is relative to $CARAVEL_PATH (see Makefile)
#include <defs.h>
#include <stub.h>
#include "ext_reg_map.h"

/*
	Wishbone Test:
		- Configures MPRJ lower 8-IO pins as outputs
		- Checks counter value through the wishbone port
*/
int i = 0; 
int clk = 0;

void delay(const int d)
{

    /* Configure timer for a single-shot countdown */
	reg_timer0_config = 0;
	reg_timer0_data = d;
    reg_timer0_config = 1;

    // Loop, waiting for value to reach zero
   reg_timer0_update = 1;  // latch current value
   while (reg_timer0_value > 0) {
           reg_timer0_update = 1;
   }

}

void putdword(uint32_t Data)
{
	reg_uart_data = Data >> 24; // MSB [31:24];
	reg_uart_data = Data >> 16; // MSB [23:16];
	reg_uart_data = Data >> 8;  // MSB [15:8];
	reg_uart_data = Data;       // MSB [7:0];
	delay(100000);
}


void configure_io()
{

//  ======= Useful GPIO mode values =============

//      GPIO_MODE_MGMT_STD_INPUT_NOPULL
//      GPIO_MODE_MGMT_STD_INPUT_PULLDOWN
//      GPIO_MODE_MGMT_STD_INPUT_PULLUP
//      GPIO_MODE_MGMT_STD_OUTPUT
//      GPIO_MODE_MGMT_STD_BIDIRECTIONAL
//      GPIO_MODE_MGMT_STD_ANALOG

//      GPIO_MODE_USER_STD_INPUT_NOPULL
//      GPIO_MODE_USER_STD_INPUT_PULLDOWN
//      GPIO_MODE_USER_STD_INPUT_PULLUP
//      GPIO_MODE_USER_STD_OUTPUT
//      GPIO_MODE_USER_STD_BIDIRECTIONAL
//      GPIO_MODE_USER_STD_ANALOG


//  ======= set each IO to the desired configuration =============

    //  GPIO 0 is turned off to prevent toggling the debug pin; For debug, make this an output and
    //  drive it externally to ground.

    reg_mprj_io_0 = GPIO_MODE_MGMT_STD_ANALOG;

    // Changing configuration for IO[1-4] will interfere with programming flash. if you change them,
    // You may need to hold reset while powering up the board and initiating flash to keep the process
    // configuring these IO from their default values.

    reg_mprj_io_1 = GPIO_MODE_MGMT_STD_OUTPUT;
    reg_mprj_io_2 = GPIO_MODE_USER_STD_BIDIRECTIONAL;
    reg_mprj_io_3 = GPIO_MODE_USER_STD_BIDIRECTIONAL;
    reg_mprj_io_4 = GPIO_MODE_USER_STD_BIDIRECTIONAL;

    // -------------------------------------------

    reg_mprj_io_5 = GPIO_MODE_USER_STD_BIDIRECTIONAL;     // UART Rx
    reg_mprj_io_6 = GPIO_MODE_USER_STD_BIDIRECTIONAL;    // USER UART RX
    reg_mprj_io_7 = GPIO_MODE_USER_STD_BIDIRECTIONAL;    // USER UART TXD
    reg_mprj_io_8 = GPIO_MODE_USER_STD_BIDIRECTIONAL;
    reg_mprj_io_9 = GPIO_MODE_USER_STD_BIDIRECTIONAL; // D3
    reg_mprj_io_10 = GPIO_MODE_USER_STD_BIDIRECTIONAL; // D4
    reg_mprj_io_11 = GPIO_MODE_USER_STD_BIDIRECTIONAL;
    reg_mprj_io_12 = GPIO_MODE_USER_STD_BIDIRECTIONAL;
    reg_mprj_io_13 = GPIO_MODE_USER_STD_BIDIRECTIONAL; // D5
    reg_mprj_io_14 = GPIO_MODE_USER_STD_BIDIRECTIONAL; // D6
    reg_mprj_io_15 = GPIO_MODE_USER_STD_BIDIRECTIONAL;
    reg_mprj_io_16 = GPIO_MODE_USER_STD_BIDIRECTIONAL;
    reg_mprj_io_17 = GPIO_MODE_USER_STD_BIDIRECTIONAL;
    reg_mprj_io_18 = GPIO_MODE_USER_STD_BIDIRECTIONAL;

    reg_mprj_io_19 = GPIO_MODE_USER_STD_BIDIRECTIONAL;
    reg_mprj_io_20 = GPIO_MODE_USER_STD_BIDIRECTIONAL;
    reg_mprj_io_21 = GPIO_MODE_USER_STD_BIDIRECTIONAL;
    reg_mprj_io_22 = GPIO_MODE_USER_STD_BIDIRECTIONAL;
    reg_mprj_io_23 = GPIO_MODE_USER_STD_BIDIRECTIONAL;
    reg_mprj_io_24 = GPIO_MODE_USER_STD_BIDIRECTIONAL;
    reg_mprj_io_25 = GPIO_MODE_USER_STD_BIDIRECTIONAL;
    reg_mprj_io_26 = GPIO_MODE_USER_STD_BIDIRECTIONAL;
    reg_mprj_io_27 = GPIO_MODE_USER_STD_BIDIRECTIONAL;
    reg_mprj_io_28 = GPIO_MODE_USER_STD_BIDIRECTIONAL;
    reg_mprj_io_29 = GPIO_MODE_USER_STD_BIDIRECTIONAL;
    reg_mprj_io_30 = GPIO_MODE_USER_STD_BIDIRECTIONAL;
    reg_mprj_io_31 = GPIO_MODE_USER_STD_BIDIRECTIONAL;
    reg_mprj_io_32 = GPIO_MODE_USER_STD_BIDIRECTIONAL;
    reg_mprj_io_33 = GPIO_MODE_USER_STD_BIDIRECTIONAL;
    reg_mprj_io_34 = GPIO_MODE_USER_STD_BIDIRECTIONAL;
    reg_mprj_io_35 = GPIO_MODE_USER_STD_BIDIRECTIONAL;
    reg_mprj_io_36 = GPIO_MODE_USER_STD_BIDIRECTIONAL;
    reg_mprj_io_37 = GPIO_MODE_USER_STD_BIDIRECTIONAL;

    // Initiate the serial transfer to configure IO
    reg_mprj_xfer = 1;
    while (reg_mprj_xfer == 1);
}


void main()
{

	int bFail = 0;
    reg_gpio_mode1 = 1;
    reg_gpio_mode0 = 0;
    reg_gpio_ien = 1;
    reg_gpio_oe = 1;


    configure_io();
    //reg_uart_enable = 1;
    reg_wb_enable = 1;


    // Configure LA probes [63:32] and [127:96] as inputs to the cpu 
	// Configure LA probes [31:0] and [63:32] as input from the cpu
    reg_la0_oenb = reg_la0_iena = 0xFFFFFFFF;    // [31:0]
	reg_la1_oenb = reg_la1_iena = 0x00000000;    // [63:32]
	reg_la2_oenb = reg_la2_iena = 0x000000000;    // [95:64]
	reg_la3_oenb = reg_la3_iena = 0x00000000;    // [127:96]

    // Enable UART MASTER with LA control
    // la0_data[1] - 1- User Uart Master Tx Enable 
    // la0_data[2] - 1- User Uart Master Rx Enable 
    // la0_data[3] - 1- User Uart Master Stop bit 2
    // la0_data[17:16] - 0x3F; // Setting User Baud to 9600 with system clock 10Mhz = (10,000,000/(16 * (63+2))
    // la0_data[17:16] - 0x40; // Setting User Baud to 57600 with system clock 10Mhz = (10,000,000/(16 * (9+2))
    reg_la0_data = 0x096;

    // Bit[13] - 1  Cache Disabled
    // Bit[14] - 1  SRAM Clock Invert
    reg_mprj_wbhost_reg5 = 0x6000; // system strap
    //putdword(reg_mprj_wbhost_reg5);

    //putdword(reg_mprj_wbhost_reg2);
    /*** Disabled for 2306Q 
    reg_mprj_wbhost_reg2 = 0x00879898;
    reg_mprj_wbhost_reg3 = 0x00;// wbs clock divr-4
    *****/

     // Remove Wishbone Reset
     reg_mprj_wbhost_ctrl = 0x0;
     reg_mprj_wbhost_ctrl = 0x1;

     // Remove Reset
     reg_glbl_cfg0 = 0x01f;

    /****************************************
      To Enable the Quad Mode in SPI Flash we need to
      write Status Reg[2] bit [1] = 1
    ******************************************/
       reg_qspi_imem_ctrl1 =  0x00000001;
       reg_qspi_imem_ctrl2 =  0x01010031;
       reg_qspi_imem_wdata =  0x00000002;

     // Setting Serial Flash to Quad Mode
     reg_qspi_dmem_g0_rd_ctrl = 0x619800EB;

    /****************************************
      To Enable the Quad Mode in SPI SRAM 
      1. Send Opcode 0x38 to CS#2
    ******************************************/
    reg_qspi_imem_ctrl1 =  0x00000004;
    reg_qspi_imem_ctrl2 =  0x00000038;
    reg_qspi_imem_wdata =  0x00000000;
 
     // Setting Serial SRAM Read control
     reg_qspi_dmem_g1_rd_ctrl = 0x408a0003;
     reg_qspi_dmem_g1_wr_ctrl = 0x708a0002;


    // Wait for 20 Second 
	delay(200000000);
    /****************************
    If the User Firmware is progress, then bypass the configuration
    Check MSB bit[31] = 1 indicate User Flashing is progress
    *****************************/
    //if((reg_mprj_wbhost_ctrl & 0x80000000) == 0x0) {
    //     reg_glbl_cfg0 = 0x11f;
    //}
    
    // blink the led if all checks passes
    while (bFail == 0) {
        //putdword(reg_la0_data_in);
        //putdword(reg_la1_data_in);
        //putdword(reg_la2_data_in);
        //putdword(reg_la3_data_in);
        //if (reg_glbl_chip_id != 0x82682501)    bFail = 1; // check chip id
        //if (reg_glbl_soft_reg_0 != 0x82738343) bFail = 1; // check signature
        //if (reg_glbl_soft_reg_1 != 0x07092022) bFail = 1; // check date
        //if (reg_glbl_soft_reg_2 != 0x00054000) bFail = 1; // check version
        reg_gpio_out = 1; // OFF
		delay(8000000);
        reg_gpio_out = 0;  // ON
		delay(8000000);

    }

}
