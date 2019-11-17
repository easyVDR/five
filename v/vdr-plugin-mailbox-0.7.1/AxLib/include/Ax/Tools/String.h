#ifndef __Ax_Tools_String_H__
#define __Ax_Tools_String_H__
//=============================================================================
/*
 * Description: This file contains the class Ax::Tools::String
 *
 * See the file README in the main directory for a description of
 * this software, copyright information, and how to reach the author.
 *
 * Author: alex
 * Date:   16.12.2002
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
#include <string>
#include <vector>

//----- C ---------------------------------------------------------------------
//----- AxLib -----------------------------------------------------------------
#include <Ax/Tools/Export.h>

//----- local -----------------------------------------------------------------

//=============================================================================
//     namespaces
//=============================================================================
namespace Ax    {
namespace Tools {

//=============================================================================
//     typedefs
//=============================================================================
typedef std::vector<std::string> StringVector;

//=============================================================================
//     class String
//=============================================================================
class AX_TOOLS_EXPORT String
{
  public:
    //---------------------------------------------------------------------
    //     split()
    //---------------------------------------------------------------------
    /** Splits the given source string at every occurency of the separator
     *  and return a vector of strings.
     *
     *  @param theSrc source string to be splitted
     *  @param theSep separator string
     *  @param fAllowEmpty allow empty strings in the returned array
     *  @returns
     */
    static StringVector split( const std::string &theSrc
                             , const std::string &theSep
                             , bool               fAllowEmpty
                             );

    //---------------------------------------------------------------------
    //     split()
    //---------------------------------------------------------------------
    /** Splits the given source string at every occurency of the separator
     *  and return a vector of strings.
     *
     *  @param theSrc source string to be splitted
     *  @param theSep separator string
     *  @param fAllowEmpty allow empty strings in the returned array
     *  @returns
     */
    static StringVector split( const std::string &theSrc
                             , char               theSep
                             , bool               fAllowEmpty
                             );

    //---------------------------------------------------------------------
    //     concat()
    //---------------------------------------------------------------------
    /** Concatenates all members of theVec to one big string and adds
     *  theFill between all the substrings.
     */
    static std::string concat( const StringVector &theVec
                             , std::string         theFill = ""
                             );

    //---------------------------------------------------------------------
    //     concat()
    //---------------------------------------------------------------------
    /** Concatenates all members of theVec to one big string and adds
     *  theFill between all the substrings.
     */
    static std::string concat( const StringVector &theVec
                             , char                theFill
                             );

    //---------------------------------------------------------------------
    //     concat()
    //---------------------------------------------------------------------
    /** Concatenates all members of theVec to one big string and adds
     *  theFill between all the substrings.
     */
    static std::string concat( StringVector::const_iterator theBegin
                             , StringVector::const_iterator theEnd
                             , std::string                  theFill = ""
                             );

    //---------------------------------------------------------------------
    //     concat()
    //---------------------------------------------------------------------
    /** Concatenates all members of theVec to one big string and adds
     *  theFill between all the substrings.
     */
    static std::string concat( StringVector::const_iterator theBegin
                             , StringVector::const_iterator theEnd
                             , char                         theFill
                             );

    //---------------------------------------------------------------------
    //     stripWhiteSpaces()
    //---------------------------------------------------------------------
    /** Strip away the white-spaces (space and tab) at the beginning
     *  and the end of the given string and returns the resulting string.
     */
    static std::string stripWhiteSpaces(const std::string &theSrc);

    //---------------------------------------------------------------------
    //     sprintf()
    //---------------------------------------------------------------------
    /**  [a]sprintf-like function to create a std::string
     */
    static std::string sprintf(const char *theFmt, ...)  
#ifndef WIN32
      __attribute__ ((format (printf, 1, 2)))
#endif
      ;

    //---------------------------------------------------------------------
    //     getHexStr()
    //---------------------------------------------------------------------
    /**  converts the given bytes to a readable representation
     */
    static std::string getHexStr(const unsigned char *theData, int theLen);

   
}; // class String

//=============================================================================
//     namespaces
//=============================================================================
} // Tools
} // Ax

#endif

