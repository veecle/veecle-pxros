/**
 * \file IfxCan_PinMap.h
 * \brief CAN I/O map
 * \ingroup IfxLld_Can
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
 * \defgroup IfxLld_Can_pinmap CAN Pin Mapping
 * \ingroup IfxLld_Can
 */

#ifndef IFXCAN_PINMAP_H
#define IFXCAN_PINMAP_H

#include <IfxCan_reg.h>
#include <_Impl/IfxCan_cfg.h>
#include <Port/Std/IfxPort.h>

/** \addtogroup IfxLld_Can_pinmap
 * \{ */

/** \brief RXD pin mapping structure */
typedef const struct
{
    Ifx_CAN*           module;   /**< \brief Base address */
    IfxCan_NodeId      nodeId;   /**< \brief Node ID */
    IfxPort_Pin        pin;      /**< \brief Port pin */
    Ifx_RxSel          select;   /**< \brief Input multiplexer value */
} IfxCan_Rxd_In;

/** \brief TXD pin mapping structure */
typedef const struct
{
    Ifx_CAN*           module;   /**< \brief Base address */
    IfxCan_NodeId      nodeId;   /**< \brief Node ID */
    IfxPort_Pin        pin;      /**< \brief Port pin */
    IfxPort_OutputIdx  select;   /**< \brief Port control code */
} IfxCan_Txd_Out;

IFX_EXTERN IfxCan_Rxd_In IfxCan_RXD00A_P02_1_IN;  /**< \brief CAN receive input node 0 */
IFX_EXTERN IfxCan_Rxd_In IfxCan_RXD00B_P20_7_IN;  /**< \brief CAN receive input node 0 */
IFX_EXTERN IfxCan_Rxd_In IfxCan_RXD00C_P12_0_IN;  /**< \brief CAN receive input node 0 */
IFX_EXTERN IfxCan_Rxd_In IfxCan_RXD00D_P33_12_IN;  /**< \brief CAN receive input node 0 */
IFX_EXTERN IfxCan_Rxd_In IfxCan_RXD00E_P33_7_IN;  /**< \brief CAN receive input node 0 */
IFX_EXTERN IfxCan_Rxd_In IfxCan_RXD00G_P34_2_IN;  /**< \brief CAN receive input node 0 */
IFX_EXTERN IfxCan_Rxd_In IfxCan_RXD01A_P15_3_IN;  /**< \brief CAN receive input node 1 */
IFX_EXTERN IfxCan_Rxd_In IfxCan_RXD01B_P14_1_IN;  /**< \brief CAN receive input node 1 */
IFX_EXTERN IfxCan_Rxd_In IfxCan_RXD01C_P01_4_IN;  /**< \brief CAN receive input node 1 */
IFX_EXTERN IfxCan_Rxd_In IfxCan_RXD01D_P33_10_IN;  /**< \brief CAN receive input node 1 */
IFX_EXTERN IfxCan_Rxd_In IfxCan_RXD01E_P02_10_IN;  /**< \brief CAN receive input node 1 */
IFX_EXTERN IfxCan_Rxd_In IfxCan_RXD02A_P15_1_IN;  /**< \brief CAN receive input node 2 */
IFX_EXTERN IfxCan_Rxd_In IfxCan_RXD02B_P02_3_IN;  /**< \brief CAN receive input node 2 */
IFX_EXTERN IfxCan_Rxd_In IfxCan_RXD02C_P32_6_IN;  /**< \brief CAN receive input node 2 */
IFX_EXTERN IfxCan_Rxd_In IfxCan_RXD02D_P14_8_IN;  /**< \brief CAN receive input node 2 */
IFX_EXTERN IfxCan_Rxd_In IfxCan_RXD02E_P10_2_IN;  /**< \brief CAN receive input node 2 */
IFX_EXTERN IfxCan_Rxd_In IfxCan_RXD03A_P00_3_IN;  /**< \brief CAN receive input node 3 */
IFX_EXTERN IfxCan_Rxd_In IfxCan_RXD03B_P32_2_IN;  /**< \brief CAN receive input node 3 */
IFX_EXTERN IfxCan_Rxd_In IfxCan_RXD03C_P20_0_IN;  /**< \brief CAN receive input node 3 */
IFX_EXTERN IfxCan_Rxd_In IfxCan_RXD03D_P11_10_IN;  /**< \brief CAN receive input node 3 */
IFX_EXTERN IfxCan_Rxd_In IfxCan_RXD03E_P20_9_IN;  /**< \brief CAN receive input node 3 */
IFX_EXTERN IfxCan_Rxd_In IfxCan_RXD10A_P00_1_IN;  /**< \brief CAN receive input node 0 */
IFX_EXTERN IfxCan_Rxd_In IfxCan_RXD10B_P14_7_IN;  /**< \brief CAN receive input node 0 */
IFX_EXTERN IfxCan_Rxd_In IfxCan_RXD10C_P23_0_IN;  /**< \brief CAN receive input node 0 */
IFX_EXTERN IfxCan_Rxd_In IfxCan_RXD10D_P13_1_IN;  /**< \brief CAN receive input node 0 */
IFX_EXTERN IfxCan_Rxd_In IfxCan_RXD11A_P02_4_IN;  /**< \brief CAN receive input node 1 */
IFX_EXTERN IfxCan_Rxd_In IfxCan_RXD11B_P00_5_IN;  /**< \brief CAN receive input node 1 */
IFX_EXTERN IfxCan_Rxd_In IfxCan_RXD11C_P23_7_IN;  /**< \brief CAN receive input node 1 */
IFX_EXTERN IfxCan_Rxd_In IfxCan_RXD11D_P11_7_IN;  /**< \brief CAN receive input node 1 */
IFX_EXTERN IfxCan_Rxd_In IfxCan_RXD12A_P20_6_IN;  /**< \brief CAN receive input node 2 */
IFX_EXTERN IfxCan_Rxd_In IfxCan_RXD12B_P10_8_IN;  /**< \brief CAN receive input node 2 */
IFX_EXTERN IfxCan_Rxd_In IfxCan_RXD12C_P23_3_IN;  /**< \brief CAN receive input node 2 */
IFX_EXTERN IfxCan_Rxd_In IfxCan_RXD12D_P11_8_IN;  /**< \brief CAN receive input node 2 */
IFX_EXTERN IfxCan_Rxd_In IfxCan_RXD13A_P14_7_IN;  /**< \brief CAN receive input node 3 */
IFX_EXTERN IfxCan_Rxd_In IfxCan_RXD13B_P33_5_IN;  /**< \brief CAN receive input node 3 */
IFX_EXTERN IfxCan_Rxd_In IfxCan_RXD13C_P22_5_IN;  /**< \brief CAN receive input node 3 */
IFX_EXTERN IfxCan_Rxd_In IfxCan_RXD13D_P11_13_IN;  /**< \brief CAN receive input node 3 */
IFX_EXTERN IfxCan_Txd_Out IfxCan_TXD00_P02_0_OUT;  /**< \brief CAN transmit output node 0 */
IFX_EXTERN IfxCan_Txd_Out IfxCan_TXD00_P12_1_OUT;  /**< \brief CAN transmit output node 0 */
IFX_EXTERN IfxCan_Txd_Out IfxCan_TXD00_P20_8_OUT;  /**< \brief CAN transmit output node 0 */
IFX_EXTERN IfxCan_Txd_Out IfxCan_TXD00_P33_13_OUT;  /**< \brief CAN transmit output node 0 */
IFX_EXTERN IfxCan_Txd_Out IfxCan_TXD00_P33_8_OUT;  /**< \brief CAN transmit output node 0 */
IFX_EXTERN IfxCan_Txd_Out IfxCan_TXD00_P34_1_OUT;  /**< \brief CAN transmit output node 0 */
IFX_EXTERN IfxCan_Txd_Out IfxCan_TXD01_P01_3_OUT;  /**< \brief CAN transmit output node 1 */
IFX_EXTERN IfxCan_Txd_Out IfxCan_TXD01_P02_9_OUT;  /**< \brief CAN transmit output node 1 */
IFX_EXTERN IfxCan_Txd_Out IfxCan_TXD01_P14_0_OUT;  /**< \brief CAN transmit output node 1 */
IFX_EXTERN IfxCan_Txd_Out IfxCan_TXD01_P15_2_OUT;  /**< \brief CAN transmit output node 1 */
IFX_EXTERN IfxCan_Txd_Out IfxCan_TXD01_P33_9_OUT;  /**< \brief CAN transmit output node 1 */
IFX_EXTERN IfxCan_Txd_Out IfxCan_TXD02_P02_2_OUT;  /**< \brief CAN transmit output node 2 */
IFX_EXTERN IfxCan_Txd_Out IfxCan_TXD02_P10_3_OUT;  /**< \brief CAN transmit output node 2 */
IFX_EXTERN IfxCan_Txd_Out IfxCan_TXD02_P14_10_OUT;  /**< \brief CAN transmit output node 2 */
IFX_EXTERN IfxCan_Txd_Out IfxCan_TXD02_P15_0_OUT;  /**< \brief CAN transmit output node 2 */
IFX_EXTERN IfxCan_Txd_Out IfxCan_TXD02_P32_5_OUT;  /**< \brief CAN transmit output node 2 */
IFX_EXTERN IfxCan_Txd_Out IfxCan_TXD03_P00_2_OUT;  /**< \brief CAN transmit output node 3 */
IFX_EXTERN IfxCan_Txd_Out IfxCan_TXD03_P11_12_OUT;  /**< \brief CAN transmit output node 3 */
IFX_EXTERN IfxCan_Txd_Out IfxCan_TXD03_P20_10_OUT;  /**< \brief CAN transmit output node 3 */
IFX_EXTERN IfxCan_Txd_Out IfxCan_TXD03_P20_3_OUT;  /**< \brief CAN transmit output node 3 */
IFX_EXTERN IfxCan_Txd_Out IfxCan_TXD03_P32_3_OUT;  /**< \brief CAN transmit output node 3 */
IFX_EXTERN IfxCan_Txd_Out IfxCan_TXD10_P00_0_OUT;  /**< \brief CAN transmit output node 0 */
IFX_EXTERN IfxCan_Txd_Out IfxCan_TXD10_P13_0_OUT;  /**< \brief CAN transmit output node 0 */
IFX_EXTERN IfxCan_Txd_Out IfxCan_TXD10_P14_9_OUT;  /**< \brief CAN transmit output node 0 */
IFX_EXTERN IfxCan_Txd_Out IfxCan_TXD10_P23_1_OUT;  /**< \brief CAN transmit output node 0 */
IFX_EXTERN IfxCan_Txd_Out IfxCan_TXD11_P00_4_OUT;  /**< \brief CAN transmit output node 1 */
IFX_EXTERN IfxCan_Txd_Out IfxCan_TXD11_P02_5_OUT;  /**< \brief CAN transmit output node 1 */
IFX_EXTERN IfxCan_Txd_Out IfxCan_TXD11_P11_0_OUT;  /**< \brief CAN transmit output node 1 */
IFX_EXTERN IfxCan_Txd_Out IfxCan_TXD11_P23_6_OUT;  /**< \brief CAN transmit output node 1 */
IFX_EXTERN IfxCan_Txd_Out IfxCan_TXD12_P10_7_OUT;  /**< \brief CAN transmit output node 2 */
IFX_EXTERN IfxCan_Txd_Out IfxCan_TXD12_P11_1_OUT;  /**< \brief CAN transmit output node 2 */
IFX_EXTERN IfxCan_Txd_Out IfxCan_TXD12_P20_7_OUT;  /**< \brief CAN transmit output node 2 */
IFX_EXTERN IfxCan_Txd_Out IfxCan_TXD12_P23_2_OUT;  /**< \brief CAN transmit output node 2 */
IFX_EXTERN IfxCan_Txd_Out IfxCan_TXD13_P11_4_OUT;  /**< \brief CAN transmit output node 3 */
IFX_EXTERN IfxCan_Txd_Out IfxCan_TXD13_P14_6_OUT;  /**< \brief CAN transmit output node 3 */
IFX_EXTERN IfxCan_Txd_Out IfxCan_TXD13_P22_4_OUT;  /**< \brief CAN transmit output node 3 */
IFX_EXTERN IfxCan_Txd_Out IfxCan_TXD13_P33_4_OUT;  /**< \brief CAN transmit output node 3 */

/** \brief Table dimensions */
#define IFXCAN_PINMAP_NUM_MODULES 2
#define IFXCAN_PINMAP_NUM_NODES 4
#define IFXCAN_PINMAP_RXD_IN_NUM_ITEMS 7
#define IFXCAN_PINMAP_TXD_OUT_NUM_ITEMS 6


/** \brief IfxCan_Rxd_In table */
IFX_EXTERN const IfxCan_Rxd_In *IfxCan_Rxd_In_pinTable[IFXCAN_PINMAP_NUM_MODULES][IFXCAN_PINMAP_NUM_NODES][IFXCAN_PINMAP_RXD_IN_NUM_ITEMS];

/** \brief IfxCan_Txd_Out table */
IFX_EXTERN const IfxCan_Txd_Out *IfxCan_Txd_Out_pinTable[IFXCAN_PINMAP_NUM_MODULES][IFXCAN_PINMAP_NUM_NODES][IFXCAN_PINMAP_TXD_OUT_NUM_ITEMS];

/** \} */

#endif /* IFXCAN_PINMAP_H */
