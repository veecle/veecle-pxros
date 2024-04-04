/***************************************************************
*   PXROS-HR Network Stack
*  Copyright HighTec EDV-Systeme GmbH 2020
***************************************************************/

#ifndef __PXNETCONFIG_H__
#define __PXNETCONFIG_H__

/* Maximum Ethernet frame length */
#include "Ifx_Cfg.h"           /* Get frame length define from driver */
#define PXETH_MAX_FRM_LEN       ETH_MAX_FRM_LEN
#define PXNET_MAC_ADDR_LEN      (6)

/* Task stack size of the Ethernet driver task */
#define PXETH_DRIVER_TASK_STACK_SIZE    (0x800)   /* in bytes */
#define PXETH_DRIVER_TASK_INT_STACK_SIZE    (64)   /* in bytes */

#endif /* __PXNETCONFIG_H__ */
