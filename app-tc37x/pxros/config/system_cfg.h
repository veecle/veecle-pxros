/**************************************************************************************************
 * FILE: system_cfg.h
 *
 * DESCRIPTION:
 *     Global initialization module for PXROS-HR system.
 *     User shall set the Core's configuration parameters
 *     according to the system needs
 *
 * HISTORY:
 *    v1   06.2020   RO
 *         - PXROS-HR BSP Base reference example
 *
 *    v2, 07.2020    JS
 *         - Update to PXROS-HR v8.x.x
 *
 **************************************************************************************************
 * SPDX-FileCopyrightText: HighTec EDV-Systeme GmbH
 *
 * SPDX-License-Identifier: Boost Software License - Version 1.0
 *************************************************************************************************/

#ifndef _SYSCONFIG_H_
#define _SYSCONFIG_H_

#include "bsp.h"
#include "pxdef.h"


/* ================================================================================================
 * GLOBAL CONFIGURATION
 * =============================================================================================*/

/* Definition which core initializes MPU system on other cores due to
 * Safety EdnInit protection that shall be handled only by ONE core!!
 */
#define PXROS_MASTER_CORE               0

/* Number of cores under the PXROS-HR kernel control
 * from Core[0] to Core[CORE_COUNT-1]
 * NOTE: PXROS-HR kernel 8.1.1 supports maximum of 8 cores (theoretically)
 */
#define MAX_CORES_SUPPORTED_BY_PXROS    8

#if (UC_NB_CORES > MAX_CORES_SUPPORTED_BY_PXROS)
  #define PXROS_CORE_COUNT              MAX_CORES_SUPPORTED_BY_PXROS
#else
  #define PXROS_CORE_COUNT              UC_NB_CORES
#endif


/* ================================================================================================
 * CORE [X]  CONFIGURATION
 *
 * COREx_ACTIVE:
 *     decision if the core is under PXROS-HR kernel control
 *
 * NUM_OF_PXOBJS_COREx:
 *     number of kernel local objects
 *     - accessible by the local core
 *     - memory allocated in the pxros-core.ld
 *
 * NUM_OF_GLOBAL_PXOBJS_COREx:
 *     Number of kernel global objects
 *     - accessible from all other cores
 *     - memory allocated in the pxros-core.ld
 *
 * NUM_OF_TASKS_COREx:
 *     Number of tasks created on a given core
 *     - memory for task's TCB allocated in the system_cfg.c file
 *
 * NUM_OF_CSA_REGION_COREx:
 *     Function call context regions
 *     - each of 64B size defined by TC2x/3x architecture
 *     - max is 256 regions
 *
 * TASKMEMSIZE_COREx:
 *     Default memory class used for purposes
 *     - messages' memories
 *     - task stacks
 *     - others
 *
 * USER_SYSTEM_STACK_SIZE_COREx:
 *     Area on top of System stack for Interrupt Handlers' needs etc.
 * ==============================================================================================*/


/*
 * ---- CORE_0 ----
 */
#define CORE0_ACTIVE                        1
#define NUM_OF_PXOBJS_CORE0                 200
#define NUM_OF_GLOBAL_PXOBJS_CORE0          0
#define NUM_OF_TASKS_CORE0                  20
#define NUM_OF_CSA_REGION_CORE0             128
#define TASKMEMSIZE_CORE0                   (32 * 1024)
#define USER_SYSTEM_STACK_SIZE_CORE0        512

/*
 * ---- CORE_1 ----
 */
#if (PXROS_CORE_COUNT > 1)
#define CORE1_ACTIVE                        1
#define NUM_OF_PXOBJS_CORE1                 200
#define NUM_OF_GLOBAL_PXOBJS_CORE1          0
#define NUM_OF_TASKS_CORE1                  20
#define NUM_OF_CSA_REGION_CORE1             128
#define TASKMEMSIZE_CORE1                   (32 * 1024)
#define USER_SYSTEM_STACK_SIZE_CORE1        512
#else
#define CORE1_ACTIVE                        0
#endif

/*
 * ----  CORE_2 ----
 */
#if (PXROS_CORE_COUNT > 2)
#define CORE2_ACTIVE                        1
#define NUM_OF_PXOBJS_CORE2                 200
#define NUM_OF_GLOBAL_PXOBJS_CORE2          0
#define NUM_OF_TASKS_CORE2                  20
#define NUM_OF_CSA_REGION_CORE2             128
#define TASKMEMSIZE_CORE2                   (32 * 1024)
#define USER_SYSTEM_STACK_SIZE_CORE2        512
#else
#define CORE2_ACTIVE                        0
#endif

/* ================================================================================================
 * EXPORTED SYMBOLS
 * ==============================================================================================*/

/* Declaration of PXROS-HR initialization structure being in system_cfg.c file
 * It is required by PxInit() function
 */
extern const PxInitSpecsArray_t InitSpecsArray;


#endif /* _SYSCONFIG_H_ */
