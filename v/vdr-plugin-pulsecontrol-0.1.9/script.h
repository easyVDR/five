#ifndef _script_h
#define _script_h

#include "loop.h"

class cPulseScriptName : public cPulseObject {
private:
  cString _filename;

public:
  cPulseScriptName(uint32_t index, const char *name, const char *filename)
   :cPulseObject(index, name)
   ,_filename(filename)
  {
  };

  virtual ~cPulseScriptName(void)
  {
  };

  const char *Filename(void) const
  {
    return *_filename;
  };
};

class cPulseScriptLine : public cListObject {
private:
  cString _line;

public:
  cPulseScriptLine(void);
  virtual ~cPulseScriptLine(void);
  
  bool Parse(const char *s);
  
  cPulseAction *ToAction(cPulseLoop &loop) const;
  };

class cPulseScript {
private:
  cPulseLoop _loop;
  cList<cPulseAction> _actions;

  cPulseScript(void);

public:
  static const char *cmdSetCardProfile;
  static const char *cmdSetDefaultSink;
  static const char *cmdSetSinkFormats;
  static const char *cmdMoveSinkInput;

  static cPulseScript *FromFile(const char *filename);
  static cPulseScript *FromLine(const char *line);
  
  virtual ~cPulseScript(void);
  
  int Run(void);
  };

#endif
