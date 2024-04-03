/*
 * SPDX-FileCopyrightText: HighTec EDV-Systeme GmbH
 *
 * SPDX-License-Identifier: Boost Software License - Version 1.0
 */

#if !defined(__clang__)
#include<stddef.h>
#include<stdlib.h>
#include<errno.h>
#include<stdio.h>
#include<stat.h>

/*
 * implementation of not supported system calls
 */

int stat (const char *file, struct stat *buf)
{
	abort();
};


int fstat (int fd, struct stat *buf)
{
	if (fd < 0 || fd > 2)
	{
		errno = EBADF;
		return -1;
	}
	buf->st_mode = S_IFCHR;
	return 0;
};

#endif