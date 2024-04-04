/**
 * \file IfxMtu_cfg.c
 * \brief Mtu on-chip implementation data
 *
 * \version iLLD_1_0_1_16_0
 * \copyright Copyright (c) 2019 Infineon Technologies AG. All rights reserved.
 *
 *
 *                                 IMPORTANT NOTICE
 *
 * Use of this file is subject to the terms of use agreed between (i) you or
 * the company in which ordinary course of business you are acting and (ii)
 * Infineon Technologies AG or its licensees. If and as long as no such terms
 * of use are agreed, use of this file is subject to following:
 *
 * Boost Software License - Version 1.0 - August 17th, 2003
 *
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer, must
 * be included in all copies of the Software, in whole or in part, and all
 * derivative works of the Software, unless such copies or derivative works are
 * solely in the form of machine-executable object code generated by a source
 * language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */

/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/

#include "IfxMtu_cfg.h"
#include "Mtu/Std/IfxMtu.h"

/******************************************************************************/
/*-----------------------Exported Variables/Constants-------------------------*/
/******************************************************************************/

const IfxMtu_SramItem           IfxMtu_sramTable[IFXMTU_NUM_MBIST_TABLE_ITEMS] = {
    {2 * 4, 16,  6,  0, 1}, /**< \brief IfxMtu_MbistSel_cpu0Dspr0 */
    {2 * 1, 19,  10, 0, 4}, /**< \brief IfxMtu_MbistSel_cpu0Dtag */
    {2 * 2, 64,  8,  0, 1}, /**< \brief IfxMtu_MbistSel_cpu0Pspr */
    {2 * 1, 19,  10, 0, 4}, /**< \brief IfxMtu_MbistSel_cpu0Ptag */
    {2 * 1, 64,  8,  0, 1}, /**< \brief IfxMtu_MbistSel_cpu0Dlmu */
    {2 * 4, 16,  6,  0, 1}, /**< \brief IfxMtu_MbistSel_cpu1Dspr0 */
    {2 * 1, 19,  10, 0, 4}, /**< \brief IfxMtu_MbistSel_cpu1Dtag */
    {2 * 2, 64,  8,  0, 1}, /**< \brief IfxMtu_MbistSel_cpu1Pspr */
    {2 * 1, 19,  10, 0, 4}, /**< \brief IfxMtu_MbistSel_cpu1Ptag */
    {2 * 1, 64,  8,  0, 1}, /**< \brief IfxMtu_MbistSel_cpu1Dlmu */
    {2 * 4, 16,  6,  0, 1}, /**< \brief IfxMtu_MbistSel_cpu2Dspr */
    {2 * 1, 19,  10, 0, 4}, /**< \brief IfxMtu_MbistSel_cpu2Dtag */
    {2 * 2, 64,  8,  0, 1}, /**< \brief IfxMtu_MbistSel_cpu2Pspr */
    {2 * 1, 19,  10, 0, 4}, /**< \brief IfxMtu_MbistSel_cpu2Ptag */
    {2 * 1, 64,  8,  0, 1}, /**< \brief IfxMtu_MbistSel_cpu2Dlmu */
    {0 * 0, 0,   0,  0, 0}, /**< \brief IfxMtu_MbistSel_none */
    {0 * 0, 0,   0,  0, 0}, /**< \brief IfxMtu_MbistSel_none */
    {0 * 0, 0,   0,  0, 0}, /**< \brief IfxMtu_MbistSel_none */
    {0 * 0, 0,   0,  0, 0}, /**< \brief IfxMtu_MbistSel_none */
    {0 * 0, 0,   0,  0, 0}, /**< \brief IfxMtu_MbistSel_none */
    {0 * 0, 0,   0,  0, 0}, /**< \brief IfxMtu_MbistSel_none */
    {0 * 0, 0,   0,  0, 0}, /**< \brief IfxMtu_MbistSel_none */
    {0 * 0, 0,   0,  0, 0}, /**< \brief IfxMtu_MbistSel_none */
    {0 * 0, 0,   0,  0, 0}, /**< \brief IfxMtu_MbistSel_none */
    {0 * 0, 0,   0,  0, 0}, /**< \brief IfxMtu_MbistSel_none */
    {0 * 0, 0,   0,  0, 0}, /**< \brief IfxMtu_MbistSel_none */
    {0 * 0, 0,   0,  0, 0}, /**< \brief IfxMtu_MbistSel_none */
    {0 * 0, 0,   0,  0, 0}, /**< \brief IfxMtu_MbistSel_none */
    {0 * 0, 0,   0,  0, 0}, /**< \brief IfxMtu_MbistSel_none */
    {0 * 0, 0,   0,  0, 0}, /**< \brief IfxMtu_MbistSel_none */
    {0 * 0, 0,   0,  0, 0}, /**< \brief IfxMtu_MbistSel_none */
    {0 * 0, 0,   0,  0, 0}, /**< \brief IfxMtu_MbistSel_none */
    {0 * 0, 0,   0,  0, 0}, /**< \brief IfxMtu_MbistSel_none */
    {0 * 0, 0,   0,  0, 0}, /**< \brief IfxMtu_MbistSel_none */
    {2 * 4, 16,  6,  0, 1}, /**< \brief IfxMtu_MbistSel_cpu0Dspr1 */
    {2 * 4, 16,  6,  0, 1}, /**< \brief IfxMtu_MbistSel_cpu1Dspr1 */
    {0 * 0, 0,   0,  0, 0}, /**< \brief IfxMtu_MbistSel_none */
    {0 * 0, 0,   0,  0, 0}, /**< \brief IfxMtu_MbistSel_none */
    {1 * 2, 64,  8,  0, 1}, /**< \brief IfxMtu_MbistSel_dam0 */
    {0 * 0, 0,   0,  0, 0}, /**< \brief IfxMtu_MbistSel_none */
    {0 * 0, 0,   0,  0, 0}, /**< \brief IfxMtu_MbistSel_none */
    {1 * 4, 64,  8,  0, 1}, /**< \brief IfxMtu_MbistSel_dma */
    {1 * 2, 128, 9,  0, 1}, /**< \brief IfxMtu_MbistSel_miniMcds */
    {0 * 0, 0,   0,  0, 0}, /**< \brief IfxMtu_MbistSel_none */
    {0 * 0, 0,   0,  0, 0}, /**< \brief IfxMtu_MbistSel_none */
    {0 * 0, 0,   0,  0, 0}, /**< \brief IfxMtu_MbistSel_none */
    {0 * 0, 0,   0,  0, 0}, /**< \brief IfxMtu_MbistSel_none */
    {0 * 0, 0,   0,  0, 0}, /**< \brief IfxMtu_MbistSel_none */
    {0 * 0, 0,   0,  0, 0}, /**< \brief IfxMtu_MbistSel_none */
    {0 * 0, 0,   0,  0, 0}, /**< \brief IfxMtu_MbistSel_none */
    {0 * 0, 0,   0,  0, 0}, /**< \brief IfxMtu_MbistSel_none */
    {0 * 0, 0,   0,  0, 0}, /**< \brief IfxMtu_MbistSel_none */
    {0 * 0, 0,   0,  0, 0}, /**< \brief IfxMtu_MbistSel_none */
    {1 * 3, 29,  7,  0, 1}, /**< \brief IfxMtu_MbistSel_gtmFifo */
    {0 * 0, 0,   0,  0, 0}, /**< \brief IfxMtu_MbistSel_none */
    {1 * 5, 32,  7,  0, 1}, /**< \brief IfxMtu_MbistSel_gtmMcs0 */
    {0 * 0, 0,   0,  0, 0}, /**< \brief IfxMtu_MbistSel_none */
    {1 * 5, 32,  7,  0, 1}, /**< \brief IfxMtu_MbistSel_gtmMcs01 */
    {1 * 1, 24,  6,  0, 1}, /**< \brief IfxMtu_MbistSel_gtmDpll1a */
    {1 * 1, 24,  6,  0, 1}, /**< \brief IfxMtu_MbistSel_gtmDpll1b */
    {1 * 1, 24,  6,  0, 1}, /**< \brief IfxMtu_MbistSel_gtmDpll2 */
    {0 * 0, 0,   0,  0, 0}, /**< \brief IfxMtu_MbistSel_none */
    {1 * 1, 32,  7,  0, 1}, /**< \brief IfxMtu_MbistSel_mcan0 */
    {1 * 1, 32,  7,  0, 1}, /**< \brief IfxMtu_MbistSel_mcan1 */
    {0 * 0, 0,   0,  0, 0}, /**< \brief IfxMtu_MbistSel_none */
    {1 * 1, 32,  7,  0, 1}, /**< \brief IfxMtu_MbistSel_psi5 */
    {1 * 2, 32,  7,  0, 1}, /**< \brief IfxMtu_MbistSel_eray0Obf */
    {0 * 0, 0,   0,  0, 0}, /**< \brief IfxMtu_MbistSel_none */
    {1 * 4, 32,  7,  0, 1}, /**< \brief IfxMtu_MbistSel_eray0IbfTbf */
    {0 * 0, 0,   0,  0, 0}, /**< \brief IfxMtu_MbistSel_none */
    {1 * 1, 32,  7,  0, 1}, /**< \brief IfxMtu_MbistSel_eray0Mbf */
    {0 * 0, 0,   0,  0, 0}, /**< \brief IfxMtu_MbistSel_none */
    {0 * 0, 0,   0,  0, 0}, /**< \brief IfxMtu_MbistSel_none */
    {0 * 0, 0,   0,  0, 0}, /**< \brief IfxMtu_MbistSel_none */
    {0 * 0, 0,   0,  0, 0}, /**< \brief IfxMtu_MbistSel_none */
    {0 * 0, 0,   0,  0, 0}, /**< \brief IfxMtu_MbistSel_none */
    {0 * 0, 0,   0,  0, 0}, /**< \brief IfxMtu_MbistSel_none */
    {2 * 2, 8,   5,  0, 1}, /**< \brief IfxMtu_MbistSel_scrXram */
    {1 * 1, 8,   5,  0, 1}, /**< \brief IfxMtu_MbistSel_scrIram */
    {0 * 0, 0,   0,  0, 0}, /**< \brief IfxMtu_MbistSel_none */
    {0 * 0, 0,   0,  0, 0}, /**< \brief IfxMtu_MbistSel_none */
    {0 * 0, 0,   0,  0, 0}, /**< \brief IfxMtu_MbistSel_none */
    {1 * 2, 35,  7,  0, 1}, /**< \brief IfxMtu_MbistSel_ethermacRx */
    {1 * 2, 35,  7,  0, 1}, /**< \brief IfxMtu_MbistSel_ethermacTx */
};

const IfxMtu_MbistConfigPattern nonDestructiveTestPattern[] = {
    {
        IFXMTU_MBIST_CONFIG_NONDISTTEST_CONFIG0,
        IFXMTU_MBIST_CONFIG_NONDISTTEST_CONFIG1
    }
};

const IfxMtu_MbistSshConfig     mbistSshConfigGang0[] = {
    {
        IfxMtu_MbistSel_cpu0Dlmu,                              /* sshSel          */
        0x00,                                                    /* lowAddress      */
        0x3F,                                                    /* highAddress     */
        5,                                                       /* uncorrectableErrorThreshold*/
        0                                                        /* preClearEnable  */
    },
    {
        IfxMtu_MbistSel_cpu0Dspr1,                              /* sshSel          */
        0x00,                                                    /* lowAddress      */
        0x7F,                                                    /* highAddress     */
        5,                                                       /* uncorrectableErrorThreshold*/
        0                                                        /* preClearEnable  */
    },
    {
        IfxMtu_MbistSel_gtmDpll2,                              /* sshSel          */
        0x00,                                                    /* lowAddress      */
        0x3F,                                                    /* highAddress     */
        5,                                                       /* uncorrectableErrorThreshold*/
        1                                                        /* preClearEnable  */
    },
    {
        IfxMtu_MbistSel_mcan0,                              /* sshSel          */
        0x00,                                                    /* lowAddress      */
        0x7F,                                                    /* highAddress     */
        5,                                                       /* uncorrectableErrorThreshold*/
        1                                                        /* preClearEnable  */
    },
    {
        IfxMtu_MbistSel_mcan1,                              /* sshSel          */
        0x00,                                                    /* lowAddress      */
        0x3F,                                                    /* highAddress     */
        5,                                                       /* uncorrectableErrorThreshold*/
        1                                                        /* preClearEnable  */
    },
    {
        IfxMtu_MbistSel_eray0Mbf,                              /* sshSel          */
        0x00,                                                    /* lowAddress      */
        0x1F,                                                    /* highAddress     */
        5,                                                       /* uncorrectableErrorThreshold*/
        1                                                        /* preClearEnable  */
    },
    {
        IfxMtu_MbistSel_scrXram,                              /* sshSel          */
        0x00,                                                    /* lowAddress      */
        0x00,                                                    /* highAddress     */
        5,                                                       /* uncorrectableErrorThreshold*/
        1                                                        /* preClearEnable  */
    },
};

const IfxMtu_MbistSshConfig     mbistSshConfigGang1[] = {
    {
        IfxMtu_MbistSel_cpu0Dspr,                              /* sshSel          */
        0x00,                                                    /* lowAddress      */
        0x7F,                                                    /* highAddress     */
        5,                                                       /* uncorrectableErrorThreshold*/
        0                                                        /* preClearEnable  */
    },
    {
        IfxMtu_MbistSel_cpu1Dspr1,                              /* sshSel          */
        0x00,                                                    /* lowAddress      */
        0x7F,                                                    /* highAddress     */
        5,                                                       /* uncorrectableErrorThreshold*/
        0                                                        /* preClearEnable  */
    },
    {
        IfxMtu_MbistSel_gtmFifo,                              /* sshSel          */
        0x00,                                                    /* lowAddress      */
        0xF,                                                    /* highAddress     */
        5,                                                       /* uncorrectableErrorThreshold*/
        1                                                        /* preClearEnable  */
    },
    {
        IfxMtu_MbistSel_scrIram,                              /* sshSel          */
        0x00,                                                    /* lowAddress      */
        0x04,                                                    /* highAddress     */
        5,                                                       /* uncorrectableErrorThreshold*/
        1                                                        /* preClearEnable  */
    },
};

const IfxMtu_MbistSshConfig     mbistSshConfigGang2[] = {
    {
        IfxMtu_MbistSel_cpu1Dspr,                              /* sshSel          */
        0x00,                                                    /* lowAddress      */
        0x7F,                                                    /* highAddress     */
        5,                                                       /* uncorrectableErrorThreshold*/
        0                                                        /* preClearEnable  */
    },
    {
        IfxMtu_MbistSel_cpu2Dspr,                              /* sshSel          */
        0x00,                                                    /* lowAddress      */
        0x6F,                                                    /* highAddress     */
        5,                                                       /* uncorrectableErrorThreshold*/
        0                                                        /* preClearEnable  */
    },
    {
        IfxMtu_MbistSel_gtmDpll1b,                              /* sshSel          */
        0x00,                                                    /* lowAddress      */
        0x5,                                                    /* highAddress     */
        5,                                                       /* uncorrectableErrorThreshold*/
        1                                                        /* preClearEnable  */
    },
    {
        IfxMtu_MbistSel_psi5,                              /* sshSel          */
        0x00,                                                    /* lowAddress      */
        0x03,                                                    /* highAddress     */
        5,                                                       /* uncorrectableErrorThreshold*/
        1                                                        /* preClearEnable  */
    },
};

const IfxMtu_MbistSshConfig     mbistSshConfigGang3[] = {
    {
        IfxMtu_MbistSel_cpu0Pspr,                              /* sshSel          */
        0x00,                                                    /* lowAddress      */
        0x2F,                                                    /* highAddress     */
        5,                                                       /* uncorrectableErrorThreshold*/
        0                                                        /* preClearEnable  */
    },
    {
        IfxMtu_MbistSel_cpu0Ptag,                              /* sshSel          */
        0x00,                                                    /* lowAddress      */
        0x7,                                                    /* highAddress     */
        5,                                                       /* uncorrectableErrorThreshold*/
        0                                                        /* preClearEnable  */
    },
    {
        IfxMtu_MbistSel_cpu1Dlmu,                              /* sshSel          */
        0x00,                                                    /* lowAddress      */
        0x3F,                                                    /* highAddress     */
        5,                                                       /* uncorrectableErrorThreshold*/
        0                                                        /* preClearEnable  */
    },
    {
        IfxMtu_MbistSel_cpu2Dlmu,                              /* sshSel          */
        0x00,                                                    /* lowAddress      */
        0x3F,                                                    /* highAddress     */
        5,                                                       /* uncorrectableErrorThreshold*/
        0                                                        /* preClearEnable  */
    },
    {
        IfxMtu_MbistSel_gtmMcs0Fast,                              /* sshSel          */
        0x00,                                                    /* lowAddress      */
        0x1F,                                                    /* highAddress     */
        5,                                                       /* uncorrectableErrorThreshold*/
        1                                                        /* preClearEnable  */
    },
};

const IfxMtu_MbistSshConfig     mbistSshConfigGang4[] = {
    {
        IfxMtu_MbistSel_cpu1Pspr,                              /* sshSel          */
        0x00,                                                    /* lowAddress      */
        0x2F,                                                    /* highAddress     */
        5,                                                       /* uncorrectableErrorThreshold*/
        0                                                        /* preClearEnable  */
    },
    {
        IfxMtu_MbistSel_cpu1Ptag,                              /* sshSel          */
        0x00,                                                    /* lowAddress      */
        0x7,                                                    /* highAddress     */
        5,                                                       /* uncorrectableErrorThreshold*/
        1                                                        /* preClearEnable  */
    },
    {
        IfxMtu_MbistSel_cpu2Pspr,                              /* sshSel          */
        0x00,                                                    /* lowAddress      */
        0x2F,                                                    /* highAddress     */
        5,                                                       /* uncorrectableErrorThreshold*/
        0                                                        /* preClearEnable  */
    },
    {
        IfxMtu_MbistSel_cpu2Ptag,                              /* sshSel          */
        0x00,                                                    /* lowAddress      */
        0x7,                                                    /* highAddress     */
        5,                                                       /* uncorrectableErrorThreshold*/
        1                                                        /* preClearEnable  */
    },
    {
        IfxMtu_MbistSel_dma,                              /* sshSel          */
        0x00,                                                    /* lowAddress      */
        0x01,                                                    /* highAddress     */
        5,                                                       /* uncorrectableErrorThreshold*/
        1                                                        /* preClearEnable  */
    },
    {
        IfxMtu_MbistSel_gtmMcs1Fast,                              /* sshSel          */
        0x00,                                                    /* lowAddress      */
        0x0F,                                                    /* highAddress     */
        5,                                                       /* uncorrectableErrorThreshold*/
        1                                                        /* preClearEnable  */
    },
    {
        IfxMtu_MbistSel_eray0IbfTbf,                              /* sshSel          */
        0x00,                                                    /* lowAddress      */
        0x00,                                                    /* highAddress     */
        5,                                                       /* uncorrectableErrorThreshold*/
        1                                                        /* preClearEnable  */
    },
};

const IfxMtu_MbistSshConfig     mbistSshConfigGang5[] = {
    {
        IfxMtu_MbistSel_cpu0Dtag,                              /* sshSel          */
        0x00,                                                    /* lowAddress      */
        0x03,                                                    /* highAddress     */
        5,                                                       /* uncorrectableErrorThreshold*/
        1                                                        /* preClearEnable  */
    },
    {
        IfxMtu_MbistSel_cpu1Dtag,                              /* sshSel          */
        0x00,                                                    /* lowAddress      */
        0x03,                                                    /* highAddress     */
        5,                                                       /* uncorrectableErrorThreshold*/
        1                                                        /* preClearEnable  */
    },
    {
        IfxMtu_MbistSel_cpu2Dtag,                              /* sshSel          */
        0x00,                                                    /* lowAddress      */
        0x03,                                                    /* highAddress     */
        5,                                                       /* uncorrectableErrorThreshold*/
        1                                                        /* preClearEnable  */
    },
    {
        IfxMtu_MbistSel_gtmDpll1a,                              /* sshSel          */
        0x00,                                                    /* lowAddress      */
        0x01,                                                    /* highAddress     */
        5,                                                       /* uncorrectableErrorThreshold*/
        1                                                        /* preClearEnable  */
    },
    {
        IfxMtu_MbistSel_eray0Obf,                              /* sshSel          */
        0x00,                                                    /* lowAddress      */
        0x00,                                                    /* highAddress     */
        5,                                                       /* uncorrectableErrorThreshold*/
        1                                                        /* preClearEnable  */
    },
};

const IfxMtu_MbistConfig        mbistGang0Config = {
    IFXMTU_MBIST_CONFIG_NONDISTTEST_MCONTROL,                             /* mcontrol        */
    sizeof(mbistSshConfigGang0) / sizeof(IfxMtu_MbistSshConfig),
    sizeof(nonDestructiveTestPattern) / sizeof(IfxMtu_MbistConfigPattern),
    mbistSshConfigGang0,
    nonDestructiveTestPattern
};

const IfxMtu_MbistConfig        mbistGang1Config = {
    IFXMTU_MBIST_CONFIG_NONDISTTEST_MCONTROL,                             /* mcontrol        */
    sizeof(mbistSshConfigGang1) / sizeof(IfxMtu_MbistSshConfig),
    sizeof(nonDestructiveTestPattern) / sizeof(IfxMtu_MbistConfigPattern),
    mbistSshConfigGang1,
    nonDestructiveTestPattern
};

const IfxMtu_MbistConfig        mbistGang2Config = {
    IFXMTU_MBIST_CONFIG_NONDISTTEST_MCONTROL,                             /* mcontrol        */
    sizeof(mbistSshConfigGang2) / sizeof(IfxMtu_MbistSshConfig),
    sizeof(nonDestructiveTestPattern) / sizeof(IfxMtu_MbistConfigPattern),
    mbistSshConfigGang2,
    nonDestructiveTestPattern
};

const IfxMtu_MbistConfig        mbistGang3Config = {
    IFXMTU_MBIST_CONFIG_NONDISTTEST_MCONTROL,                             /* mcontrol        */
    sizeof(mbistSshConfigGang3) / sizeof(IfxMtu_MbistSshConfig),
    sizeof(nonDestructiveTestPattern) / sizeof(IfxMtu_MbistConfigPattern),
    mbistSshConfigGang3,
    nonDestructiveTestPattern
};

const IfxMtu_MbistConfig        mbistGang4Config = {
    IFXMTU_MBIST_CONFIG_NONDISTTEST_MCONTROL,                             /* mcontrol        */
    sizeof(mbistSshConfigGang4) / sizeof(IfxMtu_MbistSshConfig),
    sizeof(nonDestructiveTestPattern) / sizeof(IfxMtu_MbistConfigPattern),
    mbistSshConfigGang4,
    nonDestructiveTestPattern
};

const IfxMtu_MbistConfig        mbistGang5Config = {
    IFXMTU_MBIST_CONFIG_NONDISTTEST_MCONTROL,                             /* mcontrol        */
    sizeof(mbistSshConfigGang5) / sizeof(IfxMtu_MbistSshConfig),
    sizeof(nonDestructiveTestPattern) / sizeof(IfxMtu_MbistConfigPattern),
    mbistSshConfigGang5,
    nonDestructiveTestPattern
};

const IfxMtu_MbistConfig *const mbistGangConfig[] = {
    &mbistGang0Config,
    &mbistGang1Config,
    &mbistGang2Config,
    &mbistGang3Config,
    &mbistGang4Config,
    &mbistGang5Config,
    (void *)0 //End indicator
};
