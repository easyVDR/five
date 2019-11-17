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
 *   $Date: 2013-03-20 22:41:44 +0100 (Mi, 20 Mär 2013) $
 *   $Revision: 670 $
 */
//=============================================================================

//=============================================================================
//     defines
//=============================================================================
#define CHARSET_WORKAROUND 1

//=============================================================================
//     includes
//=============================================================================

//----- qt --------------------------------------------------------------------
//----- CORBA -----------------------------------------------------------------
//----- ICE -------------------------------------------------------------------
//----- C++ -------------------------------------------------------------------
#include <algorithm>
#include <string>

//----- C ---------------------------------------------------------------------
#include <iconv.h>
#include <errno.h>
#include <ctype.h>

//----- AxLib -----------------------------------------------------------------
#include <Ax/Tools/Trace.h>
// #include <Ax/Tools/TraceOn.h>
#include <Ax/Mail/Tools.h>
#include <Ax/Mail/Globals.h>

//----- local -----------------------------------------------------------------
#include "c-client-header.h"

//=============================================================================
//     namespaces
//=============================================================================
namespace Ax    {
namespace Mail  {
namespace Tools {

//-----------------------------------------------------------------------
//     map2CharSet()
//-----------------------------------------------------------------------
bool map2CharSet( const std::string  &theSrc
                , const std::string  &theSrcCharSet
                ,       std::string  &theDst
                , const std::string  &theDstCharSet
                )
{
  wsdebug(("---------------------------------------------------------------------------"));
  wsdebug(("> Mail::Tools::map2CharSet() '%s' -> '%s'", theSrcCharSet.c_str(), theDstCharSet.c_str()));
#if 0
  wsdebug(("  Mail::Tools::map2CharSet() Src: '%s'", theSrc.c_str()));
#endif

  std::string aSrcCharSet = theSrcCharSet;
  std::string aDstCharSet = theDstCharSet;

  std::transform(aSrcCharSet.begin(), aSrcCharSet.end(), aSrcCharSet.begin(), toupper);
  std::transform(aDstCharSet.begin(), aDstCharSet.end(), aDstCharSet.begin(), toupper);

  // check some short-cuts
  if (theSrc.empty())
  {
    theDst = theSrc;
    wsdebug(("< Mail::Tools::map2CharSet() empty input -> returning!"));
    wsdebug(("---------------------------------------------------------------------------"));
    return true;
  } // if

  if (aDstCharSet.empty())
  {
    theDst = theSrc;
    wsdebug(("< Mail::Tools::map2CharSet() empty dst-charset -> returning!"));
    wsdebug(("---------------------------------------------------------------------------"));
    return false;
  } // if
  
#if CHARSET_WORKAROUND
  std::string aWoCharSet = "ISO-8859-1";
  if (aSrcCharSet.empty())
  {
    // Hinweis: Bei manchen Mails liefert mein Courier IMAP-Server keine Informationen
    //          aus dem Attribut "charset" z.B. aus der Zeile:
    //          "Content-Type: text/plain; charset=ISO-8859-1"
    //          und damit würden diese Mails nicht konvertiert werden.
    //          Als Workaround nehmen wir dann ISO-8859-1 an, was natürlich nicht
    //          100%ig korrekt ist.
    //          Aber lieber ab und zu eine 'falsche' Annahme,
    //          als dass diese Mails überhaupt nicht vernünftig lesbar sind.
    //
    wswarn(("! Mail::Tools::map2CharSet() WORKAROUND: src-charset is empty -> assuming '%s'", aWoCharSet.c_str()));
    aSrcCharSet = aWoCharSet;
  }
  else if ( aSrcCharSet == "UNKNOWN-8BIT"
         || aSrcCharSet == "UNKNOWN-7BIT"
          )
  {
    // Hinweis: Ist aSrcCharSet ein UNKOWN-xBIT, dann nehmen wir
    //          ISO-8859-1 an, was natürlich nicht 100%ig korrekt ist.
    //          Aber lieber ab und zu eine 'falsche' Annahme,
    //          als dass die meissten dieser Mails überhaupt nicht
    //          vernünftig lesbar sind.
    //
    wswarn(("! Mail::Tools::map2CharSet() WORKAROUND: charset reported '%s' -> assuming '%s'", aSrcCharSet.c_str(), aWoCharSet.c_str()));
    aSrcCharSet = aWoCharSet;
  } // if
#endif

  if (aSrcCharSet.empty())
  {
    theDst = theSrc;
    wsdebug(("< Mail::Tools::map2CharSet() empty src-charset -> returning!"));
    wsdebug(("---------------------------------------------------------------------------"));
    return false;
  } // if

  if (aSrcCharSet == aDstCharSet)
  {
    wsdebug(("< Mail::Tools::map2CharSet() same charset -> returning input unchanged!"));
    theDst = theSrc;
    return true;
  } // if

  iconv_t aCD = iconv_open(aDstCharSet.c_str(), aSrcCharSet.c_str());
  if (aCD == (iconv_t)-1)
  {
    theDst = theSrc;
    wserror(("ERROR: iconv conversion not supported from '%s' to '%s': %m"
           , aSrcCharSet.c_str()
           , aDstCharSet.c_str()
           ));
    wsdebug(("---------------------------------------------------------------------------"));
    return false;
  } // if

  // configure / copy input text
  size_t      aSizeIn = theSrc.length();
  char       *aBufIn  = (char *)malloc(aSizeIn + 1);
  char       *aPtrIn  = aBufIn;

  theSrc.copy(aBufIn, aSizeIn);
  aBufIn[aSizeIn]     = '\0';

  // configure output buffer 
  const size_t BUF_SIZE = 128;
  size_t       aSizeOut  = BUF_SIZE;
  char         aBufOut[BUF_SIZE + 1];
  char        *aPtrOut  = aBufOut;

  memset(aBufOut, 0, sizeof(aBufOut));

  // some helper
  std::string aDst;
  bool fOK      = true;
  int  aNumIter = 0;
  while (aSizeIn > 0)
  {
    ++aNumIter;
    wsdebug(("  > Iter: %3d   PosIn: %3d  SizeIn: %3d  SizeOut: %3d"
           , aNumIter, aPtrIn - aBufIn, aSizeIn, aSizeOut
           ));

    size_t aRet = iconv(aCD, &aPtrIn , &aSizeIn, &aPtrOut, &aSizeOut);
    int anErrNo = errno;

    wsdebug(("  < Iter: %3d   PosIn: %3d  SizeIn: %3d  SizeOut: %3d  Ret: %3d  errno: %d"
           , aNumIter, aPtrIn - aBufIn,  aSizeIn, aSizeOut, aRet, anErrNo
           ));

    if (aRet == size_t(-1))
    {
      if (aBufOut != aPtrOut)
      {
        // copy the remaining text and cleanup buffer
        aDst += std::string(aBufOut);
        memset(aBufOut, 0, sizeof(aBufOut));
        aPtrOut  = aBufOut;
        aSizeOut = BUF_SIZE;
      } // if

      switch (anErrNo)
      {
        case EILSEQ:
        {
          // case 1: invalid multibyte sequence encountered
          //         -> add '?' to dest-string
          //         -> skip one input character
          wsdebug(("  case 1: invalid multibyte sequence encountered, appending '?' and skipping one charcter...'"));
          aDst += "?";
          ++aPtrIn;
          --aSizeIn;
          break;
        }
        case EINVAL:
        {
          // case 3: incomplete multibyte sequence at end of input encountered
          //         -> add '?' to dest-string
          //         -> skip one input character
          wsdebug(("  case 3: incomplete multibyte sequence at end of input, appending '?' and skipping one charcter...'"));
          aDst += "?";
          ++aPtrIn;
          --aSizeIn;
          break;
        }
        case E2BIG:
        {
          // case 4: no more space in output buffer
          //         -> nothing to do, because output buffer already copied and cleared
          wsdebug(("  case 4: buffer full, iterating once more..."));
          break;
        }
      } // switch
    }
    else if (aSizeIn == 0)
    {
      wsdebug(("  Fall 2: SizeIn == 0"));
      aDst += std::string(aBufOut);
    }
    else
    {
      wserror(("while converting from %s to %s...", theSrcCharSet.c_str(), theDstCharSet.c_str()));
      wserror(("ERROR: unknown state while using iconv"));
      aSizeIn = 0;
      fOK = false;
    } // if
  } // while

  free(aBufIn);
  iconv_close(aCD);

  if (fOK)  theDst = aDst;
  else      theDst = theSrc;

  wsdebug(("  Mail::Tools::map2CharSet() SrcLen: %3d  DstLen: %3d  Iter: %3d"
         , theSrc.length(), theDst.length(), aNumIter
         ));
#if 0
  wsdebug(("  Mail::Tools::map2CharSet() Dst: '%s'", theDst.c_str()));
  wsdebug(("  Mail::Tools::map2CharSet() Dst: '%s'", theDst.c_str()));
#endif
               
  wsdebug(("< Mail::Tools::map2CharSet() fOK: %d", fOK));
  wsdebug(("---------------------------------------------------------------------------"));
  return fOK;
} // map2CharSet()

//-----------------------------------------------------------------------
//     encodeBase64()
//-----------------------------------------------------------------------
bool encodeBase64(const std::string &theSrc, std::string &theDst)
{
  wsdebug(("> Mail::Tools::encodeBase64() Src: '%s'", theSrc.c_str()));
  bool        fOK     = true;
  std::string aDst    = "";

  size_t         aSizeIn = theSrc.length();
  unsigned char *aBufIn  = (unsigned char *)malloc(aSizeIn + 1);
  memcpy(aBufIn, theSrc.c_str(), aSizeIn);
  aBufIn[aSizeIn]     = '\0';

  unsigned long aDecLen = 0;
  void *aDecoded = rfc822_binary(aBufIn, aSizeIn, &aDecLen);

  if (aDecoded)
  {
    theDst = std::string((char *)aDecoded, aDecLen);
    fs_give(&aDecoded);
  }
  else
  {
    fOK    = false;
    theDst = theSrc;
    wswarn(("Mail::Tools::encodeBase64() decoding of 'Base64' failed"));
  } // if

  wsdebug(("  Mail::Tools::encodeBase64() Dst: '%s'", theDst.c_str()));
  wsdebug(("< Mail::Tools::encodeBase64() fOK: %d", fOK));
  return fOK;
} // encodeBase64()

//-----------------------------------------------------------------------
//     decodeBase64()
//-----------------------------------------------------------------------
bool decodeBase64(const std::string &theSrc, std::string &theDst)
{
  wsdebug(("> Mail::Tools::decodeBase64() Src: '%s'", theSrc.c_str()));
  bool        fOK     = true;
  std::string aDst    = "";

  size_t         aSizeIn = theSrc.length();
  unsigned char *aBufIn  = (unsigned char *)malloc(aSizeIn + 1);
  memcpy(aBufIn, theSrc.c_str(), aSizeIn);
  aBufIn[aSizeIn]     = '\0';

  unsigned long aDecLen = 0;
  void *aDecoded = rfc822_base64(aBufIn, aSizeIn, &aDecLen);

  if (aDecoded)
  {
    theDst = std::string((char *)aDecoded);
    fs_give(&aDecoded);
  }
  else
  {
    fOK    = false;
    theDst = theSrc;
    wswarn(("Mail::Tools::decodeBase64() decoding of 'Base64' failed"));
  } // if

  wsdebug(("  Mail::Tools::decodeBase64() Dst: '%s'", theDst.c_str()));
  wsdebug(("< Mail::Tools::decodeBase64() fOK: %d", fOK));
  return fOK;
} // decodeBase64()

//-----------------------------------------------------------------------
//     decodeQuoted()
//-----------------------------------------------------------------------
bool decodeQuoted(const std::string &theSrc, std::string &theDst)
{
  wsdebug(("> Mail::Tools::decodeQuoted() Src: '%s'", theSrc.c_str()));

  bool fOK = true;

  size_t         aSizeIn = theSrc.length();
  unsigned char *aBufIn  = (unsigned char *)malloc(aSizeIn + 1);
  memcpy(aBufIn, theSrc.c_str(), aSizeIn);
  aBufIn[aSizeIn]     = '\0';

  unsigned long aDecLen = 0;
  void *aDecoded = rfc822_qprint(aBufIn, aSizeIn, &aDecLen);

  if (aDecoded)
  {
    theDst = std::string((char *)aDecoded);
    fs_give(&aDecoded);
  }
  else
  {
    fOK    = false;
    theDst = theSrc;
    wswarn(("Mail::Tools::decodeQuoted() decoding of 'quoted-printable' failed"));
  } // if

  wsdebug(("  Mail::Tools::decodeQuoted() Dst: '%s'", theDst.c_str()));
  wsdebug(("< Mail::Tools::decodeQuoted() fOK: %d", fOK));
  return fOK;
} // decodeQuoted()

//-----------------------------------------------------------------------
//     decodeEncodedWord()
//-----------------------------------------------------------------------
bool decodeEncodedWord( const std::string  &theSrc
                      , const std::string  &theEncoding
                      ,       std::string  &theDst
                      )
{
  wsdebug(("> Mail::Tools::decodeEncodedWord() Src: '%s'", theSrc.c_str()));
  wsdebug(("  Mail::Tools::decodeEncodedWord() Env: '%s'", theEncoding.c_str()));

  bool fOK = false;
  if (theEncoding == "Q")
  {
    // Q = quoted encoding
    fOK = Ax::Mail::Tools::decodeQuoted(theSrc, theDst);
  }
  else if (theEncoding == "B")
  {
    // B = Base64 encoding
    fOK = decodeBase64(theSrc, theDst);
  }
  else
  {
    // invalid encoding
    wswarn(("Mail::Tools::decodeEncodedWord() invalid encoding '%s'", theEncoding.c_str()));
    fOK    = false;
    theDst = theSrc;
  } // if

  wsdebug(("  Mail::Tools::decodeEncodedWord() Dst: '%s'", theDst.c_str()));
  wsdebug(("< Mail::Tools::decodeEncodedWord() fOK: %d", fOK));
  return fOK;
} // AxMailTools::decodeEncodedWord()

//-----------------------------------------------------------------------
//     decodeEncodedWords()
//-----------------------------------------------------------------------
bool decodeEncodedWords( const std::string  &theSrc
                       ,       std::string  &theDst
                       , const std::string  &theDstCharSet
                       , bool                fMsgHeader
                       )
{
  wsdebug(("> Mail::Tools::decodeEncodedWords() Source:  '%s'", theSrc.c_str()));
  wsdebug(("                                    Charset: '%s'", theDstCharSet.c_str()));
  wsdebug(("                                    MsgHead: %d", fMsgHeader));
  bool fOK      = true;
  std::string::size_type aPosStart = 0;

  std::string aDst = theSrc;

  // in message header we don't expect tab-characters
  // these may occur, if e.g. the subject line is wrapped
  // into multiple lines.
  if (fMsgHeader)
  {
    std::string::iterator aPos = std::remove(aDst.begin(), aDst.end(), '\t');
    if (aPos != aDst.end()) aDst.erase(aPos, aDst.end());
  } // if

  while ((aPosStart = aDst.find("=?", aPosStart)) != std::string::npos)
  {
    std::string::size_type aStrLen = aDst.length();
    std::string::size_type aPosQ1  = aPosStart == std::string::npos || aPosStart + 2 > aStrLen ? std::string::npos : aDst.find("?" , aPosStart + 2);
    std::string::size_type aPosQ2  = aPosQ1    == std::string::npos || aPosQ1    + 1 > aStrLen ? std::string::npos : aDst.find("?" , aPosQ1    + 1);
    std::string::size_type aPosEnd = aPosQ2    == std::string::npos || aPosQ2    + 1 > aStrLen ? std::string::npos : aDst.find("?=", aPosQ2    + 1);

    wsdebug(("  Mail::Tools::decodeEncodedWords() Start: %3d  Q1: %3d  Q2: %3d  End: %3d  Len: %d"
          , aPosStart, aPosQ1, aPosQ2, aPosEnd, aStrLen
          ));
    wsdebug(("  Mail::Tools::decodeEncodedWords() Remaining: '%s'", &(aDst.c_str()[aPosStart])));

    if ( aPosStart != std::string::npos
      && aPosQ1    != std::string::npos
      && aPosQ2    != std::string::npos
      && aPosEnd   != std::string::npos
       )
    {
      wsdebug(("  Mail::Tools::decodeEncodedWords() Start: %3d  Q1: %3d  Q2: %3d  End: %3d  Len: %d"
             , aPosStart, aPosQ1, aPosQ2, aPosEnd, aStrLen
             ));

      std::string aCharSet  = aDst.substr(aPosStart + 2, aPosQ1  - aPosStart - 2);
      std::string aEncoding = aDst.substr(aPosQ1    + 1, aPosQ2  - aPosQ1    - 1);
      std::string aText     = aDst.substr(aPosQ2    + 1, aPosEnd - aPosQ2    - 1);

      // convert charset and encoding to upper case
      std::transform(aCharSet .begin(), aCharSet .end(), aCharSet .begin(), toupper);
      std::transform(aEncoding.begin(), aEncoding.end(), aEncoding.begin(), toupper);

      wsdebug(("  Mail::Tools::decodeEncodedWords() charset: '%s'  encoding: '%s'  Text: '%s'"
             , aCharSet.c_str(), aEncoding.c_str(), aText.c_str()
             ));

      // in message titles spaces are encoded as '_' whereas '_' is encoded in '=5F'
      // so it's necessary to replace all '_' with ' '
      if (fMsgHeader)
      {
        std::replace(aText.begin(), aText.end(), '_', ' ');
      } // if

      if (!decodeEncodedWord(aText, aEncoding, aText))
      {
        wswarn(("Mail::Tools::decodeEncodedWords() unable to convert encoded words: charset: %-14s  encoding: '%s'  Text: '%s'"
              , aCharSet.c_str(), aEncoding.c_str(), aText.c_str()
              ));
        // continue after the current encoded word
        aPosStart = aPosEnd;
        fOK = false;
      }
      else
      {
        wsdebug(("  Mail::Tools::decodeEncodedWords() converting charset..."));
        if (!map2CharSet(aText, aCharSet, aText, theDstCharSet))
        {
          wswarn(("Mail::Tools::decodeEncodedWords() unable to convert charset: %-14s  encoding: '%s'  Text: '%s'"
                , aCharSet.c_str(), aEncoding.c_str(), aText.c_str()
                ));
        } // if

        wsdebug(("  Mail::Tools::decodeEncodedWords() replacing with: '%s'"
               , aText.c_str()
               ));

        // replace the current encoded word
        aDst.replace(aPosStart, aPosEnd - aPosStart + 2, aText);

        aPosStart += aText.length();
      } // if
    }
    else if (aPosEnd == std::string::npos)
    {
      // no further tries needed
      break;
    } // if
  } // while

  if (fOK)
  {
    theDst = aDst;
  }
  else
  {
    theDst = theSrc;
  } // if

  wsdebug(("  Mail::Tools::decodeEncodedWords() Dst: '%s'", theDst.c_str()));
  wsdebug(("< Mail::Tools::decodeEncodedWords() fOK: %d", fOK));
  return fOK;
} // AxMailTools::decodeEncodedWords()

//=============================================================================
//     namespaces
//=============================================================================
} // Tools
} // Mail
} // Ax
