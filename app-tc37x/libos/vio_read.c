/*
 * SPDX-FileCopyrightText: HighTec EDV-Systeme GmbH
 *
 * SPDX-License-Identifier: Boost Software License - Version 1.0
 */

#include <stdio.h>
#include "vio-syscalls.h"


int
read (int desc, void *buf, size_t len)
{
  asm volatile ("	\n\
    mov %%d12,%0	\n\
    j ___virtio_hnd	\n\
  ": : "i" (SYS_READ) );
  return 0;
}
