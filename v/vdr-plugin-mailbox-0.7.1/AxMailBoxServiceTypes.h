#ifndef __AxMailBoxServiceTypes_H__
#define __AxMailBoxServiceTypes_H__
//=============================================================================
/*
 * Description: This file contains the names & types for the service-interface
 *
 * See the file README in the main directory for a description of
 * this software, copyright information, and how to reach the author.
 *
 * Author: alex
 * Date:   03.03.2006
 *
 * Last modfied:
 *   $Author: alex $
 *   $Date: 2013-03-20 20:33:18 +0100 (Mi, 20 Mär 2013) $
 */
//=============================================================================

//=============================================================================
//     Service-Names (ID)
//=============================================================================
#define MailBox_HasNewMail_v1_0_NAME     "MailBox-HasNewMail-1.0"
#define MailBox_GetTotalUnseen_v1_0_NAME "MailBox-GetTotalUnseen-1.0"

//=============================================================================
//     service MailBox-HasNewMail-1.0
//=============================================================================
/** Ask if new mails are present
 *
 *  The Data-parameter to cPlugin::Service() is simply a pointer to a bool
 *  which - on return - is
 *  - true  if at least one mail account contains an unread mail
 *  - false if none of the mail accounts cotains any unread mail
 */

//=============================================================================
//     service MailBox-GetTotalUnseen-1.0
//=============================================================================
/** Query the total number of unseen mails
 *
 *  The Data-parameter to cPlugin::Service() is simply a pointer to a 
 *  unsigned long which - on return - contains the number of unseen mails
 *  in all mail-accounts which
 * 
 *  - have background-check enabled
 *  - internal interface enabled
 */

#endif

