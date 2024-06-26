/***************************************************************
*   PXROS-HR Network Stack
*  Copyright HighTec EDV-Systeme GmbH 2020
***************************************************************/

#ifndef __PXNETDRIVERMESSAGE_H__
#define __PXNETDRIVERMESSAGE_H__

#include "pxdef.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef unsigned long long PxMsgMetadata_t;

typedef enum __attribute__((__packed__))
{
    PXNET_DRV_INPKT,
    PXNET_DRV_OUTPKT,
    PXNET_DRV_RXBUF,
    PXNET_DRV_TXDONE,
} PxNetMsgType_t;

typedef union
{
    struct __attribute__((__packed__))
    {
        PxNetMsgType_t msg_type;
        PxUInt8_t instance;
        struct __attribute__((__packed__))
        {
            PxUInt16_t buf_len;
            PxUInt16_t actual_len;
        } frame;
    } s;
    PxMsgMetadata_t m;
} PxNetDrvMsg_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __PXNETDRIVERMESSAGE_H__ */
