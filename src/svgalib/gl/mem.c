#include <stdio.h>

 /*
  * Copies pixels from 4-byte-per-pixel screen to 3-byte-per-pixel screen,
  * 
  * discarding the last byte of each pixel.  Only uses 32-bit aligned word
  * accesses.  Instructions have been shuffled a bit for possible
  * avoidance of pipeline hazards.
  */
void __svgalib_memcpy4to3(void *dest, void *src, int n)
{
    printf("libgl: __svgalib_memcpy4to3 not done yet\n");
}

/* 
 * Copies pixels from 4-byte-per-pixel screen organized as BGR0 to
 * 0BGR 4-byte-per-pixel screen.
 * Used by copyscreen for ATI mach32 32-bit truecolor modes.
 */
void __svgalib_memcpy32shift8(void *dest, void *src, int n)
{
    printf("libgl: __svgalib_memcpy32shift8 not done yet\n");
}
