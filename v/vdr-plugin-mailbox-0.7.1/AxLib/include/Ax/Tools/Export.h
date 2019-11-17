#ifndef __AX_Tools_Export_H__
#define __AX_Tools_Export_H__
//=============================================================================
/*
 * See the file README in the main directory for a description of
 * this software, copyright information, and how to reach the author.
 *
 * Author: alex
 * Date:   09.08.2006
 *
 * Last modfied:
 *   $Author: alex $
 *   $Date: 2013-03-20 19:53:42 +0100 (Mi, 20 Mär 2013) $
 */
//=============================================================================

//=============================================================================
//     defines
//=============================================================================
#if defined(WIN32)
#  if defined(AX_TOOLS_NODLL)
#    undef AX_TOOLS_MAKEDLL
#    undef AX_TOOLS_DLL
#  endif
#  if defined(AX_TOOLS_MAKEDLL)       /* create a AxTools DLL */
#    undef  AX_TOOLS_DLL
#    define AX_TOOLS_EXPORT  __declspec(dllexport)
#  endif
#  if defined(AX_TOOLS_DLL)           /* use a AxTools DLL */
#    define AX_TOOLS_EXPORT  __declspec(dllimport)
#  endif
#else // ! _WIN32
#  undef AX_TOOLS_MAKEDLL             /* ignore these for other platforms */
#  undef AX_TOOLS_DLL
#endif

#ifndef AX_TOOLS_EXPORT
#  define AX_TOOLS_EXPORT
#endif

//-----------------------------------------------------------------------------
#endif
