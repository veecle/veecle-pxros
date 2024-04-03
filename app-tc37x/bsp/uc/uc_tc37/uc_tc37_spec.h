/******************************************************************************
 * FILE: uc_tc37_spec.h
 *
 * UC ARCHITECTURE
 *
 *    TC3xx
 *
 * UC FAMILY
 *
 *    TC37x
 *
 * DESCRIPTION
 *
 *    Configuration and description of uC derivative.
 *    Parameters are taken from uC Datasheet and Reference manual.
 *
 * FILE HISTORY
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

#ifndef UC_TC37_SPEC_H
#define UC_TC37_SPEC_H

/* uC derivative specific types */
#include "uc_tc37_types.h"


/* ============================================================================
 * CORES SPECIFICATION
 * ==========================================================================*/

/* Number of Cores instances */
#ifndef UC_NB_CORES
#define UC_NB_CORES                 3
#endif
/* RESET CORE: core that starts after Power On */
#define UC_RESET_CORE               0


/* ============================================================================
 * DEFAULT uC CLOCK SETTING
 *
 * Target is MAX system clock for Core frequency following recommended rules.
 * User ** should not ** change it unless he is certain what he is doing
 * ==========================================================================*/

/* System PLL */
#define UC_PLL0_CLOCK             300 /* MAX=300; SYS PLL output frequency */

/* Peripheral PLL */
#define UC_PLL1_CLOCK             320 /* MAX=320; PER PLL1 output frequency */
#define UC_PLL2_CLOCK             200 /* MAX=200; PER PLL2 output frequency */

/* Internal oscillator value [MHz] */
#define UC_BACKUP_CLOCK           100

/* Platform clocks */
#define UC_SRI_CLOCK              300 /* MAX=300; source = fPLL0 | fBACK */
#define UC_SPB_CLOCK              100 /* MAX=100; source = fPLL0 | fBACK */
#define UC_FSI_CLOCK              100 /* MAX=100; source = fSRI; */
#define UC_FSI2_CLOCK             300 /* MAX=300; source = fSRI; */

/* Peripheral clocks needed in BSP example */
#define UC_QSPI_CLOCK             200 /* MAX=200; source = fPLL1 | fPLL2 | fBACK */
#define UC_STM_CLOCK              100 /* MAX=100; source = fPLL0 | fBACK */

/* BSP supportive time macros */
#define UC_NB_TICKS_1US           (UC_STM_CLOCK)
#define UC_NB_TICKS_1MS           (UC_NB_TICKS_1US * 1000)


/* ============================================================================
 * PFLASH SPECIFICATION
 * ==========================================================================*/

/* PFLASH selection of minimum Wait states for given SYSTEM_CLOCK
 * DS Chapter 3.16 - Flash Memory */
#define UC_PFLASH_PF              30  /* [ns] Program Flash access delay */
#define UC_PFLASH_PFECC           10  /* [ns] Program Flash ECC delay */
#define UC_DFLASH_DF              100 /* [ns] Data Flash access delay */
#define UC_DFLASH_DFECC           20  /* [ns] Data Flash ECC Delay */


/* ============================================================================
 * PLL0 SPECIFICATION
 * ==========================================================================*/

/* PLL parameters */
#define UC_PLL0_INCLK_MIN         16
#define UC_PLL0_INCLK_MAX         40
#define UC_PLL0_VCO_MIN           400
#define UC_PLL0_VCO_MAX           800

/* Typical Clock Divider recommendations */
#define UC_PLL0_VCO_REC           600


/* ============================================================================
 * PLL1 SPECIFICATION
 * ==========================================================================*/

/* PLL parameters */
#define UC_PLL1_INCLK_MIN         10
#define UC_PLL1_INCLK_MAX         40
#define UC_PLL1_VCO_MIN           400
#define UC_PLL1_VCO_MAX           800

/* Typical Clock Divider recommendations */
#define UC_PLL1_VCO_REC           640


/* ============================================================================
 * MODULE BASE ADDRESSES IN MEMORY MAP
 *
 * use of semiconductor peripheral file to get peripheral modules' base
 * addresses for those modules that will access a module using their own
 * macros instead of peripheral header bit fields
 * ==========================================================================*/

#define UC_PORT_BASE                   (uint32_t) &MODULE_P00


/* ============================================================================
 * PLL SUBSYSTEM CALCULATION
 * ==========================================================================*/

/* ----------------------------------------------------------------------------
 * PLL1 CALCULATION
 * --------------------------------------------------------------------------*/

#if (UC_MODULE_PLL1)

#endif /* UC_MODULE_PLL1 */

/* ----------------------------------------------------------------------------
 * SYSTEM PLL0 CALCULATION
 * --------------------------------------------------------------------------*/

/* XTAL divider setting to fulfill PLL0 input clock */
#if (BOARD_XTAL_CLOCK > UC_PLL0_INCLK_MAX)
#define PLL0_PDIV                 2
#else
#define PLL0_PDIV                 1
#endif

/* PLL0 input clock */
#define UC_PLL0_INCLK             (BOARD_XTAL_CLOCK / PLL0_PDIV)

/* PLL0 MIN/MAX values to make following macros more readable */
#define PLL0_VCO_MIN              (UC_PLL0_VCO_MIN - 1)
#define PLL0_VCO_MAX              (UC_PLL0_VCO_MAX + 1)

/* PLL0_VCO Parameters
 * using Recommended value
 */
#define PLL0_K2                   (UC_PLL0_VCO_REC / UC_PLL0_CLOCK)
#define PLL0_NDIV_A               (UC_PLL0_VCO_REC / UC_PLL0_INCLK)
#define PLL0_NDIV_A_NOT_VALID     ((PLL0_NDIV_A * UC_PLL_INCLK) - UC_PLL_VCO_REC)
#define PLL0_K2_NOT_VALID         ((PLL0_K2 * UC_PLL0_CLOCK) - UC_PLL0_VCO_REC)
#if ( (UC_PLL0_VCO_REC > PLL0_VCO_MIN) \
		&&  (UC_PLL0_VCO_REC < PLL0_VCO_MAX) \
		&& !(PLL0_NDIV_A_NOT_VALID) \
		&& !(PLL0_K2_NOT_VALID) )
#define PLL0_NDIV                 (PLL0_NDIV_A)

#else /* UC_PLL0_VCO_REC not valid, we do not try other VCO combination */
#error Required PLL0 cannot be achieved with predefined PLL0_VCO_REC and board XTAL
#endif /* UC_PLL0_VCO_REC */

/* Values for PLL registers */
#define UC_PLL0_PDIV              (PLL0_PDIV - 1)
#define UC_PLL0_NDIV              (PLL0_NDIV - 1)
#define UC_PLL0_K2                (PLL0_K2 - 1)

/* ----------------------------------------------------------------------------
 * PERIPHERAL PLL1 CALCULATION
 * --------------------------------------------------------------------------*/

/* XTAL divider setting to fulfill PLL1 input clock */
#if (BOARD_XTAL_CLOCK > UC_PLL1_INCLK_MAX)
#define PLL1_PDIV                 2
#else
#define PLL1_PDIV                 1
#endif

#define UC_PLL1_INCLK             (BOARD_XTAL_CLOCK / PLL1_PDIV)

#define PLL1_VCO_MIN              (UC_PLL1_VCO_MIN - 1)
#define PLL1_VCO_MAX              (UC_PLL1_VCO_MAX + 1)

/* PLL1_VCO_REC Recommended: */
#define PLL1_K2                   (UC_PLL1_VCO_REC / UC_PLL1_CLOCK)
#define PLL1_K3                   (UC_PLL1_VCO_REC / 1.6 / UC_PLL2_CLOCK)
#define PLL1_NDIV_A               (UC_PLL1_VCO_REC / UC_PLL1_INCLK)
#define PLL1_NDIV_A_NOT_VALID     ((PLL1_NDIV_A * UC_PLL1_INCLK) - UC_PLL1_VCO_REC)
#define PLL1_K2_NOT_VALID         ((PLL1_K2 * UC_PLL1_CLOCK) - UC_PLL1_VCO_REC)

/* check valid parameters for PPL1 */
#if ( (UC_PLL1_VCO_REC > PLL1_VCO_MIN) \
		&&  (UC_PLL1_VCO_REC < PLL1_VCO_MAX) \
		&& !(PLL1_NDIV_A_NOT_VALID) \
		&& !(PLL1_K2_NOT_VALID) )
#define PLL1_NDIV                 PLL1_NDIV_A

#else /* UC_PLL1_VCO_REC not valid, we do not try other VCO combination */
#error Required PLL1 cannot be achieved with predefined PLL1_VCO and board XTAL
#endif /* PLL_VCO_A */

/* Values for PLL registers */
#define UC_PLL1_PDIV              (PLL1_PDIV - 1)
#define UC_PLL1_NDIV              (PLL1_NDIV - 1)
#define UC_PLL1_DIBBY             0 /* to divide PLL1 VCO (640MHz / 1.6) => for K3 divider */
#define UC_PLL1_K2                (PLL1_K2 - 1)
#define UC_PLL1_K3                (PLL1_K3 - 1)


/* ================================================================================================
 * FLASH REGISTER VALUES
 * ==============================================================================================*/

/* PFLASH */
#define UC_FCON_WSPFLASH          (((UC_PFLASH_PF    * UC_SRI_CLOCK) / 1000) - 1)
#define UC_FCON_WSECPF            (((UC_PFLASH_PFECC * UC_SRI_CLOCK) / 1000) - 1)

/* DFLASH */
#define UC_FCON_WSDFLASH          (((UC_DFLASH_DF    * UC_SRI_CLOCK) / 1000) - 1)
#define UC_FCON_WSECDF            (((UC_DFLASH_DFECC * UC_SRI_CLOCK) / 1000) - 1)


/* ================================================================================================
 *  CCUCONx VALUES FOR BSP SELECTED CLOCKS
 * ==============================================================================================*/

#define UC_CCU_SRI_DIV            (UC_PLL0_CLOCK  / UC_SRI_CLOCK)
#if ((UC_CCU_SRI_DIV * UC_SRI_CLOCK) != UC_PLL0_CLOCK)
#error Wrong SRI clock setting, not a whole number divider
#endif

#define UC_CCU_SPB_DIV            (UC_PLL0_CLOCK / UC_SPB_CLOCK)
#if ((UC_CCU_SPB_DIV * UC_SPB_CLOCK) != UC_PLL0_CLOCK)
#error Wrong SPB clock setting, not a whole number divider
#endif

#define UC_CCU_FSI2_DIV           (UC_SRI_CLOCK / UC_FSI2_CLOCK)
#if ((UC_CCU_FSI2_DIV * UC_FSI2_CLOCK) != UC_SRI_CLOCK)
#error Wrong FSI2 clock setting, not a whole number divider
#endif

#define UC_CCU_FSI_DIV            (UC_SRI_CLOCK / UC_FSI_CLOCK)
#if ((UC_CCU_FSI_DIV * UC_FSI_CLOCK) != UC_SRI_CLOCK)
#error Wrong FSI2 clock setting, not a whole number divider
#endif

#define UC_CCU_STM_DIV            (UC_PLL0_CLOCK  / UC_STM_CLOCK)
#if ((UC_CCU_STM_DIV * UC_STM_CLOCK) != UC_PLL0_CLOCK)
#error Wrong STM clock setting, not a whole number divider
#endif

#define UC_CCU_QSPI_DIV           (UC_PLL2_CLOCK / UC_QSPI_CLOCK)
#define UC_CCU_QSPI_CLKSEL        2 /* fPLL2 use as clock source for fSOURCEQSPI */
#if ((UC_CCU_QSPI_DIV * UC_QSPI_CLOCK) != UC_PLL2_CLOCK)
#error Wrong QSPI clock setting, not a whole number divider
#endif

#endif /* UC_TC37_SPEC_H */
#endif /* uC derivative protection guard */
