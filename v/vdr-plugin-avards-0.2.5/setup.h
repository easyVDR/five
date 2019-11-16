/*
 * setup.h: configuration file handling
 *
 * See the main source file 'avards.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 */

#include <vdr/plugin.h>

#define MAX_MODES 6

class cMenuSetupAvards : public cMenuSetupPage {
private:
   cParameter tmpconfig;
   const char *modes[MAX_MODES];
protected:
   virtual void Store(void);
public:
   cMenuSetupAvards(void);
};
