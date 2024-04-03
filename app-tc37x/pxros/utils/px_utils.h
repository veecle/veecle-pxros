/**************************************************************************************************
 * FILE: px_utils.h
 *
 * DESCRIPTION:
 *     Utilities to simplify common application patterns used across tasks
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

#ifndef __PXUTILS_H__
#define __PXUTILS_H__


#include "pxdef.h"
#include "pxname.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* ================================================================================================
 * DEFINES
 * ==============================================================================================*/

/* Default values */
#define NAMEQUERY_RETRY_TIMEOUT     10  /* in PXROS tick */
#define NAMEQUERY_RETRY_DEFAULT     10  /* number of trials */


/* ================================================================================================
 * API
 * ==============================================================================================*/

/* Getting NameServer entry using wait & try approach */
PxError_t HtcPxNameQuery (PxNameId_t Id, PxSize_t infosize, void *info, int retryCount,
        int retryTimeout, PxEvents_t retryEvent );

/* Sleep a task for arbitrary number of PXROS ticks */
PxError_t HtcSleep (PxTicks_t timeout, PxEvents_t taskSleepEvent);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __PXUTILS_H__ */
