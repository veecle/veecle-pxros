/**************************************************************************************************
 * FILE: tc37xA_memory.ld
 *
 * UC ARCHITECTURE:
 *
 *    TC3xx
 *
 * UC FAMILY:
 *
 *    TC37xA
 *
 * DESCRIPTION:
 *
 *    uC family memory region definition file.
 *    Not all available memory regions are defined, only those used in BSP example.
 *    It includes the application linker files locating sections to memory regions at the end.
 *
 * FILE HISTORY:
 *
 *    V1  05.2020  RO
 *    - Base reference example
 *
 *    v2  08.2020  JS
 *    - update to PXROS-HR v8.2.0
 *
 **************************************************************************************************
 *
 * SPDX-FileCopyrightText: HighTec EDV-Systeme GmbH
 *
 * SPDX-License-Identifier: Boost Software License - Version 1.0
 *
 *************************************************************************************************/
 
OUTPUT_FORMAT("elf32-tricore")
OUTPUT_ARCH(tricore)
ENTRY(_crt0_reset)

/* ================================================================================================
 * TC37XA MEMORY REGIONS
 * ==============================================================================================*/

MEMORY
{
  /* User configuration block - BMHD headers only */
  ucb_bmhd_orig (rx):  org = 0xaf400000, len = 2K
  ucb_bmhd_copy (rx):  org = 0xaf401000, len = 2K

  /* Program Flash memory - cached region */
  int_flash0 (rx):     org = 0x80000000, len = 3M
  int_flash1 (rx):     org = 0x80300000, len = 3M

  /* Program scratchpad memories */
  pspr_cpu0 (rx):      org = 0x70100000, len = 64K
  pspr_cpu1 (rx):      org = 0x60100000, len = 64K
  pspr_cpu2 (rx):      org = 0x50100000, len = 64K

  /* Data scratchpad memories */
  dspr_cpu0 (w!x):     org = 0x70000000, len = 240K
  dspr_cpu1 (w!x):     org = 0x60000000, len = 240K
  dspr_cpu2 (w!x):     org = 0x50000000, len = 96K

  /* Distributed LMU RAM - Non-Cached regions selected
   * Local core access is always non-cached */
  dlmu_cpu0 (wx):      org = 0xB0000000, len = 64K
  dlmu_cpu1 (wx):      org = 0xB0010000, len = 64K
  dlmu_cpu2 (wx):      org = 0xB0020000, len = 64K

  /* Periphery memory space region */
  periphery_base :     org = 0xF0000000, len = 0
  periphery_end  :     org = 0xFFFF0000, len = 0


  /* PXROS-HR image memory */
  pxros_mem : org = 0, len = 16K
}

/* ================================================================================================
 * MEMORY REGION SYMBOLS
 * ==============================================================================================*/

/* Internal Flash memory */
INT_FLASH_MEMORY_BASE = ORIGIN(int_flash0);
INT_FLASH_MEMORY_SIZE = 6M;

/* ================================================================================================
 * INCLUDE OF APPLICATION LINKER FILE
 * ==============================================================================================*/
 
INCLUDE tc37x_pxros_bsp_example.ld
