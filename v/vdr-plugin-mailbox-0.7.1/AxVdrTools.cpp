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
 *   $Date: 2013-03-20 20:33:18 +0100 (Mi, 20 Mär 2013) $
 *   $Revision: 1.16 $
 */
//=============================================================================

//=============================================================================
//     includes
//=============================================================================

//----- multiple defs of swap() workaround ------------------------------------
#include "AxPluginGlobals.h"

//----- qt --------------------------------------------------------------------
//----- CORBA -----------------------------------------------------------------
//----- ICE -------------------------------------------------------------------
//----- C++ -------------------------------------------------------------------
//----- C ---------------------------------------------------------------------
//----- vdr -------------------------------------------------------------------
#include <vdr/tools.h>
#include <vdr/i18n.h>
#include <vdr/menuitems.h>

//----- AxLib -----------------------------------------------------------------
#include <Ax/Tools/Trace.h>
// #include <Ax/Tools/TraceOn.h>
#include <Ax/Mail/MailBox.h>
#include <Ax/Mail/MailBoxSetup.h>

//----- local plugin ----------------------------------------------------------
#include "AxVdrTools.h"
#include "AxMailWrapper.h"

//----- locals ----------------------------------------------------------------

//=============================================================================
//     using namespaces
//=============================================================================
using namespace std;
using namespace Ax;

//=============================================================================
//     class AxVdrTools
//=============================================================================

//-------------------------------------------------------------------------
//     Access modes
//-------------------------------------------------------------------------
const char *AxVdrTools::AccountTypeNames   [Mail::AT_COUNT         ];
const char *AxVdrTools::AccessModeNames    [Mail::AM_COUNT         ];
const char *AxVdrTools::CClientOptSecNames [Mail::CC_SEC_COUNT     ];
const char *AxVdrTools::CClientOptRSHNames [Mail::CC_RSH_COUNT     ];
const char *AxVdrTools::CClientOptSSLNames [Mail::CC_SSL_COUNT     ];
const char *AxVdrTools::CClientOptTLSNames [Mail::CC_TLS_COUNT     ];
const char *AxVdrTools::CClientOptCertNames[Mail::CC_CERT_COUNT    ];
const char *AxVdrTools::CClientOptDbgNames [Mail::CC_DBG_COUNT     ];
const char *AxVdrTools::WrapModeStrings    [AxMailWrapper::WM_COUNT];

//-------------------------------------------------------------------------
//     Sort order
//-------------------------------------------------------------------------
const char *AxVdrTools::SortOrderStrings[AxVdrTools::SO_COUNT];

//-------------------------------------------------------------------------
//     AxVdrTools::initialize()
//-------------------------------------------------------------------------
void AxVdrTools::initialize()
{
  //----- account types -------------------------------------------------------
  AccountTypeNames   [Mail::AT_USER            ] = tr("User defined");
  AccountTypeNames   [Mail::AT_IMAP            ] = tr("IMAP"        );
  AccountTypeNames   [Mail::AT_POP3            ] = tr("POP3"        );
  AccountTypeNames   [Mail::AT_NNTP            ] = tr("NNTP"        );
  AccountTypeNames   [Mail::AT_INBOX           ] = tr("INBOX"       );

  //----- access modes --------------------------------------------------------
  AccessModeNames    [Mail::AM_NONE            ] = tr("Unrestricted");
  AccessModeNames    [Mail::AM_SETUP           ] = tr("Setup"       );
  AccessModeNames    [Mail::AM_SETUP_READ      ] = tr("Setup & view");

  //----- option for c-client: no option, /secure -----------------------------
  CClientOptSecNames [Mail::CC_SEC_NONE        ] = tr("---"                );
  CClientOptSecNames [Mail::CC_SEC_YES         ] = tr("yes (opt: /secure)" );

  //----- option for c-client: no option, /norsh ------------------------------
  CClientOptRSHNames [Mail::CC_RSH_NONE        ] = tr("---"                );
  CClientOptRSHNames [Mail::CC_RSH_NO          ] = tr("no  (opt: /norsh)"  );

  //----- option for c-client: no option, /ssl --------------------------------
  CClientOptSSLNames [Mail::CC_SSL_NONE        ] = tr("---"                );
  CClientOptSSLNames [Mail::CC_SSL_YES         ] = tr("yes (opt: /ssl)"    );

  //----- option for c-client: no option, /tls or /notls ----------------------
  CClientOptTLSNames [Mail::CC_TLS_NONE        ] = tr("---"                );
  CClientOptTLSNames [Mail::CC_TLS_YES         ] = tr("yes (opt: /tls)"    );
  CClientOptTLSNames [Mail::CC_TLS_NO          ] = tr("no (opt: /notls)"   );

  //----- option for c-client: no option, /validate-cert or /novalidate-cer ---
  CClientOptCertNames[Mail::CC_CERT_NONE       ] = tr("---"                        );
  CClientOptCertNames[Mail::CC_CERT_VALIDATE   ] = tr("yes (opt: /validate-cert)"  );
  CClientOptCertNames[Mail::CC_CERT_NOVALIDATE ] = tr("no (opt: /novalidate-cert)" );

  //----- option for c-client: no option, /debug ------------------------------
  CClientOptDbgNames [Mail::CC_DBG_NONE        ] = tr("---"              );
  CClientOptDbgNames [Mail::CC_DBG_YES         ] = tr("yes (opt: /debug)");

  //----- sort oder  ----------------------------------------------------------
  SortOrderStrings   [AxVdrTools::SO_ASC       ] = tr("ascending" );
  SortOrderStrings   [AxVdrTools::SO_DESC      ] = tr("descending");

  //----- word wrap mode ------------------------------------------------------
  WrapModeStrings    [AxMailWrapper::WM_NONE   ] = tr("1: wrap by VDR");
  WrapModeStrings    [AxMailWrapper::WM_QUOTE  ] = tr("2: continue quotes");
  WrapModeStrings    [AxMailWrapper::WM_FLOWED ] = tr("3: honor format flowed");
  WrapModeStrings    [AxMailWrapper::WM_SMALL  ] = tr("4: combine lines");

} // AxVdrTools::initialize()

//-------------------------------------------------------------------------
//     AxVdrTools::getVdrCharSet()
//-------------------------------------------------------------------------
std::string AxVdrTools::getVdrCharSet()
{
#if VDRVERSNUM < 10503
  std::string aCharSet = std::string(I18nCharSets()[Setup.OSDLanguage]);
  wsdebug(("- AxVdrTools::getVdrCharSet() Using Vdr < 10503 -> using '%s'", aCharSet.c_str()));
#else
  std::string aCharSet("UTF-8");
  if (cCharSetConv::SystemCharacterTable())
  {
    aCharSet = std::string(cCharSetConv::SystemCharacterTable());
    wsdebug(("- AxVdrTools::getVdrCharSet() Vdr-System-CharSet: '%s'", aCharSet.c_str()));
  }
  else
  {
    wsdebug(("- AxVdrTools::getVdrCharSet() Vdr-System-CharSet not set, using '%s'", aCharSet.c_str()));
  } // if
#endif

  return aCharSet;
} // AxVdrTools::getVdrCharSet()
