/**
 * \file IfxI2c_PinMap.c
 * \brief I2C I/O map
 * \ingroup IfxLld_I2c
 *
 * \version iLLD_1_0_1_16_0
 * \copyright Copyright (c) 2017 Infineon Technologies AG. All rights reserved.
 *
 *
 *                                 IMPORTANT NOTICE
 *
 * Use of this file is subject to the terms of use agreed between (i) you or
 * the company in which ordinary course of business you are acting and (ii)
 * Infineon Technologies AG or its licensees. If and as long as no such terms
 * of use are agreed, use of this file is subject to following:
 *
 * Boost Software License - Version 1.0 - August 17th, 2003
 *
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer, must
 * be included in all copies of the Software, in whole or in part, and all
 * derivative works of the Software, unless such copies or derivative works are
 * solely in the form of machine-executable object code generated by a source
 * language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */

#include "IfxI2c_PinMap.h"

IfxI2c_Scl_InOut IfxI2c0_SCL_P02_5_INOUT = {&MODULE_I2C0, {&MODULE_P02, 5}, Ifx_RxSel_a, IfxPort_OutputIdx_alt5};
IfxI2c_Scl_InOut IfxI2c0_SCL_P13_1_INOUT = {&MODULE_I2C0, {&MODULE_P13, 1}, Ifx_RxSel_b, IfxPort_OutputIdx_alt6};
IfxI2c_Scl_InOut IfxI2c0_SCL_P15_4_INOUT = {&MODULE_I2C0, {&MODULE_P15, 4}, Ifx_RxSel_c, IfxPort_OutputIdx_alt6};
IfxI2c_Sda_InOut IfxI2c0_SDA_P02_4_INOUT = {&MODULE_I2C0, {&MODULE_P02, 4}, Ifx_RxSel_a, IfxPort_OutputIdx_alt5};
IfxI2c_Sda_InOut IfxI2c0_SDA_P13_2_INOUT = {&MODULE_I2C0, {&MODULE_P13, 2}, Ifx_RxSel_b, IfxPort_OutputIdx_alt6};
IfxI2c_Sda_InOut IfxI2c0_SDA_P15_5_INOUT = {&MODULE_I2C0, {&MODULE_P15, 5}, Ifx_RxSel_c, IfxPort_OutputIdx_alt6};


const IfxI2c_Scl_InOut *IfxI2c_Scl_InOut_pinTable[IFXI2C_PINMAP_NUM_MODULES][IFXI2C_PINMAP_SCL_INOUT_NUM_ITEMS] = {
    {
        &IfxI2c0_SCL_P02_5_INOUT,
        &IfxI2c0_SCL_P13_1_INOUT,
        &IfxI2c0_SCL_P15_4_INOUT
    }
};

const IfxI2c_Sda_InOut *IfxI2c_Sda_InOut_pinTable[IFXI2C_PINMAP_NUM_MODULES][IFXI2C_PINMAP_SDA_INOUT_NUM_ITEMS] = {
    {
        &IfxI2c0_SDA_P02_4_INOUT,
        &IfxI2c0_SDA_P13_2_INOUT,
        &IfxI2c0_SDA_P15_5_INOUT
    }
};
