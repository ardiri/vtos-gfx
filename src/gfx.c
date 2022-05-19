/*
 * @(#)gfx.c
 *
 * Copyright 2000, Aaron Ardiri (mailto:aaron@ardiri.com)
 * All rights reserved.
 *
 * This file  was generated as part  of the "gfx" graphics library that
 * is specifically  developed for the Helio Computing Platform designed 
 * by vtech:
 *
 *   http://www.vtechinfo.com/
 *
 * The contents of this file is confidential and  proprietary in nature
 * ("Confidential Information"). Redistribution or modification without 
 * prior consent of the original author is prohibited.
 */

#include "gfx.h"

// font/bitmap resources
#include "font.inc"
#include "bitmap.inc"

// global variable structure
typedef struct
{
  GfxWindow *winDeviceLCD;
  GfxWindow *winActive;

  GfxFont   fntActive;
  GfxRegion clipArea;

  GfxWindow **winFonts;
  SHORT     *fontHeight;
  SHORT     **fontWidth;
  SHORT     **fontOffset;
} GfxGlobals;

// globals reference
static GfxGlobals *globals;
static BYTE       gfxVersion[16] = "1.0a standard";

// helper macros
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#define MAX(a,b) (((a) > (b)) ? (a) : (b))

// interface
static void  GfxGetOSVersion(BYTE *);
static void *GfxGetLCDAddress();
static BYTE  GfxDrawOperationAdjust(BYTE, BYTE, GfxDrawOperation);
static void  GfxDrawChar(BYTE, SHORT, SHORT, GfxDrawOperation);

/**
 * Initialize the graphics engine.
 */
void
GfxInitialize()
{
  SHORT i;

  // create the globals object
  globals = (GfxGlobals *)pmalloc(sizeof(GfxGlobals));  

  // initialize the "base" window
  globals->winDeviceLCD = (GfxWindow *)pmalloc(sizeof(GfxWindow));
  globals->winDeviceLCD->width   = 160;
  globals->winDeviceLCD->height  = 160;
  globals->winDeviceLCD->memSize = 12800;
  globals->winDeviceLCD->memory  = GfxGetLCDAddress();

  // initialize the "font" resources 
  globals->winFonts = (GfxWindow **)pmalloc(sizeof(GfxWindow*) * gfx_maxFonts);
  globals->fontHeight = (SHORT *)pmalloc(sizeof(SHORT) * gfx_maxFonts);
  globals->fontWidth  = (SHORT **)pmalloc(sizeof(SHORT*) * gfx_maxFonts);
  globals->fontOffset = (SHORT **)pmalloc(sizeof(SHORT*) * gfx_maxFonts);
  for (i=0; i<gfx_maxFonts; i++) {
    globals->winFonts[i]   = NULL;
    globals->fontWidth[i]  = NULL; 
    globals->fontOffset[i] = NULL; 
  }

  // define the "standard" fonts we provide :P
  GfxDefineFont(gfx_helioSmallFont,font00Size,font00,
                font00WindowWidth,font00WindowHeight,font00Width);
  GfxDefineFont(gfx_palmosNormalFont,font01Size,font01,
                font01WindowWidth,font01WindowHeight,font01Width);
  GfxDefineFont(gfx_palmosBoldFont,font02Size,font02,
                font02WindowWidth,font02WindowHeight,font02Width);

  // by default, all drawing operations to LCD
  globals->winActive = globals->winDeviceLCD;
  globals->fntActive = gfx_helioSmallFont;
  GfxResetClip();
  GfxGetClip(&globals->clipArea);

  // do we need to show some "version" information :)
  {
    UWORD pinKeys;

    // has the user requested it?
    pinKeys = (CPU->REG_IO_CTRL & READ_IO_KEY1TO7);
    if (pinKeys & 0x08) {

      GfxRegion region = {{  0,  0},{160,160}};
      BYTE      *str, *ptrStr;
      SHORT     x, y;
      GfxFont   currFont;
      GfxWindow *tmpWindow;

      // draw the info we need!
      currFont = GfxGetFont();
      GfxFillRegion(GfxGetDisplayWindow(),&region,gfx_white);
      
      // initialize
      y   = 2;
      str = (BYTE *)pmalloc(256 * sizeof(BYTE));
      GfxSetFont(gfx_palmosNormalFont);

      // draw logo info
      x = 8;
      {
        GfxRegion r = {{0,0},{pawLogoWidth,pawLogoHeight}};

        // initialize
        tmpWindow = (GfxWindow *)pmalloc(sizeof(GfxWindow));
        tmpWindow->width   = pawLogoWidth;
        tmpWindow->height  = pawLogoHeight;
        tmpWindow->memSize = pawLogoSize;
        tmpWindow->memory  = (void *)pawLogoBitmap;

        // copy the bitmap resource
        GfxCopyRegion(tmpWindow, GfxGetDisplayWindow(), &r, x, y, gfxPaint);

        // clean up
        pfree(tmpWindow);
      }

      x = 52;
      y = 8;
      strcpy(str, "http://www.ardiri.com/");
      GfxDrawString(str, strlen(str), x, y, gfxPaint);
      y += GfxGetFontHeight();

      x = 54;
      strcpy(str, "aaron@ardiri.com");
      GfxSetFont(gfx_palmosBoldFont);
      GfxDrawString(str, strlen(str), x, y, gfxPaint);
      y += GfxGetFontHeight();

      // add some space
      y += GfxGetFontHeight() - 1;

      // draw "seperator"
      GfxDrawLine(0,y,region.extent.x,y,gfx_black);
      y += GfxGetFontHeight() >> 1;

      x = 24;
      strcpy(str, "Library: ");
      GfxSetFont(gfx_palmosBoldFont);
      GfxDrawString(str, strlen(str), x, y, gfxPaint);
      strcpy(str, "gfx [Helio]");
      GfxSetFont(gfx_palmosNormalFont);
      x = 80;
      GfxDrawString(str, strlen(str), x, y, gfxPaint);
      y += GfxGetFontHeight();
      GfxSetFont(gfx_palmosBoldFont);

      x = 24;
      strcpy(str, "Version: ");
      GfxDrawString(str, strlen(str), x, y, gfxPaint);
      strcpy(str, gfxVersion);
      GfxSetFont(gfx_palmosNormalFont);
      x = 80;
      GfxDrawString(str, strlen(str), x, y, gfxPaint);
      y += GfxGetFontHeight();

      // add a space (little)
      y += GfxGetFontHeight() >> 1;

      // draw "seperator"
      GfxDrawLine(0,y-2,region.extent.x,y-2,gfx_black);
      y += GfxGetFontHeight() >> 1;

      // draw copyright notices
      x = 4;
      strcpy(str, "Copyright 2000, Aaron Ardiri");
      GfxDrawString(str, strlen(str), x, y, gfxPaint);
      y += GfxGetFontHeight();
      
      strcpy(str, 
"This library is the property of Aaron Ardiri. Its usage is bound to the \
license agreement that is distributed with the library. A copy of the \
agreement can be obtained from the website shown.");
      ptrStr = str;
      while (strlen(ptrStr) != 0) {
        BYTE count = GfxGetWordWrap(ptrStr,region.extent.x - x);

        GfxDrawString(ptrStr, count, x, y, gfxPaint);
        y += GfxGetFontHeight();

        ptrStr += count;
      }

      // add a space (little)
      y += GfxGetFontHeight() >> 1;

      strcpy(str, "Unauthorized use is prohibited.");
      GfxDrawString(str, strlen(str), x, y, gfxPaint);
      y += GfxGetFontHeight();

      // wait until they release the key :)
      do {
        pinKeys = (CPU->REG_IO_CTRL & READ_IO_KEY1TO7);
      } while (pinKeys & 0x08);

      // clean up
      GfxFillRegion(GfxGetDisplayWindow(),&region,gfx_white);
      GfxSetFont(currFont);
      pfree(str);
    }
  }
}

/**
 * Get the version of the graphics engine.
 *
 * @return the version of the graphics engine.
 */
SHORT
GfxGetVersion()
{
  return gfx_version1;
}

/**
 * Create an offscreen window.
 *
 * @param width  the width of the window
 * @param height the height of the window
 * @return a window reference, NULL if error.
 */
GfxWindow* 
GfxCreateWindow(SHORT width, SHORT height)
{
  GfxWindow *win;

  // allocate the window handle
  win = (GfxWindow *)pmalloc(sizeof(GfxWindow));
  if (win != NULL) {

    // lets make sure the "width" is always even - so, byte alignment occurs
    width = ((width + 1) >> 1) << 1; 

    // initialize the window
    win->width   = width;
    win->height  = height;
    win->memSize = (width*height) >> 1;               // NOTE: helio is 4bpp
    win->memory  = (void *)pmalloc(win->memSize);
    if (win->memory == NULL) { pfree(win); win = NULL; }
  }

  return win;
}

/**
 * Release a window and its resources.
 *
 * @param window the window to release.
 */
void
GfxDisposeWindow(GfxWindow *window)
{
  // cannot "free" the LCD window :)
  if ((window != NULL) && (window != globals->winDeviceLCD)) {

    // clean up, as needed
    if (window->memory != NULL) pfree(window->memory);
    pfree(window);
  }
}

/**
 * Get the display (LCD) window.
 *
 * @return the display (LCD) window.
 */
GfxWindow*
GfxGetDisplayWindow()
{
  return globals->winDeviceLCD;
}

/**
 * Set the current drawing window to the one specified.
 *
 * @param window the new drawing window.
 */
void
GfxSetDrawWindow(GfxWindow *window)
{
  // lets make sure it is valid :)
  if ((window != NULL) && (window->memory != NULL)) {
    globals->winActive = window;
    GfxResetClip();
  }
}

/**
 * Get the current drawing window.
 *
 * @return the current drawing window.
 */
GfxWindow*
GfxGetDrawWindow()
{
  return globals->winActive;
}

/**
 * Define a font for use in the graphics engine.
 *
 * @param font             the font index.
 * @param fontSize         the size of the memory used by the fontData pointer.
 * @param fontData         a pointer to the binary image of the font window.
 * @param fontWindowWidth  the width of the font window.
 * @param fontWindowHeight the height of the font window.
 * @param fontWidths       a pointer to an array of widths
 *                         (size = gfx_fontCharCount)
 */
void
GfxDefineFont(GfxFont font,
              SHORT   fontSize,
              BYTE    *fontData,
              SHORT   fontWindowWidth,
              SHORT   fontWindowHeight,
              SHORT   *fontWidths)
{
  // lets make sure it aint defined.
  if (globals->winFonts[font] == NULL) {

    // create the "window"
    globals->winFonts[font]          = (GfxWindow *)pmalloc(sizeof(GfxWindow));
    globals->winFonts[font]->width   = fontWindowWidth;
    globals->winFonts[font]->height  = fontWindowHeight;
    globals->winFonts[font]->memSize = fontSize;
    globals->winFonts[font]->memory  = (void *)fontData;

    // adjust the "data" stuff
    globals->fontHeight[font] = fontWindowHeight;
    globals->fontWidth[font]  = (void *)fontWidths;
    globals->fontOffset[font] = 
      (SHORT *)pmalloc(sizeof(SHORT) * gfx_fontCharCount);

    // calculate the "offsets" for the copying
    {
      SHORT i, ofs = 0;

      for (i=0; i<gfx_fontCharCount; i++) {
        globals->fontOffset[font][i] = ofs;
        ofs += fontWidths[i];
      }
    }
  }
}

/**
 * Set the current drawing font to the one specified.
 *
 * @param font the new font.
 */
void
GfxSetFont(GfxFont font)
{
  // lets make sure it is valid :)
  if (globals->winFonts[font] != NULL)
    globals->fntActive = font;
}

/**
 * Get the current drawing font.
 *
 * @return the current drawing font.
 */
GfxFont
GfxGetFont()
{
  return globals->fntActive;
}

/**
 * Clear the contents of a window.
 *
 * @param window the window to clear the contents of.
 */
void
GfxClearWindow(GfxWindow *window)
{
  // if we have a window, clear it!
  if ((window != NULL) && (window->memory != NULL)) 
    memset(window->memory, (UBYTE)0, window->memSize);
}

/**
 * Copy of region from one window to another using a specific draw operation.
 *
 * @param srcWin the source window.
 * @param dstWin the destination window.
 * @param region the region to copy. 
 * @param x the x co-ordinate of the top-left corner to copy to.
 * @param y the y co-ordinate of the top-left corner to copy to.
 * @param mode the copy mode mode.
 */
void
GfxCopyRegion(GfxWindow *srcWin, GfxWindow *dstWin, 
              GfxRegion *region, SHORT x, SHORT y, GfxDrawOperation mode)
{
  // lets make sure we have valid windows to mess around with :)
  if (((srcWin != NULL) && (srcWin->memory != NULL)) &&
      ((dstWin != NULL) && (dstWin->memory != NULL))) {

    SHORT     x1, y1, x2, y2;
    int       i, j;

    // whats a "valid" box area for this operation? - source window
    x1 = MAX(0, region->topLeft.x);
    y1 = MAX(0, region->topLeft.y);
    x2 = MIN(srcWin->width,  region->topLeft.x + region->extent.x);
    y2 = MIN(srcWin->height, region->topLeft.y + region->extent.y);

    // whats a "valid" box area for this operation? - destination window
    if (x < 0) { x1 = x1 - x; x = 0; }
    if (y < 0) { y1 = y1 - y; y = 0; }
    if ((x + (x2 - x1)) > dstWin->width)  { x2 = x1 + (dstWin->width  - x); }
    if ((y + (y2 - y1)) > dstWin->height) { y2 = y1 + (dstWin->height - y); }

    // do we still have a valid region?
    if ((x2 > x1) && (y2 > y1)) {

      BYTE    *ptrSrc, *ptrDst, *srcP, *dstP;
      BOOLEAN srcOfs, dstOfs;
      BYTE    val, src, dst;
      SHORT   cnt;

      // determine the "pointer" values..
      ptrSrc = (BYTE *)(srcWin->memory + (((y1 * srcWin->width) + x1) >> 1));
      ptrDst = (BYTE *)(dstWin->memory + (((y  * dstWin->width) + x)  >> 1));
      srcOfs = ((x1 % 2) == 1);
      dstOfs = ((x % 2)  == 1);
      cnt    = (x2-x1);

      // do each line...
      for (j=y1; j<y2; j++) {

        // prepare pointers
        srcP = ptrSrc; 
        dstP = ptrDst; 

        // do the inner blit
        for (i=0; i<cnt; i++) {

          // get the values
          if ((i % 2) == 0) {
            if (srcOfs)  { src = (*srcP & 0x0F); srcP++; }
            else           src = (*srcP & 0xF0) >> 4;
            if (dstOfs)  { dst = (*dstP & 0x0F); }
            else           dst = (*dstP & 0xF0) >> 4;
          }
          else {
            if (!srcOfs) { src = (*srcP & 0x0F); srcP++; }
            else           src = (*srcP & 0xF0) >> 4;
            if (!dstOfs) { dst = (*dstP & 0x0F); }
            else           dst = (*dstP & 0xF0) >> 4;
          }

          // adjust the values...
          dst = GfxDrawOperationAdjust(src, dst, mode);

          // set the values
          if ((i % 2) == 0) {
            if (dstOfs)  { *dstP = (*dstP & 0xF0) | (dst & 0x0F); dstP++; }
            else           *dstP = (*dstP & 0x0F) | ((dst << 4) & 0xF0);
          }
          else {
            if (!dstOfs) { *dstP = (*dstP & 0xF0) | (dst & 0x0F); dstP++; }
            else           *dstP = (*dstP & 0x0F) | ((dst << 4) & 0xF0);
          }
        }

        // adjust "pointers"...
        ptrSrc += srcWin->width >> 1;
        ptrDst += dstWin->width >> 1;              // NOTE: helio is 4bpp
      }
    }
  }
}

/**
 * Fill a region in a specific window with a specific color.
 *
 * @param window the window to clear the contents of.
 * @param region the region to fill. 
 * @param color the color to fill the region with.
 */
void
GfxFillRegion(GfxWindow *window, GfxRegion *region, GfxColor color)
{
  // lets make sure we have a window to mess around with :)
  if ((window != NULL) && (window->memory != NULL)) {

    SHORT     x1, y1, x2, y2;
    int       j;

    // whats a "valid" box area for this operation?
    x1 = MAX(globals->clipArea.topLeft.x, region->topLeft.x);
    y1 = MAX(globals->clipArea.topLeft.y, region->topLeft.y);
    x2 = MIN(globals->clipArea.topLeft.x + globals->clipArea.extent.x,
             region->topLeft.x + region->extent.x);
    y2 = MIN(globals->clipArea.topLeft.y + globals->clipArea.extent.y,
             region->topLeft.y + region->extent.y);

    // do we still have a valid region?
    if ((x2 > x1) && (y2 > y1)) {

      BYTE *ptr, val;
      int   cnt;

      // determine the "pointer" value, and how many bytes to blit
      if (x1 % 2 == 0) {
        ptr = (BYTE *)(window->memory + (((y1 * window->width) + x1) >> 1));
        cnt = (x2 - x1) >> 1;
      }
      else {
        ptr = (BYTE *)(window->memory + (((y1 * window->width) + (x1+1)) >> 1));
        cnt = (x2 - (x1+1)) >> 1;
      }
      val = (color << 4) | color;

      // do each line...
      for (j=y1; j<y2; j++) {

        // prefix pixel
        if (x1 % 2 == 1) 
          *(ptr-1) = (*(ptr-1) & 0xF0) | color;

        // inner chunk 
        memset(ptr, (UBYTE)val, cnt);

        // postfix pixel
        if ((x2-1) % 2 == 0) 
          *(ptr+cnt) = (*(ptr+cnt) & 0x0F) | (color << 4);

        // adjust "pointer"...
        ptr += window->width >> 1;              // NOTE: helio is 4bpp
      }
    }
  }
}

/**
 * Turn a pixel on in the current display window.
 * 
 * @param x the x co-ordinate of the pixel.
 * @param y the y co-ordinate of the pixel.
 * @param color the color to set the pixel to.
 */
void
GfxSetPixel(SHORT x, SHORT y, GfxColor color)
{
  GfxWindow *window = globals->winActive;

  // lets make sure the "pixel" is in the clipping region
  if ((x >= globals->clipArea.topLeft.x) && 
      (x < (globals->clipArea.topLeft.x + globals->clipArea.extent.x)) && 
      (y >= globals->clipArea.topLeft.y) && 
      (y < (globals->clipArea.topLeft.y + globals->clipArea.extent.y))) {

    BYTE *ptrPixel = 
      (BYTE *)(window->memory + (((y * window->width) + x) >> 1));

    // which nibble should we adjust? [NOTE: endian]!!
    if (x % 2 == 1) *ptrPixel = (*ptrPixel & 0xF0) | color;
    else            *ptrPixel = (*ptrPixel & 0x0F) | (color << 4);
  }
}

/**
 * Get the color of a pixel in the current display window.
 * 
 * @param x the x co-ordinate of the pixel.
 * @param y the y co-ordinate of the pixel.
 * @param color the color to set the pixel to.
 */
GfxColor
GfxGetPixel(SHORT x, SHORT y)
{
  GfxColor  result;
  GfxWindow *window = globals->winActive;

  // lets make sure the "pixel" is in the clipping region
  if ((x >= globals->clipArea.topLeft.x) && 
      (x < (globals->clipArea.topLeft.x + globals->clipArea.extent.x)) && 
      (y >= globals->clipArea.topLeft.y) && 
      (y < (globals->clipArea.topLeft.y + globals->clipArea.extent.y))) {

    BYTE *ptrPixel =
      (BYTE *)(window->memory + (((y * window->width) + x) >> 1));

    // which nibble should we grab? [NOTE: endian]!!
    if (x % 2 == 1) result = (*ptrPixel & 0xF0) >> 4;
    else            result = (*ptrPixel & 0x0F);
  }

  return result;
}

/**
 * Draw a line using bresenhams algorithm.
 * 
 * @param x1 the first x-coordinate of the line
 * @param y1 the first y-coordinate of the line
 * @param x2 the second x-coordinate of the line
 * @param y2 the second y-coordinate of the line
 * @param color the color to display the line
 */
void
GfxDrawLine(SHORT x1, SHORT y1, SHORT x2, SHORT y2, GfxColor color)
{
  SHORT krit, dx, dy, sx, sy;

  //
  // initialize bresenhams variables
  //

  dx   = (((x2 - x1) < 0) ? (x1 - x2) : (x2 - x1));
  dy   = (((y2 - y1) < 0) ? (y1 - y2) : (y2 - y1));
  krit = ((dx == 0) ? 0 : (-dx >> 1));
  sx   = ((x2 > x1) ? 1 : -1);
  sy   = ((y2 > y1) ? 1 : -1);

  //
  // draw the pixels for the line
  //

  GfxSetPixel(x1, y1, color);
  while ((x1 != x2) || (y1 != y2)) {

    // adjust x1, y1 based on differential values
    if (krit < 0) {
      x1   += sx;
      krit += dy;
    }
    else 
    if (y1 != y2) {
      y1   += sy;
      krit -= dx;
    }

    GfxSetPixel(x1, y1, color);
  }
}

/**
 * Draw a string.
 *
 * @param string the string to draw.
 * @param len the number of characters in the string to draw.
 * @param x the x co-ordinate to draw the string to.
 * @param y the y co-ordinate of draw the string to.
 * @param mode the copy mode mode.
 */
void
GfxDrawString(BYTE *string, SHORT len,
              SHORT x, SHORT y, GfxDrawOperation mode)
{
  SHORT i;

  // draw each character, as needed...
  for (i=0; i<len; i++) {
    GfxDrawChar(string[i], x, y, mode); x += GfxGetCharWidth(string[i]);
  }
}

/**
 * Calculate the number of characters that can be displayed from a
 * character string based on the number of pixels provided using the
 * traditional "word wrap" breakdown system.
 *
 * @param string the character string.
 * @param maxPixels the maximum number of pixels available on the line.
 * @return the number of characters that can be displayed.
 */
SHORT
GfxGetWordWrap(BYTE *string, SHORT maxPixels)
{
  SHORT   x, pos, brkpos;
  BOOLEAN done;

  // initialize
  x = pos = brkpos = 0;
  done = (string[pos] == '\0');

  // process
  while (!done) {

    BYTE chr = string[pos++];

    // what character do we have?
    switch (chr) 
    {
      case ' ':
           if (x > maxPixels) {
             pos  = (brkpos != 0) ? brkpos : pos;
             while (string[pos] == ' ') pos++;
             done = TRUE;
           }
           else 
             brkpos = pos;
           
           x += GfxGetCharWidth(chr);
           break;

      default:
           x += GfxGetCharWidth(chr);
           if (x > maxPixels) {
             pos  = (brkpos != 0) ? brkpos : pos - 1;
             done = TRUE;
           }
           break;
    }

    // are we done?
    done |= (string[pos] == '\0');
  }

  // return the result
  return pos;
}

/**
 * Get the height (in pixels) of the current font.
 *
 * @return the height (in pixels) of the current font.
 */
SHORT
GfxGetFontHeight()
{
  return (globals->fontHeight[globals->fntActive] + 1);
}

/**
 * Get the width of a series of characters (in pixels) with the current font.
 *
 * @param string the string to process.
 * @param len the number of characters in the string to process.
 * @return the width of the character (in pixels).
 */
SHORT
GfxGetCharsWidth(BYTE *str, SHORT len)
{
  SHORT i, result = 0;

  for (i=0; i<len; i++) {
    result += GfxGetCharWidth(str[i]);
  }

  return result;
}

/**
 * Get the width of a particular character (in pixels) with the current font.
 *
 * @param chr the character.
 * @return the width of the character (in pixels).
 */
SHORT
GfxGetCharWidth(BYTE chr)
{
  return globals->fontWidth[globals->fntActive][chr & gfx_fontCharMask];
}

/**
 * Reset the clipping region.
 */
void
GfxResetClip()
{
  globals->clipArea.topLeft.x = 0;
  globals->clipArea.topLeft.y = 0;
  globals->clipArea.extent.x  = globals->winActive->width;
  globals->clipArea.extent.y  = globals->winActive->height;
}

/**
 * Get the current clipping region.
 *
 * @param region a pointer to the region structure.
 */
void
GfxGetClip(GfxRegion *region)
{
  region->topLeft.x = globals->clipArea.topLeft.x;
  region->topLeft.y = globals->clipArea.topLeft.y;
  region->extent.x  = globals->clipArea.extent.x;
  region->extent.y  = globals->clipArea.extent.y;
}

/**
 * Set the current clipping region.
 *
 * @param region a pointer to the region structure.
 */
void
GfxSetClip(GfxRegion *region)
{
  SHORT x1, y1, x2, y2;

  x1 = MAX(0, region->topLeft.x);
  y1 = MAX(0, region->topLeft.y);
  x2 = MIN(globals->winActive->width,  region->topLeft.x + region->extent.x);
  y2 = MIN(globals->winActive->height, region->topLeft.y + region->extent.y);

  // do we still have a valid region?
  if ((x2 > x1) && (y2 > y1)) {

    // adjust the clipping area
    globals->clipArea.topLeft.x = x1;
    globals->clipArea.topLeft.y = y1;
    globals->clipArea.extent.x  = x2 - x1;
    globals->clipArea.extent.y  = y2 - y1;
  }
}

/**
 * Shutdown the graphics engine.
 */
void
GfxTerminate()
{
  SHORT i;

  // clean up
  for (i=0; i<gfx_maxFonts; i++) {
    if (globals->winFonts[i]) {
      pfree(globals->winFonts[i]);
      pfree(globals->fontOffset[i]);
      globals->fontWidth[i] = NULL;
    }
  }
  pfree(globals->winFonts);
  pfree(globals->fontHeight);
  pfree(globals->fontWidth);
  pfree(globals->fontOffset);

  pfree(globals->winDeviceLCD);
  globals->winActive = NULL;
  pfree(globals);
}

/**
 * Get the OS version number for the device.
 *
 * @param osVersion a string to store the version string info.
 */
static void
GfxGetOSVersionNumber(BYTE *osVersion)
{
  BYTE *tmpString;

  // allocate memory for the version
  tmpString = (BYTE *)pmalloc(16 * sizeof(BYTE));

  // 1.1.08 and greater call
  SysGetOSVersionNo(tmpString);

  // if we dont get a nice response, lets try the 1.1.03 way :)
  if ((tmpString[0] < '0') || (tmpString[0] > '9') || (tmpString[1] != '.'))
    SysGetOSVersionNo(&tmpString);

  // transfer the "version" string
  strcpy(osVersion, tmpString);

  // clean up
  pfree(tmpString);
}

/**
 * Get the display address of the helio LCD.
 *
 * @return the display address of the helio LCD.
 */
static void*
GfxGetLCDAddress()
{
  void *result;
  BYTE osVersion[16];

// NOTE: this is a serious hack for the helio system,  and it WILL change
//       with different versions of the operating system and such. it has
//       been implemented here as a  workaround for not being able to get
//       a reference to the "lcd_mem" global defined in the helio source.
//
// KNOWN VALUES: [hack]
//
// return (void *)0x80012770;             // vt-os 1.1.03 rom
// return (void *)0x80013770;             // vt-os 1.1.06 rom
// return (void *)0x80013760;             // vt-os 1.1.08 rom
// return (void *)0x80013380;             // vt-os 1.3.01 rom
// return (void *)LcdGetLcdMemAddress();  // vt-os 1.3.01 SDK and later
//
//    vt-os 1.3.01 SDK contains LcdGetLcdMemAddress() call to do this.
//
// -- Aaron Ardiri, 2000

  // get the os version
  GfxGetOSVersionNumber(osVersion);

  // lets "deal" with this problem
  if (strncmp(osVersion, "1.1.03", 6) == 0) 
    result = (void *)0x80012770;                 // os == 1.1.03
  else
  if (strncmp(osVersion, "1.1.06", 6) == 0) 
    result = (void *)0x80013770;                 // os == 1.1.06
  else
  if (strncmp(osVersion, "1.1.08", 6) == 0) 
    result = (void *)0x80013760;                 // os == 1.1.08
  else
    result = (void *)LcdGetLcdMemAddress();      // os == 1.3.01 and higher

  return result;
}

/**
 * Perform the draw operation (paint, mask, invert, overlay) adjustment.
 *
 * @param src the data to be copied over.
 * @param dst the existing data in place.
 * @param mode the copy mode mode.
 * @return the adjusted value.
 */
static BYTE
GfxDrawOperationAdjust(BYTE src, BYTE dst, GfxDrawOperation mode)
{
  BYTE result = 0;

  // calculate the "adjustment"
  switch (mode)
  {
    case gfxMask:    result = dst & ~src;
         break;

    case gfxInvert:  result = dst ^ src;
         break;

    case gfxOverlay: result = dst | src;
         break;

    case gfxPaint:
    default:         result = src;
         break;
  }

  return result;
}

/** 
 * Draw a single character.
 *
 * @param string the string to draw.
 * @param x the x co-ordinate to draw the string to.
 * @param y the y co-ordinate of draw the string to.
 * @param mode the copy mode mode.
 */
static void
GfxDrawChar(BYTE chr, SHORT x, SHORT y, GfxDrawOperation mode)
{
  GfxWindow *winSrc, *winDst;
  GfxRegion region;
  
  // configure
  winSrc = globals->winFonts[globals->fntActive];
  winDst = globals->winActive;

  region.topLeft.x = 
    globals->fontOffset[globals->fntActive][chr & gfx_fontCharMask];
  region.topLeft.y = 0;
  region.extent.x  = 
    globals->fontWidth[globals->fntActive][chr & gfx_fontCharMask];
  region.extent.y  = globals->fontHeight[globals->fntActive];

  // copy over the character bitmap!
  GfxCopyRegion(winSrc, winDst, &region, x, y, mode);
}
