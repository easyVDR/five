#ifndef __Ax_Mail_C_CLIENT_HEADER_H__
#define __Ax_Mail_C_CLIENT_HEADER_H__
//=============================================================================
/*
 * Description: This file contains a wrapper for the header of c-client
 *
 * See the file README in the main directory for a description of
 * this software, copyright information, and how to reach the author.
 *
 * Author: Alex
 * Date:   15.03.2006
 *
 * Last modfied:
 *   $Author: alex $
 *   $Date: 2013-03-20 19:53:42 +0100 (Mi, 20 Mär 2013) $
 */
//=============================================================================

//=============================================================================
//     includes
//=============================================================================

//----- c-client --------------------------------------------------------------
// the following undef is necessary since c-client defines
// _XOPEN_SOURCE without checking if it's already defined
#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif
#include <c-client.h>
// c-client defines min and max in misc.h to Min and Max
// and this causes trouble with stl-headers later on.
//
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

#endif

