/*
 * SPDX-FileCopyrightText: HighTec EDV-Systeme GmbH
 *
 * SPDX-License-Identifier: Boost Software License - Version 1.0
 */

#include<stddef.h>
#include<errno.h>
#include<stdio.h>

#if defined(VDK_SIM)
	#include "tspi.h"
#endif



#define FREQ 100000000	/* clock rate of processor in Hz */

char *__progname;


void _exit (int status)
{

#if defined(VDK_SIM)

	TspiVpmStop(status);
	while(1);

#else
	  if (status)
	    __asm__ volatile ("mov.a %%a14, %0" :: "d" (status));
	  else
	    __asm__ volatile ("mov.a %%a14, %0" :: "d" (0x900d));

	  asm volatile ("debug");
	  while (1) ;
#endif
}


