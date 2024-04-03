/**
 * \file Ifx_TypesReg.h
 * \brief
 * \copyright Copyright (c) 2018 Infineon Technologies AG. All rights reserved.
 *
 * Version: IFXREGTYPES_V1.0.R0
 *
 * MAY BE CHANGED BY USER [yes/no]: No
 *
 *                                 IMPORTANT NOTICE
 *
 *
 * Infineon Technologies AG (Infineon) is supplying this file for use
 * exclusively with Infineon's microcontroller products. This file can be freely
 * distributed within development tools that are supporting such microcontroller
 * products.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * INFINEON SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *
 * Data type access rules for peripheral space
 * data type                               | Allowed access size   | Alignment of address in memory
 * ----------------------------------------|-----------------------|-----------------------------------
 * Ifx_UReg_8Bit, Ifx_SReg_8Bit            | Byte                  | Byte (1H)                         
 * Ifx_UReg_16Bit, Ifx_SReg_16Bit          | Byte, Half-Word       | 2 bytes (2H)                      
 * Ifx_UReg_32Bit, Ifx_SReg_32Bit          | Byte, Half-Word, Word | 4 bytes (4H)                      
 * Ifx_Strict_16Bit                        | Half-Word             | 2 bytes (2H)                      
 * Ifx_Strict_32Bit                        | Word                  | 4 bytes (4H)                      
 * 
 * 
 * Alignment rules for peripheral space (From TriCoreTM TC1.6.2 core architecture manual, 2.2.1 Alignment Requirements)
 * Access type                             | Access size           | Required alignment of address in memory    
 * ----------------------------------------|-----------------------|-----------------------------------
 * Load, Store Data Register               | Byte (8-bits)         | Byte (1H)                         
 * Load, Store Data Register               | Half-Word (16-bits)   | 2 bytes (2H)                      
 * Load, Store Data Register               | Word (32-bits)        | 4 bytes (4H)                      
 * Load, Store Data Register               | Double-Word (64-bits) | 8 bytes (8H)                      
 * Load, Store Address Register            | Word                  | 4 bytes (4H)                      
 * Load, Store Address Register            | Double-Word           | 8 bytes (8H)                      
 * SWAP.W, LDMST, ST.T                     | Word                  | 4 bytes (4H)                      
 * CMPSWAP.W, SWAPMSK.W                    | Word                  | 4 bytes (4H)                      
 * Context Load / Store / Restore / Save   | 16 x 32-bit registers | Not Permitted                     
 * 
 * 
 * 
 *  Peripheral space : segment F and E (From TriCoreTM TC1.6.2 core architecture manual, 8.3.4 Default Memory types for all segments)
 */

#ifndef IFX_TYPESREG_H
#define IFX_TYPESREG_H
/******************************************************************************/

#if defined(__TASKING__)
 #if defined(__CTC__)
 #define Ifx_Strict_16Bit unsigned __sfrbit16
 #define Ifx_Strict_32Bit unsigned __sfrbit32
 #else
 #define Ifx_Strict_16Bit volatile unsigned short
 #define Ifx_Strict_32Bit volatile unsigned int
 #endif
#endif
#if defined(__HIGHTEC__)
#define Ifx_Strict_16Bit volatile unsigned short
#define Ifx_Strict_32Bit volatile unsigned int
#endif
#if defined(__DCC__)
#define Ifx_Strict_16Bit unsigned short
#define Ifx_Strict_32Bit unsigned int
#endif
#if defined(__ghs__)
#define Ifx_Strict_16Bit volatile unsigned short
#define Ifx_Strict_32Bit volatile unsigned int
#endif

typedef unsigned char  Ifx_UReg_8Bit;
typedef unsigned short Ifx_UReg_16Bit;
typedef unsigned int   Ifx_UReg_32Bit;
typedef signed char    Ifx_SReg_8Bit;
typedef signed short   Ifx_SReg_16Bit;
typedef signed int     Ifx_SReg_32Bit;


/******************************************************************************/
#endif /* IFX_TYPESREG_H */
