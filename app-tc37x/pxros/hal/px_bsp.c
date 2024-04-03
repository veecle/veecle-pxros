/**************************************************************************************************
 * FILE: px_bsp.c
 *
 * DESCRIPTION:
 *     Hardware abstraction module to provide PXROS-HR HW dependent API
 *     low level implementation based on present LLD solution
 *     (either it is BSP, MCAL, iLLD)
 *     This file uses Board Support Package provided by HighTec EDV.
 *
 * HISTORY:
 *    v1   06.2020   RO
 *         - PXROS-HR BSP Base reference example
 *
 **************************************************************************************************
 * SPDX-FileCopyrightText: HighTec EDV-Systeme GmbH
 *
 * SPDX-License-Identifier: Boost Software License - Version 1.0
 *************************************************************************************************/


#include "pxdef.h"
#include "pxhndcall.h"
#include "pxros/tasks/taskPrios.h"
#include "px_bsp.h"

#include <stdio.h>
#include <stdlib.h>


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * FUNCTION: systime_Isr
 *     PXROS-HR Fast Handler Service routine for STM Timer interrupt.
 *     Each kernel serves its STM timer module.
 *     It reloads the STM timer and initiates PXROS-HR tick processing.
 * IN:
 *     systime_Reload : STM ticks for frequency in [Hz] units provided by
 *                      PXROS-HR Interrupt Prolog.
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

static void systime_Isr(PxArg_t systime_Reload)
{
    unsigned int coreId = bsp_uc_core_GetCurrentCore();
    bsp_uc_stm_ReloadChannel(coreId, (unsigned int) systime_Reload);
    PxTickDefine_Hnd();

}


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * FUNCTION: set_STM_SRC
 *     Setup of TCx SRC module for STM source to generate an interrupt to the right core
 *     on a requested priority using the BSP LLD routine.
 *     !! Function must execute in Supervisor privileged level.
 * IN:
 *     coreId : core servicing the interrupt
 *     prio   : priority to register
 * OUT:
 *     int    : not needed, but required by syntax of _PxHndcall API
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

static int set_STM_SRC(va_list ap)
{
    unsigned int coreId = va_arg(ap, unsigned int);
    unsigned int prio = va_arg(ap, unsigned int);

    bsp_uc_intc_stm_SetSRC(coreId, coreId, prio);
    return 0;
}


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * FUNCTION: TicksInit
 *     Initialization of STM instances, where a dedicated STM for each core generates
 *     time intervals.
 *     Routine registers STM interrupt handler as Fast interrupt in PXROS where the call to
 *     PxTickDefine_Hnd() is done => TICK increment.
 * IN:
 *     hz : time interval between two events in [Hz] units
 * NOTE:
 *     Routine relies on BSP clock system initialization! in case of a different
 *     clock setup from the BSP one, the routine must be modified accordingly !!
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void TicksInit(unsigned int hz)
{
    unsigned int systime_Reload;
    unsigned int coreId = bsp_uc_core_GetCurrentCore();

    /* corresponding ticks for frequency given in [Hz] units */
    systime_Reload = 1e6 / hz * UC_NB_TICKS_1US;
    if (run_on_tsim())
        systime_Reload /= 2;

    /* Register Time service routine in PXROS-HR as FastHandler
     * Set corresponding TicksPerSecond parameter in PXROS-HR
     */
    PxIntInstallFastContextHandler(SYSTIME_ISR_PRIO, systime_Isr, (PxArg_t)systime_Reload);
    if (run_on_tsim())
        PxTickSetTicksPerSecond(40);
    else
        PxTickSetTicksPerSecond(hz);

    /* Activate Service Request Control in TCx SRC module.
     * An access to SRC module requires Supervisor privilege => use of _PxHndcall
     */
    _PxHndcall(set_STM_SRC, PxGetId(), 2 * sizeof(unsigned int), coreId, SYSTIME_ISR_PRIO);

    /* Setup STM time instance to generate interrupts for the given period (ticks)
     * and enable its interrupts
     */
    bsp_uc_stm_ReloadChannel(coreId, bsp_uc_stm_GetChannelCurrentValue(coreId) + systime_Reload);
    bsp_uc_stm_EnableChannelIsr(coreId);
}

void PxPanic(void)
{
    extern void exit(int);
    extern int write(int,void *,int);
    if (run_on_tsim()) {
        write(1, "Panic called\n", 14);
        exit(1);
    }
    PxAbort(PXERR_ABORT_ILLEGAL_CALL);
}

void PxAbort(PxError_t err)
{
    if (run_on_tsim()) {
        printf("PxAbort = %d\n", err);
        exit(1);
    }
    while(1) __asm__("debug");
}