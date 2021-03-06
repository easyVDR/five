#ifndef __AxMailWrapper_H__
#define __AxMailWrapper_H__
//=============================================================================
/*
 * See the file README in the main directory for a description of
 * this software, copyright information, and how to reach the author.
 *
 * Author: alex
 * Date:   28.06.2004
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
#include <deque>
#include <vector>

//----- C ---------------------------------------------------------------------
//----- AxLib -----------------------------------------------------------------
//----- vdr -------------------------------------------------------------------
#include <vdr/font.h>

//----- local -----------------------------------------------------------------
//----- other includes --------------------------------------------------------

//=============================================================================
//     forward declarations
//=============================================================================
namespace Ax   {
namespace Mail {
  class Mail;
  class MailPart;
}
}

class AxPluginMailBox;

//=============================================================================
//     class AxMailWrapper
//=============================================================================
class AxMailWrapper
{
  public:
    //-------------------------------------------------------------------------
    //     enums
    //-------------------------------------------------------------------------
    enum WrapMode
    {
      WM_FIRST   = 0

    , WM_NONE    = WM_FIRST   ///< let vdr do the word wrapping
    , WM_QUOTE   = 1          ///< continue quotes
    , WM_FLOWED  = 2          ///< honor 'format=flowed` (RFC 2646)
    , WM_SMALL   = 3          ///< continue lines which end and start with small chars

    , WM_COUNT   = 4
    };

  private:
    //-------------------------------------------------------------------------
    //     struct AxLine
    //-------------------------------------------------------------------------
    struct AxLine
    {
      //-----------------------------------------------------------------------
      AxLine(const std::string &theText = std::string(""), bool theFSep = false)
        : Text(theText)
        , fSep(theFSep)
      {
      }

      //-----------------------------------------------------------------------
      void clear()
      {
        Text = std::string("");
        fSep = false;
      }

      //-----------------------------------------------------------------------
      std::string Text;
      bool        fSep;
    }; // class AxLine

    //-------------------------------------------------------------------------
    //     typedefs
    //-------------------------------------------------------------------------
    typedef std::deque<AxLine> LineCltn;

  public:
    //-------------------------------------------------------------------------
    //     AxMailWrapper()
    //-------------------------------------------------------------------------
    AxMailWrapper( AxPluginMailBox   *thePlugin
                 , const cFont       *theFont
                 , int                theWidth
                 );

    //-------------------------------------------------------------------------
    //     ~AxMailWrapper()
    //-------------------------------------------------------------------------
    ~AxMailWrapper();

    //-------------------------------------------------------------------------
    //     getMailText()
    //-------------------------------------------------------------------------
    std::string getMailText( Ax::Mail::Mail *theMail
                           , WrapMode        theWrapMode     = WM_NONE
                           , bool            theShowAllParts = false
                           );

  private:
    //-----------------------------------------------------------------------------
    //     getQuotes()
    //-----------------------------------------------------------------------------
    std::string getQuotes(const std::string &theStr) const;

    //-------------------------------------------------------------------------
    //     cutLine()
    //-------------------------------------------------------------------------
    std::string cutLine(const std::string theSrc);

    //-------------------------------------------------------------------------
    //     splitText()
    //-------------------------------------------------------------------------
    LineCltn splitText(const std::string &theStr);

    //-------------------------------------------------------------------------
    //     getFlowedText()
    //-------------------------------------------------------------------------
    LineCltn getFlowedText(const LineCltn &theSrc);

    //-------------------------------------------------------------------------
    //     improveWrap()
    //-------------------------------------------------------------------------
    LineCltn improveWrap(const LineCltn &theSrc);

    //-------------------------------------------------------------------------
    //     wrapText()
    //-------------------------------------------------------------------------
    LineCltn wrapText(const LineCltn &theSrc);

    //-------------------------------------------------------------------------
    //     convertHtml2Text()
    //-------------------------------------------------------------------------
    std::string convertHtml2Text(const Ax::Mail::MailPart *thePart);

    //-------------------------------------------------------------------------
    //     collectParts()
    //-------------------------------------------------------------------------
    void collectParts(Ax::Mail::MailPartVector &thePartCltn, const Ax::Mail::MailPart *thePart);

    //-------------------------------------------------------------------------
    //     addPartText()
    //-------------------------------------------------------------------------
    void addPartText(LineCltn &theDst, const Ax::Mail::MailPart *thePart);
    
    //-------------------------------------------------------------------------
    //     dumpText()
    //-------------------------------------------------------------------------
    void dumpText(const std::string &theMsg, const LineCltn &theSrc);

    //-------------------------------------------------------------------------
    //     attributes
    //-------------------------------------------------------------------------
    AxPluginMailBox *myPlugin  ;
    const cFont     *myFont    ;
    int              myWidth   ;
    WrapMode         myWrapMode;

    bool             fShowPartInfo;
    bool             fHasOsdProps ;
    bool             fConvHtml    ;
    bool             fShowAllParts;
    bool             fShowHeader  ;
}; // class AxMailWrapper

#endif

