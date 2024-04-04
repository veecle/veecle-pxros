/***************************************************************
 *   PXROS-HR Network Stack
 *   Copyright HighTec EDV-Systeme GmbH 2020
 ***************************************************************/

#ifndef __GETH_AURIX_GPIO_H__
#define __GETH_AURIX_GPIO_H__

#include "_PinMap/IfxGeth_PinMap.h"

/* pin configuration DP83825I*/
const IfxGeth_Eth_RmiiPins rmii_pins = {
   .crsDiv = &IfxGeth_CRSDVA_P11_11_IN,   /* CRSDIV */
   .refClk = &IfxGeth_REFCLKA_P11_12_IN,   /* REFCLK */
   .rxd0 = &IfxGeth_RXD0A_P11_10_IN,       /* RXD0 */
   .rxd1 = &IfxGeth_RXD1A_P11_9_IN,       /* RXD1 */
   .mdc = &IfxGeth_MDC_P21_2_OUT,         /* MDC */
   .mdio = &IfxGeth_MDIO_P21_3_INOUT,       /* MDIO */
   .txd0 = &IfxGeth_TXD0_P11_3_OUT,       /* TXD0 */
   .txd1 = &IfxGeth_TXD1_P11_2_OUT,       /* TXD1 */
   .txEn = &IfxGeth_TXEN_P11_6_OUT        /* TXEN */
};

#endif /* __GETH_AURIX_GPIO_H__ */
