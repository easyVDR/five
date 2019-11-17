//=============================================================================
/*
 * See the file README in the main directory for a description of
 * this software, copyright information, and how to reach the author.
 *
 * Author: alex
 * Date:   16.04.2006
 *
 * Last modfied:
 *   $Author: alex $
 *   $Date: 2013-03-20 20:09:21 +0100 (Mi, 20 Mär 2013) $
 */
//=============================================================================

//=============================================================================
//     includes
//=============================================================================

//----- qt --------------------------------------------------------------------
//----- CORBA -----------------------------------------------------------------
//----- ICE -------------------------------------------------------------------
//----- C++ -------------------------------------------------------------------
//----- C ---------------------------------------------------------------------
//----- AxLib -----------------------------------------------------------------
#include <Ax/Tools/Trace.h>
// #include <Ax/Tools/TraceOn.h>

//----- vdr -------------------------------------------------------------------
//----- local plugin ----------------------------------------------------------
//----- locals ----------------------------------------------------------------
#include "Internal.h"
#include "MsgCache.h"

//=============================================================================
//     using namespaces
//=============================================================================
using namespace std;

//=============================================================================
//     namespaces 
//=============================================================================
namespace Ax   {
namespace Mail {

//=============================================================================
//     class MsgCache
//=============================================================================

//-------------------------------------------------------------------------
//     MsgCache::MsgCache()
//-------------------------------------------------------------------------
MsgCache::MsgCache(MESSAGECACHE *theMsgCache)
        : myMsgCache(theMsgCache)
{
  wsdebug(("> MsgCache::MsgCache()  this: %08X  theMsgCache: %08X  LockCount: %d"
         , int(this)
         , int(theMsgCache)
         , theMsgCache->lockcount
         ));

  if (myMsgCache)
  {
    myMsgCache->lockcount += 1;
  } // if

  wsdebug(("< MsgCache::MsgCache()  this: %08X  theMsgCache: %08X  LockCount: %d"
         , int(this)
         , int(theMsgCache)
         , theMsgCache->lockcount
         ));
} // MsgCache::MsgCache()

//-------------------------------------------------------------------------
//     MsgCache::~MsgCache()
//-------------------------------------------------------------------------
MsgCache::~MsgCache()
{
  if (myMsgCache)
  {
    MESSAGECACHE *aTmp = myMsgCache;
    myMsgCache = 0;
    mail_free_elt(&aTmp);
  }  // if
} // MsgCache::~MsgCache()

//-------------------------------------------------------------------------
//     MsgCache::MsgCache()
//-------------------------------------------------------------------------
MsgCache::MsgCache(const MsgCache &that)
{
  myMsgCache = that.myMsgCache;
  myMsgCache->lockcount += 1;
} // MsgCache::MsgCache()

//-------------------------------------------------------------------------
//     MsgCache::operator=()
//-------------------------------------------------------------------------
MsgCache &MsgCache::operator=(const MsgCache &that)
{
  if (this != &that)
  {
    if (myMsgCache != that.myMsgCache)
    {
      if (myMsgCache != 0)
      {
        MESSAGECACHE *aTmp = myMsgCache;
        myMsgCache = 0;
        mail_free_elt(&aTmp);
      } // if

      myMsgCache = that.myMsgCache;
      myMsgCache->lockcount += 1;
    }  // if
  } // if

  return *this;
} // MsgCache::operator=()

//-----------------------------------------------------------------------
//     MsgCache::dump()
//-----------------------------------------------------------------------
void MsgCache::dump() const
{
  Internal::dumpMsgCache(myMsgCache);
} // MsgCache::dumpMsgCache()

//=============================================================================
//     namespaces
//=============================================================================
} // Mail
} // Ax
