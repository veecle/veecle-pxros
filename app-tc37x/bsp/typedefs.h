/**************************************************************************************************
 * FILE: typedefs.h
 *
 * DESCRIPTION:
 *
 *    Definition of necessary types extensions mainly required by uC register header files.
 *
 * HISTORY:
 *
 *    V1  05.2020  RO
 *    - Base reference example
 *
 **************************************************************************************************
 *
 * SPDX-FileCopyrightText: HighTec EDV-Systeme GmbH
 *
 * SPDX-License-Identifier: Boost Software License - Version 1.0
 *
 *************************************************************************************************/

#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include <stdint.h>

/* volatile types */

typedef volatile signed char vint8_t;
typedef volatile unsigned char vuint8_t;

typedef volatile signed short vint16_t;
typedef volatile unsigned short vuint16_t;

typedef volatile signed int vint32_t;
typedef volatile unsigned int vuint32_t;

#endif
