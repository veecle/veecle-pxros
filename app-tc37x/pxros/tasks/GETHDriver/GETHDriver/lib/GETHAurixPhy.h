/***************************************************************
 *   PXROS-HR Network Stack
 *   Copyright HighTec EDV-Systeme GmbH 2020
 ***************************************************************/

#ifndef __GETH_AURIX_PHY_H__
#define __GETH_AURIX_PHY_H__

#include "EthAurix.h"
#include "EthError.h"
#include "Geth/Std/IfxGeth.h"

/* ============================================================================
 * LLD DEFINES
 * ==========================================================================*/

EthError_t EthPhyGetLinkStatus(Ifx_GETH *gethSFR);

/******************************************************************************/
/*-------------------------Global Function Prototypes-------------------------*/
/******************************************************************************/

IFX_EXTERN void IfxGet_Eth_Phy_Dp83825i_reset(void);
IFX_EXTERN uint32 IfxGeth_Eth_Phy_Dp83825i_init(void);
IFX_EXTERN uint32 IfxGeth_Eth_Phy_Dp83825i_link_status(void);
IFX_EXTERN void IfxGeth_Eth_Phy_Dp83825i_read_mdio_reg(uint32 layeraddr, uint32 regaddr, uint32 *pdata);
IFX_EXTERN void IfxGeth_Eth_Phy_Dp83825i_write_mdio_reg(uint32 layeraddr, uint32 regaddr, uint32 data);

/******************************************************************************/
/*-------------------Global Exported Variables/Constants----------------------*/
/******************************************************************************/

IFX_EXTERN uint32 IfxGeth_Eth_Phy_Dp83825i_iPhyInitDone;


#endif /* __GETH_AURIX_PHY_H__ */
