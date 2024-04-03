/**************************************************************************************************
 * FILE: inittask.ld
 *
 * DESCRIPTION:
 *     Init Task stack memories placement. 
 *     Implemented in multicore style as Init Task is instantiated 
 *     on each active core. 
 *
 * HISTORY:
 *    v1   06.2020   RO
 *         - PXROS-HR BSP Base reference example
 *
 **************************************************************************************************
 * SPDX-FileCopyrightText: HighTec EDV-Systeme GmbH
 *
 * SPDX-License-Identifier: Boost Software License - Version 1.0
 ************************************************************************************************/

SECTIONS
{
    CORE_SEC(.inittask) :
    {
        . = ALIGN(16);
        CORE_SYM(INITTASK_STACK_BEGIN) = .;

        *(CORE_SEC(.inittask.stack))

        . = ALIGN(16);
        CORE_SYM(INITTASK_STACK_END) = .;

    } > CORE_SYM(BSS)
}
#define CLEAR_INITTASK(C) LONG(ADDR(.C..inittask)); LONG(SIZEOF(.C..inittask))
