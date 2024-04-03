/*
 * SPDX-FileCopyrightText: HighTec EDV-Systeme GmbH
 *
 * SPDX-License-Identifier: Boost Software License - Version 1.0
 */

#include<stddef.h>
#include<stdlib.h>

extern int rmdir (const char *name);
extern int unlink (const char *name);
int remove(const char *path)
{
  return  (rmdir(path) < 0)?  unlink (path): 0;
}
