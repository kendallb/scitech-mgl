/* Roaming-blobs-on-mars-collect-some-dust-on-a-tropical-island-and-go-pearl-
   diving-before-population-goes-out-of-control. */

/* Each frame, a background virtual screen is copied to a virtual screen; */
/* sprites (well, pixels) are written on that virtual screen; and the */
/* virtual screen is copied to video memory. The background is updated as */
/* appropriate. This simple animation technique works well for 320x200 */
/* because it's so small. */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <vga.h>
#include <vgagl.h>


/* This can be changed into any 256 color mode. */
/* For planar 256 color modes, enable page flipping. */
/* Even 16 color modes work (ugly colors). */
#define VGAMODE G320x200x256

/* #define USE_PAGEFLIPPING */

/* #define USE_SMALLOC */

/* This is the size of the animated window. */
#define MAPWIDTH 320
#define MAPHEIGHT 190

#define MAXMOVERS 1000
#define MAXCITIES 1000
#define NUMBEROFCITIES 20
#define NUMBEROFMOVERS 400
#define MOVERTHRESHOLD 400
#define MOVERLIFETIME 1000
#define COLORTIME 2000

#define randomn( n ) (random() % n)
#define red(x) (32 + x)
#define green(x) (64 + x)
#define yellow(x) (96 + x)
#define blue(x) (128 + x)
#define magenta(x) (160 + x)
#define cyan(x) (192 + x)
#define white(x) (224 + x)


/* Data types */

typedef struct {
    int x;
    int y;
} Position;

#define STAT_ACTIVE 1

typedef struct {
    int stat;
    int x;
    int y;
    int vx;
    int vy;
    int color;
    int time;
} Mover;

typedef struct {
    int x;
    int y;
    int pop;
    int hit;
} City;

/* Global variables */

int map[MAPWIDTH][MAPHEIGHT];
/* Map encoding i: */
/* (0 - 0xffff  Mover number i) */
/* 0x10000...   Part of city (i - 0x10000) */

Mover mover[MAXMOVERS];
int nu_movers = 0;
City city[MAXCITIES];
int nu_cities = 0;
int mytime = 0;			/* used to be "time" but collids w/libc function time() */
int pop = 0;
int framerate, framecount, frameclock;

GraphicsContext *physicalscreen;
GraphicsContext *backscreen;
GraphicsContext *background;


void error(char *s)
{
    printf("%s\n", s);
    vga_setmode(TEXT);
    exit(0);
}

void setcustompalette(void)
{
    /* colors 0-31 are a RGB mix (bits 0 and 1 red, 2 green, 3 and 4 blue) */
    /* 32-63    black to red */
    /* 64-95    black to green */
    /* 96-127   black to yellow */
    /* 128-159  black to blue */
    /* 160-191  black to magenta */
    /* 192-223  black to cyan */
    /* 224-255  black to white */
    Palette pal;
    int i;
    for (i = 0; i < 256; i++) {
	int r, g, b;
	r = g = b = 0;
	if ((i & 32) > 0)
	    r = (i & 31) << 1;
	if ((i & 64) > 0)
	    g = (i & 31) << 1;
	if ((i & 128) > 0)
	    b = (i & 31) << 1;
	if (i < 32) {
	    r = (i & 3) << 4;	/* 2 bits */
	    g = (i & 4) << 3;	/* 1 bit */
	    b = (i & 24) << 1;	/* 2 bits */
	}
	pal.color[i].red = r;
	pal.color[i].green = g;
	pal.color[i].blue = b;
    }
    gl_setpalette(&pal);
}

void initfont(void)
{
    void *font;
#ifdef USE_SMALLOC
    font = smalloc(256 * 8 * 8 * BYTESPERPIXEL);
#else
    font = malloc(256 * 8 * 8 * BYTESPERPIXEL);
#endif
    gl_expandfont(8, 8, white(24), gl_font8x8, font);
    gl_setfont(8, 8, font);
}

int fsize(FILE * f)
{
    int oldpos, size;
    oldpos = ftell(f);
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    fseek(f, oldpos, SEEK_SET);
    return size;
}

void loadfile(char **buf, char *fname)
{
    FILE *f;
    int size;
    f = fopen(fname, "rb");
    size = fsize(f);
    *buf = malloc(size);
    fread(*buf, 1, size, f);
    fclose(f);
}


/* Map */

void clearmap(void)
{
    int x, y;
    for (y = 0; y < MAPHEIGHT; y++)
	for (x = 0; x < MAPWIDTH; x++)
	    map[x][y] = 0;
}

Position
findfreeposition(void)
{
    int x, y;
    Position p;
    do {
	x = randomn(MAPWIDTH);
	y = randomn(MAPHEIGHT);
    }
    while (map[x][y] != 0);
    p.x = x;
    p.y = y;
    return p;
}


/* Movers */

void initmovers(void)
{
    int i;
    for (i = 0; i < MAXMOVERS; i++)
	mover[i].stat = 0;
}

int findfreemoverslot(void)
{
    int i;
    for (i = 0; i < MAXMOVERS; i++)
	if (!(mover[i].stat & STAT_ACTIVE))
	    return i;
    error("Mover table overflow");
    return 0;
}

void addrandommover(void)
{
    Position p = findfreeposition();
    int i = findfreemoverslot();
    int c;
    mover[i].x = p.x;
    mover[i].y = p.y;
    do {
	mover[i].vx = randomn(3) - 1;
	mover[i].vy = randomn(3) - 1;
    }
    while (mover[i].vx == 0 && mover[i].vy == 0);
    mover[i].stat = STAT_ACTIVE;
    switch (randomn(4)) {
    case 0:
	c = blue(20);
	break;
    case 1:
	c = green(20);
	break;
    case 2:
	c = magenta(20);
	break;
    default:
	c = cyan(20);
	break;
    }
    mover[i].time = 0;
    mover[i].color = c;
    nu_movers++;
}

void killmover(int i)
{
    mover[i].stat = 0;
    nu_movers--;
}

void drawmover(int i)
{
    gl_setpixel(mover[i].x, mover[i].y, mover[i].color);
}


/* Cities */

void initcities(void)
{
    nu_cities = 0;
}

void addcity(int x, int y)
{
    int i = nu_cities++;
    map[x][y] = i + 0x10000;
    city[i].x = x;
    city[i].y = y;
    city[i].pop = 1;
    city[i].hit = 0;
}

int cityat(int x, int y)
{
    if (map[x][y] >= 0x10000)
	return map[x][y] - 0x10000;
    else
	return -1;
}

int citycolor(void)
{
    static int colortable[5] =
    {yellow(31), blue(31), white(31), green(31), cyan(31)};
    return colortable[(mytime / COLORTIME) % 5]
	- (mytime % COLORTIME) * 25 / COLORTIME;
}

void growcity(int cx, int cy, int x, int y, int ct)
{
/* add city unit at (x, y) adjacent to city unit (cx, cy) */
    int c;
    map[x][y] = ct + 0x10000;
    c = citycolor();
    gl_setpixel(x, y, c);
    city[ct].pop++;
    city[ct].hit = 20;
    pop++;
}


/* Main components */

void createbackground(void)
{
/* Create fancy dark red background */
    int x, y;
    for (y = 0; y < MAPHEIGHT; y++)
	for (x = 0; x < MAPWIDTH; x++) {
	    int i = 0;
	    int n = 0;
	    int c;
	    if (x > 0) {
		i += gl_getpixel(x - 1, y) - red(0);
		n++;
	    }
	    if (y > 0) {
		i += gl_getpixel(x, y - 1) - red(0);
		n++;
	    }
	    c = (i + randomn(16)) / (n + 1);
	    if (c > 9)
		c = 9;
	    gl_setpixel(x, y, red(0) + c);
	}
}

void drawbackground(void)
{
/* Build up background from map data */
    int x, y;
    gl_setcontext(background);
    gl_clearscreen(0);
    createbackground();
    for (y = 0; y < MAPHEIGHT; y++)
	for (x = 0; x < MAPWIDTH; x++) {
	    int c = cityat(x, y);
	    if (c != -1)
		gl_setpixel(x, y, citycolor());
	}
}

void createcities(void)
{
    int i;
    for (i = 0; i < NUMBEROFCITIES; i++) {
	Position p = findfreeposition();
	addcity(p.x, p.y);
    }
}

void writestat(void)
{
    char s[41];
    int i, x, y;
    int maxpopcity, maxpop;
    sprintf(s, "Pop %7d  Time %7d  Rate %5d.%d", pop, mytime,
	    framerate / 10, framerate % 10);
    gl_setwritemode(WRITEMODE_OVERWRITE);
    gl_write(0, HEIGHT - 8, s);
    maxpop = -1;
    maxpopcity = 0;
    for (i = 0; i < nu_cities; i++)
	if (city[i].pop > maxpop) {
	    maxpop = city[i].pop;
	    maxpopcity = i;
	}
    gl_enableclipping();
    gl_circle(city[maxpopcity].x, city[maxpopcity].y, 10,
	      blue(31));
    gl_disableclipping();

    gl_setwritemode(WRITEMODE_MASKED);
    x = city[maxpopcity].x;
    y = city[maxpopcity].y;
    sprintf(s, "%d", maxpop);
    /* clipping */
    if (x + strlen(s) * 8 > MAPWIDTH)
	x = MAPWIDTH - strlen(s) * 8;
    if (y + 8 > MAPHEIGHT)
	y = MAPHEIGHT - 8;
    gl_write(x, y, s);
}

void drawscreen(void)
{
    int i;
    /* Copy background to backscreen. */
    gl_setcontext(background);
    gl_copyscreen(backscreen);

    /* Now draw the objects in backscreen. */
    gl_setcontext(backscreen);

    for (i = 0; i < MAXMOVERS; i++)
	if (mover[i].stat & STAT_ACTIVE) {
	    drawmover(i);
	}
    writestat();

    /* Copy backscreen to physical screen. */
    gl_copyscreen(physicalscreen);
}

void move(void)
{
    int i;
    gl_setcontext(background);
    for (i = 0; i < MAXMOVERS; i++)
	if (mover[i].stat & STAT_ACTIVE) {
	    int nx, ny;
	    int c;
	    if (++mover[i].time == MOVERLIFETIME) {
		killmover(i);
		continue;
	    }
	    for (;;) {
		nx = mover[i].x + mover[i].vx;
		ny = mover[i].y + mover[i].vy;
		if (nx < 0 || nx >= MAPWIDTH) {
		    mover[i].vx = -mover[i].vx;
		    continue;
		}
		if (ny < 0 || ny >= MAPHEIGHT) {
		    mover[i].vy = -mover[i].vy;
		    continue;
		}
		break;
	    }
	    c = cityat(nx, ny);
	    if (c != -1) {	/* found city */
		killmover(i);
		growcity(nx, ny, mover[i].x, mover[i].y, c);
		continue;	/* next mover */
	    }
	    mover[i].x = nx;
	    mover[i].y = ny;
	}
    if (pop >= MAPWIDTH * MAPHEIGHT * 255 / 256) {
	/* start all over again */
	printf("fun: new run.\n");
	pop = 0;
	mytime = 0;
	clearmap();
	initcities();
	createcities();
	drawbackground();
    }
}

void createmovers(void)
{
    int i;
    for (i = 0; i < NUMBEROFMOVERS; i++)
	addrandommover();
}

void main(void)
{
    vga_init();

    clearmap();
    initmovers();
    createcities();
    createmovers();

    vga_setmode(VGAMODE);
    gl_setcontextvga(VGAMODE);
    physicalscreen = gl_allocatecontext();
    gl_getcontext(physicalscreen);

#ifdef USE_PAGEFLIPPING
    /* Try to enable page flipping. */
    gl_enablepageflipping(&physicalscreen);
#endif
    setcustompalette();
    /* initfont() here caused trouble with planar 256 color modes. */

    gl_setcontextvgavirtual(VGAMODE);
    backscreen = gl_allocatecontext();
    gl_getcontext(backscreen);
#ifdef USE_SMALLOC
    free(backscreen->vbuf);
    backscreen->vbuf = smalloc(BYTEWIDTH * HEIGHT);
    gl_setcontext(backscreen);
#endif

    initfont();

    gl_setcontextvgavirtual(VGAMODE);
    background = gl_allocatecontext();
    gl_getcontext(background);
#ifdef USE_SMALLOC
    free(background->vbuf);
    background->vbuf = smalloc(BYTEWIDTH * HEIGHT);
    gl_setcontext(background);
#endif

    drawbackground();

    framerate = 0;
    framecount = 0;
    frameclock = clock();

    for (;;) {
	int i;
	drawscreen();
	move();
	for (i = 0; i < 4; i++)
	    if (nu_movers < MOVERTHRESHOLD)
		addrandommover();
	mytime++;
	/* Update frame rate every 3 seconds. */
	framecount++;
	if (clock() - frameclock >= 300) {
	    framerate = framecount * 1000 / (clock() - frameclock);
	    framecount = 0;
	    frameclock = clock();
	}
    }

#ifndef USE_SMALLOC
    gl_freecontext(backscreen);
    gl_freecontext(background);
#endif

    vga_setmode(TEXT);
    exit(0);
}
