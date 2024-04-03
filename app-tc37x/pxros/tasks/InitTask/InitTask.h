/**************************************************************************************************
 * FILE: InitTask.h
 *
 * DESCRIPTION:
 *     InitTask interface file
 *
 * HISTORY:
 *    v1   06.2020   RO
 *         - PXROS-HR BSP Base reference example
 *
 *    v2   07.2020   JS
 *         - Update to PXROS-HR v8.x.x
 *
 ***************************************************************************************************
 * SPDX-FileCopyrightText: HighTec EDV-Systeme GmbH
 *
 * SPDX-License-Identifier: Boost Software License - Version 1.0
 *************************************************************************************************/

#ifndef __PXROS_INITTASK_H__
#define __PXROS_INITTASK_H__


#include "pxdef.h"
#include "pxros/config/system_cfg.h"


/* ================================================================================================
 * EXTERNAL SYMBOLS
 * ==============================================================================================*/

/* InitTask configuration table for each instantiated core */

#if CORE0_ACTIVE == 1
extern const PxTaskSpec_T InitTaskSpec_Core0;
#endif

#if CORE1_ACTIVE == 1
extern const PxTaskSpec_T InitTaskSpec_Core1;
#endif

#if CORE2_ACTIVE == 1
extern const PxTaskSpec_T InitTaskSpec_Core2;
#endif

/* ================================================================================================
 * EVENTS
 * ==============================================================================================*/

/* event to be used in waiting loops based on temporary Timeout object */
#define INITTASK_EVENT_WAIT            (PxEvents_t) (0x1 << 1)


/* ================================================================================================
 * RUNNABLES
 * ==============================================================================================*/

/* InitTask runnable executed as first task function after the PXROS-HR kernel
 * initialization phase. Here all service and user tasks are deployed.
 */
void InitTask_Func(PxTask_t myID, PxMbx_t myMailbox, PxEvents_t myActivationEvents );


#endif /* __PXROS_INITTASK_H__ */
