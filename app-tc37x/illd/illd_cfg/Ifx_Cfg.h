/**************************************************************************************************
 * FILE: Ifx_Cfg.h
 *
 * DESCRIPTION:
 *     iLLD Configuration
 *
 *************************************************************************************************/

#ifndef IFX_CFG_H
#define IFX_CFG_H

/******************************************************************************/
/*-----------------------------------Macros-----------------------------------*/
/******************************************************************************/

/* ToDo: where to place this macro ?!? */
/* macro for using 2 ISRs (GETH needs 2 ISRs) */
#define NEEDS_TWO_ISR   1

#ifndef USE_RMII_ONLY
#define USE_RMII_ONLY   0
#endif /* USE_RMII_ONLY */

/* Ethernet Parameters */
#ifndef ETH_MTU
#define ETH_MTU         1500         /* Maximum number of octets occupied by Ethernet payload */
#endif /* ETH_MTU */

#define ETH_ADDR_LEN    6           /* Octets occupied by MAC address */
#define ETH_VLAN_LEN    4           /* Octets occupied by 802.1Q tag */
#define ETH_ETYPE_LEN   2           /* Octets occupied by Ethertype */
#define ETH_DATA_LEN    ETH_MTU     /* Max. octets in payload */
#define ETH_FCS_LEN     4           /* Octets in the FCS */

/* Ethernet header length (incl. VLAN tag) */
#define ETH_HDR_LEN         ((2 * ETH_ADDR_LEN) + ETH_VLAN_LEN + ETH_ETYPE_LEN + ETH_FCS_LEN)

/* Maximum Ethernet frame length */
#define ETH_MAX_FRM_LEN     (ETH_HDR_LEN + ETH_DATA_LEN)

/* DMA Buffers */
#define ETH_RAM_SIZE        (16 * 1024)
#define ETH_BUFFER_SIZE     ((ETH_DATA_LEN + 0xFF) & 0xFF00) // 1536
#define ETH_MAX_BUFFER      (ETH_RAM_SIZE / ETH_BUFFER_SIZE / 2) // 5

#ifndef IFXGETH_MAX_TX_DESCRIPTORS
#define IFXGETH_MAX_TX_DESCRIPTORS ETH_MAX_BUFFER // 5
#endif

#ifndef IFXGETH_MAX_RX_DESCRIPTORS
#define IFXGETH_MAX_RX_DESCRIPTORS ETH_MAX_BUFFER
#endif

/*______________________________________________________________________________
** Configuration for IfxScu_cfg.h
**____________________________________________________________________________*/

/**
 * \name Frequency configuration
 * \{ */

#define IFX_CFG_SCU_XTAL_FREQUENCY (20000000)                       /**< \copydoc IFX_CFG_SCU_XTAL_FREQUENCY */
#define IFX_CFG_SCU_PLL_FREQUENCY (300000000)                       /**< \copydoc IFX_CFG_SCU_PLL_FREQUENCY */

/** \} */

/** \} */

#endif /* IFX_CFG_H */
