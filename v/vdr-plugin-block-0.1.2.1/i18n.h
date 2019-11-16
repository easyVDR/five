/**
 * based on i18n.h,v 1.1.1.1 2006/02/26 14:11:02 lordjaxom
 *
 * version by Midas
 *
 */

#ifndef VDR_BLOCK_I18N_H
#define VDR_BLOCK_I18N_H

#include <vdr/i18n.h>
#include <vdr/config.h>

#if VDRVERSNUM < 10507

#define trNOOP(s) (s)
#define trVDR(s) tr(s)

extern const tI18nPhrase Phrases[];

#endif

#endif // VDR_BLOCK_I18N_H
