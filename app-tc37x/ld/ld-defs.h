/**************************************************************************************************
 * Project:  PXROS-HR BSP example
 *
 * Architecture:
 *    TC4xx
 *
 * uC Derivative:
 *    TC49x
 *
 * Description
 *    Defines for preprocessing the linker script
 *
 * History
 *    v1   12.2021   HL
 *         - PXROS-HR BSP Base reference example
 *
 **************************************************************************************************
 *
 * SPDX-FileCopyrightText: HighTec EDV-Systeme GmbH
 *
 * SPDX-License-Identifier: Boost Software License - Version 1.0
 *
 *************************************************************************************************/

#if UC_NB_CORES > 1
#define CORE1_ACTIVE 1
#endif
#if UC_NB_CORES > 2
#define CORE2_ACTIVE 1
#endif

#define PREDEFINED(symbol , value) PROVIDE(symbol = value)

/*
 * CORE_ID must be defined before using the following macros
 * if the CORE_ID should not prefix the section names or suffix the symbol names,
 * the macros CORE_SEC(x) and CORE_SYM(x) must be undefined
 * For a later use of the CORE_ID the macros CORE_SEC(x) and CORE_SYM(x) must be
 * redefined
 */
/* prefix section name with "." CORE ID "." */
#define __CORE_SEC(x,c) .c.x
#define _CORE_SEC(x,c) __CORE_SEC(x,c)
#define CORE_SEC(x) _CORE_SEC(x,CORE_ID)

/*suffix symbol names with "_" CORE ID "_" */
#define __CORE_SYM(x,c) x ## _ ## c ## _
#define _CORE_SYM(x,c)  __CORE_SYM(x,c)
#define CORE_SYM(x) _CORE_SYM(x,CORE_ID)
