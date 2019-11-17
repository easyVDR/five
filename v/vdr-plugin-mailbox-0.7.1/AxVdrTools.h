#ifndef __MailBox_AxVdrTools_H__
#define __MailBox_AxVdrTools_H__
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
 */
//=============================================================================

//=============================================================================
//     includes
//=============================================================================

//----- qt --------------------------------------------------------------------
//----- CORBA -----------------------------------------------------------------
//----- C++ -------------------------------------------------------------------
//----- C ---------------------------------------------------------------------
//----- AxLib -----------------------------------------------------------------
//----- vdr -------------------------------------------------------------------
#include <vdr/osd.h>

//----- local -----------------------------------------------------------------

//----- other includes --------------------------------------------------------

//=============================================================================
//     forward declarations
//=============================================================================

//=============================================================================
//     class AxVdrTools
//=============================================================================
/** The class AxVdrTools contains some small helper functions for
 *  this vdr-plugin.
 */
class AxVdrTools
{
  public:
    //-------------------------------------------------------------------------
    //     names for c-client options
    //-------------------------------------------------------------------------
    static const char *AccountTypeNames   [];
    static const char *AccessModeNames    [];
    static const char *BGCheckModeNames   [];
    static const char *CClientOptSecNames [];
    static const char *CClientOptRSHNames [];
    static const char *CClientOptSSLNames [];
    static const char *CClientOptTLSNames [];
    static const char *CClientOptCertNames[];
    static const char *CClientOptDbgNames [];

    static const char *WrapModeStrings    [];
#if 0
    static const char *VdrFontNames       [];
#endif
    //-------------------------------------------------------------------------
    //     Sort order
    //-------------------------------------------------------------------------
    // the values of the enum correspond to the strings in SortOrderStrings
    enum SortOrder
    {
      SO_ASC     ///< ascending
    , SO_DESC    ///< descending
    , SO_COUNT   ///< number of entries
    };
    static const char *SortOrderStrings[];

    //-------------------------------------------------------------------------
    //     initialize()
    //-------------------------------------------------------------------------
    static void initialize();

    //-------------------------------------------------------------------------
    //     getVdrCharSet()
    //-------------------------------------------------------------------------
    static std::string getVdrCharSet();

}; // class AxVdrTools

#endif

