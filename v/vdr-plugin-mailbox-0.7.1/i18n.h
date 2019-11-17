//=============================================================================
/*
 * Description: This file contains the internationalization for the plugin
 *
 * See the file README in the main directory for a description of
 * this software, copyright information, and how to reach the author.
 *
 * Author: alex
 * Date:   15.11.2003
 *
 * Last modfied:
 *   $Author: alex $
 *   $Date: 2013-03-20 20:33:18 +0100 (Mi, 20 Mär 2013) $
 *   $Revision: 1.2 $
 */
//=============================================================================
#ifndef __MAILBOX_I18N__H__
#define __MAILBOX_I18N__H__

#include <vdr/config.h>
#include <vdr/i18n.h>

#if APIVERSNUM < 10507
#ifndef trNOOP
#define trNOOP(s) (s)
#endif
extern const tI18nPhrase MailboxPhrases[];
#endif

#if APIVERSNUM < 10509
#ifndef trVDR
#define trVDR(s) tr(s)
#endif
#endif

#endif
