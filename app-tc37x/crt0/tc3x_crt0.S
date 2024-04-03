/**************************************************************************************************
 * FILE:
 *     tc3x_crt0.S
 *
 * ARCHITECTURE:
 *     TC3xx
 *
 * UC FAMILY:
 *     All
 *
 * DESCRIPTION:
 *     C/C++ runtime environment startup file.
 *
 *     [Main duty]
 *     - To initialize ABI registers
 *     - To initialize CSA context area
 *     - To initialize Stack area needed for 'C' code
 *     - To initialize ECC in RAM memories (not done for AURIX)
 *     - To clear BSS (not-initialized) data
 *     - To initialize DATA (initialized) data
 *     - To initialize C++ global object instances
 *     - To provide startup hooks for user defined code
 *
 *     [Multicore Startup]
 *     Crt0 startup on multicore systems is driven by a configuration provided
 *     by a corresponding linker file. Configuration table contains one entry
 *     for each Core present in uC
 *
 *     [Startup Hooks]
 *     Crt0 provides two startup hooks the user can use to implement system dependent
 *     early HW initialization.
 *     1. 'Crt0PreInit()' hook called
 *        - after STACK area and CSA list on given core is initialized
 *        - before Clear and Copy table operation is executed
 *        ! HW initialization code cannot rely on global data (BSS or DATA)
 *     2. 'Crt0PostInit()' hook called
 *        - when all startup initialization is finished (data can be used)
 *        - before calling shared_main() application entry point
 *
 *     [C++ support]
 *     The crt0 startup code provides a default global object initialization based on
 *     ctor table registered per core.
 *     The format of 'ctor' table expected:
 *     ctor_setion
 *     {
 *         __CTOR_LIST__ = .;
 *         LONG((__CTOR_END__ - __CTOR_LIST__) / 4 - 2)
 *         KEEP (*(.ctors));
 *         LONG(0);
 *         __CTOR_END__ = .;
 *     }
 *     By default, the linker gathers all C++ module ctor entries into one common ctor table.
 *     In case of multicore object initialization, the user must provide their system
 *     of CTOR initialization.
 *
 *     [Application specific crt0 Routines Implementation]
 *     Startup routines are implemented as WEAK functions to allow application-specific
 *     replacement in case of need.
 *     - Crt0BssInit
 *     - Crt0DataInit
 *     - Crt0CtorInit
 *     - Crt0PreInit
 *     - Crt0PostInit
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
 * SYMBOLS & DEFINES & MACROS
 * ==============================================================================================*/

/* crt0_config : Entry offsets
 * Must correspond to __crt0_config structure defined in the Linker file
 */
#define STACK               0x00
#define STACK_SIZE          0x04
#define CSA                 0x08
#define CSA_SIZE            0x0C
#define SDATA               0x10
#define SDATA2              0x14
#define SDATA3              0x18
#define SDATA4              0x1C
#define CLEAR_TABLE         0x20
#define COPY_TABLE          0x24
#define CTOR_TABLE          0x28
#define CPUINIT_SIZE        0x2C  /* size of the init structure */

/* CSA_ENTRY supportive symbols
 *     CSA_ENTRY_SIZE  : in bytes, given by TC3xx architecture
 *     LW_OFFSET_SHIFT : Link Word, low part shift offset
 */
#define CSA_ENTRY_SIZE      64
#define LW_OFFSET_SHIFT     -6

/* CLEAR_TABLE structure offsets
 * Offsets must correspond to the Linker __clear_table structure
 * Offsets are in bytes
 * Structure
 *     1. LONG : DST  - Destaination base address
 *     2. LONG : SIZE - number of data to clear (write 0) in bytes
 */
#define CLEAR_TABLE_DST     0x00
#define CLEAR_TABLE_SIZE    0x04
#define CLEAR_TABLE_OFFSET  0x08  /* Size of one entry in bytes */

/* COPY_TABLE structure offsets
 * Offsets must correspond to the Linker __copy_table structure
 * Offsets are in bytes
 * Structure
 *     0. LONG : SRC  - Source address to copy data from
 *     1. LONG : DST  - Destaination address to copy data to
 *     2. LONG : SIZE - number of data to copy in bytes
 */
#define COPY_TABLE_SRC      0x00
#define COPY_TABLE_DST      0x04
#define COPY_TABLE_SIZE     0x08
#define COPY_TABLE_OFFSET   0x0C  /* Size of one entry in bytes */


/* ================================================================================================
 * EXTERNAL SYMBOLS REQUIRED BY CRT0
 * ==============================================================================================*/

/* application shared main entry */
.extern shared_main, STT_FUNC, 0

/* crt0 config structure from linker */
.extern __crt0_config, STT_OBJECT, 0


/* ================================================================================================
 * EXPORTED SYMBOLS FROM CRT0
 * ==============================================================================================*/

/* _crt0_reset : reset entry point from where to start inactive cores */
.global _crt0_reset
.type _crt0_reset STT_FUNC

/* _start : shared multicore crt0 startup code entry point */
.global _start
.type _start STT_FUNC


/* ================================================================================================
 * CRT0 RESET VECTOR
 *
 * Here execution starts after the Reset.
 * The first duty is to force eventual address segment change in Aurix core
 * from non-cached memory to a cacheable one
 * ==============================================================================================*/

   .section .crt0_boot.code, "ax"

_crt0_reset:
    movh.a  %a15, hi:_start
    lea     %a15, [%a15] lo:_start
    ji      %a15


/* ================================================================================================
 * CRT0 STARTUP CODE
 *
 * A multicore shared code implementation of 'C' runtime intialization
 * located in a standard .text section that might be in cacheable region
 * ==============================================================================================*/

   .section .text, "ax"

_start:

/* ------------------------------------------------------------------------------------------------
 * CRT0 CONFIG TABLE BASE POINTER SETUP
 *
 * Config table contains parameters controlling crt0 startup execution.
 * It is prepared by the linker file with the knowledge of the final placement.
 *
 * Registers used by the crt0 startup code
 * 'A14` : is used as Core Entry Base Pointer in crt0 configuration structure
 *         throughout the statup asm code.
 *
 * The A14 register value is saved by Aurix core in upper context during subroutine calls.
 * ----------------------------------------------------------------------------------------------*/

    movh.a  %a14, hi:__crt0_config
    lea     %a14, [%a14]lo:__crt0_config
    mfcr    %d15, $core_id                /* read CoreID(0xfe1c) SFR register */
    and     %d15, 7                       /* mask the lowest 3 bits */
    mul     %d15, %d15, CPUINIT_SIZE      /* get the core entry base address */
    addsc.a %a14, %a14, %d15, 0


/* ------------------------------------------------------------------------------------------------
 * SMALL ADDRESS REGISTERS INIT
 *
 * Values given by crt0 configuration structure from the linker file.
 *
 * Four dedicated registers, if they are used
 * a0 - usually small data (rw)
 * a1 - usually small const data (r)
 * a8 - usually OS / application specific
 * a9 - usually OS / application specific
 * ----------------------------------------------------------------------------------------------*/

    ld.w    %d15, [%a14] SDATA
    mov.a   %a0, %d15
    ld.w    %d15, [%a14] SDATA2
    mov.a   %a1, %d15
    ld.w    %d15, [%a14] SDATA3
    mov.a   %a8, %d15
    ld.w    %d15, [%a14] SDATA4
    mov.a   %a9, %d15


/* ------------------------------------------------------------------------------------------------
 * CSA CONTEXT AREA INIT
 *
 * Linked list initialization of CSA entries (TriCore specific feature) used to save
 * function context during standard 'C' function calls.
 *
 * CSA entry and linked list has fixed structure given by AURIX architecture.
 * Number of CSA entries (max 256 entries) is part of crt0 configuration in the linker file.
 * ----------------------------------------------------------------------------------------------*/

    mov     %d4, 0
    mtcr    $pcxi, %d4                 /* clear pcxi SFR */
    ld.w    %d4, [%a14] CSA_SIZE       /* load number of CSA entries to initialize */
    sh      %d4, %d4, LW_OFFSET_SHIFT  /* get number of entries : SIZE / 64, where 64 = CSA entry size */
    mov.a   %a4, %d4                   /* a4 <= loop counter = number of CSA entries */
    add.a   %a4, -1                    /* decrement to take into account value 0 in loop */
    mov     %d4, CSA_ENTRY_SIZE
    mov.a   %a3, %d4                   /* a3 <= 64 (CSA Entry Size) */
    ld.a    %a15, [%a14] CSA           /* a15 <= CSA region base address (first entry in the list) */
    movh    %d5, 0x000F                /* prepare mask for segment in Link Word */
    mov.d   %d15, %a15                 /* store CSA Base address to Data register for next data operations */
    sh      %d15, %d15, -12            /* move Segment bits to right position in Link Word */
    and     %d15, %d5                  /* d15 <= masked segment bits of Link Word */
    mov.u   %d5, 0xFFFF                /* prepare mask for CSA offset in Link Word */
    mov.d   %d4, %a15                  /* store CSA base address to data register for next data operations */
    sh      %d4, %d4, LW_OFFSET_SHIFT  /* shift offset 6 bits to right to right Link Word offset position */
    and     %d4, %d5                   /* mask offset bits, zero the others */
    or      %d4, %d15                  /* d4 <= Link Word (Segment + Offset) */
    mtcr    $fcx, %d4                  /* store first CSA entry in FXC SFR register */
loop_csa:
    add     %d4, 1                     /* prepare address of next CSA entry */
    st.w    [%a15], %d4                /* store it in current link word to point to next CSA entry in list */
    add.a   %a15, %a3                  /* move address pointer to next CSA entry in the list */
    loop    %a4, loop_csa              /* repeat as many time as we have entries configured in the linker file */
    mtcr    $lcx, %d4                  /* store last link word content to LCX SFR register */


/* ------------------------------------------------------------------------------------------------
 * STACK INIT
 *
 * Mandatory operation before calling any 'C' function
 *
 * Two things to do
 * 1. correct ECC checksum syndroms for complete Stack area by writing with required
 *    data size instructions
 * 2. Stack pointer init used by 'C' code
 *
 * Startup code initialize both TriCore stack pointers, User and Interrupt, to the same area.
 * - the code runs with 'PSW.IS = 1' after the reset -> shared stack
 * - the separation of User and Interrupt stack is left on Application (usually OS)
 *   later on.
 * ----------------------------------------------------------------------------------------------*/

    ld.a    %a4, [%a14] STACK       /* Load Stack bottom address */
    ld.w    %d4, [%a14] STACK_SIZE  /* Load stack size */
    mov.d   %d1, %a4                /* Store start address into the data reg */
    add     %d1, %d1, %d4           /* End address of region is base stack add. */
    mov.a   %sp, %d1                /* Set stack register A[10] */
    mtcr    $isp, %d1               /* Set stack register ISP */
    call    clear_exec              /* Call Clear routine */


/* ------------------------------------------------------------------------------------------------
 * CRT0 PRE-INIT 'C' USER CODE
 *
 * Chance for user to execute HW init at the very beginning, before longer operations
 * take place, like memory clear and copy of init data from Flash to RAM.
 *
 * In case of CORE dependent Hook execution,
 * the application must read it ourselves (physical CoreID might not correspond
 * to a consecutive sequence needed for array operations).
 *
 * Pre-init code MUST rely only on Stack variables only !
 * ----------------------------------------------------------------------------------------------*/

    call    Crt0PreInit


/* ------------------------------------------------------------------------------------------------
 * CLEAR .BSS SECTIONS
 *
 * Areas to clear are given in the __clear_table config entry.
 * The crt0 function is of WEAK type to allow the user implementation in the application
 * by for example by 'C' specific routine
 * ----------------------------------------------------------------------------------------------*/

    ld.a    %a4, [%a14] CLEAR_TABLE
    call    Crt0BssInit


/* ------------------------------------------------------------------------------------------------
 * COPY INITIALIZED DATA
 *
 * Initialization of data regions provided in __copy table in crt0 configuration structure.
 * The crt0 function is of WEAK type to allow the user implementation in the application.
 * ----------------------------------------------------------------------------------------------*/

    ld.a    %a4, [%a14] COPY_TABLE
	call    Crt0DataInit


/* ------------------------------------------------------------------------------------------------
 * C++ GLOBAL OBJECT INITIALIZATION
 *
 * The ctor table (constructors to call) is provided as one of the crt0_configr structure entry.
 * Each core can have its own ctor table array, if implemented in the linker file
 * (not in BSP case)
 * ----------------------------------------------------------------------------------------------*/

    ld.a    %a4, [%a14] CTOR_TABLE
    call    Crt0CtorInit


/* ------------------------------------------------------------------------------------------------
 * CRT0 POST-INIT 'C' USER CODE
 *
 * Chance for user to execute specific code before jump to application entry,
 * 'shared main()' in case of BSP
 *
 * In case of core dependent Hook execution,
 * the application must read it ourselves (physical CoreID might not correspond
 * to a consecutive sequence needed for array operations).
 * ----------------------------------------------------------------------------------------------*/

    call    Crt0PostInit


 /* -----------------------------------------------------------------------------------------------
 * CRT0 END : ENTRY TO APPLICATION
 *
 * Jump to the application entry point, shared across all cores in case of BSP examples
 *
 * In case of core dependent Hook execution, the application must read it ourselves,
 * physical CoreID might not correspond to a consecutive sequence needed for array operations
 *
 * The return from the application is not expected, hard to say what the embedded system
 * shall do here
 * ----------------------------------------------------------------------------------------------*/

    call    shared_main

_exit:
    debug  /* debug stop in case of active debugging process,
              otherwise 'nop' instruction */
    j .    /* infinitive loop, waiting for eventual timeout watchdog */


/* ================================================================================================
 * CRT0 'DEFAULT' FUNCTIONS
 *
 * Implemented as WEAK to allow their replacement by the user versions in the application
 * ==============================================================================================*/


/* ------------------------------------------------------------------------------------------------
 * FUNCTION: Crt0PreInit
 *
 * User hook before 'C' runtime initialization. Empty routine in case of crt0 startup code.
 * ----------------------------------------------------------------------------------------------*/

    .weak Crt0PreInit
    .type Crt0PreInit, %function

Crt0PreInit:
    ret


/* ------------------------------------------------------------------------------------------------
 * FUNCTION: Crt0PostInit
 *
 * User hook after 'C' runtime initialization. Empty routine in case of crt0 startup code.
 * ----------------------------------------------------------------------------------------------*/

    .weak Crt0PostInit
   .type Crt0PostInit, %function

Crt0PostInit:
    ret


/* ------------------------------------------------------------------------------------------------
 * FUNCTION: Crt0BssInit
 *
 * Default Crt0 BSS clear function. It goes through clear_table entries and calls the clear
 * operation for each of them
 *
 * Input
 * A[4] : core's clear_table base pointer
 * ----------------------------------------------------------------------------------------------*/

    .weak Crt0BssInit
   .type Crt0BssInit, %function

Crt0BssInit:
    mov.aa  %a13, %a4                            /* Local pointer for clear_table entry */
    mov.a   %a12, CLEAR_TABLE_OFFSET             /* Clear_table next entry offset */
_table_bss_clear_loop:
    ld.w    %d15, [%a13] CLEAR_TABLE_DST         /* Base address of the area to clear */
    jeq     %d15, -1, _table_bss_clear_loop_end  /* Checks table termination value -1, */
    mov.a   %a4, %d15                            /* Prepare area start pointer for clear routine */
    ld.w    %d4, [%a13] CLEAR_TABLE_SIZE         /* Get size of the area*/
    call    clear_exec                           /* Call Clear routine with saving Upper Context */
    add.a   %a13,%a12                            /* Next row from BSS clear table */
    j       _table_bss_clear_loop
_table_bss_clear_loop_end:
    ret


/* ------------------------------------------------------------------------------------------------
 * FUNCTION: Crt0DataInit
 *
 * Default Crt0 DATA init function. It goes through copy_table entries and calls
 * copy operation for each of them.
 *
 * Input
 * A[4] : core's copy_table pointer
 * ----------------------------------------------------------------------------------------------*/

    .weak Crt0DataInit
   .type Crt0DataInit, %function

Crt0DataInit:
    mov.aa  %a13, %a4                            /* Local Pointer for copy table */
    mov.a   %a12, COPY_TABLE_OFFSET              /* Copy table item offset in bytes */
_table_data_copy_loop:
    ld.w    %d15, [%a13]COPY_TABLE_DST           /* Start address of the destination copy area */
    jeq     %d15, -1, _table_data_copy_loop_end  /* Checks table termination value -1, */
    mov.a   %a4, %d15
    ld.a    %a5, [%a13]COPY_TABLE_SRC            /* First Address of the source copy table */
    ld.w    %d4, [%a13]COPY_TABLE_SIZE           /* Get size of the area*/
    call    copy_exec                            /* Call Copy routine */
    add.a   %a13,%a12                            /* Next row from BSS copy table */
    j       _table_data_copy_loop
_table_data_copy_loop_end:
    ret


/* ------------------------------------------------------------------------------------------------
 * FUNCTION: Crt0CtorInit
 *
 * Default global C++ object initialization. It goes through ctor table and calls
 * global constructors.
 *
 * Input
 * A[4] : CTOR table base address
 * ----------------------------------------------------------------------------------------------*/

    .weak Crt0CtorInit
   .type Crt0CtorInit, %function

Crt0CtorInit:
    jz.a    %a4, _ctor_exec_end   /* check against no table present */
    ld.a    %a15, [%a4+]4         /* get number of entries */
    jz.a    %a15, _ctor_exec_end  /* check against no entry (size = 0) */
    add.a   %a15, -1              /* consider always one 'loop' execution */
_ctor_exec_loop:
	ld.a    %a13, [%a4+]4         /* read the function pointer */
    calli   %a13                  /* call the function */
    loop    %a15,_ctor_exec_loop  /* go through all functions */
_ctor_exec_end:
	ret


/* ================================================================================================
 * MODULE LOCAL ROUTINES
 *
 * Used only within this module
 * ==============================================================================================*/


/* ------------------------------------------------------------------------------------------------
 * FUNCTION: clear_exec
 *
 * Executes the erase loop from start address for specified number of bytes.
 * It uses 64bit Store instruction
 *
 * Input
 * A[4] : start address
 * D[4] : size in bytes
 * ----------------------------------------------------------------------------------------------*/

clear_exec:
    jz      %d4,_clear_exec_end    /* Return if size is zero */
    add     %d4,-1                 /* decrement to take into account always one loop execution */
    sh      %d4,-3                 /* adjustment of the clear loop for the double word write instruction */
    mov.a   %a15,%d4               /* init loop counter */
    mov     %e14,0                 /* Zero value */
_clear_exec_loop:
    st.d    [%a4+]8,%e14           /* Store 64bit value */
    loop    %a15,_clear_exec_loop  /* execution loop until zero */
_clear_exec_end:
    ret


/* ------------------------------------------------------------------------------------------------
 * FUNCTION: copy_exec
 *
 * Executes the copy loop from start address to end address.
 * Routine is simple Byte copy without any optimization.
 *
 * Input
 * A[4] : start write address
 * A[5] : start read address
 * D[4] : size in bytes
 * ----------------------------------------------------------------------------------------------*/

copy_exec:
    mov     %d15,%d4
    jz      %d15,_copy_exec_end  /* Return if size is zero */
    add     %d15,-1              /* decrement to take into account value 0 in loop */
    mov.a   %a15,%d15
_copy_exec_loop:
    ld.b    %d15, [%a5+]1
    st.b    [%a4+]1, %d15
    loop    %a15,_copy_exec_loop
_copy_exec_end:
    ret
