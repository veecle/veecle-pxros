/**************************************************************************************************
 * FILE: taskPrios.h
 *
 * DESCRIPTION:
 *     Tasks, Services, and Hardware Interrupt priorities
 *
 * HISTORY:
 *     v1   06.2020   RO
 *          - PXROS-HR BSP Base reference example
 *
 *     v2   07.2020   JS
 *          - Removed priority definition for ReleaseServer
 *          - Removed priority for LedClientC
 *            (both not used in PXROS 8.x.x example)
 *
 **************************************************************************************************
 * SPDX-FileCopyrightText: HighTec EDV-Systeme GmbH
 *
 * SPDX-License-Identifier: Boost Software License - Version 1.0
 *************************************************************************************************/

#ifndef TASK_PRIOS_H_
#define TASK_PRIOS_H_


/* ================================================================================================
 * HARDWARE INTERRUPT PRIORITIES
 * Higher number = higher priority
 * ==============================================================================================*/

#define SYSTIME_ISR_PRIO          2


/* ================================================================================================
 * SERVICES PRIORITIES
 * Lower number = higher priority
 * ==============================================================================================*/

#define INITTASK_POSTINIT_PRIO    31
#define NAMESRV_PRIO              5


/* ================================================================================================
 * USER TASK PRIORITIES
 * One needs to pay attention to the core assignment (where task executes) the same priority
 * might mean
 *     - concurrent execution for tasks being on different cores
 *     - sequential for tasks executing on the same core
 * Lower number = higher priority (on the contrary to HW interrupt)
 * ==============================================================================================*/

#endif
