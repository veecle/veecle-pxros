/*
 * SPDX-FileCopyrightText: HighTec EDV-Systeme GmbH
 *
 * SPDX-License-Identifier: Boost Software License - Version 1.0
 */

#include<stddef.h>
#include<stdlib.h>
#include "vio-syscalls.h"

int rename(const char *opath,const char *npath)
{
  asm volatile ("	\n\
    mov %%d12,%0	\n\
    j ___virtio_hnd	\n\
  ": : "i" (SYS_RENAME) );
  return 0;
}
