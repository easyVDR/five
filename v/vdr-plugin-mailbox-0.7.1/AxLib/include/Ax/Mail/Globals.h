#ifndef __Ax_Mail_Globals_H__
#define __Ax_Mail_Globals_H__
//=============================================================================
/*
 * See the file README in the main directory for a description of
 * this software, copyright information, and how to reach the author.
 *
 * Author: alex
 * Date:   15.11.2003
 *
 * Last modfied:
 *   $Author: alex $
 *   $Date: 2013-03-20 19:43:16 +0100 (Mi, 20 Mär 2013) $
 *   $Revision: 1.5 $
 */
//=============================================================================

//=============================================================================
//     includes
//=============================================================================

//----- qt --------------------------------------------------------------------
//----- CORBA -----------------------------------------------------------------
//----- C++ -------------------------------------------------------------------
#include <list>
#include <map>
#include <string>
#include <vector>

//----- C ---------------------------------------------------------------------
//----- AxLib -----------------------------------------------------------------
//----- vdr -------------------------------------------------------------------
//----- local -----------------------------------------------------------------

//=============================================================================
//     namespaces
//=============================================================================
namespace Ax   {
namespace Mail {

//=========================================================================
//     forward declarations
//=========================================================================
class Mail;
class MailPart;
class MailBox;
class MailFolder;
class MailBoxSetup;

//=========================================================================
//     typedefs
//=========================================================================
typedef std::vector<Mail *>                 MailVector;
typedef std::vector<MailFolder *>           MailFolderCltn;
typedef std::vector<const MailPart *>       MailPartVector;
typedef std::list<const MailPart *>         MailPartList;
typedef std::list<MailBox *>                MailBoxList;
typedef std::vector<MailBoxSetup *>         MailBoxSetupCltn;
typedef std::map<std::string , std::string> String2StringMap;

//=========================================================================
//     enum CCOption
//=========================================================================
enum CCOption
{
  CCO_FIRST

, CCO_HOSTNAME = CCO_FIRST     ///< mailbox supports: host name
, CCO_FOLDER                   ///< mailbox supports: folders
, CCO_USERNAME                 ///< mailbox supports: user name
, CCO_PASSWORD                 ///< mailbox supports: password
, CCO_SEC                      ///< mailbox supports: /secure
, CCO_RSH                      ///< mailbox supports: /norsh
, CCO_SSL                      ///< mailbox supports: /ssl
, CCO_TLS                      ///< mailbox supports: /tls or /notls
, CCO_CERT                     ///< mailbox supports: /validate-cert or /novalidate-cert
, CCO_DBG                      ///< mailbox supports: /debug

, CCO_LAST
}; // enum CCOption

//=========================================================================
//     enum AccountType
//=========================================================================
enum AccountType
{
  AT_USER            = 0       ///< user defined string
, AT_IMAP            = 1       ///< Option: /imap
, AT_POP3            = 2       ///< Option: /pop3
, AT_NNTP            = 3       ///< Option: /nntp
, AT_INBOX           = 4       ///< Option: INBOX

, AT_COUNT           = 5       ///< number of options
}; // enum AxMailFlag

//=========================================================================
//     enums
//=========================================================================

//-------------------------------------------------------------------------
//     enum AccessMode
//-------------------------------------------------------------------------
/** Mailbox-access mode
  */
enum AccessMode
{
  AM_NONE            = 0x00    ///< no locks
, AM_SETUP           = 0x01    ///< code required for setup
, AM_SETUP_READ      = 0x02    ///< code required for setup and mail-read access

, AM_MASK            = 0x03    ///< mask for this option
, AM_BITS            = 2       ///< number of bits to use
, AM_COUNT           = 3       ///< number of options
}; // enum MailViewMode

//-------------------------------------------------------------------------
//     enum EBGCheckMode
//-------------------------------------------------------------------------
// The following two enums should be in application-specific
// code and not here! But for simplicity I add them here...

/** Backgound check mode
  */
enum EBGCheckMode
{
  BG_EXT_OFF         = 0x00    ///< don't start external app
, BG_EXT_ON          = 0x01    ///< do    start external app
, BG_EXT_MASK        = 0x01    ///< bit-mask for BG_EXT_*

, BG_MSG_OFF         = 0x00    ///< don't display a message
, BG_MSG_ON          = 0x02    ///< do    display a message
, BG_MSG_MASK        = 0x02    ///< bit-mask for BG_MSG_*

, BG_MEN_OFF         = 0x00    ///< don't display in main menu
, BG_MEN_ON          = 0x04    ///< do    display in main menu
, BG_MEN_MASK        = 0x04    ///< bit-mask for BG_MEN_*

, BG_CHK_OFF         = 0x00    ///< don't do background check
, BG_CHK_ON          = 0x08    ///< do    do background check
, BG_CHK_MASK        = 0x08    ///< bit-mask for BG_CHK_*

, BG_SIF_OFF         = 0x00    ///< don't use in service interface
, BG_SIF_ON          = 0x10    ///< do    use in service interface
, BG_SIF_MASK        = 0x10    ///< bit-mask for BG_SIF_*

, BG_MODE_NONE       = BG_EXT_OFF  ///< no bit set
                     | BG_MSG_OFF
                     | BG_MEN_OFF
                     | BG_CHK_OFF
                     | BG_SIF_OFF

, BG_MODE_MASK       = BG_EXT_MASK  ///< mask for this option
                     | BG_MSG_MASK
                     | BG_MEN_MASK
                     | BG_CHK_MASK
                     | BG_SIF_MASK

, BG_MODE_BITS       = 5            ///< number of bits to use

}; // enum MailViewMode

//-------------------------------------------------------------------------
//     enum MailFlag
//-------------------------------------------------------------------------
/** The flags for a mail which are currently supported
 *
 * \todo MF_RECENT aka \c \\Recent is missing
 */
enum MailFlag
{
  MF_SEEN                      ///< \c \\Seen     flag
, MF_FLAGGED                   ///< \c \\Flagged  flag
, MF_DELETED                   ///< \c \\Deleted  flag
, MF_ANSWERED                  ///< \c \\Answered flag
}; // enum AxMailFlag

//-------------------------------------------------------------------------
/** option for c-client: no option, /secure
 */
enum EClientOptSec
{
  CC_SEC_NONE        = 0x00    ///< option not given
, CC_SEC_YES         = 0x01    ///< c-client option: /secure

, CC_SEC_MASK        = 0x01    ///< mask for this option
, CC_SEC_BITS        = 1       ///< number of bits to use
, CC_SEC_COUNT       = 2       ///< number of options
};

//-------------------------------------------------------------------------
/** option for c-client: no option, /norsh
 */
enum EClientOptRSH
{
  CC_RSH_NONE        = 0x00    ///< option not given
, CC_RSH_NO          = 0x01    ///< c-client option: /norsh

, CC_RSH_MASK        = 0x01    ///< mask for this option
, CC_RSH_BITS        = 1       ///< number of bits to use
, CC_RSH_COUNT       = 2       ///< number of options
};

//-------------------------------------------------------------------------
/** option for c-client: no option, /ssl
 */
enum EClientOptSSL
{
  CC_SSL_NONE        = 0x00    ///< option not given
, CC_SSL_YES         = 0x01    ///< c-client option: /ssl

, CC_SSL_MASK        = 0x01    ///< mask for this option
, CC_SSL_BITS        = 1       ///< number of bits to use
, CC_SSL_COUNT       = 2       ///< number of options
};

//-------------------------------------------------------------------------
/** option for c-client: no option, /tls or /notls
 */
enum EClientOptTLS
{
  CC_TLS_NONE        = 0x00    ///< option not given
, CC_TLS_YES         = 0x01    ///< c-client option: /tls
, CC_TLS_NO          = 0x02    ///< c-client option: /notls

, CC_TLS_MASK        = 0x03    ///< mask for this option
, CC_TLS_BITS        = 2       ///< number of bits to use
, CC_TLS_COUNT       = 3       ///< number of options
};

//-------------------------------------------------------------------------
/** option for c-client: no option, /validate-cert or /novalidate-cert
 */
enum EClientOptCert
{
  CC_CERT_NONE       = 0x00    ///< option not given
, CC_CERT_VALIDATE   = 0x01    ///< c-client option: /validate-cert
, CC_CERT_NOVALIDATE = 0x02    ///< c-client option: /novalidate-cert

, CC_CERT_MASK       = 0x03    ///< mask for this option
, CC_CERT_BITS       = 2       ///< number of bits to use
, CC_CERT_COUNT      = 3       ///< number of options
};

//-------------------------------------------------------------------------
/** option for c-client: no option, /debug
 */
enum EClientOptDbg
{
  CC_DBG_NONE        = 0x00    ///< option not given
, CC_DBG_YES         = 0x01    ///< c-client option: /debug

, CC_DBG_MASK        = 0x01    ///< mask for this option
, CC_DBG_BITS        = 1       ///< number of bits to use
, CC_DBG_COUNT       = 2       ///< number of options
};

//-------------------------------------------------------------------------
/** body types
 *
 *  This enum is a replacement for the c-client #defines
 *  TYPE* (e.g. TYPETEXT -> BT_TEXT)
 */
enum EBodyType
{
  BT_TEXT                      ///< unformatted text
, BT_MULTIPART                 ///< multiple part
, BT_MESSAGE                   ///< encapsulated message
, BT_APPLICATION               ///< application data
, BT_AUDIO                     ///< audio
, BT_IMAGE                     ///< static image
, BT_VIDEO                     ///< video
, BT_MODEL                     ///< model
, BT_OTHER                     ///< unknown
, BT_MAX                       ///< maximum type code

, BT_UNKNOWN                   ///< no one of the above
};

//-------------------------------------------------------------------------
/** body encodings
 *
 *  This enum is a replacement for the c-client #defines
 *  ENC*
 */
enum EBodyEncoding
{
  BE_7BIT                      ///< 7 bit SMTP semantic data
, BE_8BIT                      ///< 8 bit SMTP semantic data
, BE_BINARY                    ///< 8 bit binary data
, BE_BASE64                    ///< base-64 encoded data
, BE_QUOTEDPRINTABLE           ///< human-readable 8-as-7 bit data
, BE_OTHER                     ///< unknown
, BE_MAX                       ///< maximum encoding code

, BE_UNDEFINED                 ///< no one of the above
};

//=============================================================================
//     namespaces
//=============================================================================
} // Mail
} // Ax

#endif
