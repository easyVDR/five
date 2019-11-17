
#include <vdr/thread.h>

#include "config.h"

#if VDRVERSNUM >=10712

// --- cCommand --------------------------------------------------------------

char *cCommand::result = NULL;

cCommand::cCommand(void)
{
  title = command = NULL;
  confirm = false;
}

cCommand::~cCommand()
{
  free(title);
  free(command);
}

bool cCommand::Parse(const char *s)
{
  const char *p = strchr(s, ':');
  if (p) {
     int l = p - s;
     if (l > 0) {
        title = MALLOC(char, l + 1);
        stripspace(strn0cpy(title, s, l + 1));
        if (!isempty(title)) {
           int l = strlen(title);
           if (l > 1 && title[l - 1] == '?') {
              confirm = true;
              title[l - 1] = 0;
              }
           command = stripspace(strdup(skipspace(p + 1)));
           return !isempty(command);
           }
        }
     }
  return false;
}

const char *cCommand::Execute(const char *Parameters)
{
  free(result);
  result = NULL;
  cString cmdbuf;
  if (Parameters)
     cmdbuf = cString::sprintf("%s %s", command, Parameters);
  const char *cmd = *cmdbuf ? *cmdbuf : command;
  dsyslog("executing command '%s'", cmd);
  cPipe p;
  if (p.Open(cmd, "r")) {
     int l = 0;
     int c;
     while ((c = fgetc(p)) != EOF) {
           if (l % 20 == 0)
              result = (char *)realloc(result, l + 21);
           result[l++] = char(c);
           }
     if (result)
        result[l] = 0;
     p.Close();
     }
  else
     esyslog("ERROR: can't open pipe for command '%s'", cmd);
  return result;
}

#endif

