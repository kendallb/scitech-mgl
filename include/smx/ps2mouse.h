/*
* PS2MOUSE.H                                                Version 1.0
*
* Author: Alan Moore
*
***********************************************************************/

#if __cplusplus
extern "C" 
{
#endif

int ps2Query(void);
void ps2MouseRead(int * x, int * y, int * buttons);
int ps2MouseReset(void);
int ps2MouseStart(int xmin, int xmax, int ymin, int ymax, unsigned char xscale, unsigned char yscale, unsigned char accel);
int ps2MouseStop(void);

#if __cplusplus
}
#endif

#ifdef DEBUG
#define BUFSIZE 512
extern int  ps2Bufidx;
extern byte ps2Buf[BUFSIZE];
#endif
