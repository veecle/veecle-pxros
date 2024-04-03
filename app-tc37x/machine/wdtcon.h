/*	wdtcon.h -- Lock/unlock TriCore's ENDINIT bit and modify WDTCON0.
 *
 * SPDX-FileCopyrightText: HighTec EDV-Systeme GmbH
 *
 * SPDX-License-Identifier: Boost Software License - Version 1.0
 */

#ifndef __wdtcon_h
#define __wdtcon_h


#ifdef __cplusplus
extern "C" {
#endif

/* To modify ENDINIT-protected registers, use the following sequence:

	unlock_wdtcon (); // reset ENDINIT bit
	write new value(s) to desired register(s)
	lock_wdtcon (); // set ENDINIT bit
*/

extern void lock_wdtcon (void);
extern void unlock_wdtcon (void);
/* functions for handling multi core targets' safety watchdog */
extern void lock_safety_wdtcon (void);
extern void unlock_safety_wdtcon (void);

#ifdef __cplusplus
}
#endif

#endif /* __wdtcon_h */
