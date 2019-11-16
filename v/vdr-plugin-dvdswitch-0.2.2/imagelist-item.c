#include "imagelist-item.h"

cImageListItem::cImageListItem(const char *lname, const char *sname, eFileInfo type, const char *value, bool hide)
{
  LName = NULL;
  SName = NULL;
  Value = 0;
  SString = NULL;

  Edit(lname, sname, type, value, hide);
}

cImageListItem::~ cImageListItem(void)
{
  free(LName);
  free(SName);
  free(Value);
  free(SString);
}

void cImageListItem::Edit(const char *lname, const char *sname, eFileInfo type, const char *value, bool hide)
{
  free(LName);
  free(SName);
  free(Value);

  LName = lname ? strdup(lname) : NULL;
  SName = sname ? strdup(sname) : NULL;

  if(type == tFile && value && value[0] != '.') {
    if(0 >= asprintf(&Value, ".%s", value)) {
      return;
    }
  }
  else
    Value = value ? strdup(value) : NULL;

  fType = type;
  HideExt = hide;

  MakeSetupString();
}
