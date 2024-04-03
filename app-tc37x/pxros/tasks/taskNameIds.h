/**************************************************************************************************
 * FILE:  taskNameIds.h
 *
 * DESCRIPTION:
 *     Names definition in PxNameId_t format
 *     Tasks can use these names to query entry specific content using NameServer PXROS-HR API
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

#ifndef TASK_NAMEIDS_H_
#define TASK_NAMEIDS_H_


#include "pxdef.h"
#include "pxname.h"

#define LedClientA_MID_NAMESERVERID         ((PxNameId_t){ 1 })
#define LedServer_MID_NAMESERVERID          ((PxNameId_t){ 2 })
#define LedClientB_MID_NAMESERVERID         ((PxNameId_t){ 3 })

#endif /* TASK_NAMEIDS_H_ */
