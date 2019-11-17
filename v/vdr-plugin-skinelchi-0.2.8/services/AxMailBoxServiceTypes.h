//-----------------------------------------------------------------------------
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
 *   $Date: 2006-03-03 15:52:16 +0100 (Fr, 03 Mär 2006) $
 *   $Revision: 239 $
 */
//-----------------------------------------------------------------------------
#ifndef __AxMailBoxServiceTypes_H__
#define __AxMailBoxServiceTypes_H__

//-----------------------------------------------------------------------------
//     includes
//-----------------------------------------------------------------------------

//----- C++ -------------------------------------------------------------------
//----- C ---------------------------------------------------------------------
//----- AxLib -----------------------------------------------------------------
//----- vdr -------------------------------------------------------------------
//----- local -----------------------------------------------------------------

//=============================================================================
//     Service-Names (ID)
//=============================================================================
#define MailBox_HasNewMail_v1_0_NAME "MailBox-HasNewMail-1.0"

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

#endif

