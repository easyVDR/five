//=============================================================================
/*
 * See the file README in the main directory for a description of
 * this software, copyright information, and how to reach the author.
 *
 * Author: alex
 * Date:   07.09.2003
 *
 * Last modfied:
 *   $Author: alex $
 *   $Date: 2013-03-20 19:53:42 +0100 (Mi, 20 Mär 2013) $
 */
//=============================================================================

//=============================================================================
//     includes
//=============================================================================

//----- qt --------------------------------------------------------------------
//----- CORBA -----------------------------------------------------------------
//----- C++ -------------------------------------------------------------------
//----- C ---------------------------------------------------------------------
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

//----- AxLib -----------------------------------------------------------------
#include <Ax/Tools/Trace.h>
// #include <Ax/Tools/TraceOn.h>
#include <Ax/Tools/String.h>

//----- local -----------------------------------------------------------------

//=============================================================================
//     namespaces
//=============================================================================
namespace Ax    {
namespace Tools {

//=============================================================================
//     class String
//=============================================================================

//---------------------------------------------------------------------
//     String::split()
//---------------------------------------------------------------------
Ax::Tools::StringVector String::split( const std::string &theSrc
                                     , const std::string &theSep
                                     , bool               fAllowEmpty
                                     )
{
  StringVector           aVec;
  std::string            aSrc = theSrc;
  std::string::size_type aPos = std::string::npos;
  std::string::size_type aSepLen = theSep.length();

  do
  {
    aPos = aSrc.find(theSep);

    std::string aSub = "";
    if (aPos == std::string::npos)
    {
      aSub = aSrc;
    }
    else
    {
      aSub = aSrc.substr(0, aPos);
      aSrc = aSrc.substr(aPos + aSepLen, aSrc.length() - aPos - aSepLen);
    } // if

    if (aSub.length() > 0 || fAllowEmpty)
    {
      aVec.push_back(aSub);
    } // if
  } while (aPos != std::string::npos);

  return aVec;
} // String::split()

//---------------------------------------------------------------------
//     String::split()
//---------------------------------------------------------------------
Ax::Tools::StringVector String::split( const std::string &theSrc
                                     , char               theSep
                                     , bool               fAllowEmpty
                                     )
{
  return split(theSrc, std::string(1, theSep), fAllowEmpty);
} // String::split()

//---------------------------------------------------------------------
//     String::concat()
//---------------------------------------------------------------------
std::string String::concat(const StringVector &theVec, std::string theFill)
{
  return concat(theVec.begin(), theVec.end(), theFill);
} // String::concat()

//---------------------------------------------------------------------
//     String::concat()
//---------------------------------------------------------------------
std::string String::concat(const StringVector &theVec, char theFill)
{
  return concat(theVec.begin(), theVec.end(), std::string(1, theFill));
} // String::concat()

//---------------------------------------------------------------------
//     String::concat()
//---------------------------------------------------------------------
std::string String::concat( StringVector::const_iterator theBegin
                               , StringVector::const_iterator theEnd
                               , std::string                  theFill
                               )
{
  std::string aStr;

  for (StringVector::const_iterator anIter = theBegin; anIter != theEnd; ++anIter)
  {
    if (aStr.length() > 0) aStr += theFill;
    aStr += *anIter;
  } // for

  return aStr;
} // String::concat()

//---------------------------------------------------------------------
//     String::concat()
//---------------------------------------------------------------------
std::string String::concat( StringVector::const_iterator theBegin
                          , StringVector::const_iterator theEnd
                          , char                         theFill
                          )
{
  return concat(theBegin, theEnd, std::string(1, theFill));
} // String::concat()

//---------------------------------------------------------------------
//     String::stripWhiteSpaces()
//---------------------------------------------------------------------
std::string String::stripWhiteSpaces(const std::string &theSrc)
{
  std::string aResult;
  std::string::size_type aPosFirst = theSrc.find_first_not_of(" \t");
  std::string::size_type aPosLast  = theSrc.find_last_not_of (" \t");

  if ( aPosFirst != std::string::npos
    && aPosLast  != std::string::npos
     )
  {
    aResult = theSrc.substr(aPosFirst, aPosLast - aPosFirst + 1);
  }
  else if (aPosLast != std::string::npos)
  {
    aResult = theSrc.substr(0, aPosLast + 1);
  }
  else if (aPosFirst != std::string::npos)
  {
    aResult = theSrc.substr(aPosFirst);
  }
  else
  {
    // both failed -> the string doesn't contain any readable characters
    aResult = "";
  } // if

  return aResult;
} // String::stripWhiteSpaces()

//---------------------------------------------------------------------
//     String::sprintf()
//---------------------------------------------------------------------
std::string String::sprintf(const char *theFmt, ...)
{
#ifdef WIN32
  std::string aRet;
  
  // following code fragment was originally taken from "man vsprintf"

  /* Guess we need no more than 100 bytes. */
  int n, size = 100;
  char *p, *np;
  va_list ap;
  
  if ((p = (char *)malloc (size)) != NULL)
  {
    while (1) 
    {
      /* Try to print in the allocated space. */
      va_start(ap, theFmt);
      n = _vsnprintf(p, size, theFmt, ap);
      va_end(ap);
      
      /* If that worked, return the string. */
      if (n > -1 && n < size)
      {
         aRet = std::string(p);
         free(p);
         break;
      } // if
      
      /* Else try again with more space. */
      if (n > -1)    /* glibc 2.1 */
         size = n + 1; /* precisely what is needed */
      else           /* glibc 2.0 */
         size *= 2;  /* twice the old size */

      if ((np = (char *)realloc (p, size)) == NULL) 
      {
         free(p);
         break;
      } 
      else 
      {
         p = np;
      } // if
    } // while
  } // if
  
#else  

  va_list ap;
  va_start(ap, theFmt);
  char *buffer;
  if (vasprintf(&buffer, theFmt, ap) < 0)
  {
    return std::string();
  } // if

  std::string aRet(buffer);
  free(buffer);
  va_end(ap);
#endif

  return aRet;
} // String::sprintf()

//---------------------------------------------------------------------
//     String::getHexStr()
//---------------------------------------------------------------------
std::string String::getHexStr(const unsigned char *theData, int theLen)
{
  std::string aStr;

  for (int i = 0; i < theLen; ++i)
  {
    if (i > 0) aStr += " ";
    aStr += sprintf("%02X", (theData[i] & 0xFF));
  } // for

  return aStr;
} // String::getHexStr()


//=============================================================================
//     namespaces
//=============================================================================
} // Tools
} // Ax
