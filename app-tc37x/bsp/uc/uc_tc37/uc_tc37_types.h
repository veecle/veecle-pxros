/******************************************************************************
 * FILE: uc_tc37_types.h
 *
 * UC ARCHITECTURE
 *
 *    TC3xx
 *
 * UC FAMILY
 *
 *    TC37x

 * DESCRIPTION
 *
 *    uC derivative specific types that user can use in code.
 *    A usage of these types helps to catch issues when porting between
 *    different uC derivatives.
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

#ifndef UC_TC37_TYPES_H
#define UC_TC37_TYPES_H


/* ----------------------------------------------------------------------------
 * CORE Identification
 * --------------------------------------------------------------------------*/

typedef enum{
	UC_CORE_MAIN_0 = 0,
	UC_CORE_MAIN_1 = 1,
	UC_CORE_MAIN_2 = 2,
} UC_CORE_ID_e;

/* ----------------------------------------------------------------------------
 * CCU CLOCK modules
 * --------------------------------------------------------------------------*/

/* CLOCK SOURCE selection */
typedef enum {
	UC_CCU_CON0_CLKSEL_BACKUP  = 0,
	UC_CCU_CON0_CLKSEL_PLL0    = 1,
	UC_CCU_CON0_CLKSEL_PLL1    = 2,
} UC_CCU_CON0_CLKSEL_e;

/* PLLs INPUT selection */
typedef enum {
	UC_CCU_CON1_INSEL_BACKUP   = 0,
	UC_CCU_CON1_INSEL_XOSC     = 1,
} UC_CCU_CON1_INSEL_e;

/* Fsystem clock selection */
typedef enum {
	UC_CCU_SYSTEM_CLOCK_BACKUP = 0,
	UC_CCU_SYSTEM_CLOCK_PLL0   = 1,
	UC_CCU_SYSTEM_CLOCK_XOSC   = 2,
} UC_CCU_SYSTEM_CLOCK_e;

/* ----------------------------------------------------------------------------
 * External CLOCK Output
 * --------------------------------------------------------------------------*/

/* Clock selection corresponds to EXTCLK0 */
typedef enum {
	UC_EXTCLK0_SEL_OUT      = 0,
	UC_EXTCLK0_SEL_PLL0     = 1,
	UC_EXTCLK0_SEL_PLL1		= 2,
	UC_EXTCLK0_SEL_OSC0     = 3,
	UC_EXTCLK0_SEL_BACK     = 4,
	UC_EXTCLK0_SEL_PLL2     = 5,
	UC_EXTCLK0_SEL_BBB      = 6,
	UC_EXTCLK0_SEL_SRI      = 8,
	UC_EXTCLK0_SEL_SPB      = 9,
	UC_EXTCLK0_SEL_FSI      = 10,
	UC_EXTCLK0_SEL_STM      = 11,
	UC_EXTCLK0_SEL_GTM      = 12,
	UC_EXTCLK0_SEL_TCK      = 13,
	UC_EXTCLK0_SEL_FSI2     = 14,
	UC_EXTCLK0_SEL_MT0      = 15,
} UC_EXTCLK0_SEL_e;


/* Clock selection corresponds to EXTCLK1 */
typedef enum {
	UC_EXTCLK1_SEL_OUT      = 0,
	UC_EXTCLK1_SEL_PLL0     = 1,
	UC_EXTCLK1_SEL_PLL1     = 2,
	UC_EXTCLK1_SEL_EBU      = 3,
	UC_EXTCLK1_SEL_BACK     = 4,
	UC_EXTCLK1_SEL_CAN      = 5,
	UC_EXTCLK1_SEL_ADC      = 6,
	UC_EXTCLK1_SEL_SPI      = 7,
	UC_EXTCLK1_SEL_SRI      = 8,
	UC_EXTCLK1_SEL_SPB      = 9,
	UC_EXTCLK1_SEL_I2C      = 10,
	UC_EXTCLK1_SEL_MSC      = 11,
	UC_EXTCLK1_SEL_ERAY     = 12,
	UC_EXTCLK1_SEL_LINF     = 13,
	UC_EXTCLK1_SEL_LINS     = 14,
	UC_EXTCLK1_SEL_OSC      = 15,
} UC_EXTCLK1_SEL_e;

/* ----------------------------------------------------------------------------
 * STM timer
 * --------------------------------------------------------------------------*/

/* STM CHANNELS interface type */
typedef enum {
	UC_STM_CHANNEL_0 = 0,
	UC_STM_CHANNEL_1 = 1,
	UC_STM_CHANNEL_2 = 2,
} UC_STM_CHANNEL_e;

/* ----------------------------------------------------------------------------
 * PORTs IO Interface
 * --------------------------------------------------------------------------*/

/* PORT PAD output edge rate control */
typedef enum {
	UC_PORT_PD_SPEED_1 = 0,
	UC_PORT_PD_SPEED_2 = 1,
	UC_PORT_PD_SPEED_3 = 2,
	UC_PORT_PD_SPEED_4 = 3,
} UC_PORT_PD_SPEED_e;

/* PORT PAD output level control */
typedef enum {
	UC_PORT_PL_LEVEL_AL  = 0,
	UC_PORT_PL_LEVEL_TTL = 1,
} UC_PORT_PL_LEVEL_e;

/* PORT PAD input mode control */
typedef enum {
	UC_PORT_INPUT_MODE_TRISTATE  = 0,
	UC_PORT_INPUT_MODE_PULL_DOWN = 1,
	UC_PORT_INPUT_MODE_PULL_UP   = 2,
} UC_PORT_INPUT_MODE_e;

/* PORT OUTPUT control */
typedef enum {
	UC_PORT_OUTPUT_NO_CHANGE      = 0,
	UC_PORT_OUTPUT_SET            = 1,
	UC_PORT_OUTPUT_CLEAR          = 2,
	UC_PORT_OUTPUT_TOGGLE         = 3,
} UC_PORT_OUTPUT_e;

#endif /* UC_TC37_TYPES_H */
#endif /* uC protection guard */
