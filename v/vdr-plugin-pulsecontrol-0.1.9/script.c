#include "script.h"

#include <vdr/config.h>

#include "action_listsinkinputs.h"
#include "action_listsinks.h"
#include "action_movesinkinput.h"
#include "action_saveformats.h"
#include "action_setcardprofile.h"
#include "action_setdefaultsink.h"

const char *cPulseScript::cmdSetCardProfile = "set-card-profile ";
const char *cPulseScript::cmdSetDefaultSink = "set-default-sink ";
const char *cPulseScript::cmdSetSinkFormats = "set-sink-formats ";
const char *cPulseScript::cmdMoveSinkInput = "move-sink-input ";

cPulseScriptLine::cPulseScriptLine(void)
{
}
  
cPulseScriptLine::~cPulseScriptLine(void)
{
}
  
bool cPulseScriptLine::Parse(const char *s)
{
  _line = s;
  return true;
}

cPulseAction *cPulseScriptLine::ToAction(cPulseLoop &loop) const
{
  if (!*_line || !**_line)
     return NULL;

  if (startswith(*_line, cPulseScript::cmdSetCardProfile)) {
     const char *option = skipspace(*_line + strlen(cPulseScript::cmdSetCardProfile));
     cString card;
     const char *profile = NULL;
     if (!option || !*option) {
        esyslog("pulsecontrol: missing name of card and profile");
        return NULL;
        }
     profile = option;
     while ((*profile != 0) && (*profile != ' '))
           profile++;
     card = cString(option, profile);
     if (*profile != 0)
        profile++;
     if (*card == NULL) {
        esyslog("pulsecontrol: missing name of card");
        return NULL;
        }
     if (*profile == 0) {
        esyslog("pulsecontrol: missing name of profile");
        return NULL;
        }
     return new cPulseSetCardProfileAction(loop, *card, profile);
     }

  if (startswith(*_line, cPulseScript::cmdSetDefaultSink)) {
     const char *option = skipspace(*_line + strlen(cPulseScript::cmdSetDefaultSink));
     if (!option || !*option) {
        esyslog("pulsecontrol: missing sink");
        return NULL;
        }
     return new cPulseSetDefaultSinkAction(loop, option);
     }

  if (startswith(*_line, cPulseScript::cmdSetSinkFormats)) {
     const char *option = skipspace(*_line + strlen(cPulseScript::cmdSetSinkFormats));
     cString sink;
     const char *formats = NULL;
     if (!option || !*option) {
        esyslog("pulsecontrol: missing name of sink and formats");
        return NULL;
        }
     formats = option;
     while ((*formats != 0) && (*formats != ' '))
           formats++;
     sink = cString(option, formats);
     if (*formats != 0)
        formats++;
     if (*sink == NULL) {
        esyslog("pulsecontrol: missing name of sink");
        return NULL;
        }
     if (*formats == 0) {
        esyslog("pulsecontrol: missing formats");
        return NULL;
        }
     cList<cPulseFormat> f;
     f.Add(new cPulseFormat(PA_ENCODING_PCM)); // you always want this encoding!
     if (!cPulseFormat::FromString(formats, f) || (f.Count() == 1)) {
        esyslog("pulsecontrol: error in formats");
        return NULL;
        }
     uint32_t index = PA_INVALID_INDEX;
     if (isnumber(*sink))
        index = strtol(*sink, NULL, 10);
     else {
        cPulseLoop l;
        cPulseListSinksAction sinks(l);
        int ret = l.Run();
        if (ret != 0) {
           esyslog("pulsecontrol: error %d", ret);
           return NULL;
           }
        const cPulseSink *s = cListHelper<cPulseSink>::Find(sinks.Sinks(), *sink);
        if (!s) {
           esyslog("pulsecontrol: unknown sink %s", *sink);
           return NULL;
           }
        index = s->Index();
        }
     return new cPulseSaveFormatsAction(loop, index, f);
     }

  if (startswith(*_line, cPulseScript::cmdMoveSinkInput)) {
     const char *option = skipspace(*_line + strlen(cPulseScript::cmdMoveSinkInput));
     cString input;
     uint32_t index = PA_INVALID_INDEX;
     uint32_t current_sink = PA_INVALID_INDEX;
     const char *sink = NULL;
     if (!option || !*option) {
        esyslog("pulsecontrol: missing name of input and sink");
        return NULL;
        }
     sink = option + strlen(option) - 1;
     while ((sink > option) && (*sink != ' '))
           sink--;
     if (sink == option) {
        cPulseLoop l;
        cPulseListSinkInputsAction inputs(l);
        int ret = l.Run();
        if (ret != 0) {
           esyslog("pulsecontrol: error %d", ret);
           return NULL;
           }
        if (inputs.SinkInputs().Count() != 1) {
           esyslog("pulsecontrol: missing name of input");
           return NULL;
           }
        index = inputs.SinkInputs().First()->Index();
        input = inputs.SinkInputs().First()->Name();
        current_sink = inputs.SinkInputs().First()->Sink();
        }
     else {
        input = cString(option, sink);
        if (*input == NULL) {
           esyslog("pulsecontrol: missing name of input");
           return NULL;
           }
        sink++;
        if (*sink == 0) {
           esyslog("pulsecontrol: missing name of sink");
           return NULL;
           }
        cPulseLoop l;
        cPulseListSinkInputsAction inputs(l);
        int ret = l.Run();
        if (ret != 0) {
           esyslog("pulsecontrol: error %d", ret);
           return NULL;
           }
        const cPulseSinkInput *i = NULL;
        if (isnumber(*input)) {
           index = strtol(*input, NULL, 10);
           i = cListHelper<cPulseSinkInput>::Find(inputs.SinkInputs(), index);
           }
        else {
           i = cListHelper<cPulseSinkInput>::Find(inputs.SinkInputs(), *input);
           }
        if (!i) {
           esyslog("pulsecontrol: unknown sink input %s", *input);
           return NULL;
           }
        index = i->Index();
        current_sink = i->Sink();
        }
     if (strcmp(sink, "next") == 0) {
        cPulseLoop l;
        cPulseListSinksAction sinks(l);
        int ret = l.Run();
        if (ret != 0) {
           esyslog("pulsecontrol: error %d", ret);
           return NULL;
           }
        if (sinks.Sinks().Count() == 0) {
           esyslog("pulsecontrol: can't find any sinks");
           return NULL;
           }
        const cPulseSink *s = cListHelper<cPulseSink>::Find(sinks.Sinks(), current_sink);
        if (!s) {
           esyslog("pulsecontrol: can't find current sink %d", current_sink);
           return NULL;
           }
        const cPulseSink *next_sink = sinks.Sinks().Next(s);
        if (!next_sink)
           next_sink = sinks.Sinks().First();
        return new cPulseMoveSinkInputAction(loop, index, next_sink->Index()); 
        }
     return new cPulseMoveSinkInputAction(loop, index, sink);
     }
  return NULL;
}


cPulseScript::cPulseScript(void)
{
}

cPulseScript::~cPulseScript(void)
{
}

cPulseScript *cPulseScript::FromFile(const char *filename)
{
  cConfig<cPulseScriptLine> lines;
  if (!lines.Load(filename, true, true)) {
     esyslog("pulsecontrol: error on reading script %s", filename);
     return NULL;
     }
  cPulseScript *script = new cPulseScript();
  for (const cPulseScriptLine *s = lines.First(); s; s = lines.Next(s)) {
      cPulseAction *a = s->ToAction(script->_loop);
      if (a)
         script->_actions.Add(a);
      }
  return script;
}

cPulseScript *cPulseScript::FromLine(const char *line)
{
  cPulseScriptLine l;
  if (!l.Parse(line)) {
     esyslog("pulsecontrol: error on reading line %s", line);
     return NULL;
     }
  cPulseScript *script = new cPulseScript();
  cPulseAction *a = l.ToAction(script->_loop);
  if (a)
     script->_actions.Add(a);
  return script;
}

int cPulseScript::Run(void)
{
  return _loop.Run();
}
