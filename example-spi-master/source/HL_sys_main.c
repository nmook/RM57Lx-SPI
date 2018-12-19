/** @file HL_sys_main.c 
*   @brief Application main file
*   @date 07-July-2017
*   @version 04.07.00
*
*   This file contains an empty main function,
*   which can be used for the application.
*/

/* 
* Copyright (C) 2009-2016 Texas Instruments Incorporated - www.ti.com  
* 
* 
*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions 
*  are met:
*
*    Redistributions of source code must retain the above copyright 
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the 
*    documentation and/or other materials provided with the   
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/


/* USER CODE BEGIN (0) */
/* USER CODE END */

/* Include Files */

#include "HL_sys_common.h"

/* USER CODE BEGIN (1) */
#include "HL_sys_core.h"
#include "HL_mibspi.h"
/* USER CODE END */

/** @fn void main(void)
*   @brief Application main function
*   @note This function is empty by default.
*
*   This function is called after startup.
*   The user can use this function to implement the application.
*/

/* USER CODE BEGIN (2) */



uint32_t TG3_IS_Complete;
uint32_t i;

/* USER CODE END */

int main(void)
{
/* USER CODE BEGIN (3) */


    uint16 TG0_TX_DATA[8] = {0x1000, 0x1111, 0x1222, 0x1333, 0x1444, 0x1555, 0x1666, 0x1777};
    uint16 TG0_RX_DATA[8]= {0};

    uint16 TG1_TX_DATA[8] = {0x2000, 0x2111, 0x2222, 0x2333, 0x2444, 0x2555, 0x2666, 0x2777};
    uint16 TG1_RX_DATA[8]= {0};

    uint16 TG2_TX_DATA[8] = {0x3000, 0x3111, 0x3222, 0x3333, 0x3444, 0x3555, 0x3666, 0x3777};
    uint16 TG2_RX_DATA[8]= {0};

    uint16 TG3_TX_DATA[8] = {0x4000, 0x4111, 0x4222, 0x4333, 0x4444, 0x4555, 0x4666, 0x4777};
    uint16 TG3_RX_DATA[8]= {0};

    /* Enable IRQ Interrupt in Cortex R4 CPU */
    _enable_interrupt_();

    /* - initializing mibspi - enabling tg 0,1,2,3 , length 8 bytes each
     * TG 0 - Configured for TICK Trigger
     * TG 1, 2, 3, are SOftware Triggered
     * CS 0 is used. - CS0 is in HOLD Mode,
     * CS 0 Toggles only after Each TG ( Every 8 byte) is complete
     * Trigger is One Shot, so only once the TG will transmit.
     */
    mibspiInit();

    /* - enabling loopback ( this is to emulate data transfer without external wires */
    //mibspiEnableLoopback(mibspiREG1,Digital_Lbk);


    /* Enable TG 0,1,2,3 complete interrupt to INT 0 */
    mibspiEnableGroupNotification(mibspiREG1,0,0);
    //mibspiEnableGroupNotification(mibspiREG1,1,0);
    //mibspiEnableGroupNotification(mibspiREG1,2,0);
    //mibspiEnableGroupNotification(mibspiREG1,3,0);

    /* Configure TICKCNT
     * DATA FORMAT 0 is selected which has SPICLK set as 1000KHz
     * So tick is 0xFFFF * (1/1000KHz) = 65.5535 msec*/
    mibspiREG1->TICKCNT = 0x8000FFFF;

    /* Fill the transfer Groups */
    mibspiSetData(mibspiREG1,0,TG0_TX_DATA);
    //mibspiSetData(mibspiREG1,1,TG1_TX_DATA);
    //mibspiSetData(mibspiREG1,2,TG2_TX_DATA);
    //mibspiSetData(mibspiREG1,3,TG3_TX_DATA);


    while(1)
    {
        TG3_IS_Complete = 0x55555555;

        /* Enable TG0 to start, once tickCNT triggers */
        mibspiTransfer(mibspiREG1,0);

        /* Configure TICKCNT
         * DATA FORMAT 0 is selected which has SPICLK set as 1000KHz
         * So tick is 0xFFFF * (1/1000KHz) = 65.5535 msec*/
        //mibspiREG1->TICKCNT = 0x8000FFFF;

        /* Wait until this flag is set in TG3 ISR */
        while(TG3_IS_Complete != 0xA5A5A5A5);



        mibspiGetData(mibspiREG1,0,TG0_RX_DATA);
        for (i = 0; i < 1000; i++)
        {
            ;
        }
    }

/* USER CODE END */

    return 0;
}


/* USER CODE BEGIN (4) */

void mibspiGroupNotification(mibspiBASE_t *mibspi, uint32 group)
{
    switch (group){
        case 0 :
            /* Enable TG1 to start, SW Trigger */
            TG3_IS_Complete = 0xA5A5A5A5;
            //mibspiTransfer(mibspiREG1,1);
            break;
        case 1 :
            /* Enable TG1 to start, SW Trigger */
            //mibspiTransfer(mibspiREG1,2);
            break;
        case 2 :
            /* Enable TG1 to start, SW Trigger */
            //mibspiTransfer(mibspiREG1,3);
            break;
        case 3 :
            /* Enable TG0 to start, SW Trigger */
            //mibspiTransfer(mibspiREG1,0);
            //TG3_IS_Complete = 0xA5A5A5A5;
            break;
        default :
            while(1);
            break;
    }
}

/* USER CODE END */