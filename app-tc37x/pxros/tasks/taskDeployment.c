/**************************************************************************************************
 * FILE: taskDeployment.c
 *
 * DESCRIPTION:
 *     Multicore version supporting variable number of cores for User Tasks deployment.
 *     - in case the default core is not active, PXCORE_0 (Core 0) is used instead
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

#include "pxdef.h"
#include "pxros/config/system_cfg.h"
#include "pxros/tasks/taskPrios.h"
#include "pxros/tasks/taskDeployment.h"
#include "pxros/tasks/GETHDriver/pxnet/DriverTask/include/PxEthDriverCreate.h"

/* Task Deployment Table
 * Task configuration relying on Task default Memory Class and Object pool
 */
const task_deployment_t taskTable[] = {
    { PxEthTaskCreate, GETH_DRIVER_PRIO, PXCORE_0},
};

/* FUNTION: TaskDeploy
 *     Function calls Task Create function according their core assignment in the taskTable
 *     structure.
 *     The Task is created and activated immediately, without an activation event.
 *     The Task will use Task default object pool and memory class. In case, the user needs other
 *     pools or memory classes, this routine needs to be replaced by user-specific
 *     deployment solution.
 *
 * INPUT:
 *     coreId : current execution Core
 *PxTickGetTimeInMilliSeconds
 * OUTPUT:
 *     none
 */
void TaskDeploy(PxUInt_t coreId)
{
    PxUInt_t nbTaskTableEntries = sizeof(taskTable) / sizeof(task_deployment_t);
    PxUInt_t i;

    // Deploy any auto-spawned rust-tasks
    RustDeploy(coreId);

    for (i = 0; i < nbTaskTableEntries; i++)
    {
        if (taskTable[i].core == coreId)
        {
            PxTask_t taskId = (taskTable[i].fnc)(taskTable[i].prio, 0, PXMcTaskdefault, PXOpoolTaskdefault);
            if (PxTaskIdError(taskId) != PXERR_NOERROR)
                PxPanic();
        }
    }
}