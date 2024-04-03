/**************************************************************************************************
 * FILE: system_mpu_cfg.h
 *
 * DESCRIPTION:
 *     PXROS-HR MPU setting header file
 *
 * HISTORY:
 *    v1   07.2020   JS
 *         - PXROS-HR BSP Base reference example
 *
 **************************************************************************************************
 * SPDX-FileCopyrightText: HighTec EDV-Systeme GmbH
 *
 * SPDX-License-Identifier: Boost Software License - Version 1.0
 *************************************************************************************************/

#ifndef _SYSTEM_MPU_CFG_H_
#define _SYSTEM_MPU_CFG_H_

#include "system_cfg.h"

/* System protection set coming from system_mpu_cfg.c */

 #if (CORE0_ACTIVE == 1)
  extern const PxCodeProtectSet_T       _cpu0_sys_code_protection;
  extern const PxCodeProtectSet_T       _cpu0_task_code_protection;
  extern const PxDataProtectSetInit_T   _cpu0_sys_and_kernel_data_protection;
#endif

#if (CORE1_ACTIVE == 1)
 extern const PxCodeProtectSet_T        _cpu1_sys_code_protection;
 extern const PxCodeProtectSet_T        _cpu1_task_code_protection;
 extern const PxDataProtectSetInit_T    _cpu1_sys_and_kernel_data_protection;
#endif

#if (CORE2_ACTIVE == 1)
 extern const PxCodeProtectSet_T        _cpu2_sys_code_protection;
 extern const PxCodeProtectSet_T        _cpu2_task_code_protection;
 extern const PxDataProtectSetInit_T    _cpu2_sys_and_kernel_data_protection;
#endif

#endif /* _SYSTEM_MPU_CFG_H_ */
