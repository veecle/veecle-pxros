/**************************************************************************************************
 * FILE: board_types.h
 *
 * BOARD FAMILY:
 *
 *    LiteKit TC375 V1.X
 *
 * UC SUPPORT:
 *
 *    TC375, TC365  (LQFP-176 Package)
 *
 * DESCRIPTION:
 *
 *    Board specific types.
 *
 * HISTORY:
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

/* board implementation guard */
#if defined(LITEKIT_TC375_V2_X)

#ifndef BOARD_TYPES_H
#define BOARD_TYPES_H


/* PORT LEDs */
typedef enum {
	BOARD_LED_0 = 0,
	BOARD_LED_1,
	BOARD_NB_LEDS
} BOARD_LED_e;

/* PORT OUTPUT control */
typedef enum {
	BOARD_LED_SET_NO_CHANGE = 0,
	BOARD_LED_SET_OFF       = 1,
	BOARD_LED_SET_ON        = 2,
	BOARD_LED_SET_TOGGLE    = 3,
} BOARD_LED_SET_e;


#endif /* BOARD_TYPES_H */
#endif /* LITEKIT_TC375_V2_X */
