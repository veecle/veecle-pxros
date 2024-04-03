/**************************************************************************************************
 * FILE: bsp_board.c
 *
 * BOARD FAMILY:
 *
 *    TC3xx EVB Boards
 *
 * DESCRIPTION:
 *
 *    Implementation of common board functions.
 *
 *    Each function can be re-implemented in the specific board source files by using
 *    a dedicated function Macro.
 *
 *    BSP_BOARD_<FNC>_BOARD_SPECIFIC : existing definition disables common routine here
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

/* include common BSP header file */
#include "bsp.h"


/* ------------------------------------------------------------------------------------------------
 * FUNCTION: bsp_board_led_InitAll()
 *    The initialization of the port Pins connecting to LEDs on board.
 *    All LED pins are initialized to output mode with a required value.
 * Input
 *    set : requested output level
 * ----------------------------------------------------------------------------------------------*/

#ifndef BSP_BOARD_LED_INITALL_BOARD_SPECIFIC

void bsp_board_led_InitAll(BOARD_LED_SET_e set)
{
	int i;
	for (i=0; i < BOARD_NB_LEDS; i++)
		bsp_board_led_Init((BOARD_LED_e) i, set);
}

#endif


/* ------------------------------------------------------------------------------------------------
 * FUNCTION: bsp_board_InitLed()
 *    The initialization of the port Pin connected to the LED on board.
 *    The Pin is initialized to output mode with a required level.
 *
 *    The Pin strength is set by default to 'UC_PORT_PL_LEVEL_AL' and 'UC_PORT_PD_SPEED_3' that
 *    shall be supported by pins connected to the LED.
 *
 *    The Initialization sequence
 *    1. Set the output value before enabling the output
 *    2. Enable the pin output
 *
 * Input
 *    led : index of the LED to initialize
 *    set : output level
 * ----------------------------------------------------------------------------------------------*/

#ifndef BSP_BOARD_LED_INIT_BOARD_SPECIFIC

void bsp_board_led_Init(BOARD_LED_e led, BOARD_LED_SET_e set)
{
	if (led < BOARD_NB_LEDS) {
		bsp_uc_port_SetGPIO(board_led[led].port, board_led[led].index, (UC_PORT_OUTPUT_e) set);
		bsp_uc_port_EnableOutput(board_led[led].port, board_led[led].index, 0, UC_PORT_PL_LEVEL_AL, UC_PORT_PD_SPEED_3);
	}
}

#endif


/* ------------------------------------------------------------------------------------------------
 * FUNCTION: bsp_board_SetLed()
 *    Set output value using predefined modes
 * Input
 *    led : LED index
 *    set : output level
 * ----------------------------------------------------------------------------------------------*/

#ifndef BSP_BOARD_LED_SET_BOARD_SPECIFIC

void bsp_board_led_Set(BOARD_LED_e led, BOARD_LED_SET_e set)
{
	if (led < BOARD_NB_LEDS) {
		bsp_uc_port_SetGPIO(board_led[led].port, board_led[led].index, (UC_PORT_OUTPUT_e) set);
	}
}

#endif
