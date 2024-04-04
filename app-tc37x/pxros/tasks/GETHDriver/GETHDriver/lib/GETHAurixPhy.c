/***************************************************************
 *   PXROS-HR Network Stack
 *   Copyright HighTec EDV-Systeme GmbH 2020
 ***************************************************************/

#include "GETHAurixPhy.h"


#include <machine/wdtcon.h>
#include <string.h>


/******************************************************************************/
/*----------------------------------Macros------------------------------------*/
/******************************************************************************/

#define IFXGETH_PHY_DP83825I_MDIO_BMCR     0x00

#define IFXGETH_PHY_DP83825I_MDIO_BMSR     0x01

#define IFXGETH_PHY_DP83825I_MDIO_PHYIDR1  0x02

#define IFXGETH_PHY_DP83825I_MDIO_PHYIDR2  0x03

#define IFXGETH_PHY_DP83825I_MDIO_ANAR     0x04

#define IFXGETH_PHY_DP83825I_MDIO_ALNPAR   0x05

#define IFXGETH_PHY_DP83825I_MDIO_ANER     0x06

#define IFXGETH_PHY_DP83825I_MDIO_PHYSTS   0x10

#define IFXGETH_PHY_DP83825I_WAIT_MDIO_READY() while (GETH_MAC_MDIO_ADDRESS.B.GB) {}


/******************************************************************************/
/*------------------------------Global variables------------------------------*/
/******************************************************************************/

volatile uint32 Ethernet_Phy_Id1;
volatile uint32 Ethernet_Phy_Id2;
volatile boolean Phy_IdRed = FALSE;

/******************************************************************************/
/*-----------------------Exported Variables/Constants-------------------------*/
/******************************************************************************/

uint32 IfxGeth_Eth_Phy_Dp83825i_iPhyInitDone = 0;


EthError_t EthPhyGetLinkStatus(Ifx_GETH *gethSFR)
{
    (void) gethSFR;
    Ifx_GETH_MAC_PHYIF_CONTROL_STATUS link_status;
    link_status.U = IfxGeth_Eth_Phy_Dp83825i_link_status();
    return link_status.B.LNKSTS ? ERR_OK : ERR_CONN;
}


/* --- internal functions --------------------------------------------------- */

void IfxGet_Eth_Phy_Dp83825i_reset(void) {

    /* now we check the phy to know the board version */
    IFXGETH_PHY_DP83825I_WAIT_MDIO_READY()

    // reset PHY
    // first we test to readout the reset bit to avoid timing issues (phy not yet ready)
    uint32 value;
    uint32 uiTimeout = 2000;
    do
    {
        GETH_MAC_MDIO_ADDRESS.U = (0 << 21) | (0 << 16) | (0 << 8) | (3 << 2) | (1 << 0);
        IFXGETH_PHY_DP83825I_WAIT_MDIO_READY()
        value = GETH_MAC_MDIO_DATA.U;
    } while ((value & 0x8000) && (uiTimeout--));  // wait for reset to finish

    if (uiTimeout != 0xFFFFFFFF)
    {
        // put data (reset the phy)
        GETH_MAC_MDIO_DATA.U = 0x8000;
        GETH_MAC_MDIO_ADDRESS.U = (0 << 21) | (0 << 16) | (0 << 8) |  (1 << 2) | (1 << 0);
        IFXGETH_PHY_DP83825I_WAIT_MDIO_READY()

        do
        {
            GETH_MAC_MDIO_ADDRESS.U = (0 << 21) | (0 << 16) | (0 << 8) | (3 << 2) | (1 << 0);
            IFXGETH_PHY_DP83825I_WAIT_MDIO_READY()
            value = GETH_MAC_MDIO_DATA.U;
        } while (value & 0x8000);  // wait for reset to finish

        // get ID
        GETH_MAC_MDIO_ADDRESS.U = (0 << 21) | (2 << 16) | (0 << 8) | (3 << 2) | (1 << 0);
        IFXGETH_PHY_DP83825I_WAIT_MDIO_READY()
        Ethernet_Phy_Id1 = GETH_MAC_MDIO_DATA.U;
        GETH_MAC_MDIO_ADDRESS.U = (0 << 21) | (3 << 16) | (0 << 8) | (3 << 2) | (1 << 0);
        IFXGETH_PHY_DP83825I_WAIT_MDIO_READY()
        Ethernet_Phy_Id2 = GETH_MAC_MDIO_DATA.U;
    }
    else
    {
        Ethernet_Phy_Id1 = 0;
    }
    Phy_IdRed = TRUE;
}

uint32 IfxGeth_Eth_Phy_Dp83825i_init(void)
{
    IFXGETH_PHY_DP83825I_WAIT_MDIO_READY();

    // reset PHY
    IfxGeth_Eth_Phy_Dp83825i_write_mdio_reg(0, IFXGETH_PHY_DP83825I_MDIO_BMCR, 0x8000);   // reset
    uint32 value;

    do
    {
        IfxGeth_Eth_Phy_Dp83825i_read_mdio_reg(0, IFXGETH_PHY_DP83825I_MDIO_BMCR, &value);
    } while (value & 0x8000);                                                      // wait for reset to finish

    /* Start Phy activity */
    IfxGeth_Eth_Phy_Dp83825i_write_mdio_reg(0, IFXGETH_PHY_DP83825I_MDIO_BMCR, 0x1200);    // enable auto-negotiation, restart auto-negotiation

    // done
    IfxGeth_Eth_Phy_Dp83825i_iPhyInitDone = 1;

    return 1;
}



uint32 IfxGeth_Eth_Phy_Dp83825i_link_status(void)
{
    Ifx_GETH_MAC_PHYIF_CONTROL_STATUS link_status;
    uint32 value;
    link_status.U = 0;

    if (IfxGeth_Eth_Phy_Dp83825i_iPhyInitDone)
    {
        // We read the phy status register
        IfxGeth_Eth_Phy_Dp83825i_read_mdio_reg(0, IFXGETH_PHY_DP83825I_MDIO_PHYSTS, &value);
        if (!(value & 0x2)) link_status.B.LNKSPEED = 1;
        if (value & 0x4) link_status.B.LNKMOD = 1;
        IfxGeth_Eth_Phy_Dp83825i_read_mdio_reg(0, IFXGETH_PHY_DP83825I_MDIO_BMSR, &value);
        if (value & 0x4) link_status.B.LNKSTS = 1;
    }

    return link_status.U;
}

void IfxGeth_Eth_Phy_Dp83825i_read_mdio_reg(uint32 layeraddr, uint32 regaddr, uint32 *pdata)
{
    // 5bit Physical Layer Adddress, 5bit GMII Regnr, 4bit csrclock divider, Read, Busy
    GETH_MAC_MDIO_ADDRESS.U = (layeraddr << 21) | (regaddr << 16) | (0 << 8) | (3 << 2) | (1 << 0);

    IFXGETH_PHY_DP83825I_WAIT_MDIO_READY();

    // get data
    *pdata = GETH_MAC_MDIO_DATA.U;
}


void IfxGeth_Eth_Phy_Dp83825i_write_mdio_reg(uint32 layeraddr, uint32 regaddr, uint32 data)
{
    // put data
    GETH_MAC_MDIO_DATA.U = data;

    // 5bit Physical Layer Adddress, 5bit GMII Regnr, 4bit csrclock divider, Write, Busy
    GETH_MAC_MDIO_ADDRESS.U = (layeraddr << 21) | (regaddr << 16) | (0 << 8) |  (1 << 2) | (1 << 0);

    IFXGETH_PHY_DP83825I_WAIT_MDIO_READY();
}
