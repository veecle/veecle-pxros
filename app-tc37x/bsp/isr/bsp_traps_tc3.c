/**************************************************************************************************
 * FILE: bsp_traps_tc3.c
 *
 * ARCHITECTURE:
 *
 *    TC3xx
 *
 * FAMILY:
 *
 *    TC3xx
 *
 * DESCRIPTION:
 *
 *    Early Core Trap table to be used after reset until the user change it.
 *
 * FILE HISTORY:
 *
 *    V1  05.2020  RO
 *    - Base reference example
 *
 *
 **************************************************************************************************
 *
 * SPDX-FileCopyrightText: HighTec EDV-Systeme GmbH
 *
 * SPDX-License-Identifier: Boost Software License - Version 1.0
 * 
 *************************************************************************************************/


/* ================================================================================================
 * IVOR TABLE & EXCEPTION HANDLERS
 *      Low level implementation of exception handling that the user does not need
 *      to care of at the beginning. Later he can implement his own exception
 *      handlers that will override default handlers.
 * ==============================================================================================*/

/* ------------------------------------------------------------------------------------------------
 * CPU TRAP Table
 *    Table for handling the Core exceptions. It has fixed  structure and vector alignment.
 *    TRAP_TABLe is by default positioned on 0xA000_0000 address pointed by
 *    BTV CPU register (its reset value)
 * Important:
 *    By default this table is SHARED among cores!!! It is fine up to the point
 *    it has only dummy "debug stop" implementation.
 *    The user or OS must change it by reprogramming BTV register to point
 *    to another table later on to get a meaningful implementation,
 *    individual for each core.
 * ----------------------------------------------------------------------------------------------*/

/* include common BSP header file */
#include "bsp.h"

/* Declaration of BSP Reset Trap Handlers */
void Trap_0_Handler( void );
void Trap_1_Handler( void );
void Trap_2_Handler( void );
void Trap_3_Handler( void );
void Trap_4_Handler( void );
void Trap_5_Handler( void );
void Trap_6_Handler( void );
void Trap_7_Handler( void );

/* Getting TRAP number supportive macro */
#define BSP_GET_TIN( tin ) \
{ \
  __asm__ ("mov \t%0,%%d15":"=d" (tin)); \
}

/* ================================================================================================
 * BSP Reset Trap Vector
 *     Default trap vector used by all cores until overridden by the application
 * ==============================================================================================*/

extern void BSP_TRAP_VECTOR_TABLE(void);
void bsp_uc_set_default_btv(void)
{
	unlock_wdtcon();
	__builtin_tricore_mtcr(CPU_BTV, (unsigned int)BSP_TRAP_VECTOR_TABLE);
	lock_wdtcon();
}


/* ================================================================================================
 * BSP Reset Trap Handlers
 *     In their specific code section for linker placement
 * ==============================================================================================*/

PRAGMA_SECTION_CODE(".bsp_trap_handlers",2, ax)

/* ------------------------------------------------------------------------------------------------
 * Trap class 0 handler.
 * ------------------------------------------------------------------------------------------------
 */
void Trap_0_Handler( void )
{
	uint32_t tin;

	// read TIN number stored in %d15 register to a variable
	BSP_GET_TIN(tin);

	// different reasons
	switch(tin)
	{
	// Virtual Address Fill Trap occurred
	case 0:
		break;

	// Virtual Address Protection Trap occurred
	case 1:
		break;

	default:
		break;
	}

	// stop in debug mode
	__asm("debug");

	// hard execution stop
	while (1);

	/* in case of return - restore lower context and return from exception
	__asm ("rslcx");
	__asm ("rfe");
	*/
}

/* ------------------------------------------------------------------------------------------------
 * Trap class 1 handler.
 * ------------------------------------------------------------------------------------------------
 */
void Trap_1_Handler(void)
{
	uint32_t tin;

	// read TIN number stored in %d15 register to a variable
	BSP_GET_TIN(tin);

	// different reasons
	switch(tin)
	{
	// Privilege Instruction Trap occurred
	case 1:
		break;

	// Memory Protection Read Trap occurred
	case 2:
		break;

	// Memory Protection Write Trap occurred
	case 3:
		break;

	// Memory Protection Execution Trap occurred
	case 4:
		break;

	// Memory Protection Peripheral Access Trap occurred
	case 5:
		break;

	// Memory Protection Null Address Trap occurred
	case 6:
		break;

	// Global Register Write Protection Trap occurred
	case 7:
		break;

	default:
		break;
	}

	// stop in debug mode
	__asm("debug");

	// hard execution stop
	while (1);

	/* in case of return - restore lower context and return from exception
	__asm ("rslcx");
	__asm ("rfe");
	*/
}

/* ------------------------------------------------------------------------------------------------
 * Trap class 2 handler.
 * ------------------------------------------------------------------------------------------------
 */
void Trap_2_Handler(void)
{
	uint32_t tin;

	// read TIN number stored in %d15 register to a variable
	BSP_GET_TIN(tin);

	// different reasons
	switch(tin)
	{
	// Illegal Opcode Trap occurred
	case 1:
		break;

	// Unimplemented Opcode Trap occurred
	case 2:
		break;

	// Invalid Operand Specification Trap occurred
	case 3:
		break;

	// Data Address Alignment Trap occurred
	case 4:
		break;

	// Invalid Local Memory Address Trap occurred
	case 5:
		break;

	default:
		break;
	}

	// stop in debug mode
	__asm("debug");

	// hard execution stop
	while (1);

	/* in case of return - restore lower context and return from exception
	__asm ("rslcx");
	__asm ("rfe");
	*/
}

/* ------------------------------------------------------------------------------------------------
 * Trap class 3 handler.
 * ------------------------------------------------------------------------------------------------
 */
void Trap_3_Handler(void)
{
	uint32_t tin;

	// read TIN number stored in %d15 register to a variable
	BSP_GET_TIN(tin);

	// different reasons
	switch(tin)
	{
	// Free context list depletion trap
	case 1:
		break;

	// Call depth overflow trap occurred
	case 2:
		break;

	// Call depth underflow trap occurred
	case 3:
		break;

	// Free context list underflow trap
	case 4:
		break;

	// Call stack underflow trap
	case 5:
		break;

	// Context type trap
	case 6:
		break;

	// Nesting error -> good to initiate reset
	case 7:
		break;

	default:
		break;
	}

	// stop in debug mode
	__asm("debug");

	// hard execution stop
	while (1);

	/* in case of return - restore lower context and return from exception
	__asm ("rslcx");
	__asm ("rfe");
	*/
}

/* ------------------------------------------------------------------------------------------------
 * Trap class 4 handler.
 * ------------------------------------------------------------------------------------------------
 */
void Trap_4_Handler(void)
{
	uint32_t tin;

	// read TIN number stored in %d15 register to a variable
	BSP_GET_TIN(tin);

	// different reasons
	switch(tin)
	{
	// Program Fetch Synchronous Error Trap occurred
	case 1:
		break;

	// Data Access Synchronous Error Trap occurred
	case 2:
		break;

	// Data Access Asynchronous Error Trap occurred
	case 3:
		break;

	default:
		break;
	}

	// stop in debug mode
	__asm("debug");

	// hard execution stop
	while (1);

	/* in case of return - restore lower context and return from exception
	__asm ("rslcx");
	__asm ("rfe");
	*/
}

/* ------------------------------------------------------------------------------------------------
 * Trap class 5 handler.
 * ------------------------------------------------------------------------------------------------
 */
void Trap_5_Handler(void)
{
	uint32_t tin;

	// read TIN number stored in %d15 register to a variable
	BSP_GET_TIN(tin);

	// TIN=1: Arithemetic Overflow Trap occurred
	// TIN=2: Sticky Arithemetic Overflow Trap occurred

	// stop in debug mode
	__asm("debug");

	// hard execution stop
	while (1);

	/* in case of return - restore lower context and return from exception
	__asm ("rslcx");
	__asm ("rfe");
	*/
}

/* ------------------------------------------------------------------------------------------------
 * Trap class 6 handler.
 * ------------------------------------------------------------------------------------------------
 */
void Trap_6_Handler(void)
{
	uint32_t tin;

	// read TIN number stored in %d15 register to a variable
	BSP_GET_TIN(tin);

	// System Call Trap occured

	// stop in debug mode
	__asm("debug");

	// hard execution stop
	while (1);

	/* in case of return - restore lower context and return from exception
	__asm ("rslcx");
	__asm ("rfe");
	*/
}

/* ------------------------------------------------------------------------------------------------
 * Trap class 7 handler.
 * ------------------------------------------------------------------------------------------------
 */
void Trap_7_Handler(void)
{
	uint32_t tin;

	// read TIN number stored in %d15 register to a variable
	BSP_GET_TIN(tin);

	// different reasons
	switch(tin)
	{
	// Non-Maskable Interrupt Trap occurred
	case 0:
		break;

	default:
		break;
	}

	// stop in debug mode
	__asm("debug");

	// hard execution stop
	while (1);

	/* in case of return - restore lower context and return from exception
	__asm ("rslcx");
	__asm ("rfe");
	*/
}

PRAGMA_SECTION_CODE_END  /* #pragma section ".bsp_trap_handlers" ax 2 */


/* ================================================================================================
 * BSP Trap Vectors
 *     In their specific code section (for linker placement)
 * ==============================================================================================*/

/* TRAP Vector table definition macro */
#define BSP_TRAP_VECTOR_TABLE() \
	__asm (".section .bsp_trap_vector_table , \"ax\", @progbits"); \
	__asm (".align 8"); \
	__asm (".global BSP_TRAP_VECTOR_TABLE"); \
	__asm ("BSP_TRAP_VECTOR_TABLE:")

/* TRAP Vector X */
#define BSP_TRAP_HANDLER(i, handler)  \
    __asm (".align 5"); \
    __asm ("debug"); \
	__asm ("svlcx"); \
    __asm ("movh.a %a14, hi:" #handler); \
    __asm ("lea    %a14, [%a14]lo:" #handler); \
    __asm ("ji %a14")

/* TRAP Vector Table section definition */
BSP_TRAP_VECTOR_TABLE();

/* TRAP Vector Table */
BSP_TRAP_HANDLER(0, Trap_0_Handler);
BSP_TRAP_HANDLER(1, Trap_1_Handler);
BSP_TRAP_HANDLER(2, Trap_2_Handler);
BSP_TRAP_HANDLER(3, Trap_3_Handler);
BSP_TRAP_HANDLER(4, Trap_4_Handler);
BSP_TRAP_HANDLER(5, Trap_5_Handler);
BSP_TRAP_HANDLER(6, Trap_6_Handler);
BSP_TRAP_HANDLER(7, Trap_7_Handler);



