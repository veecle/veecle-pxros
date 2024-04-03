/**************************************************************************************************
 * FILE: px_utils.c
 *
 * DESCRIPTION:
 *    Utilities to simplify common application patterns used across tasks
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
#include "px_utils.h"


/* ================================================================================================
 * API INTERFACE
 * ==============================================================================================*/

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * FUNCTION: HtcPxNameQuery
 *     Query the Name Server with wait/retry option
 * IN:
 *     Id           : Id of the entry we look for
 *     infosize     : size of the stored parameter
 *     *info        : pointer to field where to store read value
 *     retryCount   : number of retries
 *                    0 = use a default value NAMEQUERY_RETRY_DEFAULT
 *     retryTimeout : wait time in PX Ticks
 *                    0 = use a default value NAMEQUERY_RETRY_TIMEOUT
 *     retryEvent   : Caller event to use for retry timeout events
 *                    calling task shall provide the event
 * OUT:
 *     PxError      : PXROS type of error code
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

PxError_t HtcPxNameQuery (PxNameId_t Id, PxSize_t infosize, void *info, int retryCount,
        int retryTimeout, PxEvents_t retryEvent )
{
    PxError_t err = PXERR_NOERROR;

    /* Check the Default values requests by passing value '0' */
    if (retryTimeout == 0)
        retryTimeout = NAMEQUERY_RETRY_TIMEOUT;

    if (retryCount == 0)
        retryCount = NAMEQUERY_RETRY_DEFAULT;

    /* ask for a timeout object from the task default object pool to generate wake-up event */
    PxTo_t to = PxToRequest(PXOpoolTaskdefault, retryTimeout, retryEvent);

    if (PxToIdError(to) != PXERR_NOERROR)
    {
        err = PxToIdError(to);
        return err;
    }

    /* Mark object as invalid before its use */
    *((PxObj_t *)info) = PxObjIdInvalidate();

    /* Retry loop with waiting sleep in between query retries.
     * The error response from the PxNameQuery can mean two things:
     *   1. entry does not exists yet
     *   2. real error
     */
    do
    {
        err = PxNameQuery (Id, infosize, info, 0);
        if (err != PXERR_NOERROR)
        {
            if (err == PXERR_NAME_UNDEFINED)
            {
                PxToStart(to);
                PxAwaitEvents(retryEvent);
            }
            else
                break;
        }
    } while (!PxObjIdIsValid(*((PxObj_t *)info)) && (--retryCount > 0));

    /* entry not found after number of trials
     * Something must be wrong in the complete application
     */
    if (retryCount == 0)
        err =  PXERR_NAME_UNDEFINED;

    /* Stop and release the temporary timeout object */
    PxToStop(to);
    PxToRelease(to);

    return err;
}


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * FUNCTION: HtcSleep
 *     Sleep a task for arbitrary number of px ticks
 *     Function utilizes a temporary Timeout object taken from a Caller Task default object pool.
 * IN:
 *     timeout        : timeout defined in PX Ticks
 *     taskSleepEvent : Caller task event to use to signal end
 *                      of timeout object expiration
 * OUT:
 *     PxError_t : PXERR_NOERROR - sleep is over
 *                 other code    - something went wrong in object allocation
 *                                 or use
 * NOTES:
 *     Caller task must allocate a dedicated Sleep Timeout Event among its event bits to avoid
 *     unexpected wake-up by another event signaled to the task!
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

PxError_t HtcSleep (PxTicks_t timeout, PxEvents_t taskSleepEvent)
{
    if (timeout)
    {
        PxTo_t to = PxToRequest(PXOpoolTaskdefault, timeout, taskSleepEvent);
        if (PxToIdError(to) != PXERR_NOERROR)
            return PxToIdError(to);

        PxToStart(to);
        PxEvents_t ev = PxAwaitEvents(taskSleepEvent);

        to = PxToRelease(to);
        if (PxToIdError(to) != PXERR_NOERROR)
            return PxToIdError(to);

        if (ev != taskSleepEvent)
            return PXERR_EVENT_ZERO;
    }

    return PXERR_NOERROR;
}


