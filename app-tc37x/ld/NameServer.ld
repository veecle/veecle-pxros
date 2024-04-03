/**************************************************************************************************
 * Project:  PXROS-HR BSP example
 *
 * Architecture:
 *    TriCore
 *
 * uC Derivative:
 *    all
 *
 * Description
 *    Linker file to define the memory space for the name server. 
 *    The default memory space is set to 4kB.
 *
 *    NOTE: This file was previously part of the PXROS-HR delivery.
 *
 * History
 *    v1   07.2020   JS
 *         - PXROS-HR BSP Base reference example
 *
 **************************************************************************************************
 *
 * SPDX-FileCopyrightText: HighTec EDV-Systeme GmbH
 *
 * SPDX-License-Identifier: Boost Software License - Version 1.0
 *
 *************************************************************************************************/

PROVIDE(__NAMESRV_SIZE = 4096);

SECTIONS
{
  .NameServer (NOLOAD) : 
  {
    . = ALIGN(8);
    PxNameServerSpaceBase = .;
    . += __NAMESRV_SIZE;
    . = ALIGN(8);
    PxNameServerSpaceEnd = .;
  } > CORE_SYM(BSS)
}
#define CLEAR_NAMESERVER LONG(0 + ADDR(.NameServer)); LONG(SIZEOF(.NameServer))
