#include <vga.h>


int
main (void)
{
  /* We can't call vga_init, because register dumping should work */
  /* from within X, and vga_init will exit in that case. */
  /* vga_init(); */
  vga_dumpregs ();
  vga_screenon ();
  /* vga_setmode(TEXT);    Hack to unblank screen. */
  return 0;
}
