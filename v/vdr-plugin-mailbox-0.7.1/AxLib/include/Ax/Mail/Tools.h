#ifndef __Ax_Mail_Tools_H__
#define __Ax_Mail_Tools_H__
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
 *   $Revision: 665 $
 */
//=============================================================================

//=============================================================================
//     includes
//=============================================================================

//----- qt --------------------------------------------------------------------
//----- CORBA -----------------------------------------------------------------
//----- C++ -------------------------------------------------------------------
#include <string>

//----- C ---------------------------------------------------------------------
//----- AxLib -----------------------------------------------------------------
//----- vdr -------------------------------------------------------------------
//----- local -----------------------------------------------------------------
//----- other includes --------------------------------------------------------

//=============================================================================
//     forward declarations
//=============================================================================
class cDevice;
class cChannel;

//=============================================================================
//     namespaces
//=============================================================================
namespace Ax    {
namespace Mail  {
namespace Tools {

//-----------------------------------------------------------------------
//     map2CharSet()
//-----------------------------------------------------------------------
/** maps the given source-string from the source charset to the
 *  destination charset and stores the string in theDst.
 *
 *  @param theSrc        source string
 *  @param theSrcCharSet source charset
 *  @param theDst        destination string (result is stored there)
 *  @param theSrcCharSet destination charset
 */
bool map2CharSet( const std::string  &theSrc
                , const std::string  &theSrcCharSet
                ,       std::string  &theDst
                , const std::string  &theDstCharSet
                );

//-----------------------------------------------------------------------
//     encodeBase64)
//-----------------------------------------------------------------------
/** encode base-64
 *
 *  @param theSrc input string
 *  @param theDst output string
 *
 *  @returns true if encoded successful and theDst ist the encodeded string
 *           false if encoded not sucessful, theDst contains theSrc
 */
bool encodeBase64(const std::string &theSrc, std::string &theDst);

//-----------------------------------------------------------------------
//     decodeBase64()
//-----------------------------------------------------------------------
/** decode base-64
 *
 *  @param theSrc input string
 *  @param theDst output string
 *
 *  @returns true if decoded successful and theDst ist the decodeded string
 *           false if decoded not sucessful, theDst contains theSrc
 */
bool decodeBase64(const std::string &theSrc, std::string &theDst);

//-----------------------------------------------------------------------
//     decodeQuoted()
//-----------------------------------------------------------------------
/** decode quoted-printable
 *
 *  @param theSrc input string
 *  @param theDst output string
 *
 *  @returns true if decoded successful and theDst ist the decodeded string
 *           false if decoded not sucessful, theDst contains theSrc
 */
bool decodeQuoted(const std::string &theSrc, std::string &theDst);

//-----------------------------------------------------------------------
//     decodeEncodedWord()
//-----------------------------------------------------------------------
/**
 *  @param theSrc      encoded input string
 *  @param theEncoding is one of "B" for Base64 or "Q" for quoted-printable
 *  @param theDst      decoded output string
 */
bool decodeEncodedWord( const std::string  &theSrc
                      , const std::string  &theEncoding
                      ,       std::string  &theDst
                      );


//-----------------------------------------------------------------------
//     decodeEncodedWords()
//-----------------------------------------------------------------------
/** decodes encoded words and maps each of them to the desired
 *  target charset
 *
 *
 *  @param theSrc        encoded input string
 *  @param theDst        decoded output string
 *  @param theDstCharSet desired charset for theDst-string
 *  @param fMsgHeader    replace '_' with ' ' before decoding
 */
bool decodeEncodedWords( const std::string  &theSrc
                        ,       std::string  &theDst
                        , const std::string  &theDstCharSet
                        , bool                fMsgHeader
                        );

//=============================================================================
//     namespaces
//=============================================================================
} // Tools
} // Mail
} // Ax

#endif

