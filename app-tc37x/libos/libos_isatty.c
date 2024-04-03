/*
 * SPDX-FileCopyrightText: HighTec EDV-Systeme GmbH
 *
 * SPDX-License-Identifier: Boost Software License - Version 1.0
 */

#include<stddef.h>
#include<stdlib.h>
#include<errno.h>
#include<stdio.h>

/*
 * implementation of not supported system calls
 */

int isatty (int desc)
{
	if (desc==0 || desc==1 || desc==2) return 1;
	return 0;
};

