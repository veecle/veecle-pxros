/*
 * SPDX-FileCopyrightText: HighTec EDV-Systeme GmbH
 *
 * SPDX-License-Identifier: Boost Software License - Version 1.0
 */

#include "vio-syscalls.h"


int
open (char *path, int flags, int mode)
{
  asm volatile ("	\n\
    mov %%d12,%0	\n\
    j ___virtio_hnd	\n\
  ": : "i" (SYS_OPEN) );
  return 0;
}
