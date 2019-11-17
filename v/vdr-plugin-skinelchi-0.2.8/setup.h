/*
 * setup.h: Setup and configuration file handling
 *
 * See the main source file 'skinelchi.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 */

#ifndef __SKINELCHI_SETUP_H
#define __SKINELCHI_SETUP_H

#include <vdr/plugin.h>
#include "config.h"


class cSkinElchiSetup : public cMenuSetupPage
{
private:
   cSkinElchiConfig tmpElchiConfig;
   void Setup(void);

protected:
   virtual eOSState ProcessKey(eKeys Key);
   virtual void Store(void);

public:
   cSkinElchiSetup(void);
   virtual ~cSkinElchiSetup();
};

#endif //__SKINELCHI_SETUP_H
