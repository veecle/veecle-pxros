/**************************************************************************************************
 * FILE: bsp_uc.h
 *
 * UC ARCHITECTURE:
 *
 *     TC3xx
 *
 * UC FAMILY:
 *
 *     TC3xx
 *
 * DESCRIPTION:
 *
 *     Low Level Driver API interface for AURIX TC3xx uC family.
 *
 * FILE HISTORY:
 *
 *    V1  05.2020  RO
 *    - Base reference example
 *
 **************************************************************************************************
 *
 * SPDX-FileCopyrightText: HighTec EDV-Systeme GmbH
 *
 * SPDX-License-Identifier: Boost Software License - Version 1.0
 *
 *************************************************************************************************/

#ifndef BSP_UC_H
#define BSP_UC_H

/* include toolchain supportive header file */
//#include <machine/intrinsics.h>
//#include <machine/wdtcon.h>

/* Specific uC family Header file selection */
#define BSP_UC_SUPPORT    0

#ifdef __TC39XX__
#include "uc_tc39/uc_tc39.h"
#undef BSP_UC_SUPPORT
#endif

#ifdef __TC38XX__
#include "uc_tc38/uc_tc38.h"
#undef BSP_UC_SUPPORT
#endif

#ifdef __TC37XX__
#include "uc_tc37/uc_tc37.h"
#undef BSP_UC_SUPPORT
#endif

#ifdef __TC36XX__
#include "uc_tc36/uc_tc36.h"
#undef BSP_UC_SUPPORT
#endif

#ifdef __TC35XX__
#include "uc_tc35/uc_tc35.h"
#undef BSP_UC_SUPPORT
#endif

#ifdef __TC33XX__
#include "uc_tc33/uc_tc33.h"
#undef BSP_UC_SUPPORT
#endif

#ifdef __TC32XX__
#include "uc_tc32/uc_tc32.h"
#undef BSP_UC_SUPPORT
#endif

#ifdef BSP_UC_SUPPORT
#error... uC family is not supported in the toolchain
#endif

/* include BSP register definition access macros */
#include "bsp_uc_regdefs.h"

#endif /* BSP_UC_H */
