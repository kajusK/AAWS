/*
 * ML8511 UV sensor
 *
 * For license, see LICENSE.txt
 *
 * Jakub Kaderka 2016
 */

#ifndef __ml8511_H_
#define __ml8511_H_

/*
 * Returns UV index (0-15) in tenths (e.g 85 = 8.5) or 0xff if module doesn't work
 */
extern uint8_t ml8511_get(void);

#endif
