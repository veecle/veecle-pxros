/**************************************************************************************************
 * FILE: system_mpu_cfg.c
 *
 * DESCRIPTION:
 *     PXROS-HR Kernel (supervisor level) MPU setting
 *
 * HISTORY:
 *    v1   06.2020   RO
 *         - PXROS-HR BSP Base reference example
 *
 *    v2, 07.2020,   JS
 *         - Update to PXROS-HR v8.x.x
 *
 **************************************************************************************************
 * SPDX-FileCopyrightText: HighTec EDV-Systeme GmbH
 *
 * SPDX-License-Identifier: Boost Software License - Version 1.0
 *************************************************************************************************/


#include "system_cfg.h"

/* Helper defines */
#define READ_PERMITTED              (1)
#define WRITE_PERMITTED             (1)
#define EXECUTION_PERMITTED         (1)

/* ================================================================================================
 * EXTERNAL SYMBOLS
 * Coming from the Linker files
 * ==============================================================================================*/

/* SHARED CODE memories */
/* All the code of system, kernel and tasks. */
extern PxUInt_t __TEXT_BEGIN[];
extern PxUInt_t __TEXT_END[];

/* SHARED READ-ONLY data */
extern PxUInt_t PxTricSystemRodataLowerBound[];
extern PxUInt_t PxTricSystemRodataUpperBound[];

/* peripheral (SFR) range */
extern PxUInt_t PERIPHERAL_MEM_BASE[];
extern PxUInt_t PERIPHERAL_MEM_END[];

/* CORE[0] RW DATA regions */
#if (CORE0_ACTIVE == 1)
extern PxUInt_t PxTricSystemDataLowerBound_CPU0_[];
extern PxUInt_t PxTricSystemDataUpperBound_CPU0_[];
extern PxUInt_t __CSA_BEGIN_CPU0_[];
extern PxUInt_t __CSA_END_CPU0_[];
extern PxUInt_t PXROS_SYSTEM_STACK_BEGIN_CPU0_[];
extern PxUInt_t PXROS_SYSTEM_STACK_CPU0_[];
#endif /* (CORE0_ACTIVE == 1) */

/* CORE[1] RW DATA regions */
#if (CORE1_ACTIVE == 1)
extern PxUInt_t PxTricSystemDataLowerBound_CPU1_[];
extern PxUInt_t PxTricSystemDataUpperBound_CPU1_[];
extern PxUInt_t __CSA_BEGIN_CPU1_[];
extern PxUInt_t __CSA_END_CPU1_[];
extern PxUInt_t PXROS_SYSTEM_STACK_BEGIN_CPU1_[];
extern PxUInt_t PXROS_SYSTEM_STACK_CPU1_[];
#endif /* (CORE1_ACTIVE == 1) */

/* CORE[2] RW DATA regions */
#if (CORE2_ACTIVE == 1)
extern PxUInt_t PxTricSystemDataLowerBound_CPU2_[];
extern PxUInt_t PxTricSystemDataUpperBound_CPU2_[];
extern PxUInt_t __CSA_BEGIN_CPU2_[];
extern PxUInt_t __CSA_END_CPU2_[];
extern PxUInt_t PXROS_SYSTEM_STACK_BEGIN_CPU2_[];
extern PxUInt_t PXROS_SYSTEM_STACK_CPU2_[];
#endif /* (CORE2_ACTIVE == 1) */

/* ================================================================================================
 * SYSTEM MPU DATA RANGES & PERMISSIONS CONFIGURATION VALUES
 * 9 DATA access regions used by PXROS-HR kernel in Supervisor level
 *
 * REGION DESCRIPTORS
 *     DPR[x]_BEGIN_CORE[y]             : Start address of the protected region
 *     DPR[x]_END_CORE[y]               : End address of the protected region
 *     DPR[x]_KERNEL_READ_CTRL_CORE[y]  : Read access for PXROS Kernel (1=permitted, 0=disabled)
 *     DPR[x]_KERNEL_WRITE_CTRL_CORE[y] : Write access for PXROS Kernel (1=permitted, 0=disabled)
 *     DPR[x]_SYSTEM_READ_CTRL_CORE[y]  : Read access for System (1=permitted, 0=disabled)
 *     DPR[x]_SYSTEM_WRITE_CTRL_CORE[y] : Write access for System (1=permitted, 0=disabled)
 *
 * ==============================================================================================*/

/* ================================================================================================
 * SYSTEM CODE & DATA MPU CONFIGURATIONS
 *     Passed to PXROS-HR Initialization in the 'InitSpec' configuration.
 *     They are stored as constants in the RODATA memory.
 *
 * MPU CONFIGURATION STRUCTURES
 *     _cpu[x]_sys_code_protection  : 5x System Code protection region
 *     _cpu[x]_task_code_protection : 5x Task CODE protection region
 *     _cpu[x]_sys_data_protection  : 9x System DATA protection region
 *
 * CODE STRUCTURE FIELDS
 *     .cpr[x]                      : code protection MPU region
 *     .cpmr.cpxe.bits[x]           : code execution permission for the code MPU regions
 *
 * DATA STRUCTURE FIELDS
 *     .dpr[y]                      : data protection MPU regions
 *     .dpmr_kernel.dpre.bits[y]    : data read permission for the data MPU regions (for kernel)
 *     .dpmr_kernel.dpwe.bits[y]    : data write permission for the data MPU regions (for kernel)
 *     .dpmr_system.dpre.bits[y]    : data read permission for the data MPU regions (for system)
 *     .dpmr_system.dpwe.bits[y]    : data write permission for the data MPU regions (for system)
 *
 * ==============================================================================================*/

/*
 *          ---===  CODE PROTECTION SETTINGS  ===---
 *
 */

/* COMMON protection settings for system and task code.
 *
 * Note that the term "system code" covers both the code
 * of system and kernel, i.e. all interrupt and trap vector
 * table code, _PxHndcall handler code, fast and fast
 * context handlers and PXROS services.
 *
 * The task code covers functions and context handlers of
 * all tasks.
 */

/*
 *  PROTECTION SETTINGS FOR SYSTEM CODE
 *
 *  The protection settings for system code
 *  sets the execution permission for the
 *  whole program flash memory.
 */
#define _SYS_CODE_PROTECTION                            \
{                                                       \
    /* MPU Code Regions */                              \
    .cpr[0].s = {(PxUInt_t) __TEXT_BEGIN,               \
                 (PxUInt_t) __TEXT_END},                \
                                                        \
    .cpr[1].s = {(PxUInt_t) 0,                          \
                 (PxUInt_t) 0},                         \
                                                        \
    .cpr[2].s = {(PxUInt_t) 0,                          \
                 (PxUInt_t) 0},                         \
                                                        \
    .cpr[3].s = {(PxUInt_t) 0,                          \
                 (PxUInt_t) 0},                         \
                                                        \
    .cpr[4].s = {(PxUInt_t) 0,                          \
                 (PxUInt_t) 0},                         \
                                                        \
    /* MPU Code Regions Permissions */                  \
    .cpmr.cpxe.bits =                                   \
    {                                                   \
        .dp0 = EXECUTION_PERMITTED,                     \
        .dp1 = 0,                                       \
        .dp2 = 0,                                       \
        .dp3 = 0,                                       \
        .dp4 = 0                                        \
    }                                                   \
}

/*
 *  PROTECTION SETTINGS FOR ALL TASK'S CODE
 *
 *  The protection settings for task's code
 *  sets the execution permission for the
 *  whole program flash memory.
 */
#define _TASK_CODE_PROTECTION                           \
{                                                       \
    /* MPU Code regions */                              \
    .cpr[0].s = {(PxUInt_t) __TEXT_BEGIN,               \
                 (PxUInt_t) __TEXT_END },               \
                                                        \
    .cpr[1].s = {(PxUInt_t) 0,                          \
                 (PxUInt_t) 0 },                        \
                                                        \
    .cpr[2].s = {(PxUInt_t) 0,                          \
                 (PxUInt_t) 0 },                        \
                                                        \
    .cpr[3].s = {(PxUInt_t) 0,                          \
                 (PxUInt_t) 0 },                        \
                                                        \
    .cpr[4].s = {(PxUInt_t) 0,                          \
                 (PxUInt_t) 0 },                        \
                                                        \
    /* MPU Code Regions Permissions */                  \
    .cpmr.cpxe.bits =                                   \
    {                                                   \
        .dp0 = EXECUTION_PERMITTED,                     \
        .dp1 = 0,                                       \
        .dp2 = 0,                                       \
        .dp3 = 0,                                       \
        .dp4 = 0                                        \
    }                                                   \
}

/*
 *      ---===   DATA PROTECTION SETTINGS   ===---
 *
 */

/*
 *  PROTECTION SETTINGS FOR SYSTEM AND KERNEL DATA
 *
 *      To run a minimal PXROS-HR application, the
 *      following MPU regions with the corresponding
 *      access rights must be set:
 *
 *      Memory          Access rights   Access rights   Notes
 *      region          for Kernel      for System
 *      --------------  -------------   -------------  ------
 *      Read-Only data  Read-Only       None            1
 *      CSA             R/W             None            2
 *      Kernel's data   R/W             None            3
 *      Peripherals     R/W             None            4
 *      System stack    None            R/W             5
 *
 *      NOTES:
 *      1) Kernel needs to read the configuration structures
 *         stored as constants in flash memory (RO data).
 *      2) Kernel needs RW access to Context Save Area
 *         for task context switching.
 *      3) Kernel data covers internal Kernel variables,
 *         system memory and object memory. This region is
 *         defined by linker file pxros-core.ld.
 *      4) Kernel needs RW access to Peripherals to trigger
 *         interrupts via Special Function Registers.
 *      5) System stack is used solely by system interrupts,
 *         traps and fast and fast context handlers.
 */

/* Read permissions for the data regions used by
 * kernel.
 */
#define _KERNEL_DATA_READ_PERMISSIONS                   \
{                                                       \
        .dp0 = READ_PERMITTED,  /* RO data */           \
        .dp1 = READ_PERMITTED,  /* CSA */               \
        .dp2 = READ_PERMITTED,  /* Kernel's RW data */  \
        .dp3 = READ_PERMITTED,  /* peripherals */       \
        .dp4 = 0,               /* System stack */      \
        .dp5 = 0,                                       \
        .dp6 = 0,                                       \
        .dp7 = 0                                        \
}

/* Write permissions for the data regions used by
 * kernel.
 */
#define _KERNEL_DATA_WRITE_PERMISSIONS                  \
{                                                       \
        .dp0 = 0,               /* RO data */           \
        .dp1 = WRITE_PERMITTED, /* CSA */               \
        .dp2 = WRITE_PERMITTED, /* Kernel's RW data */  \
        .dp3 = WRITE_PERMITTED, /* peripherals */       \
        .dp4 = 0,               /* System stack */      \
        .dp5 = 0,                                       \
        .dp6 = 0,                                       \
        .dp7 = 0                                        \
}

/* Read permissions for the data regions used by
 * interrupts.
 */
#define _SYSTEM_DATA_READ_PERMISSIONS                \
{                                                    \
        .dp0 = 0,               /* RO data */        \
        .dp1 = 0,               /* CSA */            \
        .dp2 = 0,               /* RW data */        \
        .dp3 = 0,               /* peripherals */    \
        .dp4 = READ_PERMITTED,  /* System stack */   \
        .dp5 = 0,                                    \
        .dp6 = 0,                                    \
        .dp7 = 0                                     \
}

/* Write permissions for the data regions used by
 * interrupts.
 */
#define _SYSTEM_DATA_WRITE_PERMISSIONS               \
{                                                    \
        .dp0 = 0,               /* RO data */        \
        .dp1 = 0,               /* CSA */            \
        .dp2 = 0,               /* RW data */        \
        .dp3 = 0,               /* peripherals */    \
        .dp4 = WRITE_PERMITTED, /* System stack */   \
        .dp5 = 0,                                    \
        .dp6 = 0,                                    \
        .dp7 = 0                                     \
}

/*
 *  The last section defines for each core the  following
 *  set of initialized PXROS structures:
 *
 *  PxCodeProtectSet_T ... configuring the system code protection
 *
 *  PxCodeProtectSet_T ... configuring the task code protection
 *
 *  PxCodeProtectSet_T ... configuring the system and kernel data p.
 *
 *  These constants are located in flash memory.
 */

/*
 *
 *           ===============================
 *          |||                           |||
 *          ||      C O R E    [ 0 ]       ||
 *          |||                           |||
 *           ===============================
 *
 */

#if (CORE0_ACTIVE == 1)

/* MPU SETTINGS FOR SYSTEM CODE */
const PxCodeProtectSet_T _cpu0_sys_code_protection = _SYS_CODE_PROTECTION;

/* MPU SETTINGS FOR TASK CODE */
const PxCodeProtectSet_T _cpu0_task_code_protection = _TASK_CODE_PROTECTION;

/* MPU SETTINGS FOR SYSTEM AND KERNEL DATA */
const PxDataProtectSetInit_T _cpu0_sys_and_kernel_data_protection =
{
    /* MPU Data Regions */
    .dpr[0].s = {(PxUInt_t) PxTricSystemRodataLowerBound,
                 (PxUInt_t) PxTricSystemRodataUpperBound
    },
    .dpr[1].s = {(PxUInt_t) __CSA_BEGIN_CPU0_,
                 (PxUInt_t) __CSA_END_CPU0_
    },
    .dpr[2].s = {(PxUInt_t) PxTricSystemDataLowerBound_CPU0_,
                 (PxUInt_t) PxTricSystemDataUpperBound_CPU0_
    },
    .dpr[3].s = {(PxUInt_t) PERIPHERAL_MEM_BASE,
                 (PxUInt_t) PERIPHERAL_MEM_END
    },
    .dpr[4].s = {(PxUInt_t) PXROS_SYSTEM_STACK_BEGIN_CPU0_,
                 (PxUInt_t) PXROS_SYSTEM_STACK_CPU0_
    },
    .dpr[5].s = {(PxUInt_t) 0, (PxUInt_t) 0 },
    .dpr[6].s = {(PxUInt_t) 0, (PxUInt_t) 0 },
    .dpr[7].s = {(PxUInt_t) 0, (PxUInt_t) 0 },

    /* MPU Data Permissions for the system... */
    .dpmr_system.dpre.bits = _SYSTEM_DATA_READ_PERMISSIONS,
    .dpmr_system.dpwe.bits = _SYSTEM_DATA_WRITE_PERMISSIONS,
    /* ... and kernel */
    .dpmr_kernel.dpre.bits = _KERNEL_DATA_READ_PERMISSIONS,
    .dpmr_kernel.dpwe.bits = _KERNEL_DATA_WRITE_PERMISSIONS

};

#endif /* #if (CORE0_ACTIVE == 1) */

/*
 *
 *                   ===============================
 *                  |||                           |||
 *                  ||      C O R E    [ 1 ]       ||
 *                  |||                           |||
 *                   ===============================
 *
 */
#if (CORE1_ACTIVE == 1)

/* MPU SETTINGS FOR SYSTEM CODE */
const PxCodeProtectSet_T _cpu1_sys_code_protection = _SYS_CODE_PROTECTION;

/* MPU SETTINGS FOR TASK CODE */
const PxCodeProtectSet_T _cpu1_task_code_protection = _TASK_CODE_PROTECTION;

/* MPU SETTINGS FOR SYSTEM AND KERNEL DATA */
const PxDataProtectSetInit_T _cpu1_sys_and_kernel_data_protection =
{
    /* MPU Data Regions */
    .dpr[0].s = {(PxUInt_t) PxTricSystemRodataLowerBound,
                 (PxUInt_t) PxTricSystemRodataUpperBound
    },
    .dpr[1].s = {(PxUInt_t) __CSA_BEGIN_CPU1_,
                 (PxUInt_t) __CSA_END_CPU1_
    },
    .dpr[2].s = {(PxUInt_t) PxTricSystemDataLowerBound_CPU1_,
                 (PxUInt_t) PxTricSystemDataUpperBound_CPU1_
    },
    .dpr[3].s = {(PxUInt_t) PERIPHERAL_MEM_BASE,
                 (PxUInt_t) PERIPHERAL_MEM_END
    },
    .dpr[4].s = {(PxUInt_t) PXROS_SYSTEM_STACK_BEGIN_CPU1_,
                 (PxUInt_t) PXROS_SYSTEM_STACK_CPU1_
    },
    .dpr[5].s = {(PxUInt_t) 0, (PxUInt_t) 0 },
    .dpr[6].s = {(PxUInt_t) 0, (PxUInt_t) 0 },
    .dpr[7].s = {(PxUInt_t) 0, (PxUInt_t) 0 },

    /* MPU Data Permissions for the system... */
    .dpmr_system.dpre.bits = _SYSTEM_DATA_READ_PERMISSIONS,
    .dpmr_system.dpwe.bits = _SYSTEM_DATA_WRITE_PERMISSIONS,
    /* ... and kernel */
    .dpmr_kernel.dpre.bits = _KERNEL_DATA_READ_PERMISSIONS,
    .dpmr_kernel.dpwe.bits = _KERNEL_DATA_WRITE_PERMISSIONS

};

#endif /* #if (CORE1_ACTIVE == 1) */

/*
 *
 *                   ===============================
 *                  |||                           |||
 *                  ||      C O R E    [ 2 ]       ||
 *                  |||                           |||
 *                   ===============================
 *
 */
#if (CORE2_ACTIVE == 1)

/* MPU SETTINGS FOR SYSTEM CODE */
const PxCodeProtectSet_T _cpu2_sys_code_protection = _SYS_CODE_PROTECTION;

/* MPU SETTINGS FOR TASK CODE */
const PxCodeProtectSet_T _cpu2_task_code_protection = _TASK_CODE_PROTECTION;

/* MPU SETTINGS FOR SYSTEM AND KERNEL DATA */
const PxDataProtectSetInit_T _cpu2_sys_and_kernel_data_protection =
{
    /* MPU Data Regions */
    .dpr[0].s = {(PxUInt_t) PxTricSystemRodataLowerBound,
                 (PxUInt_t) PxTricSystemRodataUpperBound
    },
    .dpr[1].s = {(PxUInt_t) __CSA_BEGIN_CPU2_,
                 (PxUInt_t) __CSA_END_CPU2_
    },
    .dpr[2].s = {(PxUInt_t) PxTricSystemDataLowerBound_CPU2_,
                 (PxUInt_t) PxTricSystemDataUpperBound_CPU2_
    },
    .dpr[3].s = {(PxUInt_t) PERIPHERAL_MEM_BASE,
                 (PxUInt_t) PERIPHERAL_MEM_END
    },
    .dpr[4].s = {(PxUInt_t) PXROS_SYSTEM_STACK_BEGIN_CPU2_,
                 (PxUInt_t) PXROS_SYSTEM_STACK_CPU2_
    },
    .dpr[5].s = {(PxUInt_t) 0, (PxUInt_t) 0 },
    .dpr[6].s = {(PxUInt_t) 0, (PxUInt_t) 0 },
    .dpr[7].s = {(PxUInt_t) 0, (PxUInt_t) 0 },

    /* MPU Data Permissions for the system... */
    .dpmr_system.dpre.bits = _SYSTEM_DATA_READ_PERMISSIONS,
    .dpmr_system.dpwe.bits = _SYSTEM_DATA_WRITE_PERMISSIONS,
    /* ... and kernel */
    .dpmr_kernel.dpre.bits = _KERNEL_DATA_READ_PERMISSIONS,
    .dpmr_kernel.dpwe.bits = _KERNEL_DATA_WRITE_PERMISSIONS

};

#endif /* #if (CORE2_ACTIVE == 1) */