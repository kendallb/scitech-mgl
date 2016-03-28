/* 
   spin.c - A simple (or not quite so) app to test the SpaceOrb by rotating
   	    a shape of a given number of sides.

    Copyright (C) 1997  Eric Sharkey, Jason Lyons, Brett Viren

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   ---- end copyleft notice ----

   This program is FREE!!!  However, if you feel that nothing is free and you
   refuse to use something someone else made unless paid for, you can send a 
   donation to me (Jason Lyons).  For information on this, E-MAIL me at 
   jason_l@hotmail.com.

   This graphical test program was originally written by Jason Lyons and
   later hacked to bits by Eric Sharkey, but this program would not be
   possible without Brett's efforts.  Be sure to thank him!

   The original code for the SpaceOrb in Unix/Linux was written by Brett Viren
   (Brett.Viren@sunysb.edu) on his free time (ie, not when on the pay
   clock of SUNY@Stony Brook). Do with it what you will, as long as
   you abide by the GPL.

   I think by now all of Brett's code has been removed from this demo
   and incorporated into svgalib itself, but he's still responsible for
   starting this whole thing.

*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vga.h>
#include <vgagl.h>
#include <vgamouse.h>
#include <malloc.h>
#include "spintables.h"

#define DONT_MANUALLY_SETUP_MOUSE
#define NODEBUG

#define GMODE		  5
#define SCREEN_WIDTH	320
#define SCREEN_HEIGHT	200
#define SCREEN_DEPTH	100

#ifndef ROTATION_ONLY        
/* Note: 6-D protocol assumes x is left/right, y is in/out, and z is up/down, */
/* which is logical for a three d environment, but for this demo all plotting */
/* is done with x/y only, so we remap y onto z and vice versa here by         */
/* defining y as coordinate 2 and z as coordinate 1                           */
#define SCR_XCTR	t[0] 
#define SCR_YCTR	t[2]
#define SCR_ZCTR	(((float) t[1])/((float) SCREEN_DEPTH))
#else
#define SCR_XCTR	(SCREEN_WIDTH/2)
#define SCR_YCTR	(SCREEN_HEIGHT/2)
#define SCR_ZCTR	1
#endif

#define SCREEN_SIZE	(SCREEN_WIDTH*SCREEN_HEIGHT)

#define LAYERS	6
#define MAX_NUM_SIDES  20
#define MAX_NUM_POINTS  MAX_NUM_SIDES*LAYERS

#define HORIZONTAL	0

typedef unsigned int WORD;
typedef unsigned char BYTE;
typedef unsigned long DWORD;

typedef struct {
	float x, y, z;
} POINT3D;

int CurrentColor=12;
int Depth[LAYERS] = { -45, -35, -10, +10, +35, +45 };
int Radius[LAYERS] = { 5, 50, 75, 75, 50, 5 };

int  InitGraph(int mode);
void InitPoints(POINT3D *Points, int sides);
void SetColor(char c);
char GetColor(void);
void SetPixel(int x, int y, char c);
void line(int x1, int y1, int x2, int y2);
void Update(void);
void Clear(void);

void RotateX(POINT3D *dest, POINT3D src, int a);
void RotateY(POINT3D *dest, POINT3D src, int a);
void RotateZ(POINT3D *dest, POINT3D src, int a);

GraphicsContext *screen;
char *buf;

int main(void)
{
 int scale=100, a, b, c;
 POINT3D *Points;
  
 int sides;
 int NUM_POINTS;

    int t[3], r[3], oldbutton=0, button=0;

#ifdef MANUALLY_SETUP_MOUSE
    char orbdev[1024] = "/dev/ttyS0";
#endif

 printf("\n\n                                Welcome to SPIN\n\n");
 printf("   Spin is a graphical rotation program for Linux which uses the SVGA library.\n");
 printf("It's purpose is to demonstrate the new six dimensional mouse driver support.\n");
 printf("So far this driver only supports the Spaceball series of controllers including\n");
 printf("the Spaceball Avenger and SpaceOrb 360.  Spin may be used with any mouse type\n");
 printf("but two dimensional mice will only be able to move the image in two dimensions.\n\n");
 printf("Spin was originally written by Jason Lyons, based on the SpaceOrb readout code\n");
 printf("developed by Brett Viren and others.  Eric Sharkey incorporated Brett's code\n");
 printf("into svgalib and adapted spin accordingly.\n\n");
 printf("Button Functions:\n"); 
 printf(" A - Decrease Sensitivity       (Right Mouse Button)\n");
 printf(" B - Change color               (Middle Mouse Button)\n");
 printf(" C - Increase Sensitivity       (Left Mouse Button)\n");
 printf(" D - Increase Number of Sides\n");
 printf(" E - Decrease Number of Sides\n");
 printf(" F - Quit                       (CTRL-C)\n");
 printf(" Reset button toggles orientation (default is vertical) \n\n");
 printf("Warning: Default sensitivity optimized for spaceball.\n");
 printf("Most mice will need to increase this setting.\n");
 printf("\nHow many sides to start with (MAX %d) ? ",MAX_NUM_SIDES);
 scanf("%d", &c);

 if(c == 0)      exit(1);
 else if(c < 0)  printf("\nCannot draw a negative number of sides.\n"), exit(1);
 else if(c > MAX_NUM_SIDES) printf("\nCannot draw with more sides than %d.\n",MAX_NUM_SIDES), exit(1);

 sides = c;
 NUM_POINTS = sides*LAYERS;

 Points = (POINT3D *)calloc(MAX_NUM_POINTS, sizeof(POINT3D));

InitPoints(Points,sides);

printf("Initializing mouse\n");

#ifdef MANUALLY_SETUP_MOUSE
if (mouse_init(orbdev,MOUSE_SPACEBALL,MOUSE_DEFAULTSAMPLERATE)) {
  printf("mouse_init failed");
  return -1;
} else {
  printf("mouse_init successful\n");
}
#else
  vga_setmousesupport(1);
#endif

 if(!InitGraph(GMODE)) return 0;

mouse_setscale(scale);
mouse_setrange_6d(0,SCREEN_WIDTH,1,SCREEN_DEPTH,0,SCREEN_HEIGHT,-2,2,-2,2,-2,2,63);
mouse_setwrap(MOUSE_ROT_INFINITESIMAL);
mouse_setposition_6d(SCREEN_WIDTH/2,SCREEN_DEPTH/2,SCREEN_HEIGHT/2,0,0,0,63);

  while(1)
  {
   mouse_update();
   mouse_getposition_6d(&t[0],&t[1],&t[2],&r[0],&r[1],&r[2]);
#ifdef DEBUG
     printf("%d %d %d\n",t[0],t[1],t[2]);
#endif
   oldbutton = button;
   button = mouse_getbutton();
   if ((button & MOUSE_RIGHTBUTTON)&&!(oldbutton & MOUSE_RIGHTBUTTON)) {
     scale += 5;
     mouse_setscale(scale);
   }
   if ((button & MOUSE_LEFTBUTTON)&&!(oldbutton & MOUSE_LEFTBUTTON)) {
     if (scale==1) {
       printf("\007");
       fflush(stdout);
     } else {
       if (scale>6)
         scale -=5;
       else
         scale--;
       mouse_setscale(scale);
     }
   }
   if ((button & MOUSE_MIDDLEBUTTON)&&!(oldbutton & MOUSE_MIDDLEBUTTON))
     CurrentColor = random() % 15 + 1;
   if ((button & MOUSE_FOURTHBUTTON)&&!(oldbutton & MOUSE_FOURTHBUTTON))
     if (sides>1) {
       sides--;
       NUM_POINTS = sides*LAYERS;
       InitPoints(Points,sides);
     } else {
       printf("\007");
       fflush(stdout);
     }
   if ((button & MOUSE_FIFTHBUTTON)&&!(oldbutton & MOUSE_FIFTHBUTTON))
     if (sides<MAX_NUM_SIDES) {
       sides++;
       NUM_POINTS = sides*LAYERS;
       InitPoints(Points,sides);
     } else {
       printf("\007");
       fflush(stdout);
     }
   if (button & MOUSE_SIXTHBUTTON)
   {
    vga_setmode(0);
    free(buf);
    free(Points);
    return 0;
   }

   for(a = 0; a < NUM_POINTS; a++) {
        RotateX(&Points[a],Points[a],r[0]);
        RotateY(&Points[a],Points[a],r[2]);
        RotateZ(&Points[a],Points[a],r[1]);
        Points[a].x *= SCR_ZCTR;
        Points[a].y *= SCR_ZCTR;
        Points[a].z *= SCR_ZCTR;
        Points[a].x += SCR_XCTR;
        Points[a].y += SCR_YCTR;
   }
   for(a = 0; a < LAYERS; a++)
   {
    for(b = 0; b < sides-1; b++)
    {
     /* Connect the points on each layer */
     line(Points[a*sides+(b+1)].x,Points[a*sides+(b+1)].y,
          Points[a*sides+b].x,Points[a*sides+b].y);
     /* Connect between layers */
     if(a < (LAYERS - 1))
     {
      line(Points[(a+1)*sides+b].x,Points[(a+1)*sides+b].y,
           Points[a*sides+b].x,Points[a*sides+b].y);
     }
    }
    line(Points[a*sides+b].x,Points[a*sides+b].y,
         Points[a*sides+0].x,Points[a*sides+0].y);
    if(a < (LAYERS - 1))
    {
     line(Points[a*sides+b].x,Points[a*sides+b].y,
          Points[(a+1)*sides+b].x,Points[(a+1)*sides+b].y);
    }
   }
   for(a = 0; a < NUM_POINTS; a++) {
        Points[a].x -= SCR_XCTR;
        Points[a].y -= SCR_YCTR;
        Points[a].x /= SCR_ZCTR;
        Points[a].y /= SCR_ZCTR;
        Points[a].z /= SCR_ZCTR;
   }
   Update();
   Clear(); 
  }
 free(buf);
 free(Points);
}  

void SetColor(char c)               {  CurrentColor = c;                }
char GetColor()	                    {  return CurrentColor;             }
void SetPixel(int x, int y, char c) 
{ 
 if(x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT)
     buf[y*SCREEN_WIDTH+x] = c;               
}
void Clear()			    {  memset((char *)buf, 0, SCREEN_SIZE);  }
void Update()
{
 memcpy((char *)screen->vbuf, (char *)buf, SCREEN_SIZE);
}

void InitPoints(POINT3D *Points, int sides)
{
 int a, b, c;

 for(a = 0; a < LAYERS; a++)
 {
  for(b = 0; b < sides; b++)
  {
   c=(b*360)/sides;
   Points[a*sides+b].x = cosine[c] * Radius[a];
   Points[a*sides+b].y = sine[c] * Radius[a];
   Points[a*sides+b].z = Depth[a];
  }
 }
}

int InitGraph(int mode)
{
  buf = (char *)malloc(SCREEN_SIZE);
  if(!buf) 		 return 0;
  
  if(!vga_hasmode(mode)) return 0;
  vga_setmode(mode);
   
  gl_setcontextvga(mode);
  screen = gl_allocatecontext();
  gl_getcontext(screen);
  gl_setcontextvgavirtual(mode);
  gl_setcontext(screen);
  
  return 1;
}
void RotateX(POINT3D *dest, POINT3D src, int a)
{
 while (a<0) a+=360;
 dest->x = src.x;
 dest->y = src.y * cosine[a] + src.z * sine[a];
 dest->z = -1 * src.y * sine[a] + src.z * cosine[a];
}
void RotateY(POINT3D *dest, POINT3D src, int a)
{
 while (a<0) a+=360;
 dest->x = -1 * src.z * sine[a] + src.x * cosine[a];
 dest->y = src.y;
 dest->z = src.z * cosine[a] + src.x * sine[a];
}
void RotateZ(POINT3D *dest, POINT3D src, int a)
{
 while (a<0) a+=360;
 dest->x = src.x * cosine[a] + src.y * sine[a];
 dest->y = -1 * src.x * sine[a] + src.y * cosine[a];
 dest->z = src.z;
}
void line(int x1, int y1, int x2, int y2)
{
 int dx, dy;
 int sx, sy;
 int ax, ay;
 int x, y, c=CurrentColor;

 dx = x2 - x1;
 dy = y2 - y1;
 ax = abs(dx) << 1;
 ay = abs(dy) << 1;
 sx = (dx >= 0) ? 1 : -1;
 sy = (dy >= 0) ? 1 : -1;
 x = x1;
 y = y1;

 if (ax > ay)
 {
  int d = ay - (ax >> 1);
  while (x != x2)
  {
   SetPixel(x, y, c);
   if (d > 0 || (d == 0 && sx == 1))
   {
    y += sy;
    d -= ax;
   }
   x += sx;
   d += ay;
  }
 }
 else
 {
  int sy = (dy >= 0) ? 1 : -1;
  int d = ax - (ay >> 1);
  while (y != y2)
  {
   SetPixel(x, y, c);
   if (d > 0 || (d == 0 && sy == 1))
   {    
    x += sx;
    d -= ay;
   }
   y += sy;
   d += ax;
  }
 }
 SetPixel(x, y, c);
}
