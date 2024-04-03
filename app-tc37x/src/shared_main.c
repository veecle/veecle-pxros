/**************************************************************************************************
 * FILE: shared_main.c
 *
 * DESCRIPTION:
 *     Shared multicore main routines, plus crt0 (startup) hooks.
 *
 * FILE HISTORY:
 *     V1.0   11.2018   PN
 *     V2.0   04.2019   RO
 *            - Adding PXROS-HR system
 *     V3.0   11.2019   RO
 *            - Alignment to update of BSP package (v5.0)
 *     V4.0   04.2020   RO
 *            - Shared_main modified to be Number of Cores independent
 *            - It allows to have one base for PXROS BSP examples
 *
 **************************************************************************************************
 * SPDX-FileCopyrightText: HighTec EDV-Systeme GmbH
 *
 * SPDX-License-Identifier: Boost Software License - Version 1.0
 *************************************************************************************************/


#include "bsp.h"
#include "pxdef.h"
#include "pxros/config/system_cfg.h"


/* ================================================================================================
 * MODULE PUBLIC INTERFACE
 * ==============================================================================================*/


/* ------------------------------------------------------------------------------------------------
 * FUNCTION: shared_main()
 *     shared_main() entry point is called by crt0 startup code after crt0 finishes
 *     'C' runtime environment initialization.
 *     Here, it is where the PXROS-HR starts by calling PxInit().
 *     Each core under PXROS-HR control must call PxInit to initialize the kernel properly
 *     according to the specification given by InitSpecArray.
 *     PXROS_MASTER_CORE = RESET_CORE by default
 *     the MASTER_CORE will start other cores within PxInit() function.
 * NOTES:
 *     To get a HW platform in an expected state, the initialization is done
 *     within Crt0PostInit hook called before entering shared_main from crt0.
 * ------------------------------------------------------------------------------------------------
 */
void shared_main (void)
{
    PxError_t error;
	uint32_t no_of_cores = PXROS_CORE_COUNT;

    /* Start the PXROS kernel instance on current core */
	if (run_on_tsim())
		no_of_cores = 1;
    error = PxInit(InitSpecsArray, no_of_cores);
	if (error != PXERR_NOERROR)
		PxPanic();

	/* we should never come back to here */
	for(;;);
}


/* ------------------------------------------------------------------------------------------------
 * FUNCTION: Crt0PreInit()
 *     Crt0 Hook before memory clear & copy operation take place.
 * ------------------------------------------------------------------------------------------------
 */
void Crt0PreInit (void)
{
	uint32_t coreId = bsp_uc_core_GetCurrentCore();

	/* Only RESET Core activity */
	if (coreId == UC_RESET_CORE)
	{
		bsp_uc_scu_wdt_DisableSafetyWatchdog();
	}

	/* Each core activity */
	bsp_uc_scu_wdt_DisableCpuWatchdog();
	bsp_uc_core_EnableICache();

	/* set default trap table */
	bsp_uc_set_default_btv();
}


/* ------------------------------------------------------------------------------------------------
 * FUNCTION: Crt0PostInit()
 *     Crt0 Hook after complete crt0 initialization, just before shared_main() entry point
 *     is called.
 * ------------------------------------------------------------------------------------------------
 */
void Crt0PostInit (void)
{
	uint32_t coreId = bsp_uc_core_GetCurrentCore();

	/* Initialization of shared resources by RESET core */

	if (coreId == UC_RESET_CORE)
	{
		test_run_on_tsim();
		if (!run_on_tsim()) {
			bsp_uc_InitClock();
			bsp_board_wdg_Disable();
			bsp_board_led_InitAll(BOARD_LED_SET_OFF);
		}
	}
}
