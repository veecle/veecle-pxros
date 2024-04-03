/*
 * SPDX-FileCopyrightText: HighTec EDV-Systeme GmbH
 *
 * SPDX-License-Identifier: Boost Software License - Version 1.0
 */

#include<stddef.h>
#include<stdlib.h>

int system(const char *cmd)
{
  return (cmd == NULL)? 0: -1;
}
