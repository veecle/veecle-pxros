/**************************************************************************************************
 * FILE: system_cfg.ld
 *
 * DESCRIPTION:
 *     Placement of PXMcTaskdefault memory class in memory.
 *     Written in multicore style as each core needs its memory class 
 *     in its local memory, but placement pattern remains the same.
 *
 * HISTORY:
 *    v1   06.2020   RO
 *         - PXROS-HR BSP Base reference example
 *
 **************************************************************************************************
 * SPDX-FileCopyrightText: HighTec EDV-Systeme GmbH
 *
 * SPDX-License-Identifier: Boost Software License - Version 1.0
 *************************************************************************************************/

SECTIONS
{
    /* PXMCTaskdefault - Default Task Memory Class */
    CORE_SEC(.taskmemory) :
    {
        *(CORE_SEC(.taskmemory))
    } > CORE_SYM(BSS)
#define CLEAR_TASKMEMORY(C) LONG(ADDR(.C..taskmemory)); LONG(SIZEOF(.C..taskmemory))
}
