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
//     defines
//=============================================================================
// Show the part-name (number) in the header lines
#define SHOW_PART_HEADER_WITH_NAME 0

// remove temporary files after processing
#define REMOVE_TEMP_FILES          1

//=============================================================================
//     includes
//=============================================================================

//----- multiple defs of swap() workaround ------------------------------------
#include "AxPluginGlobals.h"

//----- qt --------------------------------------------------------------------
//----- CORBA -----------------------------------------------------------------
//----- ICE -------------------------------------------------------------------
//----- C++ -------------------------------------------------------------------
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

//----- C ---------------------------------------------------------------------
#include <ctype.h>
#include <unistd.h>

//----- vdr -------------------------------------------------------------------
#include <vdr/tools.h>

//----- AxLib -----------------------------------------------------------------
#include <Ax/Tools/Trace.h>
// #include <Ax/Tools/TraceOn.h>
#include <Ax/Tools/String.h>
#include <Ax/Mail/Mail.h>
#include <Ax/Mail/MailPart.h>
#include <Ax/Mail/Tools.h>

//----- local plugin ----------------------------------------------------------
#include "AxMailWrapper.h"
#include "AxVdrTools.h"
#include "AxPluginMailBox.h"
#include "AxPluginSetup.h"
#include "i18n.h"

//----- locals ----------------------------------------------------------------

//=============================================================================
//     using namespaces
//=============================================================================
using namespace Ax;

//=============================================================================
//     class AxMailWrapper
//=============================================================================

//-------------------------------------------------------------------------
//     AxMailWrapper::AxMailWrapper()
//-------------------------------------------------------------------------
AxMailWrapper::AxMailWrapper(AxPluginMailBox *thePlugin, const cFont *theFont, int theWidth)
             : myPlugin(thePlugin)
             , myFont  (theFont  )
             , myWidth (theWidth )
             , fHasOsdProps (false)
             , fConvHtml    (false)
             , fShowAllParts(false)
             , fShowHeader  (false)
{
  wsdebug(("> AxMailWrapper::AxMailWrapper() Font: %p  Width: %3d", myFont, myWidth));

  fHasOsdProps = myFont != 0 && myWidth != 0;
  fConvHtml    = thePlugin->getSetup().Settings.MailConvCmd.length() > 0
              && thePlugin->getSetup().Settings.MailSrcFile.length() > 0
              && thePlugin->getSetup().Settings.MailDstFile.length() > 0
               ;

  std::string dummy trNOOP("Multipart mail");

  wsdebug(("< AxMailWrapper::AxMailWrapper() Font: %p  Width: %3d", myFont, myWidth));
} // AxMailWrapper::AxMailWrapper()

//-------------------------------------------------------------------------
//     AxMailWrapper::~AxMailWrapper()
//-------------------------------------------------------------------------
AxMailWrapper::~AxMailWrapper()
{
  wsdebug(("> AxMailWrapper::~AxMailWrapper() Font: %p  Width: %3d", myFont, myWidth));
  wsdebug(("< AxMailWrapper::~AxMailWrapper() Font: %p  Width: %3d", myFont, myWidth));
} // AxMailWrapper::~AxMailWrapper()

//-------------------------------------------------------------------------
//     AxMailWrapper::getMailText()
//-------------------------------------------------------------------------
std::string AxMailWrapper::getMailText(Ax::Mail::Mail *theMail, WrapMode theWrapMode, bool theShowAllParts)
{
  wsdebug(("> AxMailWrapper::getMailText() Font: %p  Width: %3d  theMail: %p", myFont, myWidth, theMail));
  if (theMail == 0)
  {
    wsdebug(("< AxMailWrapper::getMailText() Font: %p  Width: %3d  theMail: %p", myFont, myWidth, theMail));
    return std::string("");
  } // if

  fShowAllParts = theShowAllParts;
  myWrapMode    = fHasOsdProps ? theWrapMode : WM_NONE;

  wsdebug(("======================================================================"));
  Ax::Mail::MailPartVector aPartCltn;
  collectParts(aPartCltn, theMail->getRootPart());
  wsdebug(("----------------------------------------------------------------------"));

  fShowHeader   = fShowAllParts || aPartCltn.size() > 1;
  LineCltn  myText;
  for (Ax::Mail::MailPartVector::iterator anIter  = aPartCltn.begin();
                                          anIter != aPartCltn.end();
                                        ++anIter)
  {
    addPartText(myText, *anIter);
  } // for

  wsdebug(("======================================================================"));

  std::string aBodyText;
  for (LineCltn::iterator anIter = myText.begin(); anIter != myText.end(); ++anIter)
  {
    aBodyText += anIter->Text + "\n";
  } // if

  wsdebug(("< AxMailWrapper::getMailText()"));
  return aBodyText;
} // AxMailWrapper::getMailText()

//-----------------------------------------------------------------------------
//     AxMailWrapper::getQuotes()
//-----------------------------------------------------------------------------
std::string AxMailWrapper::getQuotes(const std::string &theStr) const
{
  if (theStr.empty() || theStr[0] != '>')
  {
    return std::string("");
  } // if

  std::string aQuote = theStr.substr(0, theStr.find_first_not_of("> "));

  if (int(aQuote.length()) > myWidth / 2)
  {
    // ignore lines, that start with more than n characters of "> "
    return std::string("");
  } // if

  std::string::size_type aLastQPos = aQuote.find_last_not_of(' ');
  if (aLastQPos != std::string::npos)
  {
    if (aLastQPos + 1 == aQuote.length())
    {
      aQuote  = aQuote.substr(0, aLastQPos + 1);
    }
    else
    {
      aQuote  = aQuote.substr(0, aLastQPos + 2);
    } // if
  } // if

  return aQuote;
} // AxMailWrapper::getQuotes()

//-------------------------------------------------------------------------
//     AxMailWrapper::cutLine()
//-------------------------------------------------------------------------
std::string AxMailWrapper::cutLine(const std::string theSrc)
{
  if (myWidth == 0 || myFont == 0)
  {
    wswarn(("AxMailWrapper::cutLine() myFont: %p  myWidth: %d unable to cut!", myFont, myWidth));
    return theSrc;
  } // if

  wsdebug(("> AxMailWrapper::cutLine() Src: '%s'", theSrc.c_str()));

  int aLineWidth = 0;

  std::string::const_iterator anIter = theSrc.begin();
  for (; anIter != theSrc.end() && aLineWidth < myWidth; ++anIter, aLineWidth += myFont->Width(*anIter))
    ;

  // go on character back
  if (aLineWidth > myWidth && anIter != theSrc.begin())
  {
    --anIter;
  } // if

  std::string aDst(theSrc.begin(), anIter);

  wsdebug(("< AxMailWrapper::cutLine() Dst: %s", aDst.c_str()));
  return aDst;
} // AxMailWrapper::splitLine()

//-------------------------------------------------------------------------
//     AxMailWrapper::splitText()
//-------------------------------------------------------------------------
AxMailWrapper::LineCltn AxMailWrapper::splitText(const std::string &theStr)
{
  wsdebug(("> AxMailWrapper::splitText()"));
  LineCltn aDst;

  std::string::size_type aStrLen   = theStr.length();
  std::string::size_type aPosStart = 0;
  std::string::size_type aPosEnd   = std::string::npos;

  for (; aPosStart != std::string::npos; aPosStart = aPosEnd)
  {
    aPosEnd  = theStr.find_first_of("\r\n", aPosStart);

    std::string  aLine("");
    if (aPosEnd != std::string::npos)
    {
      aLine = theStr.substr(aPosStart, aPosEnd - aPosStart);
      if      (aPosEnd + 1 < aStrLen && theStr[aPosEnd] == '\r' && theStr[aPosEnd + 1] == '\n') aPosEnd += 2;
      else if (aPosEnd + 1 < aStrLen && theStr[aPosEnd] == '\n' && theStr[aPosEnd + 1] == '\r') aPosEnd += 2;
      else                                                                                      aPosEnd += 1;
      if (aPosEnd >= aStrLen) aPosEnd = std::string::npos;
    }
    else
    {
      aLine = theStr.substr(aPosStart);
    } // if

    aDst.push_back(aLine);
  } // while

  wsdebug(("< AxMailWrapper::splitText() NumLines: %d", aDst.size()));
  return aDst;
} //  AxMailWrapper::splitText()

//-------------------------------------------------------------------------
//     AxMailWrapper::getFlowedText()
//-------------------------------------------------------------------------
AxMailWrapper::LineCltn AxMailWrapper::getFlowedText(const LineCltn &theSrc)
{
  wsdebug(("> AxMailWrapper::getFlowedText()"));

  AxMailWrapper::LineCltn aDst;
  std::string             aParagraph = "";
  std::string             aQuotePrev = "";

  //----- iterate through all lines -----
  for (LineCltn::const_iterator anIter  = theSrc.begin();
                                anIter != theSrc.end();
                              ++anIter)
  {
    std::string aLine = (*anIter).Text;

    //----- signature separator or empty line -----
    if (aLine == "-- " || aLine.empty())
    {
      if (!aParagraph.empty())
      {
        aDst.push_back(AxLine(aParagraph));
        aParagraph = "";
        aQuotePrev = "";
      } // if
      aDst.push_back(AxLine(aLine));
      continue;
    } // if

    // removed stuffed space
    bool fStuffed = !aLine.empty() && aLine[0] == ' ';
    if (fStuffed)
    {
      aLine = aLine.erase(0, 1);
    } // if

    //----- check quote -----
    std::string aQuoteAct = getQuotes(aLine);
    std::string aContent  = aLine.substr(aQuoteAct.length(), aLine.length() - aQuoteAct.length());

    if (aQuotePrev != aQuoteAct)
    {
      // quote has changed

      if (!aParagraph.empty())
      {
        // paragraph present -> add to Dst
        aDst.push_back(aParagraph);
        aParagraph = "";
      } // if

      if (!aContent.empty() && aContent[aContent.length() - 1] == ' ')
      {
        // flowed line -> start new paragraph
        aParagraph = aLine;
      }
      else
      {
        // fixed line -> append directly
        aDst.push_back(AxLine(aLine));
      } // if
    }
    else
    {
      // quote is equal

      // add content or complete line to paragraph
      if (aParagraph.empty())
      {
        aParagraph = aLine;
      }
      else
      {
        aParagraph += aContent;
      } // if

      if (!aContent.empty() && aContent[aContent.length() - 1] == ' ')
      {
        // flowed line
        // already added to the paragraph
      }
      else
      {
        // fixed line
        // paragraph present -> add to Dst
        aDst.push_back(aParagraph);
        aParagraph = "";

      } // if
    } // if

    aQuotePrev = aQuoteAct;
  } // for

  wsdebug(("< AxMailWrapper::getFlowedText()"));
  return aDst;
} // AxMailWrapper::getFlowedText()

//-------------------------------------------------------------------------
//     AxMailWrapper::improveWrap()
//-------------------------------------------------------------------------
AxMailWrapper::LineCltn AxMailWrapper::improveWrap(const LineCltn &theSrc)
{
  wsdebug(("> AxMailWrapper::improveWrap()"));

  AxMailWrapper::LineCltn aDst;
  std::string             aParagraph = "";
  std::string             aQuotePrev = "";

  bool fFirst = true;

  //----- iterate through all lines -----
  for (LineCltn::const_iterator anIter  = theSrc.begin();
                                anIter != theSrc.end();
                              ++anIter)
  {
    std::string aLine = (*anIter).Text;

    //----- check quote -----
    std::string aQuoteAct  = getQuotes(aLine);
    std::string aContent   = aLine.substr(aQuoteAct.length(), aLine.length() - aQuoteAct.length());

    if (aQuotePrev != aQuoteAct)
    {
      if (anIter != theSrc.begin()) // don't insert an empty line if the first line of the mail is a quote
      {
        aDst.push_back(aParagraph);
      } // if
      aParagraph = "";

      aParagraph = aLine;
    }
    else
    {
      std::string::size_type aPosLast  = aParagraph.find_last_not_of (' ');
      std::string::size_type aPosFirst = aContent  .find_first_not_of(' ');

      if ( aPosLast  != std::string::npos
        && aPosFirst != std::string::npos
        && strchr("abcdefghijklmnopqrstuvwxyzäöüß", aParagraph[aPosLast ])
        && strchr("abcdefghijklmnopqrstuvwxyzäöüß", aContent  [aPosFirst])
         )
      {
        if (!aParagraph.empty() && aParagraph[0] != ' ' && aParagraph[aParagraph.length() - 1] != ' ')
        {
          aParagraph += " ";
        } // if
        aParagraph += aContent;
      }
      else
      {
        if (!aParagraph.empty() || !fFirst)
        {
          aDst.push_back(aParagraph);
          aParagraph = "";
        } // if

        aParagraph = aLine;
      } // if
    } // if

    aQuotePrev = aQuoteAct;
    fFirst = false;
  } // for

  // if theres still data -> append to dst
  if (!aParagraph.empty())
  {
    aDst.push_back(aParagraph);
  } // if

  wsdebug(("< AxMailWrapper::improveWrap()"));
  return aDst;
} // AxMailWrapper::improveWrap()

//-------------------------------------------------------------------------
//     AxMailWrapper::wrapText()
//-------------------------------------------------------------------------
AxMailWrapper::LineCltn AxMailWrapper::wrapText(const LineCltn &theSrc)
{
  if (myFont == 0 || myWidth == 0)
  {
    wswarn(("AxMailWrapper::wrapText() myFont: %p  myWidth: %d unable to wrap!", myFont, myWidth));
    return theSrc;
  } // if

  wsdebug(("> AxMailWrapper::wrapText() NumLines: %d", theSrc.size()));
  LineCltn aDst;

  //----- iterate through all lines -----
  for (LineCltn::const_iterator anIter  = theSrc.begin();
                                anIter != theSrc.end();
                              ++anIter)
  {
    std::string aLine = (*anIter).Text;
    bool        fSep  = (*anIter).fSep;

    std::string aQuoteChars  = getQuotes(aLine);
    std::string aCntntChars  = aLine.substr(aQuoteChars.length(), aLine.length() - aQuoteChars.length());

    //----- get width of quote ------------
    int aQuoteWidth(0);
    for (std::string::const_iterator aQuoteIter  = aQuoteChars.begin();
                                     aQuoteIter != aQuoteChars.end();
                                   ++aQuoteIter)
    {
      aQuoteWidth += myFont->Width(*aQuoteIter);
    } // for

    //----- iterate within line -----------
    do
    {
      std::string::iterator aPosLastSpace = aCntntChars.end();
      std::string::iterator aPosLastDelim = aCntntChars.end();

      std::string aNewLine ("");
      int         aDstWidth( 0);
      bool        fWrapped = false;

      //----- process up to osd width -----
      for (std::string::iterator aCharIter  = aCntntChars.begin();
                                 aCharIter != aCntntChars.end() && !fWrapped;
                               ++aCharIter)
      {
        char aChar  = *aCharIter;
        aDstWidth  += myFont->Width(aChar);

        if (aDstWidth + aQuoteWidth > myWidth)
        {
          //----- line break necessary -----

          std::string::iterator aPosWrap = aCharIter;
          if (aPosLastSpace != aCntntChars.end())
          {
            aPosWrap = aPosLastSpace + 1;
          }
          else if (aPosLastDelim != aCntntChars.end())
          {
            aPosWrap = aPosLastDelim + 1;
          }
          else
          {
            aPosWrap = aCharIter;
          } // if

          aNewLine = std::string(aCntntChars.begin(), aPosWrap);
          if (fSep)
          {
            // cut (don't wrap) 'separator' lines
            aCntntChars = "";
          }
          else
          {
            aCntntChars = std::string(aPosWrap, aCntntChars.end());
          } // if
          fWrapped = true;
        }
        else if (aChar == ' '              ) aPosLastSpace = aCharIter;
        else if (strchr("-.,:;!?_/", aChar)) aPosLastDelim = aCharIter;
      } // for

      if (!fWrapped)
      {
        aNewLine    = aCntntChars;
        aCntntChars = "";
      } // if

      aDst.push_back(AxLine(aQuoteChars + aNewLine, fSep));

      //----- if not using 'continued quotes' -> delete quote-string/width
      if (myWrapMode < WM_QUOTE)
      {
        aQuoteChars = "";
        aQuoteWidth =  0;
      } // if
    }
    while (!aCntntChars.empty());
  } // for

  wsdebug(("< AxMailWrapper::wrapText()"));
  return aDst;
} // AxMailWrapper::wrapText()

//-------------------------------------------------------------------------
//     AxMailWrapper::convertHtml2Text()
//-------------------------------------------------------------------------
std::string AxMailWrapper::convertHtml2Text(const Ax::Mail::MailPart *thePart)
{
  wsdebug(("> AxMailWrapper::convertHtml2Text()"));

  cTimeMs aTimer;
  
  std::string aSrc = thePart->getBodyText(); // quoted printable / base64 already handled in getBodyText()
#if 0
  wsdebug(("------------------------------------------------------------------------------------"));
  wsdebug(("%s", aDst.c_str()));
  wsdebug(("------------------------------------------------------------------------------------"));
#endif

  // setup some variables 
  std::string aCmdFileName = myPlugin->getSetup().Settings.MailConvCmd;
  std::string aSrcFileName = myPlugin->getSetup().Settings.MailSrcFile;
  std::string aDstFileName = myPlugin->getSetup().Settings.MailDstFile;

  int aWidth = 60; // default for OSD
  if (myWidth != 0 && myFont != 0)
  {
    int mWidth = myFont->Width("x");
    aWidth = myWidth / mWidth;
    wsdebug(("  AxMailWrapper::convertHtml2Text() OSDWidth: %d  m-width: %d  width: %d", myWidth, mWidth, aWidth));
  } // if

  //-----------------------------------------------------------------------
  //     save the current html source
  //-----------------------------------------------------------------------
  // save current umask
  mode_t anOldUMASK = umask(0x477);

  bool fOK = false;
  try
  {
    std::ofstream aFile(aSrcFileName.c_str());
    if (!aFile)
    {
      throw std::string("unable to open file!");
    } // if
    aFile << aSrc;
    fOK = true;
  }
  catch (const std::string &aMsg)
  {
    wserror(("error while writing %s: %s", aSrcFileName.c_str(), aMsg.c_str()));
    wserror(("%s", aMsg.c_str()));
  }
  catch (...)
  {
    wserror(("error while writing %s", aSrcFileName.c_str()));
  } // catch

  // restore previouse umask
  umask(anOldUMASK);

  if (!fOK)
  {
    return aSrc;
  } // if

  //-----------------------------------------------------------------------
  //     call external command
  //-----------------------------------------------------------------------
  //----- make sure the destination file doesn't exist
  remove(aDstFileName.c_str());
  
  //----- execute external command
  fOK = false;
  std::string aSrcCharSet = thePart->getParam("charset");
  if (aSrcCharSet.length() == 0)
  {
    aSrcCharSet = "ISO-8859-1";
  } // if
  std::string aCmd = Ax::Tools::String::sprintf( "%s -i %s -s %s -o %s -d %s -w %d"
                                               , aCmdFileName.c_str()
                                               , aSrcFileName.c_str()
                                               , aSrcCharSet .c_str()
                                               , aDstFileName.c_str()
                                               , AxVdrTools::getVdrCharSet().c_str()
                                               , aWidth
                                               );

  wsdebug(("mailbox: executing '%s'", aCmd.c_str()));
  int aRet = SystemExec(aCmd.c_str());
  fOK = aRet == 0;
  if (!fOK)
  {
    wserror(("unable to start '%s' (ret: %d)", aCmd.c_str(), aRet));
  }
  else
  {
    wsinfo(("mailbox: '%s' returned: %d", aCmdFileName.c_str(), aRet));
  } // if

#if REMOVE_TEMP_FILES
  if (remove(aSrcFileName.c_str()) < 0)
  {
    LOG_ERROR_STR(std::string(std::string("while removing file '") + aSrcFileName + std::string("'")).c_str());
  } // if
#endif

  if (!fOK)
  {
    return aSrc;
  } // if
  
  //-----------------------------------------------------------------------
  //     read converted text
  //-----------------------------------------------------------------------
  fOK = false;

  std::string aDst;
  try
  {
    std::ifstream aFile(aDstFileName.c_str());
    if (!aFile)
    {
      throw std::string("unable to open file for reading");
    } // if

    std::string aLine;
    while (std::getline(aFile, aLine).good())
    {
      aDst += aLine + std::string("\n");
    } // while

    fOK = true;
  }
  catch (const std::string &aMsg)
  {
    wserror(("error while reading %s: %s", aDstFileName.c_str(), aMsg.c_str()));
  }
  catch (...)
  {
    wserror(("error while reading %s", aDstFileName.c_str()));
  } // catch

#if REMOVE_TEMP_FILES
  if (remove(aDstFileName.c_str()) < 0)
  {
    LOG_ERROR_STR(std::string(std::string("while removing file '") + aDstFileName + std::string("'")).c_str());
  } // if
#endif

  if (!fOK)
  {
    return aSrc;
  } // if

#if 0
  wsdebug(("------------------------------------------------------------------------------------"));
  wsdebug((aDst.c_str()));
  wsdebug(("------------------------------------------------------------------------------------"));
#endif
               
  wsdebug(("< AxMailWrapper::convertHtml2Text() time: %ld ms", long(aTimer.Elapsed())));
  return aDst;
} // AxMailWrapper::convertHtml2Text()

//-------------------------------------------------------------------------
//     AxMailWrapper::collectParts()
//-------------------------------------------------------------------------
void AxMailWrapper::collectParts(Ax::Mail::MailPartVector &thePartCltn, const Ax::Mail::MailPart *thePart)
{
  if (!thePart)
  {
    return;
  } // if
  
  wsdebug(("  AxMailWrapper::collectParts() Part: %-10s  Type: %-12s  SubType: %-14s  children: %2d"
         , thePart->getPartName().c_str()
         , thePart->getTypeStr().c_str()
         , thePart->getSubType().c_str()
         , thePart->getSubPartCount()
         ));

  bool fProcessChildren = true;

  switch (thePart->getType())
  {
    case Mail::BT_TEXT: ///< unformatted text
    {
      // add TEXT-parts (and children)
      thePartCltn.push_back(thePart);
      break;
    }

    case Mail::BT_MULTIPART: ///< multiple part
    {
      // if not all parts should be displayed
      // -> decide which part of a MULTIPART/ALTERNATIVE we can use
      if (!fShowAllParts)
      {
        // add one TEXT-child of a MULTIPART/ALTERNATIVE if present
        // or all children
        std::string aSubType = thePart->getSubType();
        std::transform(aSubType.begin(), aSubType.end(), aSubType.begin(), toupper);

        const Ax::Mail::MailPart *aTextPart = 0;

        // choose the best sub-type we can display
        // - if we find PLAIN(-text), we display this sub-part and abort loop
        // - if we find HTML, we display this sub-part and continue searching for PLAIN(-text)
        // - if neither PLAIN(-text) nor HTML was found, we display all children
        if (aSubType == "ALTERNATIVE")
        {
          for (Mail::MailPartVector::const_iterator anIter  = thePart->getSubParts().begin();
                                                    anIter != thePart->getSubParts().end();
                                                  ++anIter)
          {
            const Ax::Mail::MailPart *aChildPart = *anIter;
            if (aChildPart && aChildPart->getType() == Mail::BT_TEXT)
            {
              std::string aChildSubType = aChildPart->getSubType();
              std::transform(aChildSubType.begin(), aChildSubType.end(), aChildSubType.begin(), toupper);
              if (aChildSubType == "PLAIN")
              {
                aTextPart = aChildPart;
                break;
              }
              else if (aChildSubType == "HTML")
              {
                aTextPart = aChildPart;
              } // if
            } // if
          } // for

          // if we found a usable part
          // -> display it and avoid searching other children
          if (aTextPart)
          {
            fProcessChildren = false;
            thePartCltn.push_back(aTextPart);
          } // if
        } // if
      } // if
      break;
    }

    case Mail::BT_MESSAGE:     ///< encapsulated message
    {
      break;
    }
#if 0
    case Mail::BT_APPLICATION: ///< application data
    {
      break;
    }
    case Mail::BT_AUDIO:       ///< audio
    {
      break;
    }
    case Mail::BT_IMAGE:       ///< static image
    {
      break;
    }
    case Mail::BT_VIDEO:       ///< video
    {
      break;
    }
    case Mail::BT_MODEL:       ///< model
    {
      break;
    }
    case Mail::BT_OTHER:       ///< unknown
    {
      break;
    }
    case Mail::BT_MAX:         ///< maximum type code
    {
      break;
    }
    case Mail::BT_UNKNOWN:     ///< no one of the above
    {
      break;
    }
#endif
    default:
    {
      thePartCltn.push_back(thePart);
      break;
    }
  } // switch

  if (fProcessChildren)
  {
    for (Mail::MailPartVector::const_iterator anIter  = thePart->getSubParts().begin();
                                              anIter != thePart->getSubParts().end();
                                            ++anIter)
    {
      collectParts(thePartCltn, *anIter);
    } // for
  } // if
} // AxMailWrapper::collectParts()

//-------------------------------------------------------------------------
//     AxMailWrapper::addPartText()
//-------------------------------------------------------------------------
void AxMailWrapper::addPartText(LineCltn &theDst, const Ax::Mail::MailPart *thePart)
{
  if (!thePart)
  {
    return;
  } // if
  bool fLocalShowHeader = fShowHeader;
  wsdebug(("  AxMailWrapper::addPartText()  Part: %-10s  Type: %-10s  SubType: %-14s  children: %2d"
         , thePart->getPartName().c_str()
         , thePart->getTypeStr().c_str()
         , thePart->getSubType().c_str()
         , thePart->getSubPartCount()
         ));

  LineCltn aText;
  
  switch (thePart->getType())
  {
    case Mail::BT_TEXT: ///< unformatted text
    {
      std::string aSubType = thePart->getSubType();
      std::transform(aSubType.begin(), aSubType.end(), aSubType.begin(), toupper);

      if (fConvHtml && aSubType == "HTML")
      {
        wsdebug(("HTML-Part: %s", thePart->getPartName().c_str()));
        std::string aStr = convertHtml2Text(thePart);
        aText = splitText(aStr);
        dumpText("after html-conversion", aText);
      }
      else
      {
        // get the body of the mail part
        std::string aStr = thePart->getBodyText(); // quoted printable / base64 already handled in getBodyText()

        //----- convert to OSD-CharSet -----
        Ax::Mail::Tools::map2CharSet( aStr, thePart->getParam("charset")
                                    , aStr, AxVdrTools::getVdrCharSet()
                                    );

        //----- do formatting---------------
        aText = splitText(aStr);
        dumpText("after splitText", aText);

        if (myWrapMode >= WM_FLOWED && thePart->getParam("FORMAT") == "FLOWED")
        {
          aText = getFlowedText(aText);
          dumpText("after getFlowedText", aText);
        } // if

        if (myWrapMode >= WM_SMALL)
        {
          aText = improveWrap(aText);
          dumpText("after improveWrap", aText);
        } // if

        if (myWrapMode >= WM_NONE)
        {
          aText = wrapText(aText);
          dumpText("after wrapText", aText);
        }
      } // if

      if (aText.size() > 0 && fLocalShowHeader)
      {
        aText.push_front(AxLine("")); // add empty line before the text
        aText.push_back (AxLine("")); // add empty line after  the text
      } // if
      break;
    }

#if 0
    case Mail::BT_MULTIPART:   ///< multiple part
    {
      theDst.push_back(AxLine(Ax::Tools::String::sprintf("Type: %s", thePart->getTypeStr()))); // add empty line after the 'header'
      break;
    }
    case Mail::BT_MESSAGE:     ///< encapsulated message
    {
      break;
    }
    case Mail::BT_APPLICATION: ///< application data
    {
      break;
    }
    case Mail::BT_AUDIO:       ///< audio
    {
      break;
    }
    case Mail::BT_IMAGE:       ///< static image
    {
      break;
    }
    case Mail::BT_VIDEO:       ///< video
    {
      break;
    }
    case Mail::BT_MODEL:       ///< model
    {
      break;
    }
    case Mail::BT_OTHER:       ///< unknown
    {
      break;
    }
    case Mail::BT_MAX:         ///< maximum type code
    {
      break;
    }
    case Mail::BT_UNKNOWN:     ///< no one of the above
    {
      break;
    }
#endif
    default:
    {
      // if we can't display the contents of a part
      // -> we display at least the header line
      fLocalShowHeader = true;
//      aText.push_back(AxLine(Ax::Tools::String::sprintf("Type: %s", thePart->getTypeStr().c_str()))); // add empty line after the 'header'
      break;
    }
  } // switch

  if (fLocalShowHeader)
  {
    std::string aLine;
    if (fShowAllParts)
    { 
      aLine = Ax::Tools::String::sprintf(
                fHasOsdProps
              ? "---[%s %s: %s/%s/%s/%s]--------------------------------------------------------------------"
              : "---[%s %s: %s/%s/%s/%s]---"
              , tr("Part")
              , thePart->getPartName().c_str()
              , thePart->getTypeStr().c_str()
              , thePart->getEncStr().c_str()
              , thePart->getSubType().c_str()
              , thePart->getParam("charset").c_str()
              );
    }
    else
    {
      aLine = Ax::Tools::String::sprintf(
                fHasOsdProps
              ? "---[%s/%s/%s/%s]--------------------------------------------------------------------"
              : "---[%s/%s/%s/%s]---"
              , thePart->getTypeStr().c_str()
              , thePart->getEncStr().c_str()
              , thePart->getSubType().c_str()
              , thePart->getParam("charset").c_str()
              );
    } // if
    theDst.push_back(AxLine(cutLine(aLine), true));
  } // if

  if (aText.size() > 0)
  {
    theDst.insert(theDst.end(), aText.begin(), aText.end());
  } // if

} // AxMailWrapper::addPartText()

//-------------------------------------------------------------------------
//     AxMailWrapper::dumpText()
//-------------------------------------------------------------------------
void AxMailWrapper::dumpText(const std::string &theMsg, const LineCltn &theSrc)
{
#if TRACE_IS_ON
  wsdebug(("> AxMailWrapper::dumpText() %s,  size: %d", theMsg.c_str(), theSrc.size()));

  int i = 1;
  for (LineCltn::const_iterator anIter  = theSrc.begin();
                                anIter != theSrc.end();
                              ++anIter, ++i)
  {
    const AxLine aLine = (*anIter);
    wsdebug(("[%3d]  Sep: %d: '%s'", i, aLine.fSep, aLine.Text.c_str()));
  } // for

  wsdebug(("< AxMailWrapper::dumpText() %s,  size: %d", theMsg.c_str(), theSrc.size()));
#endif
} // AxMailWrapper::dumpText()
