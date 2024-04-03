/**************************************************************************************************
 * FILE: taskDeployment.h
 *
 * DESCRIPTION:
 *     Task Deployment Table Interface.
 *
 * HISTORY:
 *     v1   06.2020   RO
 *          - PXROS-HR BSP Base reference example
 *
 **************************************************************************************************
 * SPDX-FileCopyrightText: HighTec EDV-Systeme GmbH
 *
 * SPDX-License-Identifier: Boost Software License - Version 1.0
 *************************************************************************************************/

#ifndef TASK_DEPLOYMENT_H_
#define TASK_DEPLOYMENT_H_

#include "pxdef.h"

/* prototype of the Task Create Function */
typedef PxTask_t (*TaskCreateFnc)(PxPrio_t prio, PxEvents_t events, PxMc_t memClass, PxOpool_t objPool);

/* Task entries to deploy in the InitTask
 *   fnc  : address of the Task create function
 *   prio : task priority on assigned core
 *   core : assigned core to run on
 */
typedef struct {
    TaskCreateFnc fnc;
    PxPrio_t prio;
    PxUInt_t core;
} task_deployment_t;

/* Task Deployment Table interface */
//extern const task_deployment_t taskTable[];
//extern const PxUInt_t nbTaskTableEntries;

/* Task Deployment API */
extern void TaskDeploy(PxUInt_t coreId);
extern void RustDeploy(PxUInt_t coreId);

#endif
