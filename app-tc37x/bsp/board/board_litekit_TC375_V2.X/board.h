/**************************************************************************************************
 * FILE: board.h
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
 *    Board support package specification and types.
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

#ifndef BOARD_H
#define BOARD_H


/* BSP Board low level specification header files */
#include "board_spec.h"
#include "board_types.h"

/* LED Board configuration array declaration */
extern const BOARD_LED_S board_led[BOARD_NB_LEDS];


#endif /* BOARD_H */
#endif /* LITEKIT_TC375_V2_X */
