

#define uchar unsigned char
#define swap(x, y) { int temp = x; x = y; y = temp; }
#define swapb(x, y) { uchar temp = x; x = y; y = temp; }
#define max(x, y) ((x > y) ? x : y)
#define min(x, y) ((x > y) ? y : x)
#define outside(x, y) (x < __clipx1 || x > __clipx2 || y < __clipy1 \
	|| y > __clipy2)
#define x_outside(x) (x < __clipx1 || x > __clipx2)
#define y_outside(y) (y < __clipy1 || y > __clipy2)
#define clipxleft(x) if (x < __clipx1) x = __clipx1;
#define clipxright(x) if (x > __clipx2) x = __clipx2;
#define clipytop(y) if (y < __clipy1) y = __clipy1;
#define clipybottom(y) if (y > __clipy2) y = __clipy2;


#define setpixel (*(__currentcontext.ff.driver_setpixel_func))
#define getpixel (*(__currentcontext.ff.driver_getpixel_func))
#define hline (*(__currentcontext.ff.driver_hline_func))
#define fillbox (*(__currentcontext.ff.driver_fillbox_func))
#define putbox (*(__currentcontext.ff.driver_putbox_func))
#define getbox (*(__currentcontext.ff.driver_getbox_func))
#define putboxmask (*(__currentcontext.ff.driver_putboxmask_func))
#define putboxpart (*(__currentcontext.ff.driver_putboxpart_func))
#define getboxpart (*(__currentcontext.ff.driver_getboxpart_func))
#define copybox (*(__currentcontext.ff.driver_copybox_func))

#define TEXT_TABSIZE 8

#ifdef DLL_CONTEXT_SHADOW

/* Library uses internal currentcontext for faster DLL library. */

#undef BYTESPERPIXEL
#undef BYTEWIDTH
#undef WIDTH
#undef HEIGHT
#undef VBUF
#undef MODETYPE
#undef MODEFLAGS
#undef BITSPERPIXEL
#undef COLORS
#undef __clip
#undef __clipx1
#undef __clipy1
#undef __clipx2
#undef __clipy2

extern GraphicsContext __currentcontext;

#define BYTESPERPIXEL (__currentcontext.bytesperpixel)
#define BYTEWIDTH (__currentcontext.bytewidth)
#define WIDTH (__currentcontext.width)
#define HEIGHT (__currentcontext.height)
#define VBUF (__currentcontext.vbuf)
#define MODETYPE (__currentcontext.modetype)
#define MODEFLAGS (__currentcontext.modeflags)
#define BITSPERPIXEL (__currentcontext.bitsperpixel)
#define COLORS (__currentcontext.colors)

#define __clip (__currentcontext.clip)
#define __clipx1 (__currentcontext.clipx1)
#define __clipy1 (__currentcontext.clipy1)
#define __clipx2 (__currentcontext.clipx2)
#define __clipy2 (__currentcontext.clipy2)

#else

#define __currentcontext currentcontext

#endif
