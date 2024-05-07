/***************************************************************
 *   PXROS-HR Network Stack
 *   Copyright HighTec EDV-Systeme GmbH 2020
 ***************************************************************/

#ifndef __PXETHDRIVER_H__
#define __PXETHDRIVER_H__

#include "EthAurix.h"
#include "PxNetDriverMessage.h"
#include "pxdef.h"

/* Driver Task Events */
#define EV_PXETHDELAY   (1 << 31)
#define EV_WRITE_TMO    (1 << 30)

/* Transmit status */
typedef enum _PxEthTxStatus
{
    PXETH_TX_IDLE,
    PXETH_TX_SENDING,
    PXETH_TX_STOPPING,
    PXETH_TX_STOPPED,
} PxEthTxStatus_t;

/* private data structure of driver */
typedef struct _PxEthData
{
    EthData_t       ethData;
    PxTask_t        driverTask;     /* driver task's ID         */
    PxMbx_t         rxBufMbx;       /* receive buffer pool      */
    PxMsg_t         txMsg;          /* current transmit msg     */
    PxMbx_t         txMbx;          /* transmit queue           */
    PxEthTxStatus_t txStatus;       /* transmit status          */
    PxUInt_t        txRetries;      /* transmit retry count     */
    PxTo_t          writeTimeout;   /* write timeout            */
    PxPe_t          linkCheckPe;    /* Pe for link checking     */
    PxBool_t        linkUp;         /* link established         */
} PxEthData_t;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* driver task function */
extern void PxEthDriverFunction(PxTask_t task, PxMbx_t pmbx, PxEvents_t events);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __PXETHDRIVER_H__ */
