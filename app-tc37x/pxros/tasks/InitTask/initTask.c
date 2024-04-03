/**************************************************************************************************
 * FILE: initTask.c
 *
 * DESCRIPTION:
 *     Implementation of the InitTask, the first running task in the system.
 *     InitTask deploys other tasks according to system needs.
 *
 * HISTORY:
 *    v1   06.2020   RO
 *         - PXROS-HR BSP Base reference example
 *
 *    v2   07.2020   JS
 *         - Update to PXROS-HR v8.x.x (ReleaseServer removed)
 *
 **************************************************************************************************
 * SPDX-FileCopyrightText: HighTec EDV-Systeme GmbH
 *
 * SPDX-License-Identifier: Boost Software License - Version 1.0
 *************************************************************************************************/


#include "pxdef.h"

/* ReleaseServer is not necessary - releasing of messages is
 * handled internally by Kernel in the PXROS-HR version 8.x.x.
 *
 * If task termination is going to be used, "task-release-service"
 * must be used. This service is implemented as a separate module
 * inside utility folder (not demonstrated in this example).
 */

#include "pxros/hal/px_bsp.h"
#include "pxros/config/system_cfg.h"
#include "pxros/tasks/taskNameIds.h"
#include "pxros/tasks/taskPrios.h"
#include "pxros/tasks/taskDeployment.h"

/* ================================================================================================
 * EXTERN SYMBOLS
 * ==============================================================================================*/

/* NameServer memory coordinates needed during NameServer Task create.
 * Coordinates comes from Linker files.
 */
extern PxMemAligned_t PxNameServerSpaceBase[];
extern PxMemAligned_t PxNameServerSpaceEnd[];

/* ================================================================================================
 * TASK RUNNABLE
 * ==============================================================================================*/

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * FUNCTION: InitTask_Func
 *     Here all user tasks and services are deployed according to system needs.
 *     Created tasks execution starts in the end of InitTask according to their system priority
 *     configured in taskPrios.h
 *     The InitTask code is implemented as shared code.
 * IN:
 *     myID               : this task ID assigned by the kernel
 *     myMailbox          : this task private Mailbox ID
 *     myActivationEvents : this task activation events
 * OUT:
 *     none
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

//extern int increment(int);

void InitTask_Func (PxTask_t myID, PxMbx_t myMailbox, PxEvents_t myActivationEvents)
{
    /* Prevent possible warnings about unused function arguments */
    (void)(myMailbox); (void)(myActivationEvents);

    /* Read execution core ID to decide right execution branch */
    PxUInt_t coreId = PxGetCoreId();

    /* Start PXROS time base ticks in each core instance
     * The time bases are not synchronized across cores
     */
    TicksInit(1000);

    /* Create Name Server service task on MASTER_CORE
     * ---------------------------------------------
     * Name Server allows the tasks to exchange data that are not known before
     * the tasks are created such as the IDs of the tasks, mailboxes and other
     * objects. The Release server holds the "database" of TAG-VALUE pairs. The
     * task needs firstly register ("announce") the VALUE for a given TAG
     * and later the other tasks can read the tagged VALUE. The task that is
     * reading the tagged VALUE should set the timeout in case the requested
     * VALUE is not yet available.
     */
    if (coreId == PXROS_MASTER_CORE)
    {
        PxUInt_t nameserverSize = (PxUInt_t) PxNameServerSpaceEnd - (PxUInt_t) PxNameServerSpaceBase;
        PxTask_t NameSrv = PxNamesrvInit (NAMESRV_PRIO, PxNameServerSpaceBase, nameserverSize);
        if (PxTaskIdError(NameSrv) != PXERR_NOERROR)
            PxPanic();
    }

    /* Enable the execution of service tasks during the user tasks deployment by setting priority
     * of InitTask lower than the lowest service priority (lower prio = higher number!)
     */
    PxTaskSetPrio( myID, NAMESRV_PRIO + 1 );

    /* Wait for the NameServer service task on MASTER_CORE to get initialized.*/
    PxError_t errRes = PxGetGlobalServerMbx (PXROS_MASTER_CORE, _PxNameSrvReqMbxId);
    if (errRes != PXERR_NOERROR)
        PxPanic();

    /* User Task Deployment
     * here each core creates and activates their own set of user tasks
     * defined in Task Deployment Table
     */
    TaskDeploy(coreId);

    /* Decrease InitTask priority on its minimum in the system.
     * InitTask will transform to Background task.
     * From here all created tasks will start its execution based on their priority.
     */
    PxTaskSetPrio (myID, INITTASK_POSTINIT_PRIO);

    /* Infinitive loop on all cores as background activity */
    while(1) {__asm__ ("wait");}
}


