#ifndef _IMAGELOAD_BITMAP_H
#define _IMAGELOAD_BITMAP_H

#include <vdr/osd.h>

class cImageloadBitmap
{
private:
  int newwidth;
  int newheight;

  uint8_t *data;

public:
  cImageloadBitmap();
  virtual ~cImageloadBitmap();
  bool Load(const char *Filename);
  void LoadImage(const char *Filename, int witdh=0, int height=0, bool ratio=false, int startx=0, bool isProgressbar=false);
  void Render(cImage &bmp);

  int newHeight(void) { return newheight; }
  int newWidth(void) { return newwidth; }
};


#endif // _IMAGELOAD_BITMAP_H
























