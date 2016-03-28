
/* 
   Simple test program for Cirrus linear addressing/color expansion.
   vgagl can take advantage of it (linear addressing).
 */


#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vga.h>
#include <time.h>
#include "../src/libvga.h"


#define USE_LINEAR_ADDRESSING
/* #define USE_BY16_ADDRESSING */


unsigned char *vbuf;


void main(int argc, char *argv[])
{
    int i,j;

    if (!(argc == 2 && strcmp(argv[1], "--force") == 0))
	if (!(vga_getmodeinfo(G640x480x256)->flags & CAPABLE_LINEAR)) {
	    printf("Linear addressing not supported for this chipset.\n");
	    exit(1);
	}
    vga_init();
    vga_setmode(G640x480x256);
    vga_setpage(0);
#ifdef USE_LINEAR_ADDRESSING
    if (vga_setlinearaddressing() == -1) {
	vga_setmode(TEXT);
	printf("Could not set linear addressing.\n");
	exit(-1);
    }
#endif

    /* Should not mess with bank register after this. */

    vbuf = vga_getgraphmem();
    printf("vbuf mapped at %08lx.\n", (unsigned long) vbuf);

    getchar();

#ifdef USE_LINEAR_ADDRESSING
    memset(vbuf, 0x88, 640 * 480);
    sleep(1);

    memset(vbuf, 0, 640 * 480);
    for (i = 0; i < 100000; i++)
	*(vbuf + (rand() & 0xfffff)) = rand();
#endif


    getchar();
    for(i = 0;i < 44;i++){
      *(vbuf + i) = 0x1c;
      *(vbuf + 640*17 + i) = 0x1c;
      *(vbuf + 640*480 - i) = 0x1c;
      *(vbuf + 640*480 - 1 - 640*17 - i) = 0x1c;
      for(j = 1;j < 17;j++){
	*(vbuf + 640*j + i) = (i == 0 || i == 43)? 0x1c:0;
	*(vbuf + 640*480 - 1 - 640*j - i) = (i == 0 || i == 43)? 0x1c:0;
      }
    }
    for(i = 3;i < 10;i++)
      for(j = 4;j < 10;j++){
	*(vbuf + i + 640*j) = 0x3f;
	*(vbuf + 640*480 -1 -640*j - i) = 0x3f;
      }
    getchar();
    vga_setmode(TEXT);
}
