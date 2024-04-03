/******************************************************************************
 * FILE: uc_tc37.h
 *
 * ARCHITECTURE
 *
 *    TC3xx
 *
 * FAMILY
 *
 *    TC37x
 *
 * DESCRIPTION
 *
 *    Include of target uC header files used by BSP
 *
 * HISTORY
 *
 *    V1  05.2020  RO
 *    - Base reference example
 *
 ******************************************************************************
 *
 * SPDX-FileCopyrightText: HighTec EDV-Systeme GmbH
 *
 * SPDX-License-Identifier: Boost Software License - Version 1.0
 *
 *****************************************************************************/

/* uC derivative protection guard */
#ifdef __TC37XX__

#ifndef UC_TC37_H
#define UC_TC37_H

/* Check supported uC Steps, if not defined in the project setting by the user
 * the most recent uC step is selected
 */
#if !defined(TC37XA)
#define TC37XA
#endif

/* Import of uC Specific header files */
#include "uc_tc37_spec.h"

/* DECLARATION OF FAMILY SPECIFIC API IMPLEMENTATIONS
 *
 * Definition of specific MACRO disables function in generic uc_bsp.c and replaces it
 * with uc_<derivative> implementation. This allows to use different header files etc.
 * that is not possible in a WEAK function system.
 *
 * MACRO:  #define <uc_fce>_UC_SPECIFIC
 */
#if defined (TC37XA)

#endif

/* Include of peripheral register headers files
 */
#if defined (TC37XA)

#include "TC37xPD_RegCHeaders/_Reg/Ifx_reg.h"

#include "TC37xPD_RegCHeaders/_Reg/IfxAsclin_bf.h"
#include "TC37xPD_RegCHeaders/_Reg/IfxCan_bf.h"
#include "TC37xPD_RegCHeaders/_Reg/IfxCbs_bf.h"
#include "TC37xPD_RegCHeaders/_Reg/IfxCcu6_bf.h"
#include "TC37xPD_RegCHeaders/_Reg/IfxConverter_bf.h"
#include "TC37xPD_RegCHeaders/_Reg/IfxCpu_bf.h"
#include "TC37xPD_RegCHeaders/_Reg/IfxDam_bf.h"
#include "TC37xPD_RegCHeaders/_Reg/IfxDma_bf.h"
#include "TC37xPD_RegCHeaders/_Reg/IfxDmu_bf.h"
#include "TC37xPD_RegCHeaders/_Reg/IfxDom_bf.h"
#include "TC37xPD_RegCHeaders/_Reg/IfxEdsadc_bf.h"
#include "TC37xPD_RegCHeaders/_Reg/IfxEray_bf.h"
#include "TC37xPD_RegCHeaders/_Reg/IfxEvadc_bf.h"
#include "TC37xPD_RegCHeaders/_Reg/IfxFce_bf.h"
#include "TC37xPD_RegCHeaders/_Reg/IfxFsi_bf.h"
#include "TC37xPD_RegCHeaders/_Reg/IfxGeth_bf.h"
#include "TC37xPD_RegCHeaders/_Reg/IfxGpt12_bf.h"
#include "TC37xPD_RegCHeaders/_Reg/IfxGtm_bf.h"
#include "TC37xPD_RegCHeaders/_Reg/IfxHsct_bf.h"
#include "TC37xPD_RegCHeaders/_Reg/IfxHssl_bf.h"
#include "TC37xPD_RegCHeaders/_Reg/IfxI2c_bf.h"
#include "TC37xPD_RegCHeaders/_Reg/IfxInt_bf.h"
#include "TC37xPD_RegCHeaders/_Reg/IfxIom_bf.h"
#include "TC37xPD_RegCHeaders/_Reg/IfxMinimcds_bf.h"
#include "TC37xPD_RegCHeaders/_Reg/IfxMsc_bf.h"
#include "TC37xPD_RegCHeaders/_Reg/IfxMtu_bf.h"
#include "TC37xPD_RegCHeaders/_Reg/IfxPfi_bf.h"
#include "TC37xPD_RegCHeaders/_Reg/IfxPms_bf.h"
#include "TC37xPD_RegCHeaders/_Reg/IfxPmu_bf.h"
#include "TC37xPD_RegCHeaders/_Reg/IfxPort_bf.h"
#include "TC37xPD_RegCHeaders/_Reg/IfxPsi5_bf.h"
#include "TC37xPD_RegCHeaders/_Reg/IfxPsi5s_bf.h"
#include "TC37xPD_RegCHeaders/_Reg/IfxQspi_bf.h"
#include "TC37xPD_RegCHeaders/_Reg/IfxSbcu_bf.h"
#include "TC37xPD_RegCHeaders/_Reg/IfxScu_bf.h"
#include "TC37xPD_RegCHeaders/_Reg/IfxSent_bf.h"
#include "TC37xPD_RegCHeaders/_Reg/IfxSmu_bf.h"
#include "TC37xPD_RegCHeaders/_Reg/IfxSrc_bf.h"
#include "TC37xPD_RegCHeaders/_Reg/IfxStm_bf.h"

#endif

#endif /* UC_TC37_H */
#endif /* #ifdef __TC37XX__ */
