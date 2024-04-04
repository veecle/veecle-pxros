/***************************************************************
 *   PXROS-HR Network Stack
 *   Copyright HighTec EDV-Systeme GmbH 2020
 ***************************************************************/

#ifndef __PXETHDRIVERCREATE_H__
#define __PXETHDRIVERCREATE_H__

#include "pxdef.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

PxTask_t PxEthTaskCreate(PxPrio_t prio, PxEvents_t events, PxMc_t memClass, PxOpool_t objPool);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __PXETHDRIVERCREATE_H__ */
