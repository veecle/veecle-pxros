/***************************************************************
 *   PXROS-HR Network Stack
 *   Copyright HighTec EDV-Systeme GmbH 2020
 ***************************************************************/

#ifndef __ETH_AURIX_H__
#define __ETH_AURIX_H__

#include "EthError.h"
#include "assert.h"
#include "Cpu/Std/Ifx_Types.h"
#include "Geth/Eth/IfxGeth_Eth.h"
#include "Cpu/Std/IfxCpu.h"
#include "Port/Std/IfxPort.h"
#include "Cpu/Irq/IfxCpu_Irq.h"
#include "Src/Std/IfxSrc.h"



/* Event definition for Ethernet interrupts */
#define ETH_EV_RECEIVE      (1 << 0)    /* frame received */
#define ETH_EV_TRANSMIT     (1 << 1)    /* frame transmitted */
#define ETH_EV_TU           (1 << 2)    /* transmit buffer unavailable */
#define ETH_EV_RU           (1 << 3)    /* receive buffer unavailable */
#define ETH_EV_LINK         (1 << 4)    /* link state changed */

#define ETH_EV_MASK         (ETH_EV_RECEIVE | ETH_EV_TRANSMIT | ETH_EV_TU | ETH_EV_RU | ETH_EV_LINK)



#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* ================================================================================================
 * Data Structures
 * ==============================================================================================*/

typedef struct _GethDriver
{
	struct
	{
		IfxGeth_Eth geth;  /* GETH handle */
	} drivers;

} GethDriver;

typedef struct _EthData
{
	GethDriver gethDriver;

	uint8 macAddr[6];
} EthData_t;


/* The aligment is set 64 bit since circular buffer is configured to 64 bit.
 * The User needs to configure the same depending on their use-case			  */
IFX_EXTERN uint8 IFX_ALIGN(256) channel0TxBuffer1[ETH_MAX_BUFFER][ETH_BUFFER_SIZE];
/* Receive buffers needs extra 4 bytes to accomodate the Frame Check Sequence bytes */
IFX_EXTERN uint8 IFX_ALIGN(256) channel0RxBuffer1[ETH_MAX_BUFFER][ETH_BUFFER_SIZE];

/* Type definition for the events returned by the EthPoll() function */
typedef unsigned int EthPollEvents_t;

/* Type definition for the delay function that must be provided by the operating system */
typedef void ((*EthDelayFct_t) (unsigned long milliseconds));


void 				EthInitPhase1(EthData_t *data, unsigned int isrPrio);
EthError_t          EthInitPhase2(EthData_t *data);
void                EthConfigureSrn(unsigned int prio, unsigned int coreId);
void                EthEnableInterrupts(EthData_t *data);
void                EthStart(EthData_t *data);
void                EthStopTransmitDma(EthData_t *data);
void                EthStopMac(EthData_t *data);
EthError_t          EthGetLinkStatus(EthData_t *data);
EthPollEvents_t     EthPoll(EthData_t *data);
EthError_t          EthRead(EthData_t *data, unsigned char *buf, size_t *len);
EthError_t          EthWrite(EthData_t *data, unsigned char *buf, size_t len);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __ETH_AURIX_H__ */
