/*                             bg_test.c


	Copyright (c) 1997  Michael Friman. All rights reserved.


*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <vga.h>
#include <vgagl.h>

/*
  If you really want to see background runin, add after 
  vga_setmode vga_runinbackground(1) in fun.
  Remember to compile the svgalib with background.
*/

int go = 0;
int linear = 0;

void drawline(int x1, int y1, int x2, int y2)

{
 if (linear)
     {
      gl_line(x1,y1,x2,y2,vga_white());
     }
 else
     {
      vga_drawline(x1,y1,x2,y2);
     }
}

void set_go(void)

{
 go=1;
 return;
}

int main(int argc, char *argv[])

{
 int x[2];
 int y[2];
 int counter;
 int mode;

 if ((argc > 2) || ((argc == 2) && (strcmp(argv[1], "linear") != 0)))
     {
      fputs("Usage: bg_test [linear]\n", stderr);
      exit(2);
     }
 vga_init();
 printf("This is small test for background runin.\n");
 if (vga_runinbackground_version()==1)
     {
      printf("Background runin enabled. mode 1\n");
     }
   else
     {
      printf("Svgalib is not mode 1 background capable.\n");
      printf("Test ended.\n");
      return(0);
     }
 printf("Switch to another console when the box appears.\n");
 printf("Press enter to continue or CTRL-c to stop.\n");
 getchar();
 
 mode = vga_getdefaultmode();
 if (mode < 0)
     mode = G320x200x256;
 if (argc == 2)
     {
      if (vga_getmodeinfo(mode)->flags & CAPABLE_LINEAR)
       {
	vga_setlinearaddressing();
	fputs("Linear mode set.\n", stderr);
        linear = 1;
       }
      else
       {
	fputs("Linear mode unavailable.\n", stderr);
       }
     }
 vga_setmode(mode);
 if (linear)
     gl_setcontextvga(mode);
 vga_runinbackground(VGA_GOTOBACK,set_go); 
 vga_runinbackground(1); 
 x[0]=0;
 y[0]=0;
 x[1]=vga_getxdim()-1;
 y[1]=vga_getydim()-1;

 if (!linear)
     vga_setcolor(vga_white()); 
 counter=(y[1]/11)*5+1;
 while(counter<=(y[1]/11)*6)
   {
    drawline((x[1]/11)*5,counter,(x[1]/11)*6,counter);
    counter++;
   }
   
 /* Program won't go further without console switching. */
 
 while(!go) usleep(1000);
  
 drawline(x[0],y[0],x[1],y[0]);
 drawline(x[1],y[0],x[1],y[1]);
 drawline(x[1],y[1],x[0],y[1]);
 drawline(x[0],y[1],x[0],y[0]);

 drawline(x[0],y[0],x[1],y[1]);
 drawline(x[1],y[0],x[0],y[1]);


 while(!vga_getkey());
 
 vga_setmode(TEXT);
 
 printf("Ok.\n");
 return(0);
}

