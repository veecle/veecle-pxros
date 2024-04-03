/*
 * SPDX-FileCopyrightText: HighTec EDV-Systeme GmbH
 *
 * SPDX-License-Identifier: Boost Software License - Version 1.0
 */

#include<stddef.h>
#include<errno.h>
#include<stdio.h>

__attribute__ ((weak)) void abort ()
{
  asm volatile ("debug");
  __asm__ volatile ("mov.a %%a14, %0;j _exit":: "d" (0xdeadbeef));
  while (1) ;
};

