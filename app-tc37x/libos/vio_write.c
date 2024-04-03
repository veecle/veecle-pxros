/*
 * SPDX-FileCopyrightText: HighTec EDV-Systeme GmbH
 *
 * SPDX-License-Identifier: Boost Software License - Version 1.0
 */

#include <stdio.h>
#include "vio-syscalls.h"

#if defined(VDK_SIM)
	#include "tspi.h"
#endif


int
write (int desc, void *buf, size_t len)
{
#if defined(VDK_SIM)
	return TspiFileWrite(desc, buf, len);
#else
	asm volatile ("	\n\
    mov %%d12,%0	\n\
    j ___virtio_hnd	\n\
  ": : "i" (SYS_WRITE) );
  return 0;
#endif
}
