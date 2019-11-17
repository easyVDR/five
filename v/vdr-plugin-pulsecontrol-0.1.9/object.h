#ifndef _object_h
#define _object_h

#include <vdr/tools.h>

class cPulseObject : public cListObject {
private:
  uint32_t _index;
  cString _name;

public:
  cPulseObject(uint32_t index, const char *name)
   :_index(index)
   ,_name(name)
  {
  }

  cPulseObject(const cPulseObject &object)
   :_index(object.Index())
   ,_name(object.Name())
  {
  }

  virtual ~cPulseObject(void)
  {
  }
  
  uint32_t Index() const { return _index; }
  const char *Name() const { return *_name; }
  
  void SetName(const char *name)
  {
    _name = name;
  }
 
  virtual int Compare(const cListObject &ListObject) const
  {
    const cPulseObject &rhs = (const cPulseObject&)ListObject;
    return strcmp(Name(), rhs.Name());
  }
  
  virtual cString MenuItemText(void) const
  {
    return cString::sprintf("%d - %s", _index, *_name);
  }
  };

template<class T> class cListHelper {
public:
  static void Copy(const cList<T> &src, cList<T> &dst)
  {
    dst.Clear();
    for (const T *o = src.First(); o; o = src.Next(o))
        dst.Add(new T(*o));
  }

  static const T *Find(const cList<T> &list, uint32_t index)
  {
    for (const T *o = list.First(); o; o = list.Next(o)) {
        const cPulseObject *obj = static_cast<const cPulseObject*>(o);
        if (obj->Index() == index)
           return o;
        }
    return NULL;
  }

  static T *Find(cList<T> &list, uint32_t index)
  {
    for (T *o = list.First(); o; o = list.Next(o)) {
        cPulseObject *obj = static_cast<cPulseObject*>(o);
        if (obj->Index() == index)
           return o;
        }
    return NULL;
  }

  static const T *Find(const cList<T> &list, const char *name)
  {
    if (name) {
       for (const T *o = list.First(); o; o = list.Next(o)) {
           const cPulseObject *obj = static_cast<const cPulseObject*>(o);
           if (strcasecmp(obj->Name(), name) == 0)
              return o;
           }
       }
    return NULL;
  }
  }; 
#endif
