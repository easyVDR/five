#ifndef _format_h
#define _format_h

#include "object.h"
#include <pulse/pulseaudio.h>

class cPulseFormat : public cPulseObject {
public:
  cPulseFormat(pa_encoding_t encoding)
   :cPulseObject(encoding, pa_encoding_to_string(encoding))
  {
  }

  cPulseFormat(const cPulseFormat &format)
   :cPulseObject(format)
  {
  }

  virtual ~cPulseFormat(void)
  {
  }

  virtual cString MenuItemText(void) const
  {
    return cString::sprintf("%s", Name());
  }

  pa_encoding_t Encoding(void) const
  {
    return (pa_encoding_t)Index();
  }

  pa_format_info *ToFormatInfo(void) const
  {
    pa_format_info *f = pa_format_info_new();
    f->encoding = Encoding();
    return f;
  }

  static void AllFormats(cList<cPulseFormat> &list)
  {
    list.Clear();
    for (pa_encoding_t enc = (pa_encoding_t)(PA_ENCODING_ANY + 1); enc < PA_ENCODING_MAX; enc = (pa_encoding_t)(enc + 1)) 
        list.Add(new cPulseFormat(enc));
  }

  static bool FromString(const char *encodings, cList<cPulseFormat> &list)
  {
    if (!encodings || !(*encodings))
       return false;

    // workaround for missing pa_encoding_from_string
    cList<cPulseFormat> all;
    cPulseFormat::AllFormats(all);

    char *str = strdup(encodings);
    const char *delim = ";"; 
    char *strtok_next = NULL;
    for (char *encoding = strtok_r(str, delim, &strtok_next); encoding; encoding = strtok_r(NULL, delim, &strtok_next)) {
        const cPulseFormat *f = cListHelper<cPulseFormat>::Find(all, encoding);
        if (f && (cListHelper<cPulseFormat>::Find(list, f->Index()) == NULL))
           list.Add(new cPulseFormat(*f));
        }
    free(str);
    return true;
  }
  };

#endif
