/***************************************************************
 *   PXROS-HR Network Stack
 *   Copyright HighTec EDV-Systeme GmbH 2020
 ***************************************************************/

#include "EthAurix.h"
#include "GETHAurixGpio.h"
#include "GETHAurixPhy.h"
#include "Scu/Std/IfxScuCcu.h"

#include <string.h>

#pragma section ".eth_descriptors" awB
/* The User needs to configure the same depending on their use-case	*/
uint8 IFX_ALIGN(256) channel0TxBuffer1[ETH_MAX_BUFFER][ETH_BUFFER_SIZE];
uint8 IFX_ALIGN(256) channel0RxBuffer1[ETH_MAX_BUFFER][ETH_BUFFER_SIZE];
#pragma section

/* Initialize Ethernet hardware (part that needs to run in Privileged mode) */
void EthInitPhase1(EthData_t *data, unsigned int isrPrio)
{
    uint32 i;
    IfxGeth_Eth_Config config;
    IfxGeth_Index gethInst;
    GethDriver *gethDriver = &data->gethDriver;

    float32 freq;

    // if GETH clock is disabled
    if (0 == SCU_CCUCON5.B.GETHDIV)
        freq = IfxScuCcu_setGethFrequency(IFX_CFG_SCU_PLL_FREQUENCY);

    if (freq == 0)
    {
        // frequency was not set correctly
        assert(freq != 0);
    }

    IfxGeth_Eth_initModuleConfig(&config, &MODULE_GETH);
    gethDriver->drivers.geth.gethSFR = config.gethSFR;

    IfxGeth_enableModule(gethDriver->drivers.geth.gethSFR);
    gethInst = IfxGeth_getIndex(gethDriver->drivers.geth.gethSFR);

    // configure pins
    config.pins.rmiiPins = &rmii_pins;
    config.phyInterfaceMode = IfxGeth_PhyInterfaceMode_rmii;
    IfxGeth_setPhyInterfaceMode(gethDriver->drivers.geth.gethSFR, config.phyInterfaceMode);
    IfxGeth_Eth_setupRmiiOutputPins(&gethDriver->drivers.geth, config.pins.rmiiPins);
    IfxGeth_Eth_setupRmiiInputPins(&gethDriver->drivers.geth, config.pins.rmiiPins);

    config.mac.loopbackMode = IfxGeth_LoopbackMode_disable;
    config.mac.lineSpeed = IfxGeth_LineSpeed_100Mbps;

    for (i = 0; i < 6; ++i)
    {
        config.mac.macAddress[i] = data->macAddr[i];
    }

    // MTL configuration
    config.mtl.txSchedulingAlgorithm = IfxGeth_TxSchedulingAlgorithm_sp;
    config.mtl.rxArbitrationAlgorithm = IfxGeth_RxArbitrationAlgorithm_wsp;

    config.mtl.txQueue[0].storeAndForward = TRUE;
    config.mtl.txQueue[0].txQueueSize = IfxGeth_QueueSize_2560Bytes;
    config.mtl.rxQueue[0].storeAndForward = TRUE;
    config.mtl.rxQueue[0].rxQueueSize = IfxGeth_QueueSize_2560Bytes;
    config.mtl.rxQueue[0].daBasedDmaChannelEnabled = FALSE;
    config.mtl.rxQueue[0].rxQueueOverflowInterruptEnabled = TRUE;

    config.mtl.rxQueue[0].rxDmaChannelMap = IfxGeth_RxDmaChannel_0;

    // configure DMA
    config.dma.fixedBurstEnabled = TRUE;
    config.dma.addressAlignedBeatsEnabled = TRUE;
    gethDriver->drivers.geth.gethSFR->DMA_MODE.B.DA = 1;

    config.dma.txChannel[0].maxBurstLength = IfxGeth_DmaBurstLength_8;
    config.dma.txChannel[0].txDescrList = (IfxGeth_TxDescrList *)&IfxGeth_Eth_txDescrList[gethInst][0];
    config.dma.txChannel[0].txBuffer1Size = ETH_BUFFER_SIZE; // 1 descriptor per frame
    config.dma.rxChannel[0].maxBurstLength = IfxGeth_DmaBurstLength_8;
    config.dma.rxChannel[0].rxDescrList = (IfxGeth_RxDescrList *)&IfxGeth_Eth_rxDescrList[gethInst][0];
    config.dma.rxChannel[0].rxBuffer1Size = ETH_BUFFER_SIZE; // 1 descriptor per frame

    config.dma.txChannel[0].maxBurstLength = IfxGeth_DmaBurstLength_8;
    config.dma.txChannel[0].txDescrList = (IfxGeth_TxDescrList *)&IfxGeth_Eth_txDescrList[gethInst][0];
    config.dma.txChannel[0].txBuffer1Size = ETH_BUFFER_SIZE; // 1 descriptor per frame
    config.dma.rxChannel[0].maxBurstLength = IfxGeth_DmaBurstLength_8;
    config.dma.rxChannel[0].rxDescrList = (IfxGeth_RxDescrList *)&IfxGeth_Eth_rxDescrList[gethInst][0];
    config.dma.rxChannel[0].rxBuffer1Size = ETH_BUFFER_SIZE; // 1 descriptor per frame

    // -Wcast-align warning cause no problem here
    config.dma.txChannel[0].txBuffer1StartAddress = (uint32 *)&channel0TxBuffer1[0][0];
    config.dma.rxChannel[0].rxBuffer1StartAddress = (uint32 *)&channel0RxBuffer1[0][0];

    config.dma.txInterrupt[0].channelId = IfxGeth_DmaChannel_0;
    config.dma.txInterrupt[0].priority = isrPrio;

    config.dma.rxInterrupt[0].channelId = IfxGeth_DmaChannel_0;
    config.dma.rxInterrupt[0].priority = isrPrio + 1;

    config.dma.txInterrupt[0].provider = IfxCpu_Irq_getTos(IfxCpu_getCoreIndex());
    config.dma.rxInterrupt[0].provider = IfxCpu_Irq_getTos(IfxCpu_getCoreIndex());

    // initialize the module
    // make sure that the connected phy is also in the selected mode
    // important for PEF7071 on some boards waked up in RGMII mode
    // our reset will not work in this case
    // we was doing this in our main function where we get the ID's to detect the phy
    IfxGeth_Eth_initModule(&gethDriver->drivers.geth, &config);

    /* We get the ID of Ethernet Phy do determine the board version, also needed for SCR */
    IfxPort_setPinModeOutput(IfxGeth_MDC_P21_2_OUT.pin.port, IfxGeth_MDC_P21_2_OUT.pin.pinIndex, IfxPort_OutputMode_pushPull, IfxGeth_MDC_P21_2_OUT.select);
    GETH_GPCTL.B.ALTI0 = IfxGeth_MDIO_P21_3_INOUT.inSelect;

    // and enable transmitter/receiver
    IfxGeth_Eth_startTransmitters(&gethDriver->drivers.geth, 1);
    IfxGeth_Eth_startReceivers(&gethDriver->drivers.geth, 1);

    IfxGeth_mac_setCrcStripping(gethDriver->drivers.geth.gethSFR, TRUE, TRUE);
    IfxGeth_mac_setCrcChecking(gethDriver->drivers.geth.gethSFR, TRUE);
}

/* Initialize Ethernet hardware (part that runs in User mode) */
EthError_t EthInitPhase2(EthData_t *data)
{
    /* unused variables */
    (void)data;

    // initialize the PHY
    IfxGeth_Eth_Phy_Dp83825i_init();

    return ERR_OK;
}

/* Poll current DMA status to process interrupts */
EthPollEvents_t EthPoll(EthData_t *data)
{
    EthPollEvents_t ev = 0;
    Ifx_GETH *gethSFR = data->gethDriver.drivers.geth.gethSFR;
    unsigned int status = gethSFR->DMA_CH[IfxGeth_DmaChannel_0].STATUS.U;

    if (status & (IFX_GETH_DMA_CH_STATUS_RBU_MSK << IFX_GETH_DMA_CH_STATUS_RBU_OFF))
    {
        /* RX Buffer Unavailable */
        ev |= ETH_EV_RU;
    }

    if (status & (IFX_GETH_DMA_CH_STATUS_RI_MSK << IFX_GETH_DMA_CH_STATUS_RI_OFF))
    {
        ev |= ETH_EV_RECEIVE;
    }

    if (status & (IFX_GETH_DMA_CH_STATUS_TBU_MSK << IFX_GETH_DMA_CH_STATUS_TBU_OFF))
    {
        /* TX Buffer Unavailable */
        ev |= ETH_EV_TU;
    }

    if (status & (IFX_GETH_DMA_CH_STATUS_TI_MSK << IFX_GETH_DMA_CH_STATUS_TI_OFF))
    {
        ev |= ETH_EV_TRANSMIT;
    }

    /* Clear status bits */
    gethSFR->DMA_CH[IfxGeth_DmaChannel_0].STATUS.U = status;

    return ev;
}

/* enable ehternet transmit and receive interrupts on DMA channels */
__inline void EthEnableInterrupts(EthData_t *data)
{
    Ifx_GETH *gethSFR = data->gethDriver.drivers.geth.gethSFR;
    IfxGeth_dma_enableInterrupt(gethSFR, IfxGeth_DmaChannel_0, IfxGeth_DmaInterruptFlag_transmitInterrupt);
    IfxGeth_dma_enableInterrupt(gethSFR, IfxGeth_DmaChannel_0, IfxGeth_DmaInterruptFlag_receiveInterrupt);
}

/* start receivers and transmitters on DMA channels and MAC */
void EthStart(EthData_t *data)
{
    IfxGeth_Eth *geth = &data->gethDriver.drivers.geth;
    /* Enable RX and TX DMA and MAC for RX and TX transfers */
    IfxGeth_Eth_startReceiver(geth, IfxGeth_RxDmaChannel_0);
    IfxGeth_Eth_startTransmitter(geth, IfxGeth_TxDmaChannel_0);
}

/* stop transmitter on DMA channel and wait for any previous transmissions to complete */
void EthStopTransmitDma(EthData_t *data)
{
    Ifx_GETH *gethSFR = data->gethDriver.drivers.geth.gethSFR;
    /* steps from UM part2 chapter "44.3.18.5 Stopping and Starting Transmission" */
    /* 1. Disable transmit DMA */
    IfxGeth_dma_stopTransmitter(gethSFR, IfxGeth_TxDmaChannel_0); // for channel 0
    /* 2. Wait for any previous frame transmissions to complete */
    /*    (Loop while MTL Tx Queue Read Controller is in Read state or the MTL Tx Queue is not empty) */
    while ((gethSFR->MTL_TXQ0.DEBUG.B.TRCSTS == 1) || (gethSFR->MTL_TXQ0.DEBUG.B.TXQSTS == 1))
        ;
}

/* stop transmitter and receiver on MAC, stop receive */
void EthStopMac(EthData_t *data)
{
    Ifx_GETH *gethSFR = data->gethDriver.drivers.geth.gethSFR;
    /* steps from UM part2 chapter "44.3.18.5 Stopping and Starting Transmission" */
    /* 3. Disable the MAC transmitter and MAC receiver */
    IfxGeth_mac_disableTransmitter(gethSFR);
    IfxGeth_mac_disableReceiver(gethSFR);

    /* 4b Stop receive DMA */
    gethSFR->DMA_CH[IfxGeth_DmaChannel_0].RX_CONTROL.B.SR = FALSE; // for channel 0
}

/* get current link status */
EthError_t EthGetLinkStatus(EthData_t *data)
{
    return EthPhyGetLinkStatus(data->gethDriver.drivers.geth.gethSFR);
}

/* prepare (return) current RX descriptor for next use by DMA */
static void EthReturnCurrRxDescriptor(IfxGeth_Eth *geth)
{
    IfxGeth_Eth_freeReceiveBuffer(geth, IfxGeth_RxDmaChannel_0);
}

/* get pointer to buffer in current descriptor and get length of data in buffer */
static unsigned char *EthGetRxBufferPtr(IfxGeth_Eth *geth, size_t *len)
{
    void *ptr = 0;

    ptr = IfxGeth_Eth_getReceiveBuffer(geth, IfxGeth_RxDmaChannel_0);
    volatile IfxGeth_RxDescr *descr = IfxGeth_Eth_getActualRxDescriptor(geth, IfxGeth_RxDmaChannel_0);

    if (ptr)
    {
        *len = descr->RDES3.W.PL;
    }

    return ptr;
}

/* if data are ready, get pointer to them else no read */
EthError_t EthRead(EthData_t *data, unsigned char *buf, size_t *len)
{
    EthError_t err;
    size_t byteCount;
    unsigned char *rxBuf;

    if (!data)
    {
        return ERR_ARG;
    }

    IfxGeth_Eth *geth = &data->gethDriver.drivers.geth;

    /*
     * Read from descriptor
     */
    rxBuf = EthGetRxBufferPtr(geth, &byteCount);

    byteCount = ((byteCount < *len) ? byteCount : *len);
    if (rxBuf)
    {
        *len = 0;
        if (buf)
        {
            memcpy(buf, rxBuf, byteCount);
            *len = byteCount;
        }

        /* clean descriptor, even if buf == 0 (no read buffer available) */
        EthReturnCurrRxDescriptor(geth);

        err = ERR_OK;
    }
    else
    {
        /* no data to read */
        err = ERR_IO;
    }

    return err;
}

/* send data in buffer of length len via ethernet*/
EthError_t EthWrite(EthData_t *data, unsigned char *buf, size_t len)
{
    char *txBuf;

    /* create frame config */
    IfxGeth_Eth_FrameConfig frameConfig;

    if (!data || !buf || (len == 0))
    {
        return ERR_ARG;
    }

    IfxGeth_Eth *geth = &data->gethDriver.drivers.geth;

    geth->gethSFR->MAC_RXQ_CTRL1.B.UPQ = IfxGeth_TxDmaChannel_0; // channel index
    txBuf = IfxGeth_Eth_getTransmitBuffer(geth, IfxGeth_TxDmaChannel_0);

    /* check for place in descriptor list */
    if (!txBuf)
        return ERR_INPROGRESS;

    /* limit number of bytes to send (Ethernet header + MTU) */
    len = ((len < ETH_MAX_FRM_LEN) ? len : ETH_MAX_FRM_LEN);
    frameConfig.packetLength = len;
    /* write the header */
    frameConfig.channelId = IfxGeth_TxDmaChannel_0;

    /* set pointer to our buffer */
    memcpy(txBuf, buf, len);

    /* send frame */
    IfxGeth_Eth_sendFrame(geth, &frameConfig);

    IfxGeth_dma_clearInterruptFlag(geth->gethSFR,
                                   (IfxGeth_DmaChannel)frameConfig.channelId,
                                   IfxGeth_DmaInterruptFlag_transmitInterrupt);

    return ERR_OK;
}
