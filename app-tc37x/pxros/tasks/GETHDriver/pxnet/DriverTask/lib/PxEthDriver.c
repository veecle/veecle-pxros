/***************************************************************
 *   PXROS-HR Network Stack
 *   Copyright HighTec EDV-Systeme GmbH 2020
 ***************************************************************/

#include "PxEthDriver.h"
#include "pxhndcall.h"
#include "EthAurix.h"
#include "PxNetDriverMessage.h"
#include "pxdef.h"
#include "PxNetConfig.h"
#include "pxros/tasks/taskPrios.h"

#include <string.h>

#ifndef NEEDS_TWO_ISR
#define NEEDS_TWO_ISR       0
#endif /* NEEDS_TWO_ISR */

/* Timeout value for write timeout */
#define WRITE_TMO_MILLIS    5L
#define WRITE_TMO_TICKS     PxTickGetTicksFromMilliSeconds(WRITE_TMO_MILLIS)
/* Timeout value for the link check */
#define LINK_CHECK_MILLIS   1000L
#define LINK_CHECK_TICKS    PxTickGetTicksFromMilliSeconds(LINK_CHECK_MILLIS)

/* Max. number of transmit retries */
#define MAXRETRIES  5


/* Adapter function to match the PxHndcall interface */
static int PxEthInitPhase1(va_list ap)
{
    PxEthData_t *drvData = va_arg(ap, PxEthData_t *);
    unsigned int isrPrio = va_arg(ap, unsigned int);
	EthInitPhase1(&drvData->ethData, isrPrio);
    return 0;
}

/* Function to install the Ethernet interrupt service routine */
static __inline void PxEthInstallIsr(void (*hnd)(PxArg_t), PxArg_t arg, unsigned int isrPrio)
{
    PxError_t err = PXERR_NOERROR;
    unsigned int prio = isrPrio;

    err = PxIntInstallFastContextHandler(prio, hnd, arg);
    if (err != PXERR_NOERROR)
    {
        PxPanic();
    }
#if (NEEDS_TWO_ISR > 0)
    /* luckily we can use the same ISR (for GETH) */
    err = PxIntInstallFastContextHandler(prio + 1, hnd, arg);
    if (err != PXERR_NOERROR)
    {
        PxPanic();
    }
#endif /* NEEDS_TWO_ISR > 0 */
}

/* Interrupt handling for ethernet driver */
static void PxEthInterruptInstaller(PxEthData_t *drvData, void (*hnd)(PxArg_t), PxArg_t arg, unsigned int isrPrio)
{
    PxEthInstallIsr(hnd, arg, isrPrio);
    EthEnableInterrupts(&drvData->ethData);
}

/* Poll the status of the ethernet module and signal the corresponding event
 * to the driver task (considering if called from handler or task level). */
static void PxEthPoll(PxEthData_t *drvData, PxBool_t fromHnd)
{
    EthPollEvents_t ev;

    assert(drvData != NULL);

    ev = EthPoll(&drvData->ethData);

    if (ev)
    {
        if (fromHnd)
        {
            PxTaskSignalEvents_Hnd(drvData->driverTask, ev);
        }
        else
        {
            PxTaskSignalEvents(drvData->driverTask, ev);
        }
    }
}

/* Handler function for Ethernet interrupts */
static void PxEthInterruptHandler(PxArg_t arg)
{
    PxEthData_t *drvData = (PxEthData_t *) arg;
    PxEthPoll(drvData, true);
}

/* Initialize the private data of the Ethernet driver. */
static __inline void PxEthDataInit(PxEthData_t *drvData, PxTask_t task)
{
    assert(drvData != NULL);

    memset(&drvData->ethData, 0, sizeof(EthData_t));
    const uint8 myMacAddressS[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
    memcpy(drvData->ethData.macAddr, myMacAddressS, PXNET_MAC_ADDR_LEN);
    drvData->driverTask      = task;
    drvData->rxBufMbx        = PxMbxRequest(PXOpoolTaskdefault);
    drvData->txMsg           = PxMsgIdInvalidate();
    drvData->txMbx           = PxMbxRequest(PXOpoolTaskdefault);
    drvData->txStatus        = PXETH_TX_STOPPED;
    drvData->txRetries       = 0;
    drvData->writeTimeout    = PxToRequest(PXOpoolTaskdefault, WRITE_TMO_TICKS, EV_WRITE_TMO);
    drvData->linkCheckPe     = PxPeRequest(PXOpoolTaskdefault, LINK_CHECK_TICKS, ETH_EV_LINK);
    drvData->linkUp          = false;
}

/* Retrieve the link status from the PHY and update the internal state
 * of the driver accordingly.
 * Inform the network stack in case the link status has changed. */
static void PxEthCheckLink(PxEthData_t *drvData)
{
    assert(drvData != NULL);

    /* Query the current link state from the driver. */
    drvData->linkUp = (ERR_OK == EthGetLinkStatus(&drvData->ethData)) ? true : false;
}

/* Release txMsg by resetting the metadata and sending it back to the network stack. */
/* Return an invalid message handle in case of success, or the original message handle otherwise. */
static PxMsg_t PxEthReleaseTxMsg(PxMsg_t txMsg)
{
    PxNetDrvMsg_t txMeta;

    /* Check that the provided message is a valid handle. */
    if (!PxMsgIdIsValid(txMsg))
    {
        assert(false);
        return txMsg;
    }

    /* Read and update the message metadata. */
    txMeta.m = PxMsgGetMetadata(txMsg);
    txMeta.s.msg_type = PXNET_DRV_TXDONE;
    /* Reset the actual length to 0. */
    txMeta.s.frame.actual_len = 0;
    PxMsgSetMetadata(txMsg, txMeta.m);
    /* Send the TX message back to the network stack by releasing it. */
    txMsg = PxMsgRelease(txMsg);
    assert(!PxMsgIdIsValid(txMsg));

    return txMsg;
}

/* Finish the current transmit job and release the according TX message. */
static void PxEthTxFinish(PxEthData_t *drvData)
{
    assert(drvData != NULL);

    assert((PXETH_TX_SENDING == drvData->txStatus)
        || (PXETH_TX_STOPPED == drvData->txStatus));

    /* Stop the transmit timeout which might still be running. */
    PxToStop(drvData->writeTimeout);
    /* Reset the timeout event. */
    PxResetEvents(EV_WRITE_TMO);

    /* Release the current TX message. */
    drvData->txMsg = PxEthReleaseTxMsg(drvData->txMsg);
    assert(!PxMsgIdIsValid(drvData->txMsg));

    /* Update the internal state of the driver */
    drvData->txMsg = PxMsgIdInvalidate();
    drvData->txStatus  = (PXETH_TX_SENDING == drvData->txStatus) ? PXETH_TX_IDLE : drvData->txStatus;
    drvData->txRetries = 0;
}

/* Read a frame from the Ethernet interface and send it via a receive message
 * to the network stack. In case no free RX message is available, the frame is
 * cleared from DMA anyway. */
static EthError_t PxEthRead(PxEthData_t *drvData)
{
    EthError_t       err;
    PxMsg_t          ioMsg;
    PxNetDrvMsg_t    ioMeta;
    unsigned char   *buf;
    size_t           len;

    assert(drvData != NULL);

    ioMeta.m = 0;
    buf = NULL;
    len = 0;

    /* Get a free receive buffer message from the RX message pool. */
    ioMsg = PxMsgReceive_NoWait(drvData->rxBufMbx);
    if (PxMsgIdIsValid(ioMsg))
    {
        /* Rx buffer message is available, get pointer to buffer. */
        buf = PxMsgGetData(ioMsg);

        /* Read metadata to get bufLen (total buffer size). */
        ioMeta.m = PxMsgGetMetadata(ioMsg);
        len = ioMeta.s.frame.buf_len;
    }

    /* Read at most len bytes of data into buf.
     * Clear frame from DMA even if no RX buffer message is available (buf == 0).
     * If no data is available to be read EthRead returns ERR_IO. */
    err = EthRead(&drvData->ethData, buf, &len);

    if (ERR_OK == err)
    {
        if (buf)
        {
            /* Rx message is available and EthRead was successful, so prepare the
             * message and send it to the network stack by releasing it. */
            ioMeta.s.msg_type = PXNET_DRV_INPKT;
            ioMeta.s.frame.actual_len = len;
            PxMsgSetMetadata(ioMsg, ioMeta.m);
            ioMsg = PxMsgRelease(ioMsg);
            /* ioMsg should be invalid now. */
            assert(!PxMsgIdIsValid(ioMsg));
        }
        else
        {
            /* No RX buffer message is available or getting the data pointer failed. */
            err = ERR_BUF;
        }
    }

    /* If ioMsg is still valid, something went wrong or no data could be read,
     * so we put the message back to the RX message pool */
    if (PxMsgIdIsValid(ioMsg))
    {
        /* Reset the metadata of the RX buffer message. */
        ioMeta.s.msg_type = PXNET_DRV_RXBUF;
        ioMeta.s.frame.actual_len = 0;
        PxMsgSetMetadata(ioMsg, ioMeta.m);
        PxMsgSend(ioMsg, drvData->rxBufMbx);
    }

    return err;
}

/* Write the buffer content of the current TX message to the Ethernet interface
 * and send this frame. */
static void PxEthWrite(PxEthData_t *drvData)
{
    PxNetDrvMsg_t    ioMeta;
    PxUChar_t       *buf;

    assert(drvData != NULL);

    /* Ensure that a TX message is actually pending. */
    assert(PxMsgIdIsValid(drvData->txMsg));

    /* If the number of retries is exceeded, drop the frame and release the TX message. */
    if (drvData->txRetries >= MAXRETRIES)
    {
        PxEthTxFinish(drvData);
    }
    else
    {
        /* Read metadata of the TX message. */
        ioMeta.m = PxMsgGetMetadata(drvData->txMsg);
        assert(PXNET_DRV_OUTPKT == ioMeta.s.msg_type);

        /* Get pointer to the buffer of the TX message. */
        buf = PxMsgGetData(drvData->txMsg);
        if (buf)
        {
            /* Check link status before sending. */
            PxEthCheckLink(drvData);
            if (drvData->linkUp)
            {
                /* Link is up, change TX status to SENDING. */
                drvData->txStatus = PXETH_TX_SENDING;
                /* Send the frame by copying the buffer to the descriptor buffer. */
                (void) EthWrite(&drvData->ethData, buf, ioMeta.s.frame.actual_len);
                /* Release data access to the buffer, because sending is asynchronous. */
                (void) PxMsgRelDataAccess(drvData->txMsg);
                /* Start timeout for getting the transmit interrupt. */
                PxToStart(drvData->writeTimeout);
                /* Increment retry count. */
                ++drvData->txRetries;
            }
            else
            {
                /* Transmit cannot be done without a link, so drop this frame. */
                PxEthTxFinish(drvData);
            }
        }
    }
}

/* Task function of the PXNET Ethernet driver task */
void PxEthDriverFunction(PxTask_t task, PxMbx_t taskMbx, PxEvents_t events)
{
    PxEthData_t       drvData;          /* Internal data of the driver  */
    PxMsgEvent_t      receivedMsgEv;    /* Encompasses ev and ioMsg     */
    PxEvents_t        ev;               /* received events              */
    PxMsg_t           ioMsg;            /* PxMsg containing the I/O job */
    PxNetDrvMsg_t     ioMeta;           /* Metadata containing job data */
    unsigned int      isrPrio;          /* Ethernet interrupt priority  */
    EthError_t        ethErr;

    (void) events;   /* suppress warning about unused parameter */

    PxError_t regErr = PxMbxRegisterMbx(_PxTcpAccessReqMbxId, taskMbx);
    if (regErr != PXERR_NOERROR)
        PxPanic();

    /* Initialize driver data. */
    PxEthDataInit(&drvData, task);

    /* Check that the allocation of all PXROS objects succeeded. */
    if ((PxMbxIdError(drvData.rxBufMbx) != PXERR_NOERROR)
        || (PxMbxIdError(drvData.txMbx) != PXERR_NOERROR)
        || (PxToIdError(drvData.writeTimeout) != PXERR_NOERROR)
        || (PxPeIdError(drvData.linkCheckPe) != PXERR_NOERROR))
    {
        PxAwaitEvents(0);
    }

    /* Initialize local variables. */
    isrPrio = GETH_ISR_PRIO;

    /* Initialize all parts that require supervisor privileges. */
    _PxHndcallVA(PxEthInitPhase1, &drvData, isrPrio);

    /* Initialize all remaining parts that do not need supervisor privileges. */
    ethErr = EthInitPhase2(&drvData.ethData);

    /* only continue if
       all OK
       no link yet
       otherwise terminate driver task
    */
    switch (ethErr)
    {
    case ERR_OK:
    case ERR_CONN:
        break;
    case ERR_NOTSUPP:
    default:
        PxAwaitEvents(0);
        break;
    }

    /* Install the interrupt handler and enable the interrupt for the Ethernet module */
    PxEthInterruptInstaller(&drvData, PxEthInterruptHandler, (PxArg_t) &drvData, isrPrio);

    EthStart(&drvData.ethData);

    /* Everything is now set up, so check the link in order to get it to make the
     * Ethernet module start operation. */
    while (!drvData.linkUp)
    {
        PxEthCheckLink(&drvData);
    }

    /* The link is up, set status to IDLE */
    drvData.txStatus = PXETH_TX_IDLE;

    /* Main loop of driver.
     * This loop will not be left for the lifetime of the application. */
    for (ioMsg = PxMsgIdInvalidate(); ; ioMsg = PxMsgIdInvalidate())
    {
        /* Wait for an event signaled by the interrupt handler or for a new command message. */
        receivedMsgEv = PxMsgReceive_EvWait(taskMbx, (ETH_EV_MASK | EV_WRITE_TMO));
        ioMsg = receivedMsgEv.msg;
        ev = receivedMsgEv.events;

        if (ev)
        {
            if (ev & ETH_EV_RECEIVE)
            {
                /* We have received something, process all received frames. */
                while (ERR_OK == PxEthRead(&drvData));
            }

            if (ev & ETH_EV_TRANSMIT)
            {
                /* We got a transmit interrupt indicating that the previous transmission
                 * is finished. */

                assert((PXETH_TX_SENDING == drvData.txStatus)
                    || (PXETH_TX_STOPPING == drvData.txStatus));

                /* Unset the write timeout event from the mask in case it was also triggered. */
                ev &= ~EV_WRITE_TMO;

                if (PXETH_TX_STOPPING == drvData.txStatus)
                {
                    /* Continue stopping the Ethernet module. */
                    EthStopMac(&drvData.ethData);
                    drvData.txStatus = PXETH_TX_STOPPED;

                    /* Flush all pending TX messages that are queued in the TX mailbox. */
                    do
                    {
                        PxEthReleaseTxMsg(drvData.txMsg);
                        drvData.txMsg = PxMsgReceive_NoWait(drvData.txMbx);
                    } while(PxMsgIdIsValid(drvData.txMsg));
                    drvData.txRetries = 0;
                }
                else if (PXETH_TX_SENDING == drvData.txStatus)
                {
                    /* Release the TX message. */
                    PxEthTxFinish(&drvData);
                }

                if (PXETH_TX_IDLE == drvData.txStatus)
                {
                    /* If there is no pending TX message, pick the next one from the TX mailbox. */
                    if (!PxMsgIdIsValid(drvData.txMsg))
                    {
                        drvData.txMsg = PxMsgReceive_NoWait(drvData.txMbx);
                    }

                    /* If there is now a pending TX message, start sending it. */
                    if (PxMsgIdIsValid(drvData.txMsg))
                    {
                        PxEthWrite(&drvData);
                    }
                }
            }

            if (ev & EV_WRITE_TMO)
            /* When the timeout expires, just try again. */
            {
                PxEthWrite(&drvData);
            }

            /* no special handling for ETH_EV_TU and ETH_EV_RU */
        }

        if (ev)
        {
            /* Repoll LAN once more to see whether there is something left. */
            PxEthPoll(&drvData, false);
        }

        /* Skip message handling if no command message was received. */
        if (!PxMsgIdIsValid(ioMsg))
        {
            continue;
        }

        /* Read metadata and handle the received command message. */
        ioMeta.m = PxMsgGetMetadata(ioMsg);
        switch (ioMeta.s.msg_type)
        {
            case PXNET_DRV_OUTPKT:
                /* We got an outgoing packet. */
                if (PXETH_TX_IDLE == drvData.txStatus)
                {
                    /* Ensure that there is currently no TX message pending. */
                    assert(!PxMsgIdIsValid(drvData.txMsg));
                    /* Memorize the TX message and write it to the Ethernet module. */
                    drvData.txMsg = ioMsg;
                    PxEthWrite(&drvData);
                }
                else if (PXETH_TX_SENDING == drvData.txStatus)
                {
                    /* There is an ongoing transmission, so enqueue the new TX message. */
                    ioMsg = PxMsgSend(ioMsg, drvData.txMbx);
                }
                else
                {
                    /* The transmit paths are being shut down, drop the TX message. */
                    (void) PxEthReleaseTxMsg(ioMsg);
                }
                break;
            case PXNET_DRV_RXBUF:
                /* We got a receive buffer message, put it into the pool. */
                PxMsgSend(ioMsg, drvData.rxBufMbx);
                break;
            default:
                /* Should never happen, break program execution */
                PxMsgRelease(ioMsg);
                assert(false);
                break;
        }
    }
}
