/**************************************************************************************************
 * FILE: system_cfg.c
 *
 * DESCRIPTION:
 *     Global initialization module for PXROS-HR system controlled by the user
 *     provided values in
 *     -> system_cfg.h file
 *     -> system_mpu_cfg.c file
 *     User SHOULD NOT need to modify this file.
 *
 * HISTORY:
 *    v1   06.2020   RO
 *         - PXROS-HR BSP Base reference example
 *
 *    v2, 07.2020    JS
 *         - Update to PXROS-HR v8.x.x
 *
 *    v3, 01.2022    HL
 *         - Adapt to LLVM
 *
 *    v4, 02.2024    HL
 *         - Restrict to 3 cores 
 *
 **************************************************************************************************
 * SPDX-FileCopyrightText: HighTec EDV-Systeme GmbH
 *
 * SPDX-License-Identifier: Boost Software License - Version 1.0
 *************************************************************************************************/


#include "system_cfg.h"
#include "system_mpu_cfg.h"
#include "pxros/tasks/InitTask/InitTask.h"


/* ================================================================================================
 * SUPPORTIVE MACRO FOR SYMBOL DEFINITION FROM #DEFINE VALUES
 * macro converts #define parameter to object file symbol visible to Linker
 * ==============================================================================================*/

#undef _STRINGIFY
#define _STRINGIFY(x)   #x
#define _DEF_SYM(n,v)                       \
    __asm (".global       " #n );           \
    __asm (".set  " #n "," _STRINGIFY(v));  \
    __asm (".type " #n ",STT_OBJECT")

#define DEF_SYM(name,val)   _DEF_SYM(name,val)


/* ================================================================================================
 * LINKER SYMBOL DEFINITION
 * #defines from system_cfg.h converted to Linker visible symbol the PXROS-HR kernel
 * needs some of the symbols for a correct image build.
 * ==============================================================================================*/

/* __PXROS_NAMESIZE__
 * 0 means we do not allocate any storage for object names
 */
#define PXROS_NAMESIZE    0
DEF_SYM(__PXROS_NAMESIZE__, PXROS_NAMESIZE);

/* __NO_OF_CORES
 * Number of cores under PXROS-HR control from <0-N>, where N <= MAX cores
 */
DEF_SYM(__NO_OF_CORES, PXROS_CORE_COUNT);

/* _MASTER_CORE_ initializes
 * - the MASTER_CORE instance
 * - other core MPU system
 * - starts other cores
 * Default PXROS-HR kernel implementation support only 'MASTER_CORE = 0'
 */
DEF_SYM(_MASTER_CORE_, PXROS_MASTER_CORE);


/* ================================================================================================
 * COMMON EXTERNAL SYMBOLS
 * ==============================================================================================*/

/* Address where slave cores (cores started by MASTER_CORE) starts their execution,
 * used in PxInit initialization structure
 */
extern void _crt0_reset(void);


/* ================================================================================================
 * MODULE DEFINES
 * supportive macros and constants
 * ==============================================================================================*/

/* PXMCVARSIZED_ADDON
 * PXMcVarsized memory type block overhead needed for allocating
 * the right size of the SYSMEM memory for requested number of tasks (TCBs)
 */
#define PXMCVARSIZED_ADDON    24


/* ================================================================================================
 * CORE [0] PXROS-HR KERNEL INSTANCE CONFIGURATION AND SYMBOLS
 * ==============================================================================================*/

 #if CORE0_ACTIVE == 1

/* Number of local objects that will be used by pxros-core.ld to allocate
 * sufficient memory area for storing objects
 */
DEF_SYM(__NUM_OF_PXOBJS__CPU0_, NUM_OF_PXOBJS_CORE0);

/* Number of global objects that will be used by pxros-core.ld to allocate
 * sufficient memory area for storing objects
 */
DEF_SYM(__NUM_OF_GLOBAL_PXOBJS__CPU0_, NUM_OF_GLOBAL_PXOBJS_CORE0);

/* Declaration of user stack size that will be added in pxros-image.ld
 * to pxros_system size
 */
DEF_SYM(USER_SYSTEM_STACK_SIZE_CPU0_, USER_SYSTEM_STACK_SIZE_CORE0);

/* Declaration of core CSA size to allocate based on user number
 * of CSA regions, where the region size is fixed of 64B on TCx arch.
 */
#define CSA_SIZE_CORE0    ((NUM_OF_CSA_REGION_CORE0) * 64)
DEF_SYM(__CSA_SIZE_CPU0_, CSA_SIZE_CORE0);

/* Object memory created in pxros-core.ld (Base and Size coordinates) */
extern PxMemAligned_t  PxObjmem_CPU0_[];
extern PxSize_t PX_OBJMEMSIZE_CPU0_[];

/* System stack coordinates coming from pxros-core.ld */
extern const PxUInt_t PXROS_SYSTEM_STACK_BEGIN_CPU0_[];
extern const PxUInt_t PXROS_SYSTEM_STACK_SIZE_CPU0_[];

/* System memory (PXMcSystemdefault)
 * shall be large enough
 * - to allocate all core's task control block (TCB)
 * - memory taken from PXMcSystemdefault
 */
PRAGMA_SECTION_BSS(".CPU0..systemmemory",8,awB)
#define SYSMEMSIZE_CORE0 (PXMEM_ADJUST((NUM_OF_TASKS_CORE0) * (PXTASK_SIZE + PXMCVARSIZED_ADDON)))
PxMemAligned_t Sysmem_Core0[SYSMEMSIZE_CORE0 / sizeof(PxMemAligned_t)];
PRAGMA_SECTION_BSS_END

/* Task memory class (PXMcTaskdefault)
 * shall be large enough for
 * - all core's message data areas
 * - User tasks stacks if they use PXStackAlloc scheme
 * - other memory taken from PXMcTaskdefault
 */
PRAGMA_SECTION_BSS(".CPU0..taskmemory",8,awB)
PxMemAligned_t Taskmem_Core0[PXMEM_ADJUST(TASKMEMSIZE_CORE0) / sizeof(PxMemAligned_t)];
PRAGMA_SECTION_BSS_END

/* PXROS-HR InitSpec for kernel instance */
static const PxInitSpec_T InitSpec_Core0 =
{

    .is_sysmc_type        = PXMcVarsizedAligned,
    .is_sysmc_size        = 8,
    .is_sysmc_blk         = Sysmem_Core0,
    .is_sysmc_blksize     = SYSMEMSIZE_CORE0,

    .is_obj_number        = NUM_OF_PXOBJS_CORE0,
    .is_global_obj_number = NUM_OF_GLOBAL_PXOBJS_CORE0,
    .is_obj_namelength    = PXROS_NAMESIZE,

    .is_inittask          = &InitTaskSpec_Core0,

    .is_monitoring        = PXMonitorMemory,
    .is_schedext          = 0,

    .is_objmc_blk         = PxObjmem_CPU0_,
    .is_objmc_blksize     = (PxSize_t) PX_OBJMEMSIZE_CPU0_,

    .is_taskmc_type       = PXMcVarsizedAdjusted,
    .is_taskmc_size       = 8,
    .is_taskmc_blk        = Taskmem_Core0,
    .is_taskmc_blksize    = TASKMEMSIZE_CORE0,

#if PXROS_MASTER_CORE == 0
    .is_core_start        = 0,
#else
    .is_core_start        = (unsigned int)_crt0_reset,
#endif

    .is_system_stack      = PXROS_SYSTEM_STACK_BEGIN_CPU0_,
    .is_system_stack_size = (PxUInt_t) PXROS_SYSTEM_STACK_SIZE_CPU0_,

    .is_sys_code          = &_cpu0_sys_code_protection,
    .is_sys_data          = &_cpu0_sys_and_kernel_data_protection,
    .is_task_code         = &_cpu0_task_code_protection,
};

#endif /* #if CORE0_ACTIVE == 1 */


/* ================================================================================================
 * CORE [1] PXROS-HR KERNEL INSTANCE CONFIGURATION AND SYMBOLS
 * ==============================================================================================*/

#if CORE1_ACTIVE

/* Number of local objects that will be used by pxros-core.ld to allocate
 * sufficient memory area for storing objects
 */
DEF_SYM(__NUM_OF_PXOBJS__CPU1_, NUM_OF_PXOBJS_CORE1);

/* Number of global objects that will be used by pxros-core.ld to allocate
 * sufficient memory area for storing objects
 */
DEF_SYM(__NUM_OF_GLOBAL_PXOBJS__CPU1_, NUM_OF_GLOBAL_PXOBJS_CORE1);

/* Declaration of user stack size that will be added in pxros-image.ld
 * to pxros_system size
 */
DEF_SYM(USER_SYSTEM_STACK_SIZE_CPU1_, USER_SYSTEM_STACK_SIZE_CORE1);

/* Declaration of core CSA size to allocate based on user number
 * of CSA regions, where the region size is fixed of 64B on TCx arch.
 */
#define CSA_SIZE_CORE1    ((NUM_OF_CSA_REGION_CORE1) * 64)
DEF_SYM(__CSA_SIZE_CPU1_, CSA_SIZE_CORE1);

/* Object memory created in pxros-core.ld (Base and Size coordinates) */
extern PxMemAligned_t  PxObjmem_CPU1_[];
extern PxSize_t PX_OBJMEMSIZE_CPU1_[];

/* System stack coordinates coming from pxros-core.ld */
extern const PxUInt_t PXROS_SYSTEM_STACK_BEGIN_CPU1_[];
extern const PxUInt_t PXROS_SYSTEM_STACK_SIZE_CPU1_[];

/* System memory (PXMcSystemdefault)
 * shall be large enough
 * - to allocate all core's task control block (TCB)
 * - memory taken from PXMcSystemdefault
 */
PRAGMA_SECTION_BSS(".CPU1..systemmemory",8,awB)
#define SYSMEMSIZE_CORE1 (PXMEM_ADJUST((NUM_OF_TASKS_CORE1) * (PXTASK_SIZE + PXMCVARSIZED_ADDON)))
PxMemAligned_t Sysmem_Core1[SYSMEMSIZE_CORE1 / sizeof(PxMemAligned_t)];
PRAGMA_SECTION_BSS_END

/* Task memory class (PXMcTaskdefault)
 * shall be large enough for
 * - all core's message data areas
 * - User tasks stacks if they use PXStackAlloc scheme
 * - other memory taken from PXMcTaskdefault
 */
PRAGMA_SECTION_BSS(".CPU1..taskmemory",8,awB)
PxMemAligned_t Taskmem_Core1[PXMEM_ADJUST(TASKMEMSIZE_CORE1) / sizeof(PxMemAligned_t)];
PRAGMA_SECTION_BSS_END

/* PXROS-HR InitSpec for kernel instance */
static const PxInitSpec_T InitSpec_Core1 =
{
    .is_sysmc_type        = PXMcVarsizedAligned,
    .is_sysmc_size        = 8,
    .is_sysmc_blk         = Sysmem_Core1,
    .is_sysmc_blksize     = SYSMEMSIZE_CORE1,

    .is_obj_number        = NUM_OF_PXOBJS_CORE1,
    .is_global_obj_number = NUM_OF_GLOBAL_PXOBJS_CORE1,
    .is_obj_namelength    = PXROS_NAMESIZE,

    .is_inittask          = &InitTaskSpec_Core1,

    .is_monitoring        = PXMonitorMemory,
    .is_schedext          = 0,

    .is_objmc_blk         = PxObjmem_CPU1_,
    .is_objmc_blksize     = (PxSize_t) PX_OBJMEMSIZE_CPU1_,

    .is_taskmc_type       = PXMcVarsizedAdjusted,
    .is_taskmc_size       = 8,
    .is_taskmc_blk        = Taskmem_Core1,
    .is_taskmc_blksize    = TASKMEMSIZE_CORE1,

#if PXROS_MASTER_CORE == 1
    .is_core_start        = 0,
#else
    .is_core_start        = (unsigned int)_crt0_reset,
#endif

    .is_system_stack      = PXROS_SYSTEM_STACK_BEGIN_CPU1_,
    .is_system_stack_size = (PxUInt_t) PXROS_SYSTEM_STACK_SIZE_CPU1_,

    .is_sys_code          = &_cpu1_sys_code_protection,
    .is_sys_data          = &_cpu1_sys_and_kernel_data_protection,
    .is_task_code         = &_cpu1_task_code_protection,
};

#endif /* #if CORE1_ACTIVE */


/* ================================================================================================
 * CORE [2] PXROS-HR KERNEL INSTANCE CONFIGURATION AND SYMBOLS
 * ==============================================================================================*/

#if CORE2_ACTIVE

/* Number of local objects that will be used by pxros-core.ld to allocate
 * sufficient memory area for storing objects
 */
DEF_SYM(__NUM_OF_PXOBJS__CPU2_, NUM_OF_PXOBJS_CORE2);

/* Number of global objects that will be used by pxros-core.ld to allocate
 * sufficient memory area for storing objects
 */
DEF_SYM(__NUM_OF_GLOBAL_PXOBJS__CPU2_, NUM_OF_GLOBAL_PXOBJS_CORE2);

/* Declaration of user stack size that will be added in pxros-image.ld
 * to pxros_system size
 */
DEF_SYM(USER_SYSTEM_STACK_SIZE_CPU2_, USER_SYSTEM_STACK_SIZE_CORE2);

/* Declaration of core CSA size to allocate based on user number
 * of CSA regions, where the region size is fixed of 64B on TCx arch.
 */
#define CSA_SIZE_CORE2    ((NUM_OF_CSA_REGION_CORE2) * 64)
DEF_SYM(__CSA_SIZE_CPU2_, CSA_SIZE_CORE2);

/* Object memory created in pxros-core.ld (Base and Size coordinates) */
extern PxMemAligned_t  PxObjmem_CPU2_[];
extern PxSize_t  PX_OBJMEMSIZE_CPU2_[];

/* System stack coordinates coming from pxros-core.ld */
extern const PxUInt_t PXROS_SYSTEM_STACK_BEGIN_CPU2_[];
extern const PxUInt_t PXROS_SYSTEM_STACK_SIZE_CPU2_[];

/* System memory (PXMcSystemdefault)
 * shall be large enough
 * - to allocate all core's task control block (TCB)
 * - memory taken from PXMcSystemdefault
 */
PRAGMA_SECTION_BSS(".CPU2..systemmemory",8,awB)
#define SYSMEMSIZE_CORE2 (PXMEM_ADJUST((NUM_OF_TASKS_CORE2) * (PXTASK_SIZE + PXMCVARSIZED_ADDON)))
PxMemAligned_t Sysmem_Core2[SYSMEMSIZE_CORE2 / sizeof(PxMemAligned_t)];
PRAGMA_SECTION_BSS_END

/* Task memory class (PXMcTaskdefault)
 * shall be large enough for
 * - all core's message data areas
 * - User tasks stacks if they use PXStackAlloc scheme
 * - other memory taken from PXMcTaskdefault
 */
PRAGMA_SECTION_BSS(".CPU2..taskmemory",8,awB)
PxMemAligned_t Taskmem_Core2[PXMEM_ADJUST(TASKMEMSIZE_CORE2) / sizeof(PxMemAligned_t)];
PRAGMA_SECTION_BSS_END

/* PXROS-HR InitSpec for kernel instance */
static const PxInitSpec_T InitSpec_Core2 =
{
    .is_sysmc_type        = PXMcVarsizedAligned,
    .is_sysmc_size        = 8,
    .is_sysmc_blk         = Sysmem_Core2,
    .is_sysmc_blksize     = SYSMEMSIZE_CORE2,

    .is_obj_number        = NUM_OF_PXOBJS_CORE2,
    .is_global_obj_number = NUM_OF_GLOBAL_PXOBJS_CORE2,
    .is_obj_namelength    = PXROS_NAMESIZE,

    .is_inittask          = &InitTaskSpec_Core2,

    .is_monitoring        = PXMonitorMemory,
    .is_schedext          = 0,

    .is_objmc_blk         = PxObjmem_CPU2_,
    .is_objmc_blksize     = (PxSize_t) PX_OBJMEMSIZE_CPU2_,

    .is_taskmc_type       = PXMcVarsizedAdjusted,
    .is_taskmc_size       = 8,
    .is_taskmc_blk        = Taskmem_Core2,
    .is_taskmc_blksize    = TASKMEMSIZE_CORE2,

#if PXROS_MASTER_CORE == 2
    .is_core_start        = 0,
#else
    .is_core_start        = (unsigned int)_crt0_reset,
#endif
     /* the system stack */
    .is_system_stack      = PXROS_SYSTEM_STACK_BEGIN_CPU2_,
    .is_system_stack_size = (PxUInt_t) PXROS_SYSTEM_STACK_SIZE_CPU2_,

    /* the protection definition */
    .is_sys_code          = &_cpu2_sys_code_protection,
    .is_sys_data          = &_cpu2_sys_and_kernel_data_protection,
    .is_task_code         = &_cpu2_task_code_protection,
};

#endif /* #if CORE2_ACTIVE */


/* ================================================================================================
 * PXROS-HR INIT SPEC ARRAY
 * needed for 'PxInit()' function.
 * ==============================================================================================*/

const PxInitSpecsArray_t InitSpecsArray =
{
#if CORE0_ACTIVE == 1
    [0] = &InitSpec_Core0,
#else
    [0] = NULL_PxInitSpec,
#endif

#if CORE1_ACTIVE == 1
    [1] = &InitSpec_Core1,
#else
    [1] = NULL_PxInitSpec,
#endif

#if CORE2_ACTIVE == 1    
    [2] = &InitSpec_Core2,
#else
    [2] = NULL_PxInitSpec,
#endif
};

