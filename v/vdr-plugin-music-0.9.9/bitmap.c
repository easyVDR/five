#include <string>

#ifdef HAVE_IMAGEMAGICK

#define X_DISPLAY_MISSING
#include <Magick++.h>
using namespace Magick;

#else

#include <Imlib2.h>

#endif

#include "bitmap.h"


static int FileExists(const char *FileName) {
  FILE *f = NULL;
  f = fopen( FileName, "rb" );
  if (f != NULL ) {
    fclose(f);
    return true;
    }
  return false;
}

cImageloadBitmap::cImageloadBitmap()
{
}

cImageloadBitmap ::~cImageloadBitmap()
{
}


bool cImageloadBitmap::Load(const char *Filename)
{
#ifdef DEBUG_COVER
  printf("music: bitmap.c : check for image '%s'\n", Filename);
#endif

  if(FileExists(Filename)) {
#ifdef DEBUG_COVER
    printf("music: bitmap.c : image found\n");
#endif
    return true;
    }
  else {
#ifdef DEBUG_COVER
    printf("music: bitmap.c : imagefile dont exists\n");
#endif
    esyslog("music: bitmap.c : imagefile dont exists");
    return false;
  }

}

#ifdef HAVE_IMLIB2
// IMLIB2

  Imlib_Image image;
  Imlib_Context ctx;

void cImageloadBitmap::LoadImage( const char *Filename, int width, int height, bool ratio, int startx, bool isProgressbar)
{
//  Imlib_Image image;

  image = NULL;

#ifdef DEBUG_COVER
  printf("music: bitmap.c : load image '%s'\n",Filename);
  printf("music: bitmap.c : values: width=%d height=%d ratio=%d startx=%d isProgressbar=%d\n", width, height, ratio, startx, isProgressbar );
#endif

  newwidth  = 0;
  newheight = 0;

  image = imlib_load_image(Filename);

  if (!image || height <=0 || width <=0) {
#ifdef DEBUG_COVER
    printf("music: bitmap.c: no image or wrong dimension\n");
#endif
    esyslog("music: bitmap.c: no image or wrong dimension");
    }
  else {
    ctx = imlib_context_new();
    imlib_context_push(ctx);

    if(isProgressbar) {
      imlib_context_set_image(image);
      image = imlib_create_cropped_image(startx, 0, width, height);
#ifdef DEBUG_COVER
      printf("music: bitmap.c: create a progressbar\n");
#endif
      }
    else {
      if(!ratio) {
        imlib_context_set_image(image);
        image = imlib_create_cropped_scaled_image(0, 0, imlib_image_get_width(), imlib_image_get_height(), width, height); 
#ifdef DEBUG_COVER
        printf("music: bitmap.c: create an image without aspect ratio:\nimlib_image_get_width=%i imlib_image_get_height=%i\n", imlib_image_get_width(), imlib_image_get_height());
#endif
        }
      else {
        imlib_context_set_image(image);

        int x1 = imlib_image_get_width();
        int y1 = imlib_image_get_height();
        int x2 = width;
        int y2 = height;
        float stretched = float(x1) * (float(y2) / float(y1));

#ifdef DEBUG_COVER
        printf("music: bitmap.c: create an image with aspect ratio x1=%i y1=%i x2=%i y2=%i\n", x1 ,y1 ,x2, y2);
#endif
        image = imlib_create_cropped_scaled_image(0, 0, x1, y1, (int)stretched < x2 ? (int)stretched : x2, y2);
      }
    }
    imlib_context_set_image(image);

    newwidth  = imlib_image_get_width();
    newheight = imlib_image_get_height();

#ifdef DEBUG_COVER
    printf("music: bitmap.c: new values of image: width=%d  height=%d\n", newwidth, newheight);
#endif

  }
}

void cImageloadBitmap::Render(cImage &bmp)
{

#ifdef DEBUG_COVER
  printf("music: bitmap.c: start to render image\n");
#endif

  for (int ix = 0; ix < newwidth; ++ix) {
    for (int iy = 0; iy < newheight; ++iy) {

      Imlib_Color iml_col;
      imlib_image_query_pixel(ix,iy,&iml_col);

      tColor col = (uint8_t)iml_col.alpha << 24 | (uint8_t)iml_col.red << 16 | (uint8_t)iml_col.green << 8 | (uint8_t)iml_col.blue;

      bmp.SetPixel(cPoint(ix, iy), col);

    }
  }

#ifdef DEBUG_COVER
  printf("music: bitmap.c: image rendered !\n");
#endif

#ifdef DEBUG_COVER
  printf("music: bitmap.c: imlib_free_image()\n");
#endif
  if(image) imlib_free_image();


#ifdef DEBUG_COVER
  printf("music: bitmap.c: imlib_context_free()\n");
#endif
  if(ctx) imlib_context_free(ctx);

}

#else
// MAGICK++

Image image;

void cImageloadBitmap::LoadImage( const char *Filename, int width, int height, bool ratio, int startx, bool isProgressbar)
{
  std::string geometry;

  newwidth  = 0;
  newheight = 0;

#ifdef DEBUG_COVER
      printf("music: bitmap.c: Load image '%s'\nwidth=%d , height=%d , ratio=%i , startx=%d , isProgressbar=%i\n" , Filename , width, height , ratio , startx , isProgressbar);
#endif



  if ( height <=0 || width <=0) {
#ifdef DEBUG_COVER
    printf("music: bitmap.c: no image or wrong dimension\n");
#endif
    esyslog("music: bitmap.c: no image or wrong dimension");
    }
  else {

    try {
      image.read(Filename);

#ifdef DEBUG_COVER
    printf("music: bitmap.c: original image dimension = %dx%d\n", image.columns(), image.rows());
#endif


      if(isProgressbar) {
#ifdef DEBUG_COVER
        printf("music: bitmap.c: create a progressbar\n");
#endif
        geometry = Geometry(width, height , startx, 0);
        image.filterType(LanczosFilter);
        image.crop(geometry);
        }
      else {
        if(!ratio) {
          geometry = Geometry(width, height);
          image.filterType(LanczosFilter);
          image.sample(geometry);
#ifdef DEBUG_COVER
        printf("music: bitmap.c: create an image without aspect ratio\n");
#endif
          }
        else {

          int x1 = image.columns();
          int y1 = image.rows();
          int x2 = width;
          int y2 = height;
          float stretched = float(x1) * (float(y2) / float(y1));
          geometry = Geometry((int)stretched < x2 ? (int)stretched : x2, y2);
          image.filterType(LanczosFilter);
          image.sample(geometry);
#ifdef DEBUG_COVER
          printf("music: bitmap.c: create an image with aspect ratio\n");
#endif

          }
      }

      newwidth  = image.columns();;
      newheight = image.rows();
#ifdef DEBUG_COVER
      printf("music: bitmap.c: new image dimensions = %dX%d\n", newwidth, newheight);
#endif


    }
    catch ( Exception &e )
    {
      esyslog("music: bitmap.c: Could not load %s: %s", Filename, e.what());
    }
  }

}

void cImageloadBitmap::Render(cImage &bmp)
{

#ifdef DEBUG_COVER
  printf("music: bitmap.c: start to render image\n");
#endif

  const PixelPacket *pix = image.getConstPixels(0, 0, newwidth, newheight);

  for (int iy = 0; iy < newheight; ++iy) {
    for (int ix = 0; ix < newwidth; ++ix) {
      tColor col = (~int(pix->opacity * 255 / MaxRGB) << 24)
                  | (int(pix->red * 255 /MaxRGB) << 16)
                  | (int(pix->green * 255 / MaxRGB) << 8)
                  | (int(pix->blue * 255 / MaxRGB) );

      bmp.SetPixel(cPoint(ix, iy), col);
      ++pix;

    }
  }

#ifdef DEBUG_COVER
  printf("music: bitmap.c: image rendered !\n");
#endif

}


#endif

