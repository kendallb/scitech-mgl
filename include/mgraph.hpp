/****************************************************************************
*
*                   SciTech Multi-platform Graphics Library
*
*  ========================================================================
*
*   Copyright (C) 1991-2004 SciTech Software, Inc. All rights reserved.
*
*   This file may be distributed and/or modified under the terms of the
*   GNU General Public License version 2.0 as published by the Free
*   Software Foundation and appearing in the file LICENSE.GPL included
*   in the packaging of this file.
*
*   Licensees holding a valid Commercial License for this product from
*   SciTech Software, Inc. may use this file in accordance with the
*   Commercial License Agreement provided with the Software.
*
*   This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING
*   THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
*   PURPOSE.
*
*   See http://www.scitechsoft.com/license/ for information about
*   the licensing options available and how to purchase a Commercial
*   License Agreement.
*
*   Contact license@scitechsoft.com if any conditions of this licensing
*   are not clear to you, or you have questions about licensing options.
*
*  ========================================================================
*
* Language:     ANSI C++
* Environment:  Any
*
* Description:  Header file for the C++ wrapper classes for the SciTech
*               MGL API. Implementation is in the MGLCPP.LIB library.
*
****************************************************************************/

#ifndef __MGRAPH_HPP
#define __MGRAPH_HPP

#include "mgraph.h"

/*-------------------------- Class Definitions ----------------------------*/

//---------------------------------------------------------------------------
// The following class encapsulates the loading and unloading of MGL cursors.
// The cursor is only unloaded if you loaded the cursor from a file using
// the filename constructor, otherwise it is not unloaded. If you copy an
// MGL cursor class, the copy does not assume ownership of the cursor and
// will not unload it when destructed.
//---------------------------------------------------------------------------

class MGLCursor {
protected:
    cursor_t    *cursor;    // MGL cursor pointer
    int         owner;      // True if we own cursor record

public:
            // Constructors
    inline  MGLCursor(const char *cursorname)
            {
                cursor = MGL_loadCursor(cursorname);
                owner = (cursor != NULL);
            };
    inline  MGLCursor(FILE *f,ulong dwOffset,ulong dwSize)
            {
                cursor = MGL_loadCursorExt(f,dwOffset,dwSize);
                owner = (cursor != NULL);
            };
    inline  MGLCursor(const MGLCursor& c) : cursor(c.cursor), owner(false) {};
    inline  MGLCursor(cursor_t *cursor) : cursor(cursor), owner(false) {};

            // Destructor
    inline  ~MGLCursor()    { if (cursor && owner) MGL_unloadCursor(cursor); };

            // Return true if cursor is valid
    inline  ibool valid()   { return cursor != NULL; };

            // Overloaded cast to an cursor_t pointer
    inline  operator cursor_t* ()   { return cursor; };

            // Return a pointer to the cursor structure
    inline  cursor_t *getCursor()   { return cursor; };
    };

//---------------------------------------------------------------------------
// The following class encapsulates the loading and unloading of MGL icons.
// The icon is only unloaded if you loaded the icon from a file using
// the filename constructor, otherwise it is not unloaded. If you copy an
// MGL icon class, the copy does not assume ownership of the icon and
// will not unload it when destructed.
//---------------------------------------------------------------------------

class MGLIcon {
protected:
    icon_t  *icon;      // MGL icon pointer
    int     owner;      // True if we own icon record

public:
            // Constructors
    inline  MGLIcon(const char *iconname,ibool loadPalette = true)
            {
                icon = MGL_loadIcon(iconname,loadPalette);
                owner = (icon != NULL);
            };
    inline  MGLIcon(FILE *f,ulong dwOffset,ulong dwSize,ibool loadPalette = true)
            {
                icon = MGL_loadIconExt(f,dwOffset,dwSize,loadPalette);
                owner = (icon != NULL);
            };
    inline  MGLIcon(const MGLIcon& i) : icon(i.icon), owner(false) {};
    inline  MGLIcon(icon_t *icon) : icon(icon), owner(false) {};

            // Destructor
    inline  ~MGLIcon()  { if (icon && owner) MGL_unloadIcon(icon); };

            // Return true if icon is valid
    inline  ibool valid()   { return icon != NULL; };

            // Overloaded cast to an icon_t pointer
    inline  operator icon_t* () { return icon; };

            // Return a pointer to the Icon structure
    inline  icon_t *getIcon()   { return icon; };
    };

//---------------------------------------------------------------------------
// The following classes encapsulates the loading and unloading of BMP, PCX,
// JPEG and PNG bitmap files. If you copy an MGL bitmap class, the copy does
// not assume ownership of the bitmap and will not unload it when destructed.
// The bitmap is only unloaded if you loaded the icon from a file using
// the filename constructor, otherwise it is not unloaded.
//---------------------------------------------------------------------------

class MGLBitmapBase {
protected:
    bitmap_t    *bitmap;    // MGL bitmap pointer
    int         owner;      // True if we own bitmap record

public:
    inline  MGLBitmapBase() : bitmap(NULL), owner(false) {};
    inline  MGLBitmapBase(bitmap_t *b,int owner) : bitmap(b), owner(owner) {};

            // Return true if bitmap is valid
    inline  ibool valid()   { return bitmap != NULL; };

            // Operators to access bitmap members
    inline  int width()         { return bitmap->width; };
    inline  int height()        { return bitmap->height; };
    inline  int bitsPerPixel()  { return bitmap->bitsPerPixel; };
    inline  int bytesPerLine()  { return bitmap->bytesPerLine; };
    inline  void *surface()     { return bitmap->surface; };
    inline  palette_t *pal()    { return bitmap->pal; };
    inline  pixel_format_t *pf(){ return bitmap->pf; };

            // Overloaded cast to an bitmap_t pointer
    inline  operator bitmap_t* ()   { return bitmap; };

            // Return a pointer to the Bitmap structure
    inline  bitmap_t *getBitmap()   { return bitmap; };

            // Destructor
    inline  ~MGLBitmapBase()    { if (bitmap && owner) MGL_unloadBitmap(bitmap); };
    };

class MGLBitmap : public MGLBitmapBase {
public:
            // Constructors
    inline  MGLBitmap(const char *bitmapname,ibool loadPalette = true)
            {
                bitmap = MGL_loadBitmap(bitmapname,loadPalette);
                owner = (bitmap != NULL);
            };
    inline  MGLBitmap(FILE *f,ulong dwOffset,ulong dwSize,ibool loadPalette = true)
            {
                bitmap = MGL_loadBitmapExt(f,dwOffset,dwSize,loadPalette);
                owner = (bitmap != NULL);
            };
    inline  MGLBitmap(const MGLBitmap& b) : MGLBitmapBase(b.bitmap,false) {};
    inline  MGLBitmap(bitmap_t *b) : MGLBitmapBase(b,false) {};
    };

class MGLPCX : public MGLBitmapBase {
public:
            // Constructors
    inline  MGLPCX(const char *bitmapname,ibool loadPalette = true)
            {
                bitmap = MGL_loadPCX(bitmapname,loadPalette);
                owner = (bitmap != NULL);
            };
    inline  MGLPCX(FILE *f,ulong dwOffset,ulong dwSize,ibool loadPalette = true)
            {
                bitmap = MGL_loadPCXExt(f,dwOffset,dwSize,loadPalette);
                owner = (bitmap != NULL);
            };
    inline  MGLPCX(const MGLPCX& b) : MGLBitmapBase(b.bitmap,false) {};
    inline  MGLPCX(bitmap_t *b) : MGLBitmapBase(b,false) {};
    };

class MGLJPEG : public MGLBitmapBase {
public:
            // Constructors
    inline  MGLJPEG(const char *bitmapname,ibool loadPalette = true)
            {
                bitmap = MGL_loadJPEG(bitmapname,loadPalette);
                owner = (bitmap != NULL);
            };
    inline  MGLJPEG(FILE *f,ulong dwOffset,ulong dwSize,ibool loadPalette = true)
            {
                bitmap = MGL_loadJPEGExt(f,dwOffset,dwSize,loadPalette);
                owner = (bitmap != NULL);
            };
    inline  MGLJPEG(const MGLJPEG& b) : MGLBitmapBase(b.bitmap,false) {};
    inline  MGLJPEG(bitmap_t *b) : MGLBitmapBase(b,false) {};
    };

class MGLPNG : public MGLBitmapBase {
public:
            // Constructors
    inline  MGLPNG(const char *bitmapname,ibool loadPalette = true)
            {
                bitmap = MGL_loadPNG(bitmapname,loadPalette);
                owner = (bitmap != NULL);
            };
    inline  MGLPNG(FILE *f,ulong dwOffset,ulong dwSize,ibool loadPalette = true)
            {
                bitmap = MGL_loadPNGExt(f,dwOffset,dwSize,loadPalette);
                owner = (bitmap != NULL);
            };
    inline  MGLPNG(const MGLPNG& b) : MGLBitmapBase(b.bitmap,false) {};
    inline  MGLPNG(bitmap_t *b) : MGLBitmapBase(b,false) {};
    };

//---------------------------------------------------------------------------
// The following class is a C++ wrapper class around the MGL region
// manipulation functions. Note that if you assign a region_t pointer to
// an object, the object takes over ownership of the region and will
// destroy it when destructed. This is designed to make the operator & ()
// style functions work properly without undue region copying taking place.
//---------------------------------------------------------------------------

class MGLRegion {
protected:
    region_t    *rgn;       // MGL region pointer

public:
            // Constructors
    inline  MGLRegion()                     { rgn = MGL_newRegion(); };
    inline  MGLRegion(const MGLRegion& r)   { rgn = MGL_copyRegion(r.rgn); };
    inline  MGLRegion(const MGLRect& r)     { rgn = MGL_rgnSolidRect(r.r()); };
    inline  MGLRegion(region_t *r)          { rgn = r; };

            // Copy constructors
            MGLRegion& operator = (const MGLRegion& r);
            MGLRegion& operator = (const MGLRect& r);
            MGLRegion& operator = (region_t *r);

            // Destructor
    inline  ~MGLRegion()                { MGL_freeRegion(rgn); };

            // Method to get the bounding rectangle
    inline  rect_t& getBounds() const   { return rgn->rect; };

            // Methods to access the left, top, right and bottom coords.
    inline  int left(void) const        { return rgn->rect.left; };
    inline  int top(void) const         { return rgn->rect.top; };
    inline  int right(void) const       { return rgn->rect.right; };
    inline  int bottom(void) const      { return rgn->rect.bottom; };

            // Return the height and width
    inline  int width(void) const       { return right() - left(); };
    inline  int height(void) const      { return bottom() - top(); };

            // Methods to get the union of a region and a region/rectangle
            region_t * operator + (const MGLRegion& r) const;
            region_t * operator + (const MGLRect& r) const;
    inline  MGLRegion& operator += (const MGLRegion& r);
    inline  MGLRegion& operator += (const MGLRect& r);
    inline  MGLRegion& unionOffset(const MGLRegion& r,int x,int y);

            // Method to subtract another region or rectangle
            region_t * operator - (const MGLRegion& r) const;
            region_t * operator - (const MGLRect& r) const;
    inline  MGLRegion& operator -= (const MGLRegion& r);
    inline  MGLRegion& operator -= (const MGLRect& r);

            // Methods to get the intersection of a region and a region/rectangle
    inline  region_t * operator & (const MGLRegion& r) const;
    inline  region_t * operator & (const MGLRect& r) const;
            MGLRegion& operator &= (const MGLRegion& r);
            MGLRegion& operator &= (const MGLRect& r);

            // Method to optimize the region
    inline  void optimize() { MGL_optimizeRegion(rgn); };

            // Method to move a region to a specified position
    inline  MGLRegion& moveTo(int x,int y);
    inline  MGLRegion& moveTo(const MGLPoint& p) { return moveTo(p.x,p.y); };

            // Method to offset a region by a specified amount
    inline  MGLRegion& offset(int dx,int dy);
    inline  MGLRegion& offset(const MGLPoint& d)    { return offset(d.x,d.y); };

            // Method's to determine if a point is within the region
    inline  ibool includes(int x,int y) const;
    inline  ibool includes(const MGLPoint& p) const { return includes(p.x,p.y); };

            // Method to traverse all rectangles in the region
    inline  void traverse(rgncallback_t doRect) const;

            // Method to clear the region
    inline  void empty()                    { MGL_clearRegion(rgn); };

            // Method to determine if a region is empty
    inline  ibool isEmpty(void) const       { return MGL_emptyRegion(rgn); };

            // Return true if region is simple
    inline  ibool isSimple() const  { return MGL_isSimpleRegion(rgn); };

            // Method to determine if two regions are equal
    inline  ibool operator == (const MGLRegion& r) const;

            // Method to determine if two regions are not equal
    inline  ibool operator != (const MGLRegion& r) const;

            // Return true if region is valid
    inline  ibool valid() const             { return rgn != NULL; };

            // Get the region_t pointer
    inline  region_t * rgnPointer() const   { return rgn; };
    };

//---------------------------------------------------------------------------
// The following class encapsulates the entire MGL device context API,
// providing overloaded methods to access the different routines in the MGL.
// Most of the code is implemented inline, so there is no performance
// penalty in using the C++ wrapper class routines instead of the
// conventional C based API. For many of the functions in the MGL, there is
// a set of corresponding methods in this class overloaded with different
// arguments.
//---------------------------------------------------------------------------

class MGLDevCtx {
protected:
    MGLDC           *dc;        // MGL device context pointer

public:
            // Constructors
    inline  MGLDevCtx(MGLDC *dc) : dc(dc) {};
    inline  MGLDevCtx(const MGLDevCtx& dc) : dc(dc.dc) {};

            // Method to destroy the DC, and assign a NULL to the dc pointer
    inline  void destroy()              { MGL_destroyDC(dc); dc = NULL; };

            // Return the MGL device context pointer
    inline  MGLDC *getDC() const        { return dc; };
    inline  void setDC(MGLDC *newdc)    { dc = newdc; };
    inline  operator MGLDC* ()          { return dc; };

            // Return true if the context is valid
    inline  ibool isValid()             { return dc != NULL; };

            // Make the context the current one
    inline  MGLDC *makeCurrent()        { return MGL_makeCurrentDC(dc); };
    inline  ibool isCurrent()           { return MGL_isCurrentDC(dc); };

            // Error code handling
    inline  int     result() const      { return MGL_result(); };
    inline  void    setResult(int result)   { MGL_setResult(result); };
    inline  const char *errorMsg(int err) const { return MGL_errorMsg(err); };

            // Miscellaneous routines specific to display DC's
    inline  void    setActivePage(int page) { MGL_setActivePage(dc,page); };
    inline  int     getActivePage() const   { return MGL_getActivePage(dc); };
    inline  void    setVisualPage(int page,int waitVRT);
    inline  int     getVisualPage() const   { return MGL_getVisualPage(dc); };
    inline  ibool   vSync() const           { return MGL_vSync(dc); };
    inline  int     isVSync() const         { return MGL_isVSync(dc); };
    inline  int     getCurrentScanLine() const { return MGL_getCurrentScanLine(dc); };

            // Hardware double buffering support
    inline  ibool   doubleBuffer()          { return MGL_doubleBuffer(dc); };
    inline  void    singleBuffer()          { MGL_singleBuffer(dc); };
    inline  void    swapBuffers(int waitVRT){ MGL_swapBuffers(dc,waitVRT); };

            // Device clearing
    inline  void    clearDevice()       { MGL_clearDevice(); };
    inline  void    clearViewport()     { MGL_clearViewport(); };

            // Pixel plotting
    inline  void    pixel(const MGLPoint& p){ MGL_pixel(p); };
    inline  void    pixel(int x,int y)      { MGL_pixelCoord(x,y); };
    inline  color_t getPixel(const MGLPoint& p) { return MGL_getPixel(p); };
    inline  color_t getPixel(int x,int y)   { return MGL_getPixelCoord(x,y); };
    inline  void    beginPixel()            { MGL_beginPixel(); };
    inline  void    pixelFast(const MGLPoint& p){ MGL_pixelFast(p); };
    inline  void    pixelFast(int x,int y)      { MGL_pixelCoordFast(x,y); };
    inline  color_t getPixelFast(const MGLPoint& p) { return MGL_getPixelFast(p); };
    inline  color_t getPixelFast(int x,int y)   { return MGL_getPixelCoordFast(x,y); };
    inline  void    endPixel()              { MGL_endPixel(); };

            // Line drawing and clipping
    inline  void    moveTo(const MGLPoint& p) { MGL_moveTo(p); };
    inline  void    moveTo(int x,int y)     { MGL_moveToCoord(x,y); };
    inline  void    moveRel(const MGLPoint& p) { MGL_moveRel(p); };
    inline  void    moveRel(int dx,int dy)  { MGL_moveRelCoord(dx,dy); };
    inline  void    lineTo(const MGLPoint& p)   { MGL_lineTo(p); };
    inline  void    lineTo(int x,int y)     { MGL_lineToCoord(x,y); };
    inline  void    lineRel(const MGLPoint& p)  { MGL_lineRel(p); };
    inline  void    lineRel(int dx,int dy)  { MGL_lineRelCoord(dx,dy); };
    inline  int     getX() const            { return MGL_getX(); };
    inline  int     getY() const            { return MGL_getY(); };
    inline  void    getCP(MGLPoint& CP) const { MGL_getCP(&CP.p()); };
    inline  void    line(const MGLPoint& p1,const MGLPoint& p2);
    inline  void    lineExt(const MGLPoint& p1,const MGLPoint& p2,ibool drawLast);
    inline  void    line(int x1,int y1,int x2,int y2);
    inline  void    lineExt(int x1,int y1,int x2,int y2,ibool drawLast);

            // Begin/end drawing routines
    inline  void    beginDirectAccess()     { MGL_beginDirectAccess(); };
    inline  void    endDirectAccess()       { MGL_endDirectAccess(); };

            // Polyline drawing
    inline  void    polyPoint(int count,MGLPoint *vArray);
    inline  void    polyLine(int count,MGLPoint *vArray);

            // Polygon drawing
    inline  void    fillPolygon(int count,MGLPoint *vArray,int xOffset,int yOffset);
    inline  void    fillPolygon(int count,fxpoint_t *vArray,int vinc,fix32_t xOffset,fix32_t yOffset);
    inline  void    fillPolygonCnvx(int count,MGLPoint *vArray,int xOffset,int yOffset);
    inline  void    fillPolygonCnvx(int count,fxpoint_t *vArray,int vinc,fix32_t xOffset,fix32_t yOffset);

            // Rectangle drawing
    inline  void    rect(const MGLRect& r);
    inline  void    rect(const MGLPoint& lt,const MGLPoint& rb);
    inline  void    rect(int left,int top,int right,int bottom);
    inline  void    fillRect(const MGLRect& r);
    inline  void    fillRect(const MGLPoint& lt,const MGLPoint& rb);
    inline  void    fillRect(int left,int top,int right,int bottom);

            // Region drawing
    inline  void    drawRegion(int x,int y,const region_t *r);
    inline  void    drawRegion(int x,int y,const MGLRegion &r);

            // Ellipse drawing
    inline  void    ellipse(const MGLRect& extentRect);
    inline  void    ellipse(int x,int y,int xradius,int yradius);
    inline  void    ellipseArc(const MGLRect& r,int startAngle,int endAngle);
    inline  void    ellipseArc(int x,int y,int xradius,int yradius,int startAngle,int endAngle);
    inline  void    getArcCoords(arc_coords_t& coords);
    inline  void    fillEllipse(const MGLRect& r);
    inline  void    fillEllipse(int x,int y,int xradius,int yradius);
    inline  void    fillEllipseArc(const MGLRect& r,int startAngle,int endAngle);
    inline  void    fillEllipseArc(int x,int y,int xradius,int yradius,int startAngle,int endAngle);

            // Device context information and manipulation
    inline  int     getBitsPerPixel() const { return MGL_getBitsPerPixel(dc); };
    inline  void    getPixelFormat(pixel_format_t& pf);
    inline  color_t maxColor() const        { return MGL_maxColor(dc); };
    inline  int     maxPage() const         { return MGL_maxPage(dc); };
    inline  int     sizex() const           { return MGL_sizex(dc); };
    inline  int     sizey() const           { return MGL_sizey(dc); };
    inline  int     getAspectRatio() const  { return MGL_getAspectRatio(); };
    inline  void    setAspectRatio(int a)   { MGL_setAspectRatio(a); };
    inline  void    setColor(color_t color) { MGL_setColor(color); };
    inline  void    setColorRGB(uchar R,uchar G,uchar B);
    inline  void    setColorCI(int index)   { MGL_setColorCI(index); };
    inline  color_t getColor() const        { return MGL_getColor(); };
    inline  void    setBackColor(color_t color) { MGL_setBackColor(color); };
    inline  color_t getBackColor() const    { return MGL_getBackColor(); };
    inline  color_t packColor(uchar R,uchar G,uchar B);
    inline  color_t packColorExt(uchar A,uchar R,uchar G,uchar B);
    inline  color_t packColorFast(uchar R,uchar G,uchar B);
    inline  color_t packColorFastExt(uchar A,uchar R,uchar G,uchar B);
    inline  void    unpackColor(color_t color,uchar& R,uchar& G,uchar& B);
    inline  void    unpackColorExt(color_t color,uchar& A,uchar& R,uchar& G,uchar& B);
    inline  void    unpackColorFast(color_t color,uchar& R,uchar& G,uchar& B);
    inline  void    unpackColorFastExt(color_t color,uchar& A,uchar& R,uchar& G,uchar& B);
    inline  color_t defaultColor() const    { return MGL_defaultColor(); };
    inline  void    setWriteMode(int mode)  { MGL_setWriteMode(mode); };
    inline  int     getWriteMode() const    { return MGL_getWriteMode(); };
    inline  void    setPenStyle(int style)  { MGL_setPenStyle(style); };
    inline  int     getPenStyle() const     { return MGL_getPenStyle(); };
    inline  void    setLineStyle(MGL_lineStyleType l) { MGL_setLineStyle(l); }
    inline  int     getLineStyle()          { return MGL_getLineStyle(); }
    inline  void    setLineStipple(ushort s) { MGL_setLineStipple(s); }
    inline  ushort  getLineStipple()        { return MGL_getLineStipple(); }
    inline  void    setLineStippleCount(uint c) { MGL_setLineStippleCount(c); }
    inline  uint    getLineStippleCount()       { return MGL_getLineStippleCount(); }
    inline  void    setPenBitmapPattern(int index,const pattern_t *p);
    inline  void    usePenBitmapPattern(int index);
    inline  int     getPenBitmapPattern(int index,pattern_t *p) const;
    inline  void    setPenPixmapPattern(int index,const pixpattern_t *p);
    inline  void    usePenPixmapPattern(int index);
    inline  int     getPenPixmapPattern(int index,pixpattern_t *p) const;
    inline  void    setPenPixmapTransparent(color_t color)  { MGL_setPenPixmapTransparent(color); };
    inline  color_t getPenPixmapTransparent()   { return MGL_getPenPixmapTransparent(); };
    inline  void    setPenSize(int h,int w) { MGL_setPenSize(h,w); };
    inline  void    getPenSize(int& h,int& w) const { MGL_getPenSize(&h,&w); };
    inline  void    setBackMode(int mode)   { MGL_setBackMode(mode); };
    inline  int     getBackMode()           { return MGL_getBackMode(); };
    inline  void    setPolygonType(int t)   { MGL_setPolygonType(t); };
    inline  int     getPolygonType() const  { return MGL_getPolygonType(); };
    inline  void    setFontAntiAliasPalette(color_t colorfg, color_t color75, color_t color50, color_t color25, color_t colorbg);
    inline  void    getFontAntiAliasPalette(color_t colorfg, color_t& color75, color_t& color50, color_t& color25, color_t& colorbg);
    inline  void    setFontBlendMode(int type)      { MGL_setFontBlendMode(type); };
    inline  int     getFontBlendMode()              { return MGL_getFontBlendMode(); };
    inline  void    setBlendFunc(int srcBlendFunc,int dstBlendFunc)     { MGL_setBlendFunc(srcBlendFunc,dstBlendFunc); };
    inline  void    getBlendFunc(int *srcBlendFunc,int *dstBlendFunc)   { MGL_getBlendFunc(srcBlendFunc,dstBlendFunc); };
    inline  void    setAlphaValue(uchar alpha)      { MGL_setAlphaValue(alpha); };
    inline  uchar   getAlphaValue()                 { return MGL_getAlphaValue(); };
    inline  void    setPlaneMask(ulong mask)        { MGL_setPlaneMask(mask); };
    inline  ulong   getPlaneMask()                  { return MGL_getPlaneMask(); };
    inline  void    getAttributes(attributes_t& a) const;
    inline  void    restoreAttributes(const attributes_t& a);
    inline  int     surfaceAccessType()     { return MGL_surfaceAccessType(dc); };
    inline  long    getHardwareFlags()      { return MGL_getHardwareFlags(dc); };
    inline  void    defaultAttributes()     { MGL_defaultAttributes(dc); };
    inline  ibool   isDisplayDC()           { return MGL_isDisplayDC(dc); }
    inline  ibool   isWindowedDC()          { return MGL_isWindowedDC(dc); }
    inline  ibool   isMemoryDC()            { return MGL_isMemoryDC(dc); }
    inline  ibool   isStereoDC()            { return MGL_isStereoDC(dc); }

            // Color and palette manipulation
    inline  color_t realColor(int color) const { return MGL_realColor(dc,color); };
    inline  color_t rgbColor(uchar R,uchar G,uchar B) const { return MGL_rgbColor(dc,R,G,B); };
    inline  void    setPaletteEntry(int entry,uchar red,uchar green,uchar blue);
    inline  void    getPaletteEntry(int entry,uchar& red,uchar& green,uchar& blue) const;
    inline  void    setPalette(palette_t *pal,int numColors,int startIndex);
    inline  void    getPalette(palette_t *pal,int numColors,int startIndex) const;
    inline  void    rotatePalette(int numColors,int startIndex,int dir);
    inline  ibool   fadePalette(palette_t *fullIntensity,int numColors,int startIndex,uchar intensity);
    inline  void    realizePalette(int numColors,int startIndex,int waitVRT) const;
    inline  int     getPaletteSize() const      { return MGL_getPaletteSize(dc); };
    inline  void    getDefaultPalette(palette_t *p) const { MGL_getDefaultPalette(dc,p); };
    inline  void    setDefaultPalette()         { MGL_setDefaultPalette(dc); };
    inline  void    mapToPalette(palette_t *p)  { MGL_mapToPalette(dc,p); };
    inline  ibool   setGammaRamp(palette_ext_t *gamma,int num,int index,ibool vSync);
    inline  ibool   getGammaRamp(palette_ext_t *gamma,int num,int index);

            // Viewport and clip rectangle manipulation
    inline  void    setViewport(const MGLRect& v) { MGL_setViewportDC(dc,v); };
    inline  void    setRelViewport(const MGLRect& v) { MGL_setRelViewportDC(dc,v); };
    inline  void    getViewport(MGLRect& view) const { MGL_getViewportDC(dc,&view.r()); };
    inline  void    setViewportOrg(const MGLPoint& p) { MGL_setViewportOrgDC(dc,p.p()); };
    inline  void    getViewportOrg(MGLPoint& p) { MGL_getViewportOrgDC(dc,&p.p()); };
    inline  void    globalToLocal(MGLPoint& p) const { MGL_globalToLocalDC(dc,&p.p()); };
    inline  void    localToGlobal(MGLPoint& p) const { MGL_localToGlobalDC(dc,&p.p()); };
    inline  int     maxx() const            { return MGL_maxx(); };
    inline  int     maxy() const            { return MGL_maxy(); };
    inline  void    setClipRect(const MGLRect& clip) { MGL_setClipRectDC(dc,clip.r()); };
    inline  void    getClipRect(MGLRect& clip) const { MGL_getClipRectDC(dc,&clip.r()); };
    inline  void    setClipRegion(region_t *clip)       { MGL_setClipRegionDC(dc,clip); };
    inline  void    setClipRegion(const MGLRegion &clip){ MGL_setClipRegionDC(dc,clip.rgnPointer()); };
    inline  void    getClipRegion(region_t *clip)       { MGL_getClipRegionDC(dc,clip); };
    inline  void    getClipRegion(MGLRegion &clip)      { MGL_getClipRegionDC(dc,clip.rgnPointer()); };

            // BitBlt support
    inline  void    bitBlt(MGLDevCtx& src,const MGLRect& srcRect,int dstLeft,int dstTop,int op);
    inline  void    bitBlt(MGLDevCtx& src,int left,int top,int right,int bottom,int dstLeft,int dstTop,int op);
    inline  void    copyPage(int srcPage,const MGLRect& srcRect,int dstLeft,int dstTop,int op);
    inline  void    copyPage(int srcPage,int left,int top,int right,int bottom,int dstLeft,int dstTop,int op);
    inline  void    srcTransBlt(MGLDevCtx& src,const MGLRect& srcRect,int dstLeft,int dstTop,color_t transparent,int op);
    inline  void    srcTransBlt(MGLDevCtx& src,int left,int top,int right,int bottom,int dstLeft,int dstTop,color_t transparent,int op);
    inline  void    dstTransBlt(MGLDevCtx& src,const MGLRect& srcRect,int dstLeft,int dstTop,color_t transparent,int op);
    inline  void    dstTransBlt(MGLDevCtx& src,int left,int top,int right,int bottom,int dstLeft,int dstTop,color_t transparent,int op);
    inline  void    stretchBlt(MGLDevCtx& src,const MGLRect& srcRect,const MGLRect& dstRect,int op);
    inline  void    stretchBlt(MGLDevCtx& src,int left,int top,int right,int bottom,int dstLeft,int dstTop,int dstRight,int dstBottom,int op);
    inline  void    getDivot(const MGLRect& r,void *divot);
    inline  void    getDivot(int left,int top,int right,int bottom,void *divot);
    inline  void    putDivot(void *divot);
    inline  long    divotSize(const MGLRect& r);
    inline  long    divotSize(int left,int top,int right,int bottom);

            // Bitmap blitting support
    inline  void    putBitmap(int x,int y,const bitmap_t *bitmap,int op);
    inline  void    putBitmapSection(int left,int top,int right,int bottom,int dstLeft,int dstTop,const bitmap_t *bitmap,int op);
    inline  void    putBitmapSection(const MGLRect& r,int dstLeft,int dstTop,const bitmap_t *bitmap,int op);
    inline  void    putBitmapSrcTrans(int x,int y,const bitmap_t *bitmap,color_t transparent,int op);
    inline  void    putBitmapSrcTransSection(int left,int top,int right,int bottom,int dstLeft,int dstTop,const bitmap_t *bitmap,color_t transparent,int op);
    inline  void    putBitmapSrcTransSection(const MGLRect& r,int dstLeft,int dstTop,const bitmap_t *bitmap,color_t transparent,int op);
    inline  void    putBitmapDstTrans(int x,int y,const bitmap_t *bitmap,color_t transparent,int op);
    inline  void    putBitmapDstTransSection(int left,int top,int right,int bottom,int dstLeft,int dstTop,const bitmap_t *bitmap,color_t transparent,int op);
    inline  void    putBitmapDstTransSection(const MGLRect& r,int dstLeft,int dstTop,const bitmap_t *bitmap,color_t transparent,int op);
    inline  void    putBitmapMask(int x,int y,const bitmap_t *mask,color_t color,int op);
    inline  void    stretchBitmap(int left,int top,int right,int bottom,const bitmap_t *bitmap,int op);
    inline  void    stretchBitmap(const MGLRect& r,const bitmap_t *bitmap,int op);
    inline  void    stretchBitmapSection(int left,int top,int right,int bottom,int dstLeft,int dstTop,int dstRight,int dstBottom,const bitmap_t *bitmap,int op);
    inline  void    stretchBitmapSection(const MGLRect& src,const MGLRect& dst,const bitmap_t *bitmap,int op);
    inline  void    putMonoImage(int x,int y,int width,int byteWidth,int height,void *image);
    inline  void    putIcon(int x,int y,const icon_t *icon);

            // Offscreen buffer blitting support
    inline  void    copyToBuffer(int left,int top,int right,int bottom,int dstLeft,int dstTop,MGLBUF *buf);
    inline  void    copyToBuffer(const MGLRect& r,int dstLeft,int dstTop,MGLBUF *buf);
    inline  void    putBuffer(int x,int y,MGLBUF *buf,int op);
    inline  void    putBufferSection(int left,int top,int right,int bottom,int dstLeft,int dstTop,MGLBUF *buf,int op);
    inline  void    putBufferSection(const MGLRect& r,int dstLeft,int dstTop,MGLBUF *buf,int op);
    inline  void    putBufferSrcTrans(int x,int y,MGLBUF *buf,color_t transparent,int op);
    inline  void    putBufferSrcTransSection(int left,int top,int right,int bottom,int dstLeft,int dstTop,MGLBUF *buf,color_t transparent,int op);
    inline  void    putBufferSrcTransSection(const MGLRect& r,int dstLeft,int dstTop,MGLBUF *buf,color_t transparent,int op);
    inline  void    putBufferDstTrans(int x,int y,MGLBUF *buf,color_t transparent,int op);
    inline  void    putBufferDstTransSection(int left,int top,int right,int bottom,int dstLeft,int dstTop,MGLBUF *buf,color_t transparent,int op);
    inline  void    putBufferDstTransSection(const MGLRect& r,int dstLeft,int dstTop,MGLBUF *buf,color_t transparent,int op);
    inline  void    stretchBuffer(int left,int top,int right,int bottom,MGLBUF *buf,int op);
    inline  void    stretchBuffer(const MGLRect& r,MGLBUF *buf,int op);
    inline  void    stretchBufferSection(int left,int top,int right,int bottom,int dstLeft,int dstTop,int dstRight,int dstBottom,MGLBUF *buf,int op);
    inline  void    stretchBufferSection(const MGLRect& src,const MGLRect& dst,MGLBUF *buf,int op);

            // Load/save bitmap files directly from the device context
    inline  ibool   loadBitmapIntoDC(const char *bitmapName,int dstLeft,int dstTop,ibool loadPalette);
    inline  ibool   loadBitmapIntoDC(FILE *f,ulong dwOffset,ulong size,int dstLeft,int dstTop,ibool loadPalette);
    inline  ibool   saveBitmapFromDC(const char *bitmapName,const MGLRect& src);
    inline  ibool   saveBitmapFromDC(const char *bitmapName,int left,int top,int right,int bottom);
    inline  bitmap_t *getBitmapFromDC(const MGLRect& src,ibool savePalette);
    inline  bitmap_t *getBitmapFromDC(int left,int top,int right,int bottom,ibool savePalette);

            // Load/save PCX files directly from the device context
    inline  ibool   loadPCXIntoDC(const char *bitmapName,int dstLeft,int dstTop,ibool loadPalette);
    inline  ibool   loadPCXIntoDC(FILE *f,ulong dwOffset,ulong size,int dstLeft,int dstTop,ibool loadPalette);
    inline  ibool   savePCXFromDC(const char *bitmapName,const MGLRect& src);
    inline  ibool   savePCXFromDC(const char *bitmapName,int left,int top,int right,int bottom);

            // Load/save JPEG files directly from the device context
    inline  ibool   loadJPEGIntoDC(const char *bitmapName,int dstLeft,int dstTop,ibool loadPalette);
    inline  ibool   loadJPEGIntoDC(FILE *f,ulong dwOffset,ulong size,int dstLeft,int dstTop,ibool loadPalette);
    inline  ibool   saveJPEGFromDC(const char *bitmapName,const MGLRect& src,int quality);
    inline  ibool   saveJPEGFromDC(const char *bitmapName,int left,int top,int right,int bottom,int quality);

            // Load/save PNG files directly from the device context
    inline  ibool   loadPNGIntoDC(const char *bitmapName,int dstLeft,int dstTop,ibool loadPalette);
    inline  ibool   loadPNGIntoDC(FILE *f,ulong dwOffset,ulong size,int dstLeft,int dstTop,ibool loadPalette);
    inline  ibool   savePNGFromDC(const char *bitmapName,const MGLRect& src);
    inline  ibool   savePNGFromDC(const char *bitmapName,int left,int top,int right,int bottom);

            // Monochrome glyph manipulation
    inline  void    drawGlyph(font_t *g,int x,int y,uchar glyph);
    inline  int     getGlyphWidth(font_t *font,uchar glyph);
    inline  int     getGlyphHeight(font_t *font);
    inline  void    rotateGlyph(uchar *dst,uchar *src,int& byteWidth,int& height,int rotation);
    inline  void    mirrorGlyph(uchar *dst,uchar *src,int byteWidth,int height);

            // Text attribute manipulation
    inline  void    setTextJustify(int h,int v) { MGL_setTextJustify(h,v); };
    inline  void    getTextJustify(int& h,int& v) const { MGL_getTextJustify(&h,&v); };
    inline  void    setTextDirection(int d) { MGL_setTextDirection(d); };
    inline  int     getTextDirection() const { return MGL_getTextDirection(); };
    inline  void    setTextSize(int numerx,int denomx,int numery,int denomy);
    inline  void    getTextSize(int& numerx,int& denomx,int& numery,int& denomy) const;
    inline  void    setSpaceExtra(int e)    { MGL_setSpaceExtra(e); };
    inline  int     getSpaceExtra() const   { return MGL_getSpaceExtra(); };
    inline  void    setTextSettings(const text_settings_t& s);
    inline  void    getTextSettings(text_settings_t& s) const;
    inline  int     textHeight() const      { return MGL_textHeight(); };
    inline  int     textWidth(const char *s) const { return MGL_textWidth(s); };
    inline  int     textWidth(const wchar_t *s) const { return MGL_textWidth_W(s); };
    inline  void    textBounds(int x,int y,const char *s,MGLRect& r)        { MGL_textBounds(x,y,s,&r.r()); };
    inline  void    textBounds_W(int x,int y,const wchar_t *s,MGLRect& r)   { MGL_textBounds_W(x,y,s,&r.r()); };
    inline  int     charWidth(char ch) const { return MGL_charWidth(ch); };
    inline  int     charWidth(wchar_t ch) const { return MGL_charWidth_W(ch); };
    inline  void    getFontMetrics(metrics_t& m) const { MGL_getFontMetrics(&m); };
    inline  void    getCharMetrics(char ch,metrics_t& m) const  { MGL_getCharMetrics(ch,&m); };
    inline  void    getCharMetrics(wchar_t ch,metrics_t& m) const   { MGL_getCharMetrics_W(ch,&m); };
    inline  int     maxCharWidth() const    { return MGL_maxCharWidth(); };
    inline  void    underScoreLocation(int& x,int& y,const char *s) const   { MGL_underScoreLocation(&x,&y,s); };
    inline  void    underScoreLocation(int& x,int& y,const wchar_t *s) const    { MGL_underScoreLocation_W(&x,&y,s); };
    inline  void    setTextEncoding(int encoding) const { MGL_setTextEncoding(encoding); };
    inline  ibool   useFont(font_t *f)          { return MGL_useFont(f); };
    inline  font_t *getFont() const             { return MGL_getFont(); };

            // Text drawing
    inline  void    drawStr(const char *str)                    { MGL_drawStr(str); };
    inline  void    drawStr(int x,int y,const char *str)        { MGL_drawStrXY(x,y,str); };
    inline  void    drawStr(const MGLPoint& p,const char *str)  { MGL_drawStrXY(p.x,p.y,str); };
    inline  void    drawStr(const wchar_t *str)                     { MGL_drawStr_W(str); };
    inline  void    drawStr(int x,int y,const wchar_t *str)         { MGL_drawStrXY_W(x,y,str); };
    inline  void    drawStr(const MGLPoint& p,const wchar_t *str)   { MGL_drawStrXY_W(p.x,p.y,str); };
    };

//---------------------------------------------------------------------------
// Class to create instances of a display device context.
//---------------------------------------------------------------------------

class MGLDisplayDC : public MGLDevCtx {
protected:
    int     owner;      // True if we own the device context

public:
            // Constructors
    inline  MGLDisplayDC(int mode,int numBuffers,int refreshRate) : MGLDevCtx(NULL)
            {
                dc = MGL_createDisplayDC(mode,numBuffers,refreshRate);
                owner = true;
            };
    inline  MGLDisplayDC(const MGLDisplayDC& dc) : MGLDevCtx(dc.dc), owner(false) {};

            // Destructor
    inline  ~MGLDisplayDC() { if (dc && owner) MGL_destroyDC(dc); };
    };


//---------------------------------------------------------------------------
// Class to create instances of a stereo display device context.
//---------------------------------------------------------------------------

class MGLStereoDisplayDC : public MGLDevCtx {
protected:
    int     owner;      // True if we own the device context

public:
            // Constructors
    inline  MGLStereoDisplayDC(int mode,int numBuffers,int refreshRate) : MGLDevCtx(NULL)
            {
                dc = MGL_createStereoDisplayDC(mode,numBuffers,refreshRate);
                owner = true;
            };
    inline  MGLStereoDisplayDC(const MGLStereoDisplayDC& dc) : MGLDevCtx(dc.dc), owner(false) {};

            // Destructor
    inline  ~MGLStereoDisplayDC()   { if (dc && owner) MGL_destroyDC(dc); };
    };

//---------------------------------------------------------------------------
// Class to create instances of a scrolling display device context.
//---------------------------------------------------------------------------

class MGLScrollingDC : public MGLDevCtx {
protected:
    int     owner;      // True if we own the device context

public:
            // Constructors
    inline  MGLScrollingDC(int mode,int virtualX,int virtualY,int numBuffers,int refreshRate) : MGLDevCtx(NULL)
            {
                dc = MGL_createScrollingDC(mode,virtualX,virtualY,numBuffers,refreshRate);
                owner = true;
            };
    inline  MGLScrollingDC(const MGLScrollingDC& dc) : MGLDevCtx(dc.dc), owner(false) {};

            // Destructor
    inline  ~MGLScrollingDC()   { if (dc && owner) MGL_destroyDC(dc); };
    };

//---------------------------------------------------------------------------
// Class to create instances of an overlay display device context.
//---------------------------------------------------------------------------

/*
class MGLOverlayDC : public MGLDevCtx {
protected:
    int     owner;      // True if we own the device context

public:
            // Constructors
    inline  MGLOverlayDC(MGLDC *dc,int format,int numBuffers,int left,int top,int right,int bottom) : MGLDevCtx(NULL)
            {
                dc = MGL_createOverlayDC(dc,format,numBuffers,left,top,right,bottom);
                owner = true;
            };
    inline  MGLOverlayDC(const MGLOverlayDC& dc) : MGLDevCtx(dc.dc), owner(false) {};

            // Destructor
    inline  ~MGLOverlayDC() { if (dc && owner) MGL_destroyDC(dc); };
    };
*/

//---------------------------------------------------------------------------
// Class to create instances of an offscreen display device context.
//---------------------------------------------------------------------------

class MGLOffscreenDC : public MGLDevCtx {
protected:
    int     owner;      // True if we own the device context

public:
            // Constructors
    inline  MGLOffscreenDC(MGLDC *dc,int width,int height) : MGLDevCtx(NULL)
            {
                dc = MGL_createOffscreenDC(dc,width,height);
                owner = true;
            };
    inline  MGLOffscreenDC(const MGLOffscreenDC& dc) : MGLDevCtx(dc.dc), owner(false) {};

            // Destructor
    inline  ~MGLOffscreenDC()   { if (dc && owner) MGL_destroyDC(dc); };
    };

//---------------------------------------------------------------------------
// Class to create instances of an windowed device context.
//---------------------------------------------------------------------------

class MGLWindowedDC : public MGLDevCtx {
protected:
    int     owner;      // True if we own the device context

public:
            // Constructors
    inline  MGLWindowedDC(MGL_HWND hwnd) : MGLDevCtx(NULL)
            {
                dc = MGL_createWindowedDC(hwnd);
                owner = true;
            };
    inline  MGLWindowedDC(const MGLWindowedDC& dc) : MGLDevCtx(dc.dc), owner(false) {};

            // Destructor
    inline  ~MGLWindowedDC()    { if (dc && owner) MGL_destroyDC(dc); };
    };

//---------------------------------------------------------------------------
// Class to create instances of a memory device context.
//---------------------------------------------------------------------------

class MGLMemoryDC : public MGLDevCtx {
protected:
    int     owner;      // True if we own the device context

public:
            // Constructors
    inline  MGLMemoryDC(int xSize,int ySize,int bitsPerPixel,pixel_format_t *pf) : MGLDevCtx(NULL)
            {
                dc = MGL_createMemoryDC(xSize,ySize,bitsPerPixel,pf);
                owner = true;
            };
    inline  MGLMemoryDC(int xSize,int ySize,int bitsPerPixel,pixel_format_t *pf,int bytesPerLine,void *surface,MGL_HBITMAP hbm) : MGLDevCtx(NULL)
            {
                dc = MGL_createCustomDC(xSize,ySize,bitsPerPixel,pf,bytesPerLine,surface,hbm);
                owner = true;
            };
    inline  MGLMemoryDC(const MGLMemoryDC& dc) : MGLDevCtx(dc.dc), owner(false) {};

            // Destructor
    inline  ~MGLMemoryDC()  { if (dc && owner) MGL_destroyDC(dc); };
    };

//---------------------------------------------------------------------------
// Class to encapsulate offscreen buffers in the MGL.
//---------------------------------------------------------------------------

class MGLBuffer {
protected:
    MGLBUF  *buf;       // MGL lightweight buffer pointer

public:
            // Constructors
    inline  MGLBuffer(MGLBUF *buf) : buf(buf) {};
    inline  MGLBuffer(const MGLBuffer& buf) : buf(buf.buf) {};

            // Method to destroy the buffer, and assign a NULL to the buf pointer
    inline  void destroy(void)          { MGL_destroyBuffer(buf); buf = NULL; };

            // Return the MGL buffer pointer
    inline  MGLBUF *getBUF() const      { return buf; };
    inline  void setBUF(MGLBUF *newbuf) { buf = newbuf; };
    inline  operator MGLBUF* ()         { return buf; };

            // Return true if the context is valid
    inline  ibool isValid()             { return buf != NULL; };

            // Functions to lock the buffer for rendering
    inline  void lock()                 { MGL_lockBuffer(buf); }
    inline  void unlock()               { MGL_unlockBuffer(buf); }

            // Operators to access buffer members
    inline  int width()         { return buf->width; };
    inline  int height()        { return buf->height; };
    inline  int bytesPerLine()  { return buf->bytesPerLine; };
    inline  void *surface()     { return buf->surface; };

            // Functions to populate the offscreen buffer contents
    inline  void copyFrom(MGLDC *dc,int left,int top,int right,int bottom,int dstLeft,int dstTop);
    inline  void copyFrom(MGLDC *dc,const MGLRect& r,int dstLeft,int dstTop);
    inline  void copyFrom(bitmap_t *bitmap,int left,int top,int right,int bottom,int dstLeft,int dstTop);
    inline  void copyFrom(bitmap_t *bitmap,const MGLRect& r,int dstLeft,int dstTop);
    };

/*------------------------- Inline member functions -----------------------*/

inline MGLRegion& MGLRegion::operator += (const MGLRegion& r)
{ MGL_unionRegion(rgn,r.rgn); return *this; }

inline MGLRegion& MGLRegion::operator += (const MGLRect& r)
{ MGL_unionRegionRect(rgn,&r.r()); return *this; }

inline MGLRegion& MGLRegion::unionOffset(const MGLRegion& r,int x,int y)
{ MGL_unionRegionOfs(rgn,r.rgn,x,y); return *this; }

inline region_t * MGLRegion::operator & (const MGLRegion& r) const
{ return MGL_sectRegion(rgn,r.rgn); };

inline region_t * MGLRegion::operator & (const MGLRect& r) const
{ return MGL_sectRegionRect(rgn,&r.r()); };

inline MGLRegion& MGLRegion::operator -= (const MGLRegion& r)
{ MGL_diffRegion(rgn,r.rgn); return *this; }

inline MGLRegion& MGLRegion::operator -= (const MGLRect& r)
{ MGL_diffRegionRect(rgn,&r.r()); return *this; }

inline MGLRegion& MGLRegion::moveTo(int x,int y)
{ return offset(x-left(),y-top()); }

inline MGLRegion& MGLRegion::offset(int dx,int dy)
{ MGL_offsetRegion(rgn,dx,dy); return *this; }

inline ibool MGLRegion::includes(int x,int y) const
{ return MGL_ptInRegionCoord(x,y,rgn); }

inline ibool MGLRegion::operator == (const MGLRegion& r) const
{ return MGL_equalRegion(rgn,r.rgn); }

inline ibool MGLRegion::operator != (const MGLRegion& r) const
{ return !MGL_equalRegion(rgn,r.rgn); }

inline void MGLRegion::traverse(rgncallback_t doRect) const
{ MGL_traverseRegion(rgn,doRect); };

inline void MGLDevCtx::setColorRGB(uchar R,uchar G,uchar B)
{ MGL_setColorRGB(R,G,B); }

inline void MGLDevCtx::getPixelFormat(pixel_format_t& pf)
{ MGL_getPixelFormat(dc,&pf); }

inline color_t MGLDevCtx::packColor(uchar R,uchar G,uchar B)
{ return MGL_packColor(&dc->pf,R,G,B); }

inline color_t MGLDevCtx::packColorExt(uchar A,uchar R,uchar G,uchar B)
{ return MGL_packColorExt(&dc->pf,A,R,G,B); }

inline color_t MGLDevCtx::packColorFast(uchar R,uchar G,uchar B)
{ return MGL_packColorFast(&dc->pf,R,G,B); }

inline color_t MGLDevCtx::packColorFastExt(uchar A,uchar R,uchar G,uchar B)
{ return MGL_packColorFastExt(&dc->pf,A,R,G,B); }

inline void MGLDevCtx::unpackColor(color_t color,uchar& R,uchar& G,uchar& B)
{ MGL_unpackColor(&dc->pf,color,&R,&G,&B); }

inline void MGLDevCtx::unpackColorExt(color_t color,uchar& A,uchar& R,uchar& G,uchar& B)
{ MGL_unpackColorExt(&dc->pf,color,&A,&R,&G,&B); }

inline void MGLDevCtx::unpackColorFast(color_t color,uchar& R,uchar& G,uchar& B)
{ MGL_unpackColorFast(&dc->pf,color,R,G,B); }

inline void MGLDevCtx::unpackColorFastExt(color_t color,uchar& A,uchar& R,uchar& G,uchar& B)
{ MGL_unpackColorFastExt(&dc->pf,color,A,R,G,B); }

inline void MGLDevCtx::line(const MGLPoint& p1,const MGLPoint& p2)
{ MGL_line(p1,p2); }

inline void MGLDevCtx::lineExt(const MGLPoint& p1,const MGLPoint& p2,ibool drawLast)
{ MGL_lineExt(p1,p2,drawLast); }

inline void MGLDevCtx::line(int x1,int y1,int x2,int y2)
{ MGL_lineCoord(x1,y1,x2,y2); }

inline void MGLDevCtx::lineExt(int x1,int y1,int x2,int y2,ibool drawLast)
{ MGL_lineCoordExt(x1,y1,x2,y2,drawLast); }

inline void MGLDevCtx::polyPoint(int count,MGLPoint *vArray)
{ MGL_polyPoint(count,(point_t*)vArray); }

inline void MGLDevCtx::polyLine(int count,MGLPoint *vArray)
{ MGL_polyLine(count,(point_t*)vArray); }

inline void MGLDevCtx::fillPolygon(int count,MGLPoint *vArray,int xOffset,
    int yOffset)
{ MGL_fillPolygon(count,(point_t*)vArray,xOffset,yOffset); }

inline void MGLDevCtx::fillPolygonCnvx(int count,MGLPoint *vArray,int xOffset,
    int yOffset)
{ MGL_fillPolygonCnvx(count,(point_t*)vArray,xOffset,yOffset); }

inline void MGLDevCtx::fillPolygon(int count,fxpoint_t *vArray,int vinc,
    fix32_t xOffset,fix32_t yOffset)
{ MGL_fillPolygonFX(count,vArray,vinc,xOffset,yOffset); }

inline void MGLDevCtx::fillPolygonCnvx(int count,fxpoint_t *vArray,int vinc,
    fix32_t xOffset,fix32_t yOffset)
{ MGL_fillPolygonCnvxFX(count,vArray,vinc,xOffset,yOffset); }

inline void MGLDevCtx::rect(const MGLRect& r)
{ MGL_rect(r.r()); }

inline void MGLDevCtx::rect(const MGLPoint& lt,const MGLPoint& rb)
{ MGL_rectPt(lt,rb); }

inline void MGLDevCtx::rect(int left,int top,int right,int bottom)
{ MGL_rectCoord(left,top,right,bottom); }

inline void MGLDevCtx::fillRect(const MGLRect& r)
{ MGL_fillRect(r.r()); }

inline void MGLDevCtx::fillRect(const MGLPoint& lt,const MGLPoint& rb)
{ MGL_fillRectPt(lt,rb); }

inline void MGLDevCtx::fillRect(int left,int top,int right,int bottom)
{ MGL_fillRectCoord(left,top,right,bottom); }

inline void MGLDevCtx::drawRegion(int x,int y,const region_t *rgn)
{ MGL_drawRegion(x,y,rgn); }

inline void MGLDevCtx::drawRegion(int x,int y,const MGLRegion& rgn)
{ MGL_drawRegion(x,y,rgn.rgnPointer()); }

inline void MGLDevCtx::ellipseArc(const MGLRect& r,int startAngle,int endAngle)
{ MGL_ellipseArc(r.r(),startAngle,endAngle); }

inline void MGLDevCtx::ellipseArc(int x,int y,int xradius,int yradius,
    int startAngle,int endAngle)
{ MGL_ellipseArcCoord(x,y,xradius,yradius,startAngle,endAngle); }

inline void MGLDevCtx::getArcCoords(arc_coords_t& coords)
{ MGL_getArcCoords(&coords); }

inline void MGLDevCtx::ellipse(const MGLRect& r)
{ MGL_ellipse(r.r()); }

inline void MGLDevCtx::ellipse(int x,int y,int xradius,int yradius)
{ MGL_ellipseCoord(x,y,xradius,yradius); }

inline void MGLDevCtx::fillEllipseArc(const MGLRect& r,int startAngle,int endAngle)
{ MGL_fillEllipseArc(r.r(),startAngle,endAngle); }

inline void MGLDevCtx::fillEllipseArc(int x,int y,int xradius,int yradius,
    int startAngle,int endAngle)
{ MGL_fillEllipseArcCoord(x,y,xradius,yradius,startAngle,endAngle); }

inline void MGLDevCtx::fillEllipse(const MGLRect& r)
{ MGL_fillEllipse(r.r()); }

inline void MGLDevCtx::fillEllipse(int x,int y,int xradius,int yradius)
{ MGL_fillEllipseCoord(x,y,xradius,yradius); }

inline void MGLDevCtx::setPenBitmapPattern(int index,const pattern_t *p)
{ MGL_setPenBitmapPattern(index,p); }

inline void MGLDevCtx::usePenBitmapPattern(int index)
{ MGL_usePenBitmapPattern(index); }

inline int MGLDevCtx::getPenBitmapPattern(int index,pattern_t *p) const
{ return MGL_getPenBitmapPattern(index,p); }

inline void MGLDevCtx::setPenPixmapPattern(int index,const pixpattern_t *p)
{ MGL_setPenPixmapPattern(index,p); }

inline void MGLDevCtx::usePenPixmapPattern(int index)
{ MGL_usePenPixmapPattern(index); }

inline int MGLDevCtx::getPenPixmapPattern(int index,pixpattern_t *p) const
{ return MGL_getPenPixmapPattern(index,p); }

inline void MGLDevCtx::setFontAntiAliasPalette(color_t colorfg, color_t color75, color_t color50, color_t color25, color_t colorbg)
{ MGL_setFontAntiAliasPalette(colorfg,color75,color50,color25,colorbg); }

inline void MGLDevCtx::getFontAntiAliasPalette(color_t colorfg, color_t& color75, color_t& color50, color_t& color25, color_t& colorbg)
{ MGL_getFontAntiAliasPalette(&colorfg,&color75,&color50,&color25,&colorbg); }

inline void MGLDevCtx::getAttributes(attributes_t& a) const
{ MGL_getAttributes(&a); }

inline void MGLDevCtx::restoreAttributes(const attributes_t& a)
{ MGL_restoreAttributes((attributes_t*)&a); }

inline void MGLDevCtx::setPaletteEntry(int entry,uchar red,uchar green,uchar blue)
{ MGL_setPaletteEntry(dc,entry,red,green,blue); }

inline void MGLDevCtx::getPaletteEntry(int entry,uchar& red,uchar& green,uchar& blue) const
{ MGL_getPaletteEntry(dc,entry,&red,&green,&blue); }

inline void MGLDevCtx::setPalette(palette_t *pal,int numColors,int startIndex)
{ MGL_setPalette(dc,pal,numColors,startIndex); }

inline void MGLDevCtx::getPalette(palette_t *pal,int numColors,int startIndex) const
{ MGL_getPalette(dc,pal,numColors,startIndex); }

inline void MGLDevCtx::rotatePalette(int numColors,int startIndex,int dir)
{ MGL_rotatePalette(dc,numColors,startIndex,dir); }

inline ibool MGLDevCtx::fadePalette(palette_t *fullIntensity,int numColors,
    int startIndex,uchar intensity)
{ return MGL_fadePalette(dc,fullIntensity,numColors,startIndex,intensity); }

inline void MGLDevCtx::realizePalette(int numColors,int startIndex,int waitVRT) const
{ MGL_realizePalette(dc,numColors,startIndex,waitVRT); }

inline ibool MGLDevCtx::setGammaRamp(palette_ext_t *gamma,int num,int index,ibool vSync)
{ return MGL_setGammaRamp(dc,gamma,num,index,vSync); }

inline ibool MGLDevCtx::getGammaRamp(palette_ext_t *gamma,int num,int index)
{ return MGL_getGammaRamp(dc,gamma,num,index); }

inline void MGLDevCtx::putMonoImage(int x,int y,int width,int byteWidth,int height,void *image)
{ MGL_putMonoImage(dc,x,y,width,byteWidth,height,image); }

inline void MGLDevCtx::putBitmap(int x,int y,const bitmap_t *bitmap,int op)
{ MGL_putBitmap(dc,x,y,bitmap,op); }

inline void MGLDevCtx::putBitmapSection(int left,int top,int right,int bottom,int dstLeft,int dstTop,const bitmap_t *bitmap,int op)
{ MGL_putBitmapSection(dc,left,top,right,bottom,dstLeft,dstTop,bitmap,op); }

inline void MGLDevCtx::putBitmapSection(const MGLRect& r,int dstLeft,int dstTop,const bitmap_t *bitmap,int op)
{ MGL_putBitmapSection(dc,r.left(),r.top(),r.right(),r.bottom(),dstLeft,dstTop,bitmap,op); }

inline void MGLDevCtx::putBitmapSrcTrans(int x,int y,
    const bitmap_t *bitmap,color_t transparent,int op)
{
    MGL_putBitmapSrcTrans(dc,x,y,bitmap,transparent,op);
}

inline void MGLDevCtx::putBitmapSrcTransSection(int left,int top,int right,int bottom,int dstLeft,int dstTop,const bitmap_t *bitmap,color_t transparent,int op)
{ MGL_putBitmapSrcTransSection(dc,left,top,right,bottom,dstLeft,dstTop,bitmap,transparent,op); }

inline void MGLDevCtx::putBitmapSrcTransSection(const MGLRect& r,int dstLeft,int dstTop,const bitmap_t *bitmap,color_t transparent,int op)
{ MGL_putBitmapSrcTransSection(dc,r.left(),r.top(),r.right(),r.bottom(),dstLeft,dstTop,bitmap,transparent,op); }

inline void MGLDevCtx::putBitmapDstTrans(int x,int y,
    const bitmap_t *bitmap,color_t transparent,int op)
{
    MGL_putBitmapDstTrans(dc,x,y,bitmap,transparent,op);
}

inline void MGLDevCtx::putBitmapDstTransSection(int left,int top,int right,int bottom,int dstLeft,int dstTop,const bitmap_t *bitmap,color_t transparent,int op)
{ MGL_putBitmapDstTransSection(dc,left,top,right,bottom,dstLeft,dstTop,bitmap,transparent,op); }

inline void MGLDevCtx::putBitmapDstTransSection(const MGLRect& r,int dstLeft,int dstTop,const bitmap_t *bitmap,color_t transparent,int op)
{ MGL_putBitmapDstTransSection(dc,r.left(),r.top(),r.right(),r.bottom(),dstLeft,dstTop,bitmap,transparent,op); }

inline void MGLDevCtx::putBitmapMask(int x,int y,const bitmap_t *mask,color_t color,int op)
{ MGL_putBitmapMask(dc,x,y,mask,color,op); }

inline void MGLDevCtx::stretchBitmap(int left,int top,int right,int bottom,
    const bitmap_t *bitmap,int op)
{
    MGL_stretchBitmap(dc,left,top,right,bottom,bitmap,op);
}

inline void MGLDevCtx::stretchBitmap(const MGLRect& r,const bitmap_t *bitmap,int op)
{
    MGL_stretchBitmap(dc,r.left(),r.top(),r.right(),r.bottom(),bitmap,op);
}

inline void MGLDevCtx::stretchBitmapSection(int left,int top,int right,int bottom,int dstLeft,int dstTop,int dstRight,int dstBottom,const bitmap_t *bitmap,int op)
{ MGL_stretchBitmapSection(dc,left,top,right,bottom,dstLeft,dstTop,dstRight,dstBottom,bitmap,op); }

inline void MGLDevCtx::stretchBitmapSection(const MGLRect& src,const MGLRect& dst,const bitmap_t *bitmap,int op)
{ MGL_stretchBitmapSection(dc,src.left(),src.top(),src.right(),src.bottom(),dst.left(),dst.top(),dst.right(),dst.bottom(),bitmap,op); }

inline void MGLDevCtx::putIcon(int x,int y,const icon_t *icon)
{ MGL_putIcon(dc,x,y,icon); }

inline void MGLDevCtx::getDivot(const MGLRect& r,void *divot)
{ MGL_getDivot(dc,r.r(),divot); }

inline void MGLDevCtx::getDivot(int left,int top,int right,int bottom,
    void *divot)
{ MGL_getDivotCoord(dc,left,top,right,bottom,divot); }

inline void MGLDevCtx::putDivot(void *divot)
{ MGL_putDivot(dc,divot); }

inline long MGLDevCtx::divotSize(const MGLRect& r)
{ return MGL_divotSize(dc,r.r()); }

inline long MGLDevCtx::divotSize(int left,int top,int right,int bottom)
{ return MGL_divotSizeCoord(dc,left,top,right,bottom); }

inline void MGLDevCtx::bitBlt(MGLDevCtx& src,const MGLRect& srcRect,int dstLeft,
    int dstTop,int op)
{ MGL_bitBlt(dc,src.dc,srcRect.r(),dstLeft,dstTop,op); }

inline void MGLDevCtx::bitBlt(MGLDevCtx& src,int left,int top,int right,int bottom,
    int dstLeft,int dstTop,int op)
{ MGL_bitBltCoord(dc,src.dc,left,top,right,bottom,dstLeft,dstTop,op); }

inline void MGLDevCtx::copyPage(int srcPage,const MGLRect& srcRect,int dstLeft,
    int dstTop,int op)
{ MGL_copyPage(dc,srcPage,srcRect.r(),dstLeft,dstTop,op); }

inline void MGLDevCtx::copyPage(int srcPage,int left,int top,int right,int bottom,
    int dstLeft,int dstTop,int op)
{ MGL_copyPageCoord(dc,srcPage,left,top,right,bottom,dstLeft,dstTop,op); }

inline void MGLDevCtx::stretchBlt(MGLDevCtx& src,const MGLRect& srcRect,
    const MGLRect& dstRect,int op)
{ MGL_stretchBlt(dc,src,srcRect.r(),dstRect.r(),op); };

inline void MGLDevCtx::stretchBlt(MGLDevCtx& src,int left,int top,int right,int bottom,
    int dstLeft,int dstTop,int dstRight,int dstBottom,int op)
{ MGL_stretchBltCoord(dc,src,left,top,right,bottom,dstLeft,dstTop,dstRight,dstBottom,op); };

inline void MGLDevCtx::srcTransBlt(MGLDevCtx& src,const MGLRect& srcRect,int dstLeft,
    int dstTop,color_t transparent,int op)
{ MGL_srcTransBlt(dc,src,srcRect.r(),dstLeft,dstTop,transparent,op); };

inline void MGLDevCtx::srcTransBlt(MGLDevCtx& src,int left,int top,int right,int bottom,
    int dstLeft,int dstTop,color_t transparent,int op)
{
    MGL_srcTransBltCoord(dc,src,left,top,right,bottom,dstLeft,dstTop,
        transparent,op);
}

inline void MGLDevCtx::dstTransBlt(MGLDevCtx& src,const MGLRect& srcRect,int dstLeft,
    int dstTop,color_t transparent,int op)
{ MGL_dstTransBlt(dc,src,srcRect.r(),dstLeft,dstTop,transparent,op); };

inline void MGLDevCtx::dstTransBlt(MGLDevCtx& src,int left,int top,int right,int bottom,
    int dstLeft,int dstTop,color_t transparent,int op)
{
    MGL_dstTransBltCoord(dc,src,left,top,right,bottom,dstLeft,dstTop,
        transparent,op);
}

inline void MGLDevCtx::copyToBuffer(int left,int top,int right,int bottom,int dstLeft,int dstTop,MGLBUF *buf)
{ MGL_copyToBuffer(dc,left,top,right,bottom,dstLeft,dstTop,buf); }

inline void MGLDevCtx::copyToBuffer(const MGLRect& r,int dstLeft,int dstTop,MGLBUF *buf)
{ MGL_copyToBuffer(dc,r.left(),r.top(),r.right(),r.bottom(),dstLeft,dstTop,buf); }

inline void MGLDevCtx::putBuffer(int x,int y,MGLBUF *buf,int op)
{ MGL_putBuffer(dc,x,y,buf,op); }

inline void MGLDevCtx::putBufferSection(int left,int top,int right,int bottom,int dstLeft,int dstTop,MGLBUF *buf,int op)
{ MGL_putBufferSection(dc,left,top,right,bottom,dstLeft,dstTop,buf,op); }

inline void MGLDevCtx::putBufferSection(const MGLRect& r,int dstLeft,int dstTop,MGLBUF *buf,int op)
{ MGL_putBufferSection(dc,r.left(),r.top(),r.right(),r.bottom(),dstLeft,dstTop,buf,op); }

inline void MGLDevCtx::putBufferSrcTrans(int x,int y,MGLBUF *buf,color_t transparent,int op)
{ MGL_putBufferSrcTrans(dc,x,y,buf,transparent,op); }

inline void MGLDevCtx::putBufferSrcTransSection(int left,int top,int right,int bottom,int dstLeft,int dstTop,MGLBUF *buf,color_t transparent,int op)
{ MGL_putBufferSrcTransSection(dc,left,top,right,bottom,dstLeft,dstTop,buf,transparent,op); }

inline void MGLDevCtx::putBufferSrcTransSection(const MGLRect& r,int dstLeft,int dstTop,MGLBUF *buf,color_t transparent,int op)
{ MGL_putBufferSrcTransSection(dc,r.left(),r.top(),r.right(),r.bottom(),dstLeft,dstTop,buf,transparent,op); }

inline void MGLDevCtx::putBufferDstTrans(int x,int y,MGLBUF *buf,color_t transparent,int op)
{ MGL_putBufferDstTrans(dc,x,y,buf,transparent,op); }

inline void MGLDevCtx::putBufferDstTransSection(int left,int top,int right,int bottom,int dstLeft,int dstTop,MGLBUF *buf,color_t transparent,int op)
{ MGL_putBufferDstTransSection(dc,left,top,right,bottom,dstLeft,dstTop,buf,transparent,op); }

inline void MGLDevCtx::putBufferDstTransSection(const MGLRect& r,int dstLeft,int dstTop,MGLBUF *buf,color_t transparent,int op)
{ MGL_putBufferDstTransSection(dc,r.left(),r.top(),r.right(),r.bottom(),dstLeft,dstTop,buf,transparent,op); }

inline void MGLDevCtx::stretchBuffer(int left,int top,int right,int bottom,MGLBUF *buf,int op)
{ MGL_stretchBuffer(dc,left,top,right,bottom,buf,op); }

inline void MGLDevCtx::stretchBuffer(const MGLRect& r,MGLBUF *buf,int op)
{ MGL_stretchBuffer(dc,r.left(),r.top(),r.right(),r.bottom(),buf,op); }

inline void MGLDevCtx::stretchBufferSection(int left,int top,int right,int bottom,int dstLeft,int dstTop,int dstRight,int dstBottom,MGLBUF *buf,int op)
{ MGL_stretchBufferSection(dc,left,top,right,bottom,dstLeft,dstTop,dstRight,dstBottom,buf,op); }

inline void MGLDevCtx::stretchBufferSection(const MGLRect& src,const MGLRect& dst,MGLBUF *buf,int op)
{ MGL_stretchBufferSection(dc,src.left(),src.top(),src.right(),src.bottom(),dst.left(),dst.top(),dst.right(),dst.bottom(),buf,op); }

inline ibool MGLDevCtx::loadBitmapIntoDC(const char *bitmapName,int dstLeft,
    int dstTop,ibool loadPalette)
{ return MGL_loadBitmapIntoDC(dc,bitmapName,dstLeft,dstTop,loadPalette); }

inline ibool MGLDevCtx::loadBitmapIntoDC(FILE *f,ulong dwOffset,ulong size,int dstLeft,
    int dstTop,ibool loadPalette)
{ return MGL_loadBitmapIntoDCExt(dc,f,dwOffset,size,dstLeft,dstTop,loadPalette); }

inline ibool MGLDevCtx::saveBitmapFromDC(const char *bitmapName,const MGLRect& src)
{ return MGL_saveBitmapFromDC(dc,bitmapName,src.left(),src.top(),src.right(),src.bottom()); }

inline ibool MGLDevCtx::saveBitmapFromDC(const char *bitmapName,int left,int top,int right,int bottom)
{ return MGL_saveBitmapFromDC(dc,bitmapName,left,top,right,bottom); }

inline bitmap_t *MGLDevCtx::getBitmapFromDC(const MGLRect& src,
    ibool savePalette)
{
    return MGL_getBitmapFromDC(dc,src.left(),src.top(),src.right(),src.bottom(),savePalette);
}

inline bitmap_t *MGLDevCtx::getBitmapFromDC(int left,int top,int right,
    int bottom,ibool savePalette)
{ return MGL_getBitmapFromDC(dc,left,top,right,bottom,savePalette); }

inline ibool MGLDevCtx::loadPCXIntoDC(const char *bitmapName,int dstLeft,int dstTop,ibool loadPalette)
{ return MGL_loadPCXIntoDC(dc,bitmapName,dstLeft,dstTop,loadPalette); }

inline ibool MGLDevCtx::loadPCXIntoDC(FILE *f,ulong dwOffset,ulong size,int dstLeft,int dstTop,ibool loadPalette)
{ return MGL_loadPCXIntoDCExt(dc,f,dwOffset,size,dstLeft,dstTop,loadPalette); }

inline ibool MGLDevCtx::savePCXFromDC(const char *bitmapName,const MGLRect& src)
{ return MGL_savePCXFromDC(dc,bitmapName,src.left(),src.top(),src.right(),src.bottom()); }

inline ibool MGLDevCtx::savePCXFromDC(const char *bitmapName,int left,int top,int right,int bottom)
{ return MGL_savePCXFromDC(dc,bitmapName,left,top,right,bottom); }

inline ibool MGLDevCtx::loadJPEGIntoDC(const char *bitmapName,int dstLeft,int dstTop,ibool loadPalette)
{ return MGL_loadJPEGIntoDC(dc,bitmapName,dstLeft,dstTop,loadPalette); }

inline ibool MGLDevCtx::loadJPEGIntoDC(FILE *f,ulong dwOffset,ulong size,int dstLeft,int dstTop,ibool loadPalette)
{ return MGL_loadJPEGIntoDCExt(dc,f,dwOffset,size,dstLeft,dstTop,loadPalette); }

inline ibool MGLDevCtx::saveJPEGFromDC(const char *bitmapName,const MGLRect& src,int quality)
{ return MGL_saveJPEGFromDC(dc,bitmapName,src.left(),src.top(),src.right(),src.bottom(),quality); }

inline ibool MGLDevCtx::saveJPEGFromDC(const char *bitmapName,int left,int top,int right,int bottom,int quality)
{ return MGL_saveJPEGFromDC(dc,bitmapName,left,top,right,bottom,quality); }

inline ibool MGLDevCtx::loadPNGIntoDC(const char *bitmapName,int dstLeft,int dstTop,ibool loadPalette)
{ return MGL_loadPNGIntoDC(dc,bitmapName,dstLeft,dstTop,loadPalette); }

inline ibool MGLDevCtx::loadPNGIntoDC(FILE *f,ulong dwOffset,ulong size,int dstLeft,int dstTop,ibool loadPalette)
{ return MGL_loadPNGIntoDCExt(dc,f,dwOffset,size,dstLeft,dstTop,loadPalette); }

inline ibool MGLDevCtx::savePNGFromDC(const char *bitmapName,const MGLRect& src)
{ return MGL_savePNGFromDC(dc,bitmapName,src.left(),src.top(),src.right(),src.bottom()); }

inline ibool MGLDevCtx::savePNGFromDC(const char *bitmapName,int left,int top,int right,int bottom)
{ return MGL_savePNGFromDC(dc,bitmapName,left,top,right,bottom); }

inline void MGLDevCtx::drawGlyph(font_t *g,int x,int y,uchar glyph)
{ MGL_drawGlyph(g,x,y,glyph); }

inline void MGLDevCtx::rotateGlyph(uchar *dst,uchar *src,int& byteWidth,
    int& height,int rotation)
{ MGL_rotateGlyph(dst,src,&byteWidth,&height,rotation); }

inline void MGLDevCtx::mirrorGlyph(uchar *dst,uchar *src,int byteWidth,
    int height)
{ MGL_mirrorGlyph(dst,src,byteWidth,height); }

inline int MGLDevCtx::getGlyphWidth(font_t *font,uchar glyph)
{ return MGL_getGlyphWidth(font,glyph); }

inline int MGLDevCtx::getGlyphHeight(font_t *font)
{ return MGL_getGlyphHeight(font); }

inline void MGLDevCtx::setTextSize(int nx,int dx,int ny,int dy)
{ MGL_setTextSize(nx,dx,ny,dy); }

inline void MGLDevCtx::getTextSize(int& nx,int& dx,int& ny,int& dy) const
{ MGL_getTextSize(&nx,&dx,&ny,&dy); }

inline void MGLDevCtx::setTextSettings(const text_settings_t& s)
{ MGL_setTextSettings((text_settings_t*)&s); }

inline void MGLDevCtx::getTextSettings(text_settings_t& s) const
{ MGL_getTextSettings(&s); }

inline void MGLDevCtx::setVisualPage(int page,int waitVRT)
{ MGL_setVisualPage(dc,page,waitVRT); }

inline void MGLBuffer::copyFrom(MGLDC *dc,int left,int top,int right,int bottom,int dstLeft,int dstTop)
{ MGL_copyToBuffer(dc,left,top,right,bottom,dstLeft,dstTop,buf); }

inline void MGLBuffer::copyFrom(MGLDC *dc,const MGLRect& r,int dstLeft,int dstTop)
{ MGL_copyToBuffer(dc,r.left(),r.top(),r.right(),r.bottom(),dstLeft,dstTop,buf); }

inline void MGLBuffer::copyFrom(bitmap_t *bitmap,int left,int top,int right,int bottom,int dstLeft,int dstTop)
{ MGL_copyBitmapToBuffer(bitmap,left,top,right,bottom,dstLeft,dstTop,buf); }

inline void MGLBuffer::copyFrom(bitmap_t *bitmap,const MGLRect& r,int dstLeft,int dstTop)
{ MGL_copyBitmapToBuffer(bitmap,r.left(),r.top(),r.right(),r.bottom(),dstLeft,dstTop,buf); }

#endif  // __MGRAPH_HPP
