/**************************************************************************************************
 * FILE: initTask_cfg.c
 *
 * DESCRIPTION:
 *     InitTask configuration for all PXROS-HR core instances.
 *
 * HISTORY:
 *    v1   06.2020   RO
 *         - PXROS-HR BSP Base reference example
 *
 *    v2   07.2020   JS
 *         - Update to PXROS-HR v8.x.x
 *
 *    v3, 01.2022    HL
 *         - Adapt to LLVM
 *
 **************************************************************************************************
 * SPDX-FileCopyrightText: HighTec EDV-Systeme GmbH
 *
 * SPDX-License-Identifier: Boost Software License - Version 1.0
 *************************************************************************************************/

#include "pxdef.h"
#include "pxros/hal/px_bsp.h"
#include "pxros/config/system_cfg.h"
#include "pxros/tasks/InitTask/InitTask.h"

/* ================================================================================================
 * VEECLE ADDED EXTERNAL SYMBOLS
 * Coming from the Linker files
 * ==============================================================================================*/

/* DATA section start and end */
extern unsigned int __DATA_START;
extern unsigned int __DATA_END;
/* RODATA section start and end */
extern unsigned int __RODATA_START;
extern unsigned int __RODATA_END;
/* BSS section start and end */
extern unsigned int __BSS_START;
extern unsigned int __BSS_END;

/* ================================================================================================
 * COMMON EXTERNAL SYMBOLS
 * Coming from the Linker files
 * ==============================================================================================*/

/* READ-ONLY data area description for the system */
extern unsigned int PxTricSystemRodataLowerBound;
extern unsigned int PxTricSystemRodataUpperBound;


/* ===============================================================================================
 * CORE_0 INIT TASK CONFIGURATION
 * =============================================================================================*/

/* CORE is under PXROS-HR kernel control */
#if CORE0_ACTIVE == 1

/* STACKs in [Bytes] */
#define INITTASK_STACKSIZE_CORE0            2048
#define INITTASK_INTSTACKSIZE_CORE0         64

/* ACCESS RIGHTS */
#define INITTASK_PXACCESS_CORE0 \
    PXACCESS_HANDLERS  | \
    PXACCESS_INSTALL_HANDLERS  | \
    PXACCESS_INSTALL_SERVICES  | \
    PXACCESS_REGISTERS  | \
    PXACCESS_SYSTEMDEFAULT  | \
    PXACCESS_RESOURCES  | \
    PXACCESS_NEW_RESOURCES  | \
    PXACCESS_SYSTEM_CONTROL  | \
    PXACCESS_MODEBITS  | \
    PXACCESS_TASK_CREATE  | \
    PXACCESS_TASK_CREATE_HIGHER_PRIO  | \
    PXACCESS_TASK_SET_HIGHER_PRIO  | \
    PXACCESS_CHANGE_PRIO  | \
    PXACCESS_TASK_RESTORE_ACCESS_RIGHTS  | \
    PXACCESS_TASK_CREATE_HIGHER_ACCESS  | \
    PXACCESS_GLOBAL_OBJECTS

/* PRIVILEGES for Peripheral register access
 * PXUser0Privilege : No direct register access, only via PXROS-HR API (syscall)
 * PXUser1Privilege : Direct register access allowed
 */
#define INITTASK_PRIVILIGES_CORE0    PXUser1Privilege

#define LOWER
#define UPPER

/* EXTENDED MEMORY REGIONS
 * {LowerBound, UpperBound, AccesType}
 * {0, 0, 0} End delimiter
 */
static const PxProtectRegion_T InitTaskRegions_core0[] =
{
    { (unsigned int) &MODULE_STM0, (unsigned int) &MODULE_STM0 + sizeof(Ifx_STM), WRProtection },
    { (unsigned int) &MODULE_SRC,  (unsigned int) &MODULE_SRC  + sizeof(Ifx_SRC), WRProtection },
    { (unsigned int) &MODULE_SCU,  (unsigned int) &MODULE_SCU  + sizeof(Ifx_SCU), ReadProtection },
    { (unsigned int) &__DATA_START, (unsigned int) &__DATA_END, WRProtection },
    { (unsigned int) &__RODATA_START, (unsigned int) &__RODATA_END, WRProtection },
    { (unsigned int) &__BSS_START, (unsigned int) &__BSS_END, WRProtection },
    { 0, 0, 0 }
};

/* STACK MEMORY ALLOCATION
 * as the first task must use a dedicated STACK memory
 * and cannot use PxStackAlloc method creating task's stack in PXMCTaskdefault memory
 * In its own section to allow linker placement during integration
 */
#define INITTASK_STACKSIZE_ADJUSTED_CORE0 (PXSTACK_ADJUST(INITTASK_STACKSIZE_CORE0) / sizeof(PxStackAligned_t))
#define INITTASK_INTSTACKSIZE_ADJUSTED_CORE0 (PXSTACK_ADJUST(INITTASK_INTSTACKSIZE_CORE0) / sizeof(PxStackAligned_t))

PRAGMA_SECTION_BSS(".CPU0..inittask.stack",8, awB)
PxStackAligned_t inittask0_stack[INITTASK_STACKSIZE_ADJUSTED_CORE0];
PxStackAligned_t inittask0_intstack[INITTASK_INTSTACKSIZE_ADJUSTED_CORE0];
PRAGMA_SECTION_BSS_END

/* TASK DATA PROTECTION CONTEXT
 * .protection[0] : READ-ONLY data area
 * .protection[1] : READ-WRITE data area
 */
extern unsigned int INITTASK_STACK_BEGIN_CPU0_;
extern unsigned int INITTASK_STACK_END_CPU0_;

static const PxTaskContext_T InitTaskContext_0 =
{
    .protection[0] =
    {
        (unsigned int) &PxTricSystemRodataLowerBound,
        (unsigned int) &PxTricSystemRodataUpperBound,
        ReadProtection
    },
    .protection[1] =
    {
        (unsigned int) &INITTASK_STACK_BEGIN_CPU0_,
        (unsigned int) &INITTASK_STACK_END_CPU0_,
        WRProtection
    }
};

/* TASK CONFIGURATION STRUCTURE */
const PxTaskSpec_T InitTaskSpec_Core0 =
{
    .ts_name           = (const PxChar_t *)"InitTask_C0",
    .ts_fun            = InitTask_Func,
    .ts_mc             = PXMcTaskdefaultInit,
    .ts_opool          = PXOpoolSystemdefaultInit,
    .ts_privileges     = INITTASK_PRIVILIGES_CORE0,
    .ts_accessrights   = INITTASK_PXACCESS_CORE0,
    .ts_context        = &InitTaskContext_0,
    .ts_protect_region = InitTaskRegions_core0,
    .ts_taskstack =
    {
        PXStackFall,
        PXStackDontCheck,
        {&inittask0_stack[INITTASK_STACKSIZE_ADJUSTED_CORE0]}
    },
    .ts_inttaskstack =
    {
        PXStackFall,
        PXStackDontCheck,
        {&inittask0_intstack[INITTASK_INTSTACKSIZE_ADJUSTED_CORE0]}
    },
    .ts_abortstacksize = 0
};

#endif /* #if CORE0_ACTIVE == 1 */


/* ================================================================================================
 * CORE_1 INIT TASK CONFIGURATION
 * ==============================================================================================*/

/* CORE is under PXROS-HR kernel control */
#if CORE1_ACTIVE == 1

/* STACKs in [Bytes] */
#define INITTASK_STACKSIZE_CORE1            2048
#define INITTASK_INTSTACKSIZE_CORE1         64

/* ACCESS RIGHTS */
#define INITTASK_PXACCESS_CORE1  \
    PXACCESS_HANDLERS  | \
    PXACCESS_INSTALL_HANDLERS  | \
    PXACCESS_INSTALL_SERVICES  | \
    PXACCESS_REGISTERS  | \
    PXACCESS_SYSTEMDEFAULT  | \
    PXACCESS_RESOURCES  | \
    PXACCESS_NEW_RESOURCES  | \
    PXACCESS_SYSTEM_CONTROL  | \
    PXACCESS_MODEBITS  | \
    PXACCESS_TASK_CREATE  | \
    PXACCESS_TASK_CREATE_HIGHER_PRIO  | \
    PXACCESS_TASK_SET_HIGHER_PRIO  | \
    PXACCESS_CHANGE_PRIO  | \
    PXACCESS_TASK_RESTORE_ACCESS_RIGHTS  | \
    PXACCESS_TASK_CREATE_HIGHER_ACCESS  | \
    PXACCESS_GLOBAL_OBJECTS

/* PRIVILEGES for Peripheral register access
 * PXUser0Privilege : No direct register access, only via PXROS-HR API (syscall)
 * PXUser1Privilege : Direct register access allowed
 */
#define INITTASK_PRIVILIGES_CORE1      PXUser1Privilege

/* EXTENDED MEMORY REGIONS
 * {LowerBound, UpperBound, AccesType}
 */
static const PxProtectRegion_T InitTaskRegions_core1[] =
{
    { (unsigned int) &MODULE_STM1, (unsigned int) &MODULE_STM1 + sizeof(Ifx_STM), WRProtection },
    { (unsigned int) &MODULE_SRC,  (unsigned int) &MODULE_SRC + sizeof(Ifx_SRC), WRProtection },
    { (unsigned int) &__DATA_START, (unsigned int) &__DATA_END, WRProtection },
    { (unsigned int) &__RODATA_START, (unsigned int) &__RODATA_END, WRProtection },
    { (unsigned int) &__BSS_START, (unsigned int) &__BSS_END, WRProtection },
    { 0, 0, 0 }
};

/* STACK MEMORY ALLOCATION
 * as the first task must use a dedicated STACK memory
 * and cannot use PxStackAlloc method creating task's stack in PXMCTaskdefault memory
 * In its own section to allow linker placement during integration
 */
#define INITTASK_STACKSIZE_ADJUSTED_CORE1 (PXSTACK_ADJUST(INITTASK_STACKSIZE_CORE1) / sizeof(PxStackAligned_t))
#define INITTASK_INTSTACKSIZE_ADJUSTED_CORE1 (PXSTACK_ADJUST(INITTASK_INTSTACKSIZE_CORE1) / sizeof(PxStackAligned_t))

PRAGMA_SECTION_BSS(".CPU1..inittask.stack",8, awB)
PxStackAligned_t inittask1_stack[INITTASK_STACKSIZE_ADJUSTED_CORE1];
PxStackAligned_t inittask1_intstack[INITTASK_INTSTACKSIZE_ADJUSTED_CORE1];
PRAGMA_SECTION_BSS_END

/* TASK DATA PROTECTION CONTEXT
 * .protection[0] : READ-ONLY data area
 * .protection[1] : READ-WRITE data area
 */
extern unsigned int INITTASK_STACK_BEGIN_CPU1_;
extern unsigned int INITTASK_STACK_END_CPU1_;

static const PxTaskContext_T InitTaskContext_1 =
{
    .protection[0] =
    {
        (unsigned int) &PxTricSystemRodataLowerBound,
        (unsigned int) &PxTricSystemRodataUpperBound,
        ReadProtection
    },
    .protection[1] =
    {
        (unsigned int) &INITTASK_STACK_BEGIN_CPU1_,
        (unsigned int) &INITTASK_STACK_END_CPU1_,
        WRProtection
    }
};

/* TASK CONFIGURATION STRUCTURE */
const PxTaskSpec_T InitTaskSpec_Core1 =
{
    .ts_name           = (const PxChar_t *)"InitTask_C1",
    .ts_fun            = InitTask_Func,
    .ts_mc             = PXMcTaskdefaultInit,
    .ts_opool          = PXOpoolSystemdefaultInit,
    .ts_privileges     = INITTASK_PRIVILIGES_CORE1,
    .ts_accessrights   = INITTASK_PXACCESS_CORE1,
    .ts_context        = &InitTaskContext_1,
    .ts_protect_region = InitTaskRegions_core1,
    .ts_taskstack =
    {
        PXStackFall,
        PXStackDontCheck,
        {&inittask1_stack[INITTASK_STACKSIZE_ADJUSTED_CORE1]}
    },
    .ts_inttaskstack =
    {
        PXStackFall,
        PXStackDontCheck,
        {&inittask1_intstack[INITTASK_INTSTACKSIZE_ADJUSTED_CORE1]}
    },
    .ts_abortstacksize = 0
};

#endif /* #if CORE1_ACTIVE == 1 */


/* ================================================================================================
 * CORE_2 INIT TASK CONFIGURATION
 * ==============================================================================================*/

/* CORE is under PXROS-HR kernel control */
#if CORE2_ACTIVE == 1

/* STACKs in [Bytes] */
#define INITTASK_STACKSIZE_CORE2       2048
#define INITTASK_INTSTACKSIZE_CORE2    64

/* ACCESS RIGHTS */
#define INITTASK_PXACCESS_CORE2  \
    PXACCESS_HANDLERS  | \
    PXACCESS_INSTALL_HANDLERS  | \
    PXACCESS_INSTALL_SERVICES  | \
    PXACCESS_REGISTERS  | \
    PXACCESS_SYSTEMDEFAULT  | \
    PXACCESS_RESOURCES  | \
    PXACCESS_NEW_RESOURCES  | \
    PXACCESS_SYSTEM_CONTROL  | \
    PXACCESS_MODEBITS  | \
    PXACCESS_TASK_CREATE  | \
    PXACCESS_TASK_CREATE_HIGHER_PRIO  | \
    PXACCESS_TASK_SET_HIGHER_PRIO  | \
    PXACCESS_CHANGE_PRIO  | \
    PXACCESS_TASK_RESTORE_ACCESS_RIGHTS  | \
    PXACCESS_TASK_CREATE_HIGHER_ACCESS  | \
    PXACCESS_GLOBAL_OBJECTS

/* PRIVILEGES for Peripheral register access
 * PXUser0Privilege : No direct register access, only via PXROS-HR API (syscall)
 * PXUser1Privilege : Direct register access allowed
 */
#define INITTASK_PRIVILIGES_CORE2      PXUser1Privilege

/* EXTENDED MEMORY REGIONS
 * {LowerBound, UpperBound, AccesType}
 */
static const PxProtectRegion_T InitTaskRegions_core2[] =
{
    { (unsigned int) &MODULE_STM2, (unsigned int) &MODULE_STM2 + sizeof(Ifx_STM), WRProtection },
    { (unsigned int) &MODULE_SRC,  (unsigned int) &MODULE_SRC + sizeof(Ifx_SRC), WRProtection },
    { (unsigned int) &__DATA_START, (unsigned int) &__DATA_END, WRProtection },
    { (unsigned int) &__RODATA_START, (unsigned int) &__RODATA_END, WRProtection },
    { (unsigned int) &__BSS_START, (unsigned int) &__BSS_END, WRProtection },
    { 0, 0, 0 }
};

/* STACK MEMORY ALLOCATION
 * as the first task must use a dedicated STACK memory
 * and cannot use PxStackAlloc method creating task's stack in PXMCTaskdefault memory
 * In its own section to allow linker placement during integration
 */
#define INITTASK_STACKSIZE_ADJUSTED_CORE2 (PXSTACK_ADJUST(INITTASK_STACKSIZE_CORE2) / sizeof(PxStackAligned_t))
#define INITTASK_INTSTACKSIZE_ADJUSTED_CORE2 (PXSTACK_ADJUST(INITTASK_INTSTACKSIZE_CORE2) / sizeof(PxStackAligned_t))

PRAGMA_SECTION_BSS(".CPU2..inittask.stack",8, awB)
PxStackAligned_t inittask2_stack[INITTASK_STACKSIZE_ADJUSTED_CORE2];
PxStackAligned_t inittask2_intstack[INITTASK_INTSTACKSIZE_ADJUSTED_CORE2];
PRAGMA_SECTION_BSS_END

/* TASK DATA PROTECTION CONTEXT
 * .protection[0] : READ-ONLY data area
 * .protection[1] : READ-WRITE data area
 */
extern unsigned int INITTASK_STACK_BEGIN_CPU2_;
extern unsigned int INITTASK_STACK_END_CPU2_;

static const PxTaskContext_T InitTaskContext_2 =
{
    .protection[0] =
    {
        (unsigned int) &PxTricSystemRodataLowerBound,
        (unsigned int) &PxTricSystemRodataUpperBound,
        ReadProtection
    },
    .protection[1] =
    {
        (unsigned int) &INITTASK_STACK_BEGIN_CPU2_,
        (unsigned int) &INITTASK_STACK_END_CPU2_,
        WRProtection
    }
};

/* TASK CONFIGURATION STRUCTURE */
const PxTaskSpec_T InitTaskSpec_Core2 =
{
    .ts_name           = (const PxChar_t *)"InitTask_C2",
    .ts_fun            = InitTask_Func,
    .ts_mc             = PXMcTaskdefaultInit,
    .ts_opool          = PXOpoolSystemdefaultInit,
    .ts_privileges     = INITTASK_PRIVILIGES_CORE2,
    .ts_accessrights   = INITTASK_PXACCESS_CORE2,
    .ts_context        = &InitTaskContext_2,
    .ts_protect_region = InitTaskRegions_core2,
    .ts_taskstack =
    {
        PXStackFall,
        PXStackDontCheck,
        {&inittask2_stack[INITTASK_STACKSIZE_ADJUSTED_CORE2]}
    },
    .ts_inttaskstack =
    {
        PXStackFall,
        PXStackDontCheck,
        {&inittask2_intstack[INITTASK_INTSTACKSIZE_ADJUSTED_CORE2]}
    },
    .ts_abortstacksize = 0
};

#endif /* #if CORE2_ACTIVE == 1 */



