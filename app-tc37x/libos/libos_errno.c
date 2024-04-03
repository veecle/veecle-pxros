/*
 * SPDX-FileCopyrightText: HighTec EDV-Systeme GmbH
 *
 * SPDX-License-Identifier: Boost Software License - Version 1.0
 */

#include<stddef.h>
#include<stdlib.h>
#include<errno.h>
#include<stdio.h>

int __attribute__ ((weak)) _Errno;
__attribute__ ((weak)) int *
__errno ()
{
    return &_Errno;
}

/* define an empty environment */
__attribute__ ((weak)) char *__my_environ[2] = {"",0};
__attribute__ ((weak)) char **environ = __my_environ;

