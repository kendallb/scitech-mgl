#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vga.h>
#include <errno.h>
#include <string.h>            /* strerror */
#include <asm/io.h>
#include "../src/libvga.h"


/* default palette values */

static char default_red[256]
=
{0, 0, 0, 0, 42, 42, 42, 42, 21, 21, 21, 21, 63, 63, 63, 63,
 0, 5, 8, 11, 14, 17, 20, 24, 28, 32, 36, 40, 45, 50, 56, 63,
 0, 16, 31, 47, 63, 63, 63, 63, 63, 63, 63, 63, 63, 47, 31, 16,
 0, 0, 0, 0, 0, 0, 0, 0, 31, 39, 47, 55, 63, 63, 63, 63,
 63, 63, 63, 63, 63, 55, 47, 39, 31, 31, 31, 31, 31, 31, 31, 31,
 45, 49, 54, 58, 63, 63, 63, 63, 63, 63, 63, 63, 63, 58, 54, 49,
 45, 45, 45, 45, 45, 45, 45, 45, 0, 7, 14, 21, 28, 28, 28, 28,
 28, 28, 28, 28, 28, 21, 14, 7, 0, 0, 0, 0, 0, 0, 0, 0,
 14, 17, 21, 24, 28, 28, 28, 28, 28, 28, 28, 28, 28, 24, 21, 17,
 14, 14, 14, 14, 14, 14, 14, 14, 20, 22, 24, 26, 28, 28, 28, 28,
 28, 28, 28, 28, 28, 26, 24, 22, 20, 20, 20, 20, 20, 20, 20, 20,
 0, 4, 8, 12, 16, 16, 16, 16, 16, 16, 16, 16, 16, 12, 8, 4,
 0, 0, 0, 0, 0, 0, 0, 0, 8, 10, 12, 14, 16, 16, 16, 16,
 16, 16, 16, 16, 16, 14, 12, 10, 8, 8, 8, 8, 8, 8, 8, 8,
 11, 12, 13, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 15, 13, 12,
 11, 11, 11, 11, 11, 11, 11, 11, 0, 0, 0, 0, 0, 0, 0, 0};
static char default_green[256]
=
{0, 0, 42, 42, 0, 0, 21, 42, 21, 21, 63, 63, 21, 21, 63, 63,
 0, 5, 8, 11, 14, 17, 20, 24, 28, 32, 36, 40, 45, 50, 56, 63,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 31, 47, 63, 63, 63, 63,
 63, 63, 63, 63, 63, 47, 31, 16, 31, 31, 31, 31, 31, 31, 31, 31,
 31, 39, 47, 55, 63, 63, 63, 63, 63, 63, 63, 63, 63, 55, 47, 39,
 45, 45, 45, 45, 45, 45, 45, 45, 45, 49, 54, 58, 63, 63, 63, 63,
 63, 63, 63, 63, 63, 58, 54, 49, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 7, 14, 21, 29, 28, 28, 28, 28, 28, 28, 28, 28, 21, 14, 7,
 14, 14, 14, 14, 14, 14, 14, 14, 14, 17, 21, 24, 28, 28, 28, 28,
 28, 28, 28, 28, 28, 24, 21, 17, 20, 20, 20, 20, 20, 20, 20, 20,
 20, 22, 24, 26, 28, 28, 28, 28, 28, 28, 28, 28, 28, 26, 24, 22,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 8, 12, 16, 16, 16, 16,
 16, 16, 16, 16, 16, 12, 8, 4, 8, 8, 8, 8, 8, 8, 8, 8,
 8, 10, 12, 14, 16, 16, 16, 16, 16, 16, 16, 16, 16, 14, 12, 10,
 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 13, 15, 16, 16, 16, 16,
 16, 16, 16, 16, 16, 15, 13, 12, 0, 0, 0, 0, 0, 0, 0, 0};
static char default_blue[256]
=
{0, 42, 0, 42, 0, 42, 0, 42, 21, 63, 21, 63, 21, 63, 21, 63,
 0, 5, 8, 11, 14, 17, 20, 24, 28, 32, 36, 40, 45, 50, 56, 63,
 63, 63, 63, 63, 63, 47, 31, 16, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 16, 31, 47, 63, 63, 63, 63, 63, 63, 63, 63, 63, 55, 47, 39,
 31, 31, 31, 31, 31, 31, 31, 31, 31, 39, 47, 55, 63, 63, 63, 63,
 63, 63, 63, 63, 63, 58, 54, 49, 45, 45, 45, 45, 45, 45, 45, 45,
 45, 49, 54, 58, 63, 63, 63, 63, 28, 28, 28, 28, 28, 21, 14, 7,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 14, 21, 28, 28, 28, 28,
 28, 28, 28, 28, 28, 24, 21, 17, 14, 14, 14, 14, 14, 14, 14, 14,
 14, 17, 21, 24, 28, 28, 28, 28, 28, 28, 28, 28, 28, 26, 24, 22,
 20, 20, 20, 20, 20, 20, 20, 20, 20, 22, 24, 26, 28, 28, 28, 28,
 16, 16, 16, 16, 16, 12, 8, 4, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 4, 8, 12, 16, 16, 16, 16, 16, 16, 16, 16, 16, 14, 12, 10,
 8, 8, 8, 8, 8, 8, 8, 8, 8, 10, 12, 14, 16, 16, 16, 16,
 16, 16, 16, 16, 16, 15, 13, 12, 11, 11, 11, 11, 11, 11, 11, 11,
 11, 12, 13, 15, 16, 16, 16, 16, 0, 0, 0, 0, 0, 0, 0, 0};

void
delay (void)
{
  int i;
  for (i = 0; i < 10; i++);
}

static __inline__ void port_out(int value, int port)
{
    __asm__ volatile ("outb %0,%1"
	      ::"a" ((unsigned char) value), "d"((unsigned short) port));
}

static __inline__ int port_in(int port)
{
    unsigned char value;
    __asm__ volatile ("inb %1,%0"
		      :"=a" (value)
		      :"d"((unsigned short) port));
    return value;
}

/* Modifications to restorepalette to allow custom palette setting from a file.
 * The format of the file is simply a table of color number -> color mappings:
 * <color(0-255)> <red(0-63)> <green(0-63)> <blue(0-63)> <rest of line ignored>
 *                       By Charles Blake, chuckb@alice.wonderland.caltech.edu
 */

void
process_palette_file (char **argv)
{
  int i, n, r, g, b;
  FILE *palette_file;

  if ((palette_file = fopen (argv[1], "r")) == NULL)
    {
      fprintf (stderr, "%s opening %s: %s\n", argv[0], argv[1], strerror (errno));
      exit (1);
    }
/*
 * NOTE:The tricky %*[^\n] conversion specifier allows arbitrary text following
 * any line with a valid entry on it. This is a simple comment syntax device.
 */
  for (n = 1; fscanf (palette_file, "%d %d %d %d %*[^\n]", &i, &r, &g, &b) == 4; n++)
    {
      default_red[i] = r;	/* deferred assignment prevents partial */
      default_green[i] = g;	/* assignment in case of in-line errors */
      default_blue[i] = b;
    }
  if (!feof (palette_file))
    {
      fprintf (stderr, "%s parse error: %s: line %d\n", argv[0], argv[1], n);
      exit (1);
    }
}

void
main (int argc, char *argv[])
{
  int i;

  vga_init ();

  if (argc > 1)
    process_palette_file (argv);

  if (vga_getcurrentchipset () != EGA)
    {
      /* Restore textmode/16-color mode palette */
      for (i = 0; i < 16; i++)
	{
	  port_in (IS1_RC);
	  delay ();
	  port_out (i, ATT_IW);	/* set palette color number */
	  delay ();
	  port_out (i, ATT_IW);	/* set palette color value */
	  delay ();
	}

      /* Restore 256-color VGA RGB look-up table */
      for (i = 0; i < 256; i++)
	vga_setpalette (i, default_red[i], default_green[i],
			default_blue[i]);

      port_in (IS1_RC);
      delay ();
      port_out (0x20, ATT_IW);	/* enable display */
    }
  exit (0);
}
