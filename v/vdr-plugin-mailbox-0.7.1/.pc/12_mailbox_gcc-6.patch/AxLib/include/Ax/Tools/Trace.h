//-----------------------------------------------------------------------------
/*
 * Description: This file contains some defines for log-/debug-output
 *
 * See the file README in the main directory for a description of
 * this software, copyright information, and how to reach the author.
 *
 * Author: alex
 * Date:   17.11.2003
 *
 * Last modfied:
 *   $Author: alex $
 *   $Date: 2005-10-29 13:38:33 +0200 (Sa, 29 Okt 2005) $
 *   $Revision: 1.4 $
 */
//-----------------------------------------------------------------------------
#ifndef __Ax_Tools_Trace_h__
#define __Ax_Tools_Trace_h__

//-----------------------------------------------------------------------------
//     includes
//-----------------------------------------------------------------------------

//----- qt --------------------------------------------------------------------
//----- CORBA -----------------------------------------------------------------
//----- C++ -------------------------------------------------------------------
#include <list>

//----- C ---------------------------------------------------------------------
//----- vdr -------------------------------------------------------------------
#include <vdr/tools.h>

//----- AxLib -----------------------------------------------------------------
//----- local -----------------------------------------------------------------

//-----------------------------------------------------------------------------
//     forward declarations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//     define macros for users
//-----------------------------------------------------------------------------
#define wsdebugsyslog(Y...) dsyslog("mailbox: "Y)
#define wswarnsyslog(Y...)  isyslog("mailbox: "Y)
#define wserrorsyslog(Y...) esyslog("mailbox: "Y)

#define wsdebug(X) do { } while(0);
#define wsinfo(X)  do { wsdebugsyslog X ;} while(0);
#define wswarn(X)  do { wswarnsyslog X  ;} while(0);
#define wserror(X) do { wserrorsyslog X ;} while(0);

#endif // __Ax_Tools_Trace_h__
