/*  wdtcon.c -- Lock/unlock the ENDINIT bit in WDTCON to gain write access
 *              for ENDINIT-protected SFRs (such as BIV, BTV and the CLC
 *               registers of the various core functional blocks).
 *
 * SPDX-FileCopyrightText: HighTec EDV-Systeme GmbH
 *
 * SPDX-License-Identifier: Boost Software License - Version 1.0
 */

// #include <tricoreintrin.h>
#include "wdtcon.h"

typedef struct
{
    unsigned int _con0;
} WdtCon_t;


/* on the AURIX (TC27XX) and AURIX 2G (TC39XX) each core has its own WDTCON address
   therefore we need special read/write-function to the wdtcon0/1
   registers
   */
#define CORE_ID_ADDR            0xFE1C
#define CORE_ID_MASK            0x00000007
#if defined(__TC161__)
#define WDTCON_BASE_ADDR        0xF0036100
#define WDTCON_CPU_STRIDE       0x0C
/* safety watchdog */
#define WDTS_BASE_ADDR          0xF00360F0
#elif defined(__TC162__)
#define WDTCON_BASE_ADDR        0xF003624C
#define WDTCON_CPU_STRIDE       0x0C
/* safety watchdog */
#define WDTS_BASE_ADDR          0xF00362A8
#endif /* __TC161__ */

static inline WdtCon_t * __getWdtBase(void)
{
    unsigned int coreId;

    /* get the core ID from CPU_ID (csfr_base + 0xfe1c) */
    coreId = __builtin_tricore_mfcr(CORE_ID_ADDR) & CORE_ID_MASK;

#if defined(__TC162__)
    /* special mapping for CPU5: CORE_ID == 6 */
    if (coreId == 6)
    {
        coreId = 5;
    }
#endif /* __TC162__ */

    return (WdtCon_t *)(WDTCON_BASE_ADDR + (coreId * WDTCON_CPU_STRIDE));
}

static inline WdtCon_t * __getWdtsBase(void)
{
    return (WdtCon_t *)(WDTS_BASE_ADDR);
}


/* Unlock (reset) the ENDINIT bit.  */
void unlock_wdtcon(void)
{
    unsigned int passwd;
    volatile WdtCon_t *wdtaddr;

    wdtaddr = __getWdtBase();

    passwd  = wdtaddr->_con0;
    passwd &= 0xffffff00;
    wdtaddr->_con0 = passwd | 0xf1;
    __builtin_tricore_dsync();
    wdtaddr->_con0 = passwd | 0xf2;
    /* read back new value ==> synchronise LFI */
    passwd = wdtaddr->_con0;
}

/* Lock (set) the ENDINIT bit.  */
void lock_wdtcon(void)
{
    unsigned int passwd;
    volatile WdtCon_t *wdtaddr;

    wdtaddr = __getWdtBase();

    passwd  = wdtaddr->_con0;
    passwd &= 0xffffff00;
    wdtaddr->_con0 = passwd | 0xf1;
    __builtin_tricore_dsync();
    passwd |= 3;
    wdtaddr->_con0 = passwd | 0xf2;
    /* read back new value ==> synchronise LFI */
    passwd = wdtaddr->_con0;
}

/* Unlock (reset) the SAFETY ENDINIT bit.  */
void unlock_safety_wdtcon(void)
{
    unsigned int passwd;
    volatile WdtCon_t *wdtaddr;

    wdtaddr = __getWdtsBase();

    passwd  = wdtaddr->_con0;
    passwd &= 0xffffff00;
    wdtaddr->_con0 = passwd | 0xf1;
    __builtin_tricore_dsync();
    wdtaddr->_con0 = passwd | 0xf2;
    /* read back new value ==> synchronise LFI */
    passwd = wdtaddr->_con0;
}

/* Lock (set) the SAFETY ENDINIT bit.  */
void lock_safety_wdtcon(void)
{
    unsigned int passwd;
    volatile WdtCon_t *wdtaddr;

    wdtaddr = __getWdtsBase();

    passwd  = wdtaddr->_con0;
    passwd &= 0xffffff00;
    wdtaddr->_con0 = passwd | 0xf1;
    __builtin_tricore_dsync();
    passwd |= 3;
    wdtaddr->_con0 = passwd | 0xf2;
    /* read back new value ==> synchronise LFI */
    passwd = wdtaddr->_con0;
}