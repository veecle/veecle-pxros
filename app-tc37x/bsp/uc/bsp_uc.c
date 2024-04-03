/**************************************************************************************************
 * FILE: bsp_uc.c
 *
 * UC ARCHITECTURE:
 *
 *     TC3xx
 *
 * UC FAMILY:
 *
 *     TC3xx
 *
 * DESCRIPTION:
 *
 *     Low Level routines for TC3x architecture.
 *     They represent family generic implementation, that might be replaced
 *     by a family particular version in bsp_uc_<family>.c file
 *
 * FILE HISTORY:
 *
 *    V1  05.2020  RO
 *    - Base reference example
 *
 **************************************************************************************************
 *
 * SPDX-FileCopyrightText: HighTec EDV-Systeme GmbH
 *
 * SPDX-License-Identifier: Boost Software License - Version 1.0
 *
 *************************************************************************************************/

/* Include its own API interface and types */
#include "bsp.h"


/* ------------------------------------------------------------------------------------------------
 * FUNCTION: bsp_uc_scu_wdt_DisableSafetyWatchdog()
 *    Disables Safety WATCHDOG module (one in TC3x uC)
 * ----------------------------------------------------------------------------------------------*/

#ifndef BSP_UC_SCU_WDT_DISABLESAFETYWATCHDOG_UC_SPECIFIC

void bsp_uc_scu_wdt_DisableSafetyWatchdog(void)
{
    unlock_safety_wdtcon();
    SCU_WDTS_CON1.B.DR = 1;
    lock_safety_wdtcon();
}

#endif


/* ------------------------------------------------------------------------------------------------
 * FUNCTION: bsp_uc_scu_wdt_DisableCpuWatchdog()
 *    Disables Core WATCHDOG module.
 * ----------------------------------------------------------------------------------------------*/

#ifndef BSP_UC_SCU_WDT_DISABLECPUWATCHDOG_UC_SPECIFIC

void bsp_uc_scu_wdt_DisableCpuWatchdog()
{
    uint32_t coreId = bsp_uc_core_GetCurrentCore();
    unlock_wdtcon();
    MODULE_SCU.WDTCPU[coreId].CON1.B.DR = 1;
    lock_wdtcon();
}

#endif


/* ------------------------------------------------------------------------------------------------
 * FUNCTION: bsp_uc_scu_EnableXOSC()
 *    It enables an external oscillator.
 *    To wait for stabilization of the XOSC it shall be enough to use
 *    OSC_WDG PLLLV and PLLHV bits. For that XOSC must be selected as source for PLL
 *       1. Select XTAL as input to PLL
 *       2. Set right foscref frequency through OSCVAL
 *       3. Restart OSC WDG in PLL
 *       4. Wait for PLLLV and PLLHV status bits from OSC WDG
 * ----------------------------------------------------------------------------------------------*/

#ifndef BSP_UC_SCU_ENABLEXOSC_UC_SPECIFIC

void bsp_uc_scu_EnableXOSC(void)
{
    /* reset safety ENDINIT bit, any CCU write is SE protected */
    unlock_safety_wdtcon();

    /* switch fSYSTEM to BACKUP clock in case it is not done already
	 * XOSC setting change might have an impact on system timing otherwise
	 */
    if (SCU_CCUCON0.B.CLKSEL != UC_CCU_CON0_CLKSEL_BACKUP)
    {
        /* request the change */
    	while (SCU_CCUCON0.B.LCK != 0U) {}
        SCU_CCUCON0.B.CLKSEL = UC_CCU_CON0_CLKSEL_BACKUP;
        SCU_CCUCON0.B.UP = 1;
        /* wait for the change to complete */
        while (SCU_CCUCON0.B.LCK != 0U) {}
    }

    /* Enable XOSC (external crystal mode) */
    SCU_OSCCON.B.MODE = 0U;

    /* Select XTAL as PLL input clock to enable XOSC WDG functionality */
    SCU_SYSPLLCON0.B.INSEL = UC_CCU_CON1_INSEL_XOSC;

    /* OSCVAL defines the divider value that generates the reference clock 'fOSCREF'
     * that is supervised by the oscillator WDG.
     * fOSCREF = fOSC / (OSCVAL + 1) = ~ 2.5Mhz  => OSCVAL = (fOSC / 2.5Mhz) - 1
     */
    SCU_OSCCON.B.OSCVAL = ((uint32_t)(BOARD_XTAL_CLOCK * 1000000) / 2500000) - 1;

    /* The Oscillator WDG is cleared and restarted
     * both PLLLV & PLLHV bits are cleared
     */
    SCU_OSCCON.B.OSCRES = 1U;

    /* set safety ENDINIT bit */
    lock_safety_wdtcon();

    /* wait until PLLLV and PLLHV flags are set => then XOSC should be stable
     * for PLL => shall be stable in general
     * Good to be outside SafetyEndInit in case it takes longer than expected
     * - according DS it can be up to 5ms
     */
    while ((SCU_OSCCON.B.PLLLV == 0) || (SCU_OSCCON.B.PLLHV == 0))
        ;
}

#endif


/* ------------------------------------------------------------------------------------------------
 * FUNCTION: bsp_uc_scu_EnablePLL0()
 *    It enables SYSTEM-PLL with required parameters.
 * Input
 *    pdiv       : Input divider
 *    ndiv       : VCO feedback divider
 *    k2         : end output divider
 * Note
 *    It waits for PLL stabilization in infinitive loop!
 * ----------------------------------------------------------------------------------------------*/

#ifndef BSP_UC_SCU_ENABLEPLL0_UC_SPECIFIC

void bsp_uc_scu_EnablePLL0(uint32_t pdiv, uint32_t ndiv, uint32_t k2)
{
    /* reset safety ENDINIT bit */
    unlock_safety_wdtcon();

    /* check SYSPLL POWERDOWN mode;
     * if not force it and wait for it
     */
    if (0 == SCU_SYSPLLSTAT.B.PWDSTAT)
    {
        SCU_SYSPLLCON0.B.PLLPWD = 0;
        while (0 == SCU_SYSPLLSTAT.B.PWDSTAT) ;
    }

    /* setup PLL and disable POWERDOWN mode */
    SCU_SYSPLLCON0.B.NDIV = ndiv;
    SCU_SYSPLLCON0.B.PDIV = pdiv;
    SCU_SYSPLLCON0.B.PLLPWD = 1;

    /* wait until power up is finished and PLL is locked
     * restart PLL lock mechanism after power up
     */
    while (SCU_SYSPLLSTAT.B.PWDSTAT == 1) ;
    SCU_SYSPLLCON0.B.RESLD = 1;
    while (0 == SCU_SYSPLLSTAT.B.LOCK) ;

    /* Set final output divider - no stepping implemented */
    while (SCU_SYSPLLSTAT.B.K2RDY == 0U) ;
    SCU_SYSPLLCON1.B.K2DIV = k2;

    /* set safety ENDINIT bit */
    lock_safety_wdtcon();
}

#endif


/* ------------------------------------------------------------------------------------------------
 * FUNCTION: bsp_uc_scu_EnablePLL1()
 *    It enables PERIPHERAL-PLL with required parameters.
 * Input
 *    pdiv      : Input divider
 *    ndiv      : VCO feedback divider
 *    k2        : fPLL1 output divider
 *    k3        : fPLL2 output divider
 * ----------------------------------------------------------------------------------------------*/

#ifndef BSP_UC_SCU_ENABLEPLL1_UC_SPECIFIC

void bsp_uc_scu_EnablePLL1(uint32_t pdiv, uint32_t ndiv, uint32_t k2, uint32_t k3)
{
    /* reset safety ENDINIT bit */
    unlock_safety_wdtcon();

    /* check SYSPLL POWERDOWN mode;
     * if not force it and wait for it
     */
    if (0 == SCU_PERPLLSTAT.B.PWDSTAT)
    {
        SCU_PERPLLCON0.B.PLLPWD = 0;
        while (0 == SCU_PERPLLSTAT.B.PWDSTAT) ;
    }

    /* setup PLL and disable POWERDOWN mode */
    SCU_PERPLLCON0.B.NDIV = ndiv;
    SCU_PERPLLCON0.B.PDIV = pdiv;
    SCU_PERPLLCON0.B.DIVBY = UC_PLL1_DIBBY;
    SCU_PERPLLCON0.B.PLLPWD = 1;

    /* wait until power up is finished and PLL is locked
     * restart PLL lock mechanism after power up
     */
    while (SCU_PERPLLSTAT.B.PWDSTAT == 1) ;
    SCU_PERPLLCON0.B.RESLD = 1;
    while (0 == SCU_PERPLLSTAT.B.LOCK) ;

    /* Set final output dividers - no stepping implemented
     * K2 - fPLL1
     * K3 - fPLL2
     */
    while (SCU_PERPLLSTAT.B.K2RDY == 0U) ;
    SCU_PERPLLCON1.B.K2DIV = k2;
    while (SCU_PERPLLSTAT.B.K3RDY == 0U) ;
    SCU_PERPLLCON1.B.K3DIV = k3;

    /* set safety ENDINIT bit */
    lock_safety_wdtcon();
}

#endif


/* ------------------------------------------------------------------------------------------------
 * FUNCTION: bsp_uc_ccu_SetSystemClock()
 *    Selects requested clock input for SYSTEM clock. This operation shall be executed when
 *    the requested input clock is stable and System clock dividers are set properly.
 * Input
 *    sysclk : clock input from fixed set in uc_types.h
 * ----------------------------------------------------------------------------------------------*/

#ifndef BSP_UC_SCU_SWITCHSYSTEMCLOCK_UC_SPECIFIC

void bsp_uc_scu_SwitchSystemClock(UC_CCU_SYSTEM_CLOCK_e sysclk)
{
    /* Switch right clock path based on the request */
    switch (sysclk)
    {
    case UC_CCU_SYSTEM_CLOCK_BACKUP:

        /* only switch to BACKUP clock if not already present */
        if (SCU_CCUCON0.B.CLKSEL != UC_CCU_CON0_CLKSEL_BACKUP)
        {
            unlock_safety_wdtcon();
            /* request the change */
            while (SCU_CCUCON0.B.LCK != 0U) ;
            SCU_CCUCON0.B.CLKSEL = UC_CCU_CON0_CLKSEL_BACKUP;
            SCU_CCUCON0.B.UP = 1;
            /* wait for the request to complete */
            while (SCU_CCUCON0.B.LCK != 0U) ;
            lock_safety_wdtcon();
        }
        break;

    case UC_CCU_SYSTEM_CLOCK_PLL0:

        /* switch FSYSTEM to BACKUP clock if not done already, before we set it for PLL */
        if (SCU_CCUCON0.B.CLKSEL != UC_CCU_CON0_CLKSEL_BACKUP)
        {
            bsp_uc_scu_SwitchSystemClock(UC_CCU_SYSTEM_CLOCK_BACKUP);
        }

        /* check if XOSC is stable as it should be after XOSC enable
		 * if not, enable XOSC
		 */
        if ((SCU_OSCCON.B.PLLLV == 0) || (SCU_OSCCON.B.PLLHV == 0))
        {
            bsp_uc_scu_EnableXOSC();
        }

        /* Check if SYSPLL is locked, if not Enable PLL */
        if (SCU_SYSPLLSTAT.B.LOCK == 0)
        {
            bsp_uc_scu_EnablePLL0(UC_PLL0_PDIV, UC_PLL0_NDIV, UC_PLL0_K2);
        }

        /* Check if PERPLL is locked, if not Enable PLL */
        if (SCU_PERPLLSTAT.B.LOCK == 0)
        {
            bsp_uc_scu_EnablePLL1(UC_PLL1_PDIV, UC_PLL1_NDIV, UC_PLL1_K2, UC_PLL1_K3);
        }

        /* Change the clock source */
        unlock_safety_wdtcon();
        while (SCU_CCUCON0.B.LCK != 0U) ;
        SCU_CCUCON0.B.CLKSEL = UC_CCU_CON0_CLKSEL_PLL0;
        SCU_CCUCON0.B.UP = 1;
        while (SCU_CCUCON0.B.LCK != 0U) ;
        lock_safety_wdtcon();

        break;

    default:
        break;
    }
}

#endif


/* ------------------------------------------------------------------------------------------------
 * FUNCTION: bsp_uc_scu_SetCcuCon()
 *    Setting of System clock dividers. Their values depend on System clock frequency.
 *    Their default (reset) values are set to safe values.
 *    No need to change it when running on max clock.
 * Input
 *    reg     : pointer to right SC_CCU_CONx register
 *    value   : content to write to the register
 *    update  : request to update CCU module with the content of the CONx
 *              register(s)
 * Note
 *    It shall work on all CONx registers, even those for CPU not having
 *    LCK bit (shall be read as 0 - meaning ready for update)
 * ----------------------------------------------------------------------------------------------*/

#ifndef BSP_UC_SCU_SETCCUCON_UC_SPECIFIC

void bsp_uc_scu_SetCcuCon(volatile unsigned int *reg, uint32_t value, uint32_t update)
{
    volatile uint32_t *pcon = (volatile uint32_t *)reg;

    /* reset safety ENDINIT bit */
    unlock_safety_wdtcon();

    /* Wait until register can be updated (lock = 0) */
    while ((*pcon & (uint32_t)0x80000000) != 0) ;

    /* request CCU module to take content of CON register(s)
     * or prepare only the value in CON
     */
    if (update)
        value = value | (uint32_t)0x40000000;
    else
        value = value & ~(uint32_t)0x40000000;

    /* write the SCU_CCU_CONx register */
    *pcon = value;

    /* set safety ENDINIT bit */
    lock_safety_wdtcon();
}

#endif


/* ------------------------------------------------------------------------------------------------
 * FUNCTION: bsp_uc_ccu_SelectExtClk0()
 *    Sets requested input clock to EXTCLK0 and enable.
 * Input
 *    inclk : clock selection from fixed set
 *    div   : divider for fout, the others do not have it
 *            div is divided by 2 to get user expected clock
 *            fout = ((fspb / n) / 2)
 *            div is expected to be even
 * ----------------------------------------------------------------------------------------------*/

#ifndef BSP_UC_CCU_SELECTEXTCLK0_UC_SPECIFIC

void bsp_uc_ccu_SelectExtClk0(UC_EXTCLK0_SEL_e inclk, uint32_t div)
{
    unlock_safety_wdtcon();
    SCU_FDR.B.STEP = 1024 - ((div >> 1) & 0x3ff);
    SCU_FDR.B.DM = 1;
    SCU_EXTCON.B.SEL0 = inclk;
    SCU_EXTCON.B.EN0 = 1;
    lock_safety_wdtcon();
}

#endif


/* ------------------------------------------------------------------------------------------------
 * FUNCTION: bsp_uc_ccu_SelectExtClk1()
 *    Sets requested input clock to EXTCLK0 and enable.
 * Input
 *    inclk : clock selection from fixed set
 *    div   : divider only for Fout frequency (Fspb / (DIV + 1)),
 *            others will ignore it
 * ----------------------------------------------------------------------------------------------*/

#ifndef BSP_UC_CCU_SELECTEXTCLK1_UC_SPECIFIC

void bsp_uc_ccu_SelectExtClk1(UC_EXTCLK1_SEL_e inclk, uint32_t div)
{
    unlock_safety_wdtcon();
    SCU_EXTCON.B.SEL1 = inclk;
    SCU_EXTCON.B.DIV1 = div - 1;
    SCU_EXTCON.B.EN1 = 1;
    lock_safety_wdtcon();
}

#endif


/* ------------------------------------------------------------------------------------------------
 * FUNCTION: bsp_uc_core_StartCore()
 *    Starting requested core from given reset_vector
 * Input
 *    coreId       : [0..n] logical core index
 *    reset_vector : pointer to function address where the Core will start
 *                   after the reset
 * ----------------------------------------------------------------------------------------------*/

#ifndef BSP_UC_CORE_STARTCORE_UC_SPECIFIC

void bsp_uc_core_StartCore(uint32_t coreId, uint32_t *reset_vector)
{
	volatile Ifx_CPU_PC* corePC = &CPU0_PC;
	volatile Ifx_CPU_SYSCON* coreSYSCON = &CPU0_SYSCON;

    /* do not switch current core */
    if (coreId == bsp_uc_core_GetCurrentCore())
        return;

    /* check coreId limit */
    if (coreId >= UC_NB_CORES)
    	return;

    /* Adjust logical CoreId to Physical index */
    if (coreId == 5)
    	coreId++;

    /* adjust register pointers */
    corePC = (Ifx_CPU_PC*) ((uint32_t) corePC + ((uint32_t) 0x20000 * coreId));
    coreSYSCON = (Ifx_CPU_SYSCON*) ((uint32_t) coreSYSCON + ((uint32_t) 0x20000 * coreId));

    /* do not start already active core */
    if (coreSYSCON->B.BHALT == 0)
    	return;

    /* update registers */
    corePC->U = (unsigned int)reset_vector;     /* Set the start address */
    coreSYSCON->B.BHALT = 0;                    /* Reset HALT sate */
}

#endif


/* ------------------------------------------------------------------------------------------------
 * FUNCTION: bsp_uc_core_StartAllCores()
 *    Starts all inactive cores on multi-core uC derivative.
 *    Active ones are left untouched.
 *    All inactive cores will start at _crt0_reset entry point.
 *    All cores share the same crt0 startup routine.
 * ----------------------------------------------------------------------------------------------*/

#ifndef BSP_UC_CORE_STARTALLCORES_UC_SPECIFIC

extern void _crt0_reset(void);

void bsp_uc_core_StartAllCores(void)
{
    uint32_t i;
    for (i = 0; i < UC_NB_CORES; i++)
        bsp_uc_core_StartCore(i, (uint32_t *)&_crt0_reset);
}

#endif


/* ------------------------------------------------------------------------------------------------
 * FUNCTION: bsp_uc_port_EnableInput()
 *    Configures requested pin to Input mode
 * Input
 *    port     : port number
 *    pinIndex : pin index in the selected port (from 0 till 15)
 *    mode     : selection of mode
 * ----------------------------------------------------------------------------------------------*/

#ifndef BSP_UC_PORT_ENABLEINPUT_UC_SPECIFIC

void bsp_uc_port_EnableInput(uint32_t port, uint32_t pinIndex, UC_PORT_INPUT_MODE_e mode)
{
    uint32_t bitOffset = (8 * (pinIndex % 4));
    __builtin_tricore_ldmst((volatile int *)UC_PORT_IOCR_REG_BASE(port, pinIndex), ((mode << 3) << bitOffset), (0xff << bitOffset));
}

#endif


/* ------------------------------------------------------------------------------------------------
 * FUNCTION: bsp_uc_port_EnableOutput()
 *    Configures requested pin to output mode with desired characteristics.
 * Input
 *    port     : port number
 *    pinIndex : pin index in the selected port
 *    source   : Port Control for Port's Pin (Register Pn_IOCRy.PCx).
 *               Selected Output Function
 *               (see Target user manual - PCx Coding table)
 *    pl       : Pad Level Selection, Pad Driver Mode (Register Pn_PDRy.PLx),
 *               (see Target user manual -  Pad Level Selection table)
 *    pd       : Pad Driver Mode Selection, Pad Driver Mode (Register Pn_PDRy.PDx),
 *               (see Target user manual - Pad Driver Mode and Hysteresis
 *               Selection for Pads table)
 * ----------------------------------------------------------------------------------------------*/

#ifndef BSP_UC_PORT_ENABLEOUTPUT_UC_SPECIFIC

void bsp_uc_port_EnableOutput(uint32_t port, uint32_t pinIndex, uint32_t source, UC_PORT_PL_LEVEL_e pl, UC_PORT_PD_SPEED_e pd)
{
    /* Set Pin control (IOCR register) to output using required signal source */
    uint32_t bitOffset = (8 * (pinIndex % 4));
    __builtin_tricore_ldmst((volatile int *)UC_PORT_IOCR_REG_BASE(port, pinIndex), (((0x10 | source) << 3) << bitOffset), (0xff << bitOffset));

    /* Set Pin output strength and levels in PDR register */
    unlock_wdtcon();
    bitOffset = (4 * (pinIndex % 8));
    __builtin_tricore_ldmst((volatile int *)UC_PORT_PDR_REG_BASE(port, pinIndex), (((pl << 3) | pd) << bitOffset), (0xf << bitOffset));
    lock_wdtcon();
}

#endif


/* ------------------------------------------------------------------------------------------------
 * FUNCTION: bsp_uc_port_SetGPIO()
 *    Sets output value to the GPIO pin
 * Input
 *    port     : port number
 *    pinIndex : pin index in the selected port
 *    value    : one bit value clear-2, set-1, toggle-3
 * ----------------------------------------------------------------------------------------------*/

#ifndef BSP_UC_PORT_SETGPIO_UC_SPECIFIC

void bsp_uc_port_SetGPIO(uint32_t port, uint32_t pinIndex, UC_PORT_OUTPUT_e value)
{
    if (!run_on_tsim()) {
        uint32_t pcl = (((uint32_t)value) & 0x2) << (pinIndex + 15);
        uint32_t ps = ((uint32_t)value & 0x1) << pinIndex;
        UC_PORT_OMR_REG(port, pinIndex) = (pcl | ps);
    }
}

#endif


/* ------------------------------------------------------------------------------------------------
 * FUNCTION: bsp_uc_port_GetGPIO()
 *    Reads value from the GPIO pin
 * Input
 *    port     : port number
 *    pinIndex : pin index in the selected port
 * Output
 *    return   : bit value (0/1)
 * ----------------------------------------------------------------------------------------------*/

#ifndef BSP_UC_PORT_GETGPIO_UC_SPECIFIC

uint8_t bsp_uc_port_GetGPIO(uint32_t port, uint32_t pinIndex)
{
    uint32_t input = ((UC_PORT_IN_REG(port, pinIndex) | (1 << pinIndex))) >> pinIndex;
    return (uint8_t)input;
}

#endif


/* ------------------------------------------------------------------------------------------------
 * FUNCTION: bsp_uc_pflash_SetWs()
 *    Wait States Configuration of PLASH controller Port0.
 * Input
 *    wspflash    : Wait States for read access to PFlash
 *    wsecpf      : Wait States for Error Correction of PFlash
 * ----------------------------------------------------------------------------------------------*/

#ifndef BSP_UC_PFLASH_SETWS_UC_SPECIFIC

void bsp_uc_pflash_SetWs(uint32_t wspflash, uint32_t wsecpf)
{
    unlock_wdtcon();
    DMU_HF_PWAIT.B.RFLASH = (uint32_t)wspflash;
    DMU_HF_PWAIT.B.RECC = (uint32_t)wsecpf;
    lock_wdtcon();
}

#endif


/* ------------------------------------------------------------------------------------------------
 * FUNCTION: bsp_uc_dflash_SetWs()
 *    Wait States Configuration of PLASH controller Port0.
 * Input
 *    wsdflash    : Wait States for read access to DFlash
 *    wsecdf      : Wait States for Error Correction of DFlash
 * ----------------------------------------------------------------------------------------------*/

#ifndef BSP_UC_DFLASH_SETWS_UC_SPECIFIC

void bsp_uc_dflash_SetWs(uint32_t wsdflash, uint32_t wsecdf)
{
    unlock_wdtcon();
    DMU_HF_DWAIT.B.RFLASH = (uint32_t)wsdflash;
    DMU_HF_DWAIT.B.RECC = (uint32_t)wsecdf;
    lock_wdtcon();
}

#endif


/* ------------------------------------------------------------------------------------------------
 * FUNCTION: bsp_uc_core_EnableICache()
 *    Enables Instruction cache on the current core
 * ----------------------------------------------------------------------------------------------*/

#ifndef BSP_UC_CORE_ENABLEICACHE_UC_SPECIFIC

void bsp_uc_core_EnableICache(void)
{
    /* Initiate invalidation of current cache contents if any */
    Ifx_CPU_PCON1 pcon1;
    pcon1.U = 0;
    pcon1.B.PCINV = 1;
    __builtin_tricore_mtcr(CPU_PCON1, pcon1.U);

    /* PCACHE enable */
    unlock_wdtcon();
    Ifx_CPU_PCON0 pcon0;
    pcon0.U = 0;
    pcon0.B.PCBYP = 0;
    __builtin_tricore_mtcr(CPU_PCON0, pcon0.U);
    __builtin_tricore_isync();
    lock_wdtcon();
}

#endif


/* ------------------------------------------------------------------------------------------------
 * FUNCTION: bsp_uc_core_EnableDCache()
 *    Enables Data cache on the current core
 * ----------------------------------------------------------------------------------------------*/

#ifndef BSP_UC_CORE_ENABLEDCACHE_UC_SPECIFIC

void bsp_uc_core_EnableDCache(void)
{
    Ifx_CPU_DCON0 dcon0;
    unlock_wdtcon();
    dcon0.U = 0;
    dcon0.B.DCBYP = 0;
    __builtin_tricore_mtcr(CPU_DCON0, dcon0.U);
    __builtin_tricore_isync();
    lock_wdtcon();
}

#endif


/* ------------------------------------------------------------------------------------------------
 * FUNCTION: bsp_uc_core_GetCurrentCore()
 *    Reads value of the current execution Core calling this function.
 * Output
 *    return : logical CoreId (linear sequence number)
 * ----------------------------------------------------------------------------------------------*/

#ifndef BSP_UC_CORE_GETCURRENTCORE_UC_SPECIFIC

uint32_t bsp_uc_core_GetCurrentCore(void)
{
	/* retrieve physical Core ID */
	uint32_t coreId = __builtin_tricore_mfcr(CPU_CORE_ID) & (uint32_t) 0x7;

    /* Adjust physical index to logical index */
    if (coreId == 6)
    	coreId--;

    return coreId;
}

#endif


/* ------------------------------------------------------------------------------------------------
 * FUNCTION: GetCurrentInterruptPriority()
 *    Reads value of the current core interrupt priority
 * Output
 *    return : Current Interrupt priority
 * ----------------------------------------------------------------------------------------------*/

#ifndef BSP_UC_CORE_GETCURRENTINTERRUPTPRIORITY_UC_SPECIFIC

uint32_t bsp_uc_core_GetCurrentInterruptPriority(void)
{
    Ifx_CPU_ICR reg;
    reg.U = __builtin_tricore_mfcr(CPU_ICR);
    return (uint32_t)reg.B.CCPN;
}

#endif


/* ------------------------------------------------------------------------------------------------
 * FUNCTION: bsp_uc_stm_ReloadChannel()
 *    Initializes requested channel with reload value.
 * Input
 *    channel      : STM channel to initialize
 *    reload_value : period in timer ticks (PER_CLOCK)
 * ----------------------------------------------------------------------------------------------*/

#ifndef BSP_UC_STM_RELOADCHANNEL_UC_SPECIFIC

void bsp_uc_stm_ReloadChannel(uint32_t channel, uint32_t reload_value)
{
    Ifx_STM *const module_stm = ((Ifx_STM *)((uint32_t)&MODULE_STM0 + (0x100 * channel)));
    module_stm->CMP[0].U += reload_value;
}

#endif


/* ------------------------------------------------------------------------------------------------
 * FUNCTION: bsp_uc_stm_InitChannelIsr()
 *    Enable STM Compare interrupt
 * Input
 *    channel : STM channel to initialize
 * ----------------------------------------------------------------------------------------------*/

#ifndef BSP_UC_STM_ENABLECHANNELISR_UC_SPECIFIC

void bsp_uc_stm_EnableChannelIsr(uint32_t channel)
{
    Ifx_STM *const module_stm = ((Ifx_STM *)((uint32_t)&MODULE_STM0 + (0x100 * channel)));

    module_stm->CMCON.B.MSIZE0 = 31;
    module_stm->ISCR.U = (IFX_STM_ISCR_CMP0IRR_MSK << IFX_STM_ISCR_CMP0IRR_OFF);
    module_stm->ISCR.B.CMP0IRR = 1;
    module_stm->ICR.B.CMP0EN = 0x1;
}

#endif


/* ------------------------------------------------------------------------------------------------
 * FUNCTION: bsp_uc_stm_ClearChannelIsrFlag()
 *    Clear active interrupt flag on requested channel
 * Input
 *    channel : STM channel to initialize
 * ----------------------------------------------------------------------------------------------*/

#ifndef BSP_UC_STM_CLEARCHANNELISRFLAG_UC_SPECIFIC

void bsp_uc_stm_ClearChannelIsrFlag(uint32_t channel)
{
    Ifx_STM *const module_stm = ((Ifx_STM *)((uint32_t)&MODULE_STM0 + (0x100 * channel)));
    module_stm->ISCR.B.CMP0IRR = 1;
}

#endif


/* ------------------------------------------------------------------------------------------------
 * FUNCTION: bsp_uc_stm_IsChannelIsrFlag()
 *    Reads the interrupt flag status
 * Input
 *    channel : STM channel
 * Output
 *    return : 1 = timeout occurred
 *             0 = not yet from last clear
 * ----------------------------------------------------------------------------------------------*/

#ifndef BSP_UC_STM_ISCHANNELISRFLAG_UC_SPECIFIC

uint32_t bsp_uc_stm_IsChannelIsrFlag(uint32_t channel)
{
    Ifx_STM *const module_stm = ((Ifx_STM *)((uint32_t)&MODULE_STM0 + (0x100 * channel)));
    return (module_stm->ICR.B.CMP0IR);
}

#endif


/* ------------------------------------------------------------------------------------------------
 * FUNCTION: bsp_uc_stm_GetChannelCurrentValue()
 *    Reads current value of the timer from requested channel
 * Input
 *    channel : STM channel
 * Output
 *    return : STM counter value
 * ----------------------------------------------------------------------------------------------*/

#ifndef BSP_UC_STM_GETCHANNELCURRENTVALUE_UC_SPECIFIC

uint32_t bsp_uc_stm_GetChannelCurrentValue(uint32_t channel)
{
    Ifx_STM *const module_stm = ((Ifx_STM *)((uint32_t)&MODULE_STM0 + (0x100 * channel)));
    return (module_stm->TIM0.U);
}

#endif


/* ------------------------------------------------------------------------------------------------
 * FUNCTION: bsp_uc_Wait(uint32_t time)
 *    Wait for <time> in micro seconds using core local STM channel.
 * Input
 *    time           : time in microseconds
 *    stm_src_clk    : clock source frequency [MHz]
 * ----------------------------------------------------------------------------------------------*/

#ifndef BSP_UC_STM_WAIT_UC_SPECIFIC

void bsp_uc_stm_Wait(uint32_t stm_src_clk, uint32_t time)
{
    int coreId = bsp_uc_core_GetCurrentCore();
    uint32_t stm_val = bsp_uc_stm_GetChannelCurrentValue(coreId);
    uint32_t stm_div = SCU_CCUCON0.B.STMDIV;
    while ((uint32_t)(bsp_uc_stm_GetChannelCurrentValue(coreId) - stm_val) < (time * stm_src_clk / stm_div));
}

#endif


/* ------------------------------------------------------------------------------------------------
 * FUNCTION: bsp_uc_intc_SetBIV()
 *    Register Interrupt vector table in the current Core.
 * Input
 *    table  : vector table base address
 *    vss    : 32B/8B offset between interrupt entries (1/0)
 * ----------------------------------------------------------------------------------------------*/

#ifndef BSP_UC_INTC_SETBIV_UC_SPECIFIC

void bsp_uc_intc_SetBIV(uint32_t table, uint32_t vss)
{
    uint32_t biv = ((uint32_t)0xFFFFFFFE & table) | ((uint32_t)0x1 & vss);
    unlock_wdtcon();
    __builtin_tricore_mtcr(CPU_BIV, biv);
    lock_wdtcon();
    __builtin_tricore_isync();
}

#endif


/* ------------------------------------------------------------------------------------------------
 * FUNCTION: bsp_uc_intc_SetSRC()
 *    Configure STM Interrupt in SRC Interrupt module
 * Input
 *    channel  : stm channel
 *    coreId   : which core will handle the interrupt
 *    prio     : priority level
 * ----------------------------------------------------------------------------------------------*/

#ifndef BSP_UC_INTC_STM_SETSRC_UC_SPECIFIC

void bsp_uc_intc_stm_SetSRC(uint32_t channel, uint32_t coreId, uint32_t prio)
{
    Ifx_SRC *const module_src = (Ifx_SRC *) &MODULE_SRC;

    /* Convert CoreId to correct TOS ID - see TC3x architecture */
    if (coreId != 0)
        coreId = coreId + 1;

    /* set processor and priority values */
    module_src->STM.STM[channel].SR[0].B.TOS = coreId;
    module_src->STM.STM[channel].SR[0].B.SRPN = prio;

    /* ... and enable it */
    module_src->STM.STM[channel].SR[0].B.SRE = 1;
}

#endif


/* ------------------------------------------------------------------------------------------------
 * FUNCTION: bsp_uc_intc_EnableExternalInterrupts()
 *    Enables external interrupt source in the current Core's ICR register
 * ----------------------------------------------------------------------------------------------*/

#ifndef BSP_UC_INTC_ENABLEEXTERNALINTERRUPTS_UC_SPECIFIC

void bsp_uc_intc_EnableExternalInterrupts(void)
{
    __builtin_tricore_enable();
}

#endif


/* ------------------------------------------------------------------------------------------------
 * FUNCTION: bsp_uc_InitClock()
 *    It configures the uC clock system to the optimum setting for the max clock
 *    and peripherals needed by BSP example.
 *    The other peripheral clock are left in their default state to be modified
 *    by the user in the time of its need.
 *    CPUx clocks are set to fSRI by default in CCUCON registers, no need to set them.
 * ----------------------------------------------------------------------------------------------*/

#ifndef BSP_UC_INITCLOCK_UC_SPECIFIC

void bsp_uc_InitClock(void)
{
    /* SCU CCUCON0 Clock distribution (dividers) */
    Ifx_SCU_CCUCON0 con0;
    con0.U = SCU_CCUCON0.U;
    con0.B.STMDIV = UC_CCU_STM_DIV;
    con0.B.SRIDIV = UC_CCU_SRI_DIV;
    con0.B.SPBDIV = UC_CCU_SPB_DIV;
    con0.B.FSI2DIV = UC_CCU_FSI2_DIV;
    con0.B.FSIDIV = UC_CCU_FSI_DIV;
    bsp_uc_scu_SetCcuCon(&SCU_CCUCON0.U, con0.U, 1);

    /* SCU CCUCON1 Clock distribution (dividers) */
    Ifx_SCU_CCUCON1 con1;
    con1.U = SCU_CCUCON1.U;
    con1.B.QSPIDIV = UC_CCU_QSPI_DIV;
    con1.B.CLKSELQSPI = UC_CCU_QSPI_CLKSEL;
    bsp_uc_scu_SetCcuCon(&SCU_CCUCON1.U, con1.U, 1);

    /* Flash modules - timing */
    bsp_uc_pflash_SetWs(UC_FCON_WSPFLASH, UC_FCON_WSECPF);
    bsp_uc_dflash_SetWs(UC_FCON_WSDFLASH, UC_FCON_WSECDF);

    /* Clock Init */
    bsp_uc_scu_EnableXOSC();
    bsp_uc_scu_EnablePLL0(UC_PLL0_PDIV, UC_PLL0_NDIV, UC_PLL0_K2);
    bsp_uc_scu_EnablePLL1(UC_PLL1_PDIV, UC_PLL1_NDIV, UC_PLL1_K2, UC_PLL1_K3);

    /* Switch System Clock to PLL */
    bsp_uc_scu_SwitchSystemClock(UC_CCU_SYSTEM_CLOCK_PLL0);
}

#endif


/* ------------------------------------------------------------------------------------------------
 * FUNCTION: bsp_uc_intc_InitBIV
 *    It initializes BSP provided ISR Vector Table to the current Core's BIV register.
 *    The function can be removed from build be definition of the macro either in the project
 *    setting or on the command line.
 *       - BSP_ISR_SUPPORT_DISABLED
 *    This is the case when user has its own ISR vector implementation (like RTOS)
 * ----------------------------------------------------------------------------------------------*/

#ifndef BSP_ISR_SUPPORT_DISABLED

/* BSP Interrupt vector tables are defined in 'bsp_isr_vector_table.c' */
extern uint32_t* BSP_ISR_VECTOR_TABLE_CPU0;
#if (UC_NB_CORES > 1)
extern uint32_t* BSP_ISR_VECTOR_TABLE_CPU1;
#if (UC_NB_CORES > 2)
extern uint32_t* BSP_ISR_VECTOR_TABLE_CPU2;
#endif
#endif

#ifndef BSP_UC_INITCLOCK_UC_SPECIFIC

void bsp_uc_intc_InitBIV(void)
{
	uint32_t isr_vector_table;
	int coreId = bsp_uc_core_GetCurrentCore();

	/* Select right vector table */
	switch (coreId)
	{
		case UC_CORE_MAIN_0:
			isr_vector_table = (uint32_t) &BSP_ISR_VECTOR_TABLE_CPU0;
			break;
#if (UC_NB_CORES > 1)
		case UC_CORE_MAIN_1:
			isr_vector_table = (uint32_t) &BSP_ISR_VECTOR_TABLE_CPU1;
			break;
#if (UC_NB_CORES > 2)
		case UC_CORE_MAIN_2:
			isr_vector_table = (uint32_t) &BSP_ISR_VECTOR_TABLE_CPU2;
			break;
#endif
#endif

		default:
			return; // nothing to do in case of wrong coreId parameter
	}

	/* register ISR Vector Table (dispatch table) to Core BIV register
	 * VSS = 0 to get a BSP expected 32B spacing in between vectors
	 */
	bsp_uc_intc_SetBIV(isr_vector_table, 0);
}

#endif  /* #ifndef BSP_UC_INITCLOCK_UC_SPECIFIC */
#endif  /* #ifndef BSP_ISR_SUPPORT_DISABLED */

/* ------------------------------------------------------------------------------------------------
 * FUNCTION: run_on_tsim
 *    if SCU_CHIP_ID is 0 assume to run on a simulator
 * -----------------------------------------------------------------------------------------------*/

unsigned int RunOnTSIM = 0;

unsigned int run_on_tsim(void)
{
    return RunOnTSIM;
}

void test_run_on_tsim(void) 
{
	unsigned int volatile chip_id;
	chip_id = SCU_CHIPID.U;
	RunOnTSIM = (chip_id == 0);
}