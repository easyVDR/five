#include "imageloader.h"
#include "geometrymanager.h"
#include "styledpixmap.h"

cStyledPixmap::cStyledPixmap(void) {
    pixmap = NULL;
}

cStyledPixmap::cStyledPixmap(cPixmap *pixmap) {
    this->pixmap = pixmap;
}

cStyledPixmap::~cStyledPixmap(void) {
    if (pixmap)
        osdManager.releasePixmap(pixmap);
}

void cStyledPixmap::setPixmap(cPixmap *pixmap) {
    if (pixmap) {
        this->pixmap = pixmap;
    }
}

void cStyledPixmap::drawBackground() {
    if (tvguideConfig.style == eStyleBlendingDefault){
        drawBlendedBackground();
    } else if (tvguideConfig.style == eStyleBlendingMagick){
        drawSparsedBackground();
    } else {
        pixmap->Fill(color);
    }
}

void cStyledPixmap::drawBackgroundGraphical(eBackgroundType type, bool active) {
    cImage *back = NULL;
    if (type == bgGrid) {
        back = imgCache.GetGrid(pixmap->ViewPort().Width(), pixmap->ViewPort().Height(), active);
    } else if (type == bgChannelHeader) {
        back = imgCache.GetOsdElement(oeLogoBack);
    } else if (type == bgChannelGroup) {
        back = imgCache.GetChannelGroup(pixmap->ViewPort().Width(), pixmap->ViewPort().Height());
    } else if (type == bgStatusHeaderWindowed) {
        back = imgCache.GetOsdElement(oeStatusHeaderContentWindowed);
    } else if (type == bgStatusHeaderFull) {
        back = imgCache.GetOsdElement(oeStatusHeaderContentFull);
    }  else if (type == bgClock) {
        back = imgCache.GetOsdElement(oeClock);
    } else if (type == bgButton) {
        drawBackgroundButton(active);
        return;
    } else if (type == bgRecMenuBack) {
        cImageLoader imgLoader;
        if (imgLoader.LoadOsdElement("recmenu_background", pixmap->ViewPort().Width(), pixmap->ViewPort().Height())) {
            cImage background = imgLoader.GetImage();
            pixmap->DrawImage(cPoint(0, 0), background);
        } else {
            pixmap->Fill(clrTransparent);
        }
        return;
    } else if (type == bgChannelJump) {
        back = imgCache.GetOsdElement(oeChannelJump);
    }
    if (back) {
        pixmap->DrawImage(cPoint(0,0), *back);
    } else {
        pixmap->Fill(clrTransparent);
    }
}

void cStyledPixmap::drawBackgroundButton(bool active) {
    std::string buttonName = "";
    int buttonWidth = pixmap->ViewPort().Width();
    int buttonHeight = pixmap->ViewPort().Height();
    if (buttonWidth > geoManager.osdWidth * 50 / 100) {
        if (active)
            buttonName = "button_active_70percent";
        else
            buttonName = "button_70percent";
    } else  {
        if (active)
            buttonName = "button_active_30percent";
        else
            buttonName = "button_30percent";            
    }
    cImageLoader imgLoader;
    if (imgLoader.LoadOsdElement(buttonName.c_str(), buttonWidth, buttonHeight)) {
        cImage button = imgLoader.GetImage();
        pixmap->DrawImage(cPoint(0, 0), button);
    } else {
        pixmap->Fill(clrTransparent);
    }
}


void cStyledPixmap::drawBlendedBackground() {
    int width = pixmap->ViewPort().Width();
    int height = pixmap->ViewPort().Height();
    pixmap->Fill(color);
    int numSteps = 64;
    int alphaStep = 0x04;
    if (height < 30)
        return;
    else if (height < 100) {
        numSteps = 32;
        alphaStep = 0x08;
    }
    int stepY = 0.5*height / numSteps;
    if (stepY == 0) stepY = 1;
    int alpha = 0x00;
    tColor clr;
    for (int i = 0; i<numSteps; i++) {
        clr = AlphaBlend(color, colorBlending, alpha);
        pixmap->DrawRectangle(cRect(0,i*stepY,width,stepY), clr);
        alpha += alphaStep;
    }
}

void cStyledPixmap::drawSparsedBackground() {
    int width = pixmap->ViewPort().Width();
    int height = pixmap->ViewPort().Height();
    cImageLoader imgLoader;
    if (imgLoader.DrawBackground(colorBlending, color, width, height))
        pixmap->DrawImage(cPoint(0,0), imgLoader.GetImage());

}

void cStyledPixmap::drawBorder() {
    int width = pixmap->ViewPort().Width();
    int height = pixmap->ViewPort().Height();
    
    drawDefaultBorder(width, height);
    if (tvguideConfig.roundedCorners) {
        int borderRadius = 12;
        drawRoundedCorners(width, height, borderRadius);
    }
}

void cStyledPixmap::drawDefaultBorder(int width, int height) {
    pixmap->DrawRectangle(cRect(0,0,width,2), clrTransparent);          //top
    pixmap->DrawRectangle(cRect(0,0,2,height), clrTransparent);         //left
    pixmap->DrawRectangle(cRect(0,height-2,width,2), clrTransparent);   //bottom
    pixmap->DrawRectangle(cRect(width-2,0,2,height), clrTransparent);   //right
    
    pixmap->DrawRectangle(cRect(2,2,width-4,1), theme.Color(clrBorder));            //top
    pixmap->DrawRectangle(cRect(2,2,1,height-4), theme.Color(clrBorder));           //left
    pixmap->DrawRectangle(cRect(2,height-3,width-4,1), theme.Color(clrBorder));     //bottom
    pixmap->DrawRectangle(cRect(width-3,2,1,height-4), theme.Color(clrBorder));     //right
}

void cStyledPixmap::drawBoldBorder() {
    int width = pixmap->ViewPort().Width();
    int height = pixmap->ViewPort().Height();
    pixmap->DrawRectangle(cRect(0,0,width,2), theme.Color(clrBorder));          //top
    pixmap->DrawRectangle(cRect(0,0,2,height), theme.Color(clrBorder));         //left
    pixmap->DrawRectangle(cRect(0,height-2,width,2), theme.Color(clrBorder));   //bottom
    pixmap->DrawRectangle(cRect(width-2,0,2,height), theme.Color(clrBorder));   //right
}

void cStyledPixmap::drawRoundedCorners(int width, int height, int radius) {
    pixmap->DrawEllipse(cRect(2,2,radius,radius), theme.Color(clrBorder), -2);
    pixmap->DrawEllipse(cRect(1,1,radius,radius), clrTransparent, -2);

    pixmap->DrawEllipse(cRect(width-radius - 2,2,radius,radius), theme.Color(clrBorder), -1);
    pixmap->DrawEllipse(cRect(width-radius - 1,1,radius,radius), clrTransparent, -1);
    
    if( height > 2*radius) {
        pixmap->DrawEllipse(cRect(2,height-radius - 2,radius,radius), theme.Color(clrBorder), -3);
        pixmap->DrawEllipse(cRect(1,height-radius - 1,radius,radius), clrTransparent, -3);
        
        pixmap->DrawEllipse(cRect(width-radius - 2,height-radius - 2,radius,radius), theme.Color(clrBorder), -4);
        pixmap->DrawEllipse(cRect(width-radius - 1,height-radius - 1,radius,radius), clrTransparent, -4);
    }
}

void cStyledPixmap::drawVerticalLine(int x, int yStart, int yStop, tColor col) {
    for (int y = yStart; y <= yStop; y++) {
        pixmap->DrawPixel(cPoint(x,y), col);
    }
}

void cStyledPixmap::drawHorizontalLine(int y, int xStart, int xStop, tColor col) {
    for (int x = xStart; x <= xStop; x++) {
        pixmap->DrawPixel(cPoint(x,y), col);
    }
}

void cStyledPixmap::DrawText(const cPoint &Point, const char *s, tColor ColorFg, tColor ColorBg, const cFont *Font) {
    pixmap->DrawText(Point, s, ColorFg, ColorBg, Font);
}

void cStyledPixmap::DrawImage(const cPoint &Point, const cImage &Image) {
    pixmap->DrawImage(Point, Image);
}

void cStyledPixmap::DrawRectangle(const cRect &Rect, tColor Color) {
    pixmap->DrawRectangle(Rect,Color);
}

void cStyledPixmap::DrawEllipse(const cRect &Rect, tColor Color, int Quadrant) {
    pixmap->DrawEllipse(Rect,Color,Quadrant);
}

void cStyledPixmap::SetViewPort(const cRect &Rect) {
    pixmap->SetViewPort(Rect);
}