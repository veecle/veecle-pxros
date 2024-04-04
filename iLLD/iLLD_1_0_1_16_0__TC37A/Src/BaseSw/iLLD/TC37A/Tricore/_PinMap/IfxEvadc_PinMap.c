/**
 * \file IfxEvadc_PinMap.c
 * \brief EVADC I/O map
 * \ingroup IfxLld_Evadc
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

#include "IfxEvadc_PinMap.h"

IfxEvadc_Emux_Out IfxEvadc_EMUX00_P02_6_OUT = {&MODULE_EVADC, {&MODULE_P02, 6}, IfxPort_OutputIdx_alt5};
IfxEvadc_Emux_Out IfxEvadc_EMUX00_P33_3_OUT = {&MODULE_EVADC, {&MODULE_P33, 3}, IfxPort_OutputIdx_alt5};
IfxEvadc_Emux_Out IfxEvadc_EMUX01_P02_7_OUT = {&MODULE_EVADC, {&MODULE_P02, 7}, IfxPort_OutputIdx_alt5};
IfxEvadc_Emux_Out IfxEvadc_EMUX01_P33_2_OUT = {&MODULE_EVADC, {&MODULE_P33, 2}, IfxPort_OutputIdx_alt5};
IfxEvadc_Emux_Out IfxEvadc_EMUX02_P02_8_OUT = {&MODULE_EVADC, {&MODULE_P02, 8}, IfxPort_OutputIdx_alt5};
IfxEvadc_Emux_Out IfxEvadc_EMUX02_P33_1_OUT = {&MODULE_EVADC, {&MODULE_P33, 1}, IfxPort_OutputIdx_alt5};
IfxEvadc_Emux_Out IfxEvadc_EMUX10_P00_6_OUT = {&MODULE_EVADC, {&MODULE_P00, 6}, IfxPort_OutputIdx_alt5};
IfxEvadc_Emux_Out IfxEvadc_EMUX10_P33_6_OUT = {&MODULE_EVADC, {&MODULE_P33, 6}, IfxPort_OutputIdx_alt5};
IfxEvadc_Emux_Out IfxEvadc_EMUX11_P00_7_OUT = {&MODULE_EVADC, {&MODULE_P00, 7}, IfxPort_OutputIdx_alt5};
IfxEvadc_Emux_Out IfxEvadc_EMUX11_P33_5_OUT = {&MODULE_EVADC, {&MODULE_P33, 5}, IfxPort_OutputIdx_alt5};
IfxEvadc_Emux_Out IfxEvadc_EMUX12_P00_8_OUT = {&MODULE_EVADC, {&MODULE_P00, 8}, IfxPort_OutputIdx_alt5};
IfxEvadc_Emux_Out IfxEvadc_EMUX12_P33_4_OUT = {&MODULE_EVADC, {&MODULE_P33, 4}, IfxPort_OutputIdx_alt5};
IfxEvadc_GxBfl_Out IfxEvadc_FC0BFLOUT_P00_5_OUT = {&MODULE_EVADC, IfxEvadc_GroupId_12, {&MODULE_P00, 5}, IfxPort_OutputIdx_alt5};
IfxEvadc_GxBfl_Out IfxEvadc_FC0BFLOUT_P33_4_OUT = {&MODULE_EVADC, IfxEvadc_GroupId_12, {&MODULE_P33, 4}, IfxPort_OutputIdx_alt6};
IfxEvadc_GxBfl_Out IfxEvadc_FC1BFLOUT_P10_1_OUT = {&MODULE_EVADC, IfxEvadc_GroupId_13, {&MODULE_P10, 1}, IfxPort_OutputIdx_alt5};
IfxEvadc_GxBfl_Out IfxEvadc_FC1BFLOUT_P33_6_OUT = {&MODULE_EVADC, IfxEvadc_GroupId_13, {&MODULE_P33, 6}, IfxPort_OutputIdx_alt6};
IfxEvadc_GxBfl_Out IfxEvadc_FC2BFLOUT_P00_7_OUT = {&MODULE_EVADC, IfxEvadc_GroupId_14, {&MODULE_P00, 7}, IfxPort_OutputIdx_alt3};
IfxEvadc_GxBfl_Out IfxEvadc_FC2BFLOUT_P33_0_OUT = {&MODULE_EVADC, IfxEvadc_GroupId_14, {&MODULE_P33, 0}, IfxPort_OutputIdx_alt6};
IfxEvadc_GxBfl_Out IfxEvadc_FC2BFLOUT_P33_5_OUT = {&MODULE_EVADC, IfxEvadc_GroupId_14, {&MODULE_P33, 5}, IfxPort_OutputIdx_alt6};
IfxEvadc_GxBfl_Out IfxEvadc_FC3BFLOUT_P10_2_OUT = {&MODULE_EVADC, IfxEvadc_GroupId_15, {&MODULE_P10, 2}, IfxPort_OutputIdx_alt5};
IfxEvadc_GxBfl_Out IfxEvadc_FC3BFLOUT_P33_2_OUT = {&MODULE_EVADC, IfxEvadc_GroupId_15, {&MODULE_P33, 2}, IfxPort_OutputIdx_alt6};
IfxEvadc_GxBfl_Out IfxEvadc_FC3BFLOUT_P33_7_OUT = {&MODULE_EVADC, IfxEvadc_GroupId_15, {&MODULE_P33, 7}, IfxPort_OutputIdx_alt6};
IfxEvadc_Vadcg_In IfxEvadc_FC0CH0_AN16_IN = {&MODULE_EVADC, IfxEvadc_GroupId_12, {NULL_PTR,16}, 0};
IfxEvadc_Vadcg_In IfxEvadc_FC1CH0_P40_10_IN = {&MODULE_EVADC, IfxEvadc_GroupId_13, {&MODULE_P40,10}, 0};
IfxEvadc_Vadcg_In IfxEvadc_FC2CH0_P00_12_IN = {&MODULE_EVADC, IfxEvadc_GroupId_14, {&MODULE_P00,12}, 0};
IfxEvadc_Vadcg_In IfxEvadc_FC3CH0_P00_11_IN = {&MODULE_EVADC, IfxEvadc_GroupId_15, {&MODULE_P00,11}, 0};
IfxEvadc_Vadcg_In IfxEvadc_G0CH0_AN0_IN = {&MODULE_EVADC, IfxEvadc_GroupId_0, {NULL_PTR, 0}, 0};
IfxEvadc_Vadcg_In IfxEvadc_G0CH1_AN1_IN = {&MODULE_EVADC, IfxEvadc_GroupId_0, {NULL_PTR, 1}, 1};
IfxEvadc_Vadcg_In IfxEvadc_G0CH2_AN2_IN = {&MODULE_EVADC, IfxEvadc_GroupId_0, {NULL_PTR, 2}, 2};
IfxEvadc_Vadcg_In IfxEvadc_G0CH3_AN3_IN = {&MODULE_EVADC, IfxEvadc_GroupId_0, {NULL_PTR, 3}, 3};
IfxEvadc_Vadcg_In IfxEvadc_G0CH4_AN4_IN = {&MODULE_EVADC, IfxEvadc_GroupId_0, {NULL_PTR, 4}, 4};
IfxEvadc_Vadcg_In IfxEvadc_G0CH5_AN5_IN = {&MODULE_EVADC, IfxEvadc_GroupId_0, {NULL_PTR, 5}, 5};
IfxEvadc_Vadcg_In IfxEvadc_G0CH6_AN6_IN = {&MODULE_EVADC, IfxEvadc_GroupId_0, {NULL_PTR, 6}, 6};
IfxEvadc_Vadcg_In IfxEvadc_G0CH7_AN7_IN = {&MODULE_EVADC, IfxEvadc_GroupId_0, {NULL_PTR, 7}, 7};
IfxEvadc_Vadcg_In IfxEvadc_G10CH0_P33_7_IN = {&MODULE_EVADC, IfxEvadc_GroupId_10, {&MODULE_P33, 7}, 0};
IfxEvadc_Vadcg_In IfxEvadc_G10CH10_P34_2_IN = {&MODULE_EVADC, IfxEvadc_GroupId_10, {&MODULE_P34, 2}, 10};
IfxEvadc_Vadcg_In IfxEvadc_G10CH11_P34_1_IN = {&MODULE_EVADC, IfxEvadc_GroupId_10, {&MODULE_P34, 1}, 11};
IfxEvadc_Vadcg_In IfxEvadc_G10CH1_P33_6_IN = {&MODULE_EVADC, IfxEvadc_GroupId_10, {&MODULE_P33, 6}, 1};
IfxEvadc_Vadcg_In IfxEvadc_G10CH2_P33_5_IN = {&MODULE_EVADC, IfxEvadc_GroupId_10, {&MODULE_P33, 5}, 2};
IfxEvadc_Vadcg_In IfxEvadc_G10CH3_P33_4_IN = {&MODULE_EVADC, IfxEvadc_GroupId_10, {&MODULE_P33, 4}, 3};
IfxEvadc_Vadcg_In IfxEvadc_G10CH4_P33_3_IN = {&MODULE_EVADC, IfxEvadc_GroupId_10, {&MODULE_P33, 3}, 4};
IfxEvadc_Vadcg_In IfxEvadc_G10CH5_P33_2_IN = {&MODULE_EVADC, IfxEvadc_GroupId_10, {&MODULE_P33, 2}, 5};
IfxEvadc_Vadcg_In IfxEvadc_G10CH6_P33_1_IN = {&MODULE_EVADC, IfxEvadc_GroupId_10, {&MODULE_P33, 1}, 6};
IfxEvadc_Vadcg_In IfxEvadc_G10CH7_P33_0_IN = {&MODULE_EVADC, IfxEvadc_GroupId_10, {&MODULE_P33, 0}, 7};
IfxEvadc_Vadcg_In IfxEvadc_G10CH8_P34_4_IN = {&MODULE_EVADC, IfxEvadc_GroupId_10, {&MODULE_P34, 4}, 8};
IfxEvadc_Vadcg_In IfxEvadc_G10CH9_P34_3_IN = {&MODULE_EVADC, IfxEvadc_GroupId_10, {&MODULE_P34, 3}, 9};
IfxEvadc_Vadcg_In IfxEvadc_G11CH0_AN4_IN = {&MODULE_EVADC, IfxEvadc_GroupId_11, {NULL_PTR, 4}, 0};
IfxEvadc_Vadcg_In IfxEvadc_G11CH10_P40_2_IN = {&MODULE_EVADC, IfxEvadc_GroupId_11, {&MODULE_P40, 2}, 10};
IfxEvadc_Vadcg_In IfxEvadc_G11CH11_P40_3_IN = {&MODULE_EVADC, IfxEvadc_GroupId_11, {&MODULE_P40, 3}, 11};
IfxEvadc_Vadcg_In IfxEvadc_G11CH12_P40_4_IN = {&MODULE_EVADC, IfxEvadc_GroupId_11, {&MODULE_P40, 4}, 12};
IfxEvadc_Vadcg_In IfxEvadc_G11CH13_P40_5_IN = {&MODULE_EVADC, IfxEvadc_GroupId_11, {&MODULE_P40, 5}, 13};
IfxEvadc_Vadcg_In IfxEvadc_G11CH14_AN34_IN = {&MODULE_EVADC, IfxEvadc_GroupId_11, {NULL_PTR,34}, 14};
IfxEvadc_Vadcg_In IfxEvadc_G11CH15_AN35_IN = {&MODULE_EVADC, IfxEvadc_GroupId_11, {NULL_PTR,35}, 15};
IfxEvadc_Vadcg_In IfxEvadc_G11CH1_AN5_IN = {&MODULE_EVADC, IfxEvadc_GroupId_11, {NULL_PTR, 5}, 1};
IfxEvadc_Vadcg_In IfxEvadc_G11CH2_AN6_IN = {&MODULE_EVADC, IfxEvadc_GroupId_11, {NULL_PTR, 6}, 2};
IfxEvadc_Vadcg_In IfxEvadc_G11CH3_AN7_IN = {&MODULE_EVADC, IfxEvadc_GroupId_11, {NULL_PTR, 7}, 3};
IfxEvadc_Vadcg_In IfxEvadc_G11CH4_AN8_IN = {&MODULE_EVADC, IfxEvadc_GroupId_11, {NULL_PTR, 8}, 4};
IfxEvadc_Vadcg_In IfxEvadc_G11CH5_AN9_IN = {&MODULE_EVADC, IfxEvadc_GroupId_11, {NULL_PTR, 9}, 5};
IfxEvadc_Vadcg_In IfxEvadc_G11CH6_AN10_IN = {&MODULE_EVADC, IfxEvadc_GroupId_11, {NULL_PTR,10}, 6};
IfxEvadc_Vadcg_In IfxEvadc_G11CH7_AN11_IN = {&MODULE_EVADC, IfxEvadc_GroupId_11, {NULL_PTR,11}, 7};
IfxEvadc_Vadcg_In IfxEvadc_G11CH8_P40_11_IN = {&MODULE_EVADC, IfxEvadc_GroupId_11, {&MODULE_P40,11}, 8};
IfxEvadc_Vadcg_In IfxEvadc_G11CH9_P40_12_IN = {&MODULE_EVADC, IfxEvadc_GroupId_11, {&MODULE_P40,12}, 9};
IfxEvadc_Vadcg_In IfxEvadc_G1CH0_AN8_IN = {&MODULE_EVADC, IfxEvadc_GroupId_1, {NULL_PTR, 8}, 0};
IfxEvadc_Vadcg_In IfxEvadc_G1CH1_AN9_IN = {&MODULE_EVADC, IfxEvadc_GroupId_1, {NULL_PTR, 9}, 1};
IfxEvadc_Vadcg_In IfxEvadc_G1CH2_AN10_IN = {&MODULE_EVADC, IfxEvadc_GroupId_1, {NULL_PTR,10}, 2};
IfxEvadc_Vadcg_In IfxEvadc_G1CH3_AN11_IN = {&MODULE_EVADC, IfxEvadc_GroupId_1, {NULL_PTR,11}, 3};
IfxEvadc_Vadcg_In IfxEvadc_G1CH4_AN12_IN = {&MODULE_EVADC, IfxEvadc_GroupId_1, {NULL_PTR,12}, 4};
IfxEvadc_Vadcg_In IfxEvadc_G1CH5_AN13_IN = {&MODULE_EVADC, IfxEvadc_GroupId_1, {NULL_PTR,13}, 5};
IfxEvadc_Vadcg_In IfxEvadc_G1CH6_AN14_IN = {&MODULE_EVADC, IfxEvadc_GroupId_1, {NULL_PTR,14}, 6};
IfxEvadc_Vadcg_In IfxEvadc_G1CH7_AN15_IN = {&MODULE_EVADC, IfxEvadc_GroupId_1, {NULL_PTR,15}, 7};
IfxEvadc_Vadcg_In IfxEvadc_G2CH0_AN16_IN = {&MODULE_EVADC, IfxEvadc_GroupId_2, {NULL_PTR,16}, 0};
IfxEvadc_Vadcg_In IfxEvadc_G2CH1_P40_10_IN = {&MODULE_EVADC, IfxEvadc_GroupId_2, {&MODULE_P40,10}, 1};
IfxEvadc_Vadcg_In IfxEvadc_G2CH2_P40_11_IN = {&MODULE_EVADC, IfxEvadc_GroupId_2, {&MODULE_P40,11}, 2};
IfxEvadc_Vadcg_In IfxEvadc_G2CH3_P40_12_IN = {&MODULE_EVADC, IfxEvadc_GroupId_2, {&MODULE_P40,12}, 3};
IfxEvadc_Vadcg_In IfxEvadc_G2CH4_AN20_IN = {&MODULE_EVADC, IfxEvadc_GroupId_2, {NULL_PTR,20}, 4};
IfxEvadc_Vadcg_In IfxEvadc_G2CH5_AN21_IN = {&MODULE_EVADC, IfxEvadc_GroupId_2, {NULL_PTR,21}, 5};
IfxEvadc_Vadcg_In IfxEvadc_G2CH6_AN22_IN = {&MODULE_EVADC, IfxEvadc_GroupId_2, {NULL_PTR,22}, 6};
IfxEvadc_Vadcg_In IfxEvadc_G2CH7_AN23_IN = {&MODULE_EVADC, IfxEvadc_GroupId_2, {NULL_PTR,23}, 7};
IfxEvadc_Vadcg_In IfxEvadc_G3CH0_P40_0_IN = {&MODULE_EVADC, IfxEvadc_GroupId_3, {&MODULE_P40, 0}, 0};
IfxEvadc_Vadcg_In IfxEvadc_G3CH1_P40_1_IN = {&MODULE_EVADC, IfxEvadc_GroupId_3, {&MODULE_P40, 1}, 1};
IfxEvadc_Vadcg_In IfxEvadc_G3CH2_P40_2_IN = {&MODULE_EVADC, IfxEvadc_GroupId_3, {&MODULE_P40, 2}, 2};
IfxEvadc_Vadcg_In IfxEvadc_G3CH3_P40_3_IN = {&MODULE_EVADC, IfxEvadc_GroupId_3, {&MODULE_P40, 3}, 3};
IfxEvadc_Vadcg_In IfxEvadc_G3CH4_P40_13_IN = {&MODULE_EVADC, IfxEvadc_GroupId_3, {&MODULE_P40,13}, 4};
IfxEvadc_Vadcg_In IfxEvadc_G3CH5_P40_14_IN = {&MODULE_EVADC, IfxEvadc_GroupId_3, {&MODULE_P40,14}, 5};
IfxEvadc_Vadcg_In IfxEvadc_G3CH6_AN30_IN = {&MODULE_EVADC, IfxEvadc_GroupId_3, {NULL_PTR,30}, 6};
IfxEvadc_Vadcg_In IfxEvadc_G3CH7_AN31_IN = {&MODULE_EVADC, IfxEvadc_GroupId_3, {NULL_PTR,31}, 7};
IfxEvadc_Vadcg_In IfxEvadc_G8CH0_P40_4_IN = {&MODULE_EVADC, IfxEvadc_GroupId_8, {&MODULE_P40, 4}, 0};
IfxEvadc_Vadcg_In IfxEvadc_G8CH10_AN42_IN = {&MODULE_EVADC, IfxEvadc_GroupId_8, {NULL_PTR,42}, 10};
IfxEvadc_Vadcg_In IfxEvadc_G8CH11_AN43_IN = {&MODULE_EVADC, IfxEvadc_GroupId_8, {NULL_PTR,43}, 11};
IfxEvadc_Vadcg_In IfxEvadc_G8CH12_AN44_IN = {&MODULE_EVADC, IfxEvadc_GroupId_8, {NULL_PTR,44}, 12};
IfxEvadc_Vadcg_In IfxEvadc_G8CH13_AN45_IN = {&MODULE_EVADC, IfxEvadc_GroupId_8, {NULL_PTR,45}, 13};
IfxEvadc_Vadcg_In IfxEvadc_G8CH14_AN46_IN = {&MODULE_EVADC, IfxEvadc_GroupId_8, {NULL_PTR,46}, 14};
IfxEvadc_Vadcg_In IfxEvadc_G8CH15_AN47_IN = {&MODULE_EVADC, IfxEvadc_GroupId_8, {NULL_PTR,47}, 15};
IfxEvadc_Vadcg_In IfxEvadc_G8CH1_P40_5_IN = {&MODULE_EVADC, IfxEvadc_GroupId_8, {&MODULE_P40, 5}, 1};
IfxEvadc_Vadcg_In IfxEvadc_G8CH2_AN34_IN = {&MODULE_EVADC, IfxEvadc_GroupId_8, {NULL_PTR,34}, 2};
IfxEvadc_Vadcg_In IfxEvadc_G8CH3_AN35_IN = {&MODULE_EVADC, IfxEvadc_GroupId_8, {NULL_PTR,35}, 3};
IfxEvadc_Vadcg_In IfxEvadc_G8CH4_P40_6_IN = {&MODULE_EVADC, IfxEvadc_GroupId_8, {&MODULE_P40, 6}, 4};
IfxEvadc_Vadcg_In IfxEvadc_G8CH5_P40_7_IN = {&MODULE_EVADC, IfxEvadc_GroupId_8, {&MODULE_P40, 7}, 5};
IfxEvadc_Vadcg_In IfxEvadc_G8CH6_P40_8_IN = {&MODULE_EVADC, IfxEvadc_GroupId_8, {&MODULE_P40, 8}, 6};
IfxEvadc_Vadcg_In IfxEvadc_G8CH7_P40_9_IN = {&MODULE_EVADC, IfxEvadc_GroupId_8, {&MODULE_P40, 9}, 7};
IfxEvadc_Vadcg_In IfxEvadc_G8CH8_AN40_IN = {&MODULE_EVADC, IfxEvadc_GroupId_8, {NULL_PTR,40}, 8};
IfxEvadc_Vadcg_In IfxEvadc_G8CH9_AN41_IN = {&MODULE_EVADC, IfxEvadc_GroupId_8, {NULL_PTR,41}, 9};
IfxEvadc_Vadcg_In IfxEvadc_G9CH0_P00_12_IN = {&MODULE_EVADC, IfxEvadc_GroupId_9, {&MODULE_P00,12}, 0};
IfxEvadc_Vadcg_In IfxEvadc_G9CH10_P00_2_IN = {&MODULE_EVADC, IfxEvadc_GroupId_9, {&MODULE_P00, 2}, 10};
IfxEvadc_Vadcg_In IfxEvadc_G9CH11_P00_1_IN = {&MODULE_EVADC, IfxEvadc_GroupId_9, {&MODULE_P00, 1}, 11};
IfxEvadc_Vadcg_In IfxEvadc_G9CH12_P01_5_IN = {&MODULE_EVADC, IfxEvadc_GroupId_9, {&MODULE_P01, 5}, 12};
IfxEvadc_Vadcg_In IfxEvadc_G9CH13_P01_4_IN = {&MODULE_EVADC, IfxEvadc_GroupId_9, {&MODULE_P01, 4}, 13};
IfxEvadc_Vadcg_In IfxEvadc_G9CH14_P01_3_IN = {&MODULE_EVADC, IfxEvadc_GroupId_9, {&MODULE_P01, 3}, 14};
IfxEvadc_Vadcg_In IfxEvadc_G9CH15_P02_11_IN = {&MODULE_EVADC, IfxEvadc_GroupId_9, {&MODULE_P02,11}, 15};
IfxEvadc_Vadcg_In IfxEvadc_G9CH1_P00_11_IN = {&MODULE_EVADC, IfxEvadc_GroupId_9, {&MODULE_P00,11}, 1};
IfxEvadc_Vadcg_In IfxEvadc_G9CH2_P00_10_IN = {&MODULE_EVADC, IfxEvadc_GroupId_9, {&MODULE_P00,10}, 2};
IfxEvadc_Vadcg_In IfxEvadc_G9CH3_P00_9_IN = {&MODULE_EVADC, IfxEvadc_GroupId_9, {&MODULE_P00, 9}, 3};
IfxEvadc_Vadcg_In IfxEvadc_G9CH4_P00_8_IN = {&MODULE_EVADC, IfxEvadc_GroupId_9, {&MODULE_P00, 8}, 4};
IfxEvadc_Vadcg_In IfxEvadc_G9CH5_P00_7_IN = {&MODULE_EVADC, IfxEvadc_GroupId_9, {&MODULE_P00, 7}, 5};
IfxEvadc_Vadcg_In IfxEvadc_G9CH6_P00_6_IN = {&MODULE_EVADC, IfxEvadc_GroupId_9, {&MODULE_P00, 6}, 6};
IfxEvadc_Vadcg_In IfxEvadc_G9CH7_P00_5_IN = {&MODULE_EVADC, IfxEvadc_GroupId_9, {&MODULE_P00, 5}, 7};
IfxEvadc_Vadcg_In IfxEvadc_G9CH8_P00_4_IN = {&MODULE_EVADC, IfxEvadc_GroupId_9, {&MODULE_P00, 4}, 8};
IfxEvadc_Vadcg_In IfxEvadc_G9CH9_P00_3_IN = {&MODULE_EVADC, IfxEvadc_GroupId_9, {&MODULE_P00, 3}, 9};


const IfxEvadc_Emux_Out *IfxEvadc_Emux_Out_pinTable[IFXEVADC_PINMAP_NUM_MODULES][IFXEVADC_PINMAP_EMUX_OUT_NUM_ITEMS] = {
    {
        &IfxEvadc_EMUX10_P00_6_OUT,
        &IfxEvadc_EMUX11_P00_7_OUT,
        &IfxEvadc_EMUX12_P00_8_OUT,
        &IfxEvadc_EMUX00_P02_6_OUT,
        &IfxEvadc_EMUX01_P02_7_OUT,
        &IfxEvadc_EMUX02_P02_8_OUT,
        &IfxEvadc_EMUX02_P33_1_OUT,
        &IfxEvadc_EMUX01_P33_2_OUT,
        &IfxEvadc_EMUX00_P33_3_OUT,
        &IfxEvadc_EMUX12_P33_4_OUT,
        &IfxEvadc_EMUX11_P33_5_OUT,
        &IfxEvadc_EMUX10_P33_6_OUT
    }
};

const IfxEvadc_GxBfl_Out *IfxEvadc_GxBfl_Out_pinTable[IFXEVADC_PINMAP_NUM_MODULES][IFXEVADC_PINMAP_NUM_GROUPS][IFXEVADC_PINMAP_GXBFL_OUT_NUM_ITEMS] = {
    {
        {
            NULL_PTR,
            NULL_PTR,
            NULL_PTR
        },
        {
            NULL_PTR,
            NULL_PTR,
            NULL_PTR
        },
        {
            NULL_PTR,
            NULL_PTR,
            NULL_PTR
        },
        {
            NULL_PTR,
            NULL_PTR,
            NULL_PTR
        },
        {
            NULL_PTR,
            NULL_PTR,
            NULL_PTR
        },
        {
            NULL_PTR,
            NULL_PTR,
            NULL_PTR
        },
        {
            NULL_PTR,
            NULL_PTR,
            NULL_PTR
        },
        {
            NULL_PTR,
            NULL_PTR,
            NULL_PTR
        },
        {
            NULL_PTR,
            NULL_PTR,
            NULL_PTR
        },
        {
            NULL_PTR,
            NULL_PTR,
            NULL_PTR
        },
        {
            NULL_PTR,
            NULL_PTR,
            NULL_PTR
        },
        {
            NULL_PTR,
            NULL_PTR,
            NULL_PTR
        },
        {
            &IfxEvadc_FC0BFLOUT_P00_5_OUT,
            &IfxEvadc_FC0BFLOUT_P33_4_OUT,
            NULL_PTR
        },
        {
            &IfxEvadc_FC1BFLOUT_P10_1_OUT,
            &IfxEvadc_FC1BFLOUT_P33_6_OUT,
            NULL_PTR
        },
        {
            &IfxEvadc_FC2BFLOUT_P00_7_OUT,
            &IfxEvadc_FC2BFLOUT_P33_0_OUT,
            &IfxEvadc_FC2BFLOUT_P33_5_OUT
        },
        {
            &IfxEvadc_FC3BFLOUT_P10_2_OUT,
            &IfxEvadc_FC3BFLOUT_P33_2_OUT,
            &IfxEvadc_FC3BFLOUT_P33_7_OUT
        }
    }
};

const IfxEvadc_Vadcg_In *IfxEvadc_Vadcg_In_pinTable[IFXEVADC_PINMAP_NUM_MODULES][IFXEVADC_PINMAP_NUM_GROUPS][IFXEVADC_PINMAP_VADCG_IN_NUM_ITEMS] = {
    {
        {
            &IfxEvadc_G0CH0_AN0_IN,
            &IfxEvadc_G0CH1_AN1_IN,
            &IfxEvadc_G0CH2_AN2_IN,
            &IfxEvadc_G0CH3_AN3_IN,
            &IfxEvadc_G0CH4_AN4_IN,
            &IfxEvadc_G0CH5_AN5_IN,
            &IfxEvadc_G0CH6_AN6_IN,
            &IfxEvadc_G0CH7_AN7_IN,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR
        },
        {
            &IfxEvadc_G1CH0_AN8_IN,
            &IfxEvadc_G1CH1_AN9_IN,
            &IfxEvadc_G1CH2_AN10_IN,
            &IfxEvadc_G1CH3_AN11_IN,
            &IfxEvadc_G1CH4_AN12_IN,
            &IfxEvadc_G1CH5_AN13_IN,
            &IfxEvadc_G1CH6_AN14_IN,
            &IfxEvadc_G1CH7_AN15_IN,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR
        },
        {
            &IfxEvadc_G2CH0_AN16_IN,
            &IfxEvadc_G2CH1_P40_10_IN,
            &IfxEvadc_G2CH2_P40_11_IN,
            &IfxEvadc_G2CH3_P40_12_IN,
            &IfxEvadc_G2CH4_AN20_IN,
            &IfxEvadc_G2CH5_AN21_IN,
            &IfxEvadc_G2CH6_AN22_IN,
            &IfxEvadc_G2CH7_AN23_IN,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR
        },
        {
            &IfxEvadc_G3CH0_P40_0_IN,
            &IfxEvadc_G3CH1_P40_1_IN,
            &IfxEvadc_G3CH2_P40_2_IN,
            &IfxEvadc_G3CH3_P40_3_IN,
            &IfxEvadc_G3CH4_P40_13_IN,
            &IfxEvadc_G3CH5_P40_14_IN,
            &IfxEvadc_G3CH6_AN30_IN,
            &IfxEvadc_G3CH7_AN31_IN,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR
        },
        {
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR
        },
        {
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR
        },
        {
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR
        },
        {
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR
        },
        {
            &IfxEvadc_G8CH0_P40_4_IN,
            &IfxEvadc_G8CH1_P40_5_IN,
            &IfxEvadc_G8CH2_AN34_IN,
            &IfxEvadc_G8CH3_AN35_IN,
            &IfxEvadc_G8CH4_P40_6_IN,
            &IfxEvadc_G8CH5_P40_7_IN,
            &IfxEvadc_G8CH6_P40_8_IN,
            &IfxEvadc_G8CH7_P40_9_IN,
            &IfxEvadc_G8CH8_AN40_IN,
            &IfxEvadc_G8CH9_AN41_IN,
            &IfxEvadc_G8CH10_AN42_IN,
            &IfxEvadc_G8CH11_AN43_IN,
            &IfxEvadc_G8CH12_AN44_IN,
            &IfxEvadc_G8CH13_AN45_IN,
            &IfxEvadc_G8CH14_AN46_IN,
            &IfxEvadc_G8CH15_AN47_IN
        },
        {
            &IfxEvadc_G9CH0_P00_12_IN,
            &IfxEvadc_G9CH1_P00_11_IN,
            &IfxEvadc_G9CH2_P00_10_IN,
            &IfxEvadc_G9CH3_P00_9_IN,
            &IfxEvadc_G9CH4_P00_8_IN,
            &IfxEvadc_G9CH5_P00_7_IN,
            &IfxEvadc_G9CH6_P00_6_IN,
            &IfxEvadc_G9CH7_P00_5_IN,
            &IfxEvadc_G9CH8_P00_4_IN,
            &IfxEvadc_G9CH9_P00_3_IN,
            &IfxEvadc_G9CH10_P00_2_IN,
            &IfxEvadc_G9CH11_P00_1_IN,
            &IfxEvadc_G9CH12_P01_5_IN,
            &IfxEvadc_G9CH13_P01_4_IN,
            &IfxEvadc_G9CH14_P01_3_IN,
            &IfxEvadc_G9CH15_P02_11_IN
        },
        {
            &IfxEvadc_G10CH0_P33_7_IN,
            &IfxEvadc_G10CH1_P33_6_IN,
            &IfxEvadc_G10CH2_P33_5_IN,
            &IfxEvadc_G10CH3_P33_4_IN,
            &IfxEvadc_G10CH4_P33_3_IN,
            &IfxEvadc_G10CH5_P33_2_IN,
            &IfxEvadc_G10CH6_P33_1_IN,
            &IfxEvadc_G10CH7_P33_0_IN,
            &IfxEvadc_G10CH8_P34_4_IN,
            &IfxEvadc_G10CH9_P34_3_IN,
            &IfxEvadc_G10CH10_P34_2_IN,
            &IfxEvadc_G10CH11_P34_1_IN,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR
        },
        {
            &IfxEvadc_G11CH0_AN4_IN,
            &IfxEvadc_G11CH1_AN5_IN,
            &IfxEvadc_G11CH2_AN6_IN,
            &IfxEvadc_G11CH3_AN7_IN,
            &IfxEvadc_G11CH4_AN8_IN,
            &IfxEvadc_G11CH5_AN9_IN,
            &IfxEvadc_G11CH6_AN10_IN,
            &IfxEvadc_G11CH7_AN11_IN,
            &IfxEvadc_G11CH8_P40_11_IN,
            &IfxEvadc_G11CH9_P40_12_IN,
            &IfxEvadc_G11CH10_P40_2_IN,
            &IfxEvadc_G11CH11_P40_3_IN,
            &IfxEvadc_G11CH12_P40_4_IN,
            &IfxEvadc_G11CH13_P40_5_IN,
            &IfxEvadc_G11CH14_AN34_IN,
            &IfxEvadc_G11CH15_AN35_IN
        },
        {
            &IfxEvadc_FC0CH0_AN16_IN,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR
        },
        {
            &IfxEvadc_FC1CH0_P40_10_IN,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR
        },
        {
            &IfxEvadc_FC2CH0_P00_12_IN,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR
        },
        {
            &IfxEvadc_FC3CH0_P00_11_IN,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR,
            NULL_PTR
        }
    }
};
