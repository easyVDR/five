#ifndef __ICONS_H
#define __ICONS_H

//taken from extrecmenu
class Icons
{
  private:
    static bool IsUTF8;
  public:
    static void InitCharSet();
    // symbols for menu
    static const char* Menu_Full_Star()   {return IsUTF8?"\uE018":"\x98";}  //ok
    static const char* Menu_Half_Star()   {return IsUTF8?"\uE019":"\x99";}  //ok
    static const char* Menu_Bomb()        {return IsUTF8?"\uE01A":"\x9A";}  //ok
    static const char* Menu_Note()        {return IsUTF8?"\uE01B":"\x9B";}  //ok
    static const char* Menu_Folder()      {return IsUTF8?"\uE002":"\x82";}  //ok
    static const char* Menu_Checkbox()    {return IsUTF8?"\uE016":"\x96";}  //ok
    static const char* Menu_CheckedBox()  {return IsUTF8?"\uE017":"\x97";}  //ok



    //symbols for OSD
    static const char* OSD_Shuffle()      {return IsUTF8?"\uE056":"\x21";}  //ok
    static const char* OSD_Replay()       {return IsUTF8?"\uE057":"\x22";}  //ok
    static const char* OSD_Record()       {return IsUTF8?"\uE058":"\x23";}  //ok
    static const char* OSD_Lyrics()       {return IsUTF8?"\uE059":"\x24";}  //ok
    static const char* OSD_Copy()         {return IsUTF8?"\uE061":"\x25";}  //ok
    static const char* OSD_Shutdown()     {return IsUTF8?"\uE062":"\x26";}  //ok
    static const char* OSD_Timer()        {return IsUTF8?"\uE063":"\x27";}  //ok
    static const char* OSD_Volume()       {return IsUTF8?"\uE064":"\x29";}  //ok
    static const char* OSD_Mute()         {return IsUTF8?"\uE065":"\x2a";}  //ok
    static const char* OSD_Full_Star()    {return IsUTF8?"\uE066":"\x2b";}  //ok
    static const char* OSD_Half_Star()    {return IsUTF8?"\uE067":"\x2c";}  //ok
    static const char* OSD_Bomb()         {return IsUTF8?"\uE068":"\x2d";}  //ok

};

#endif //__ICONS_H
