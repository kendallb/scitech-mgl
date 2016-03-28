
#include <stdlib.h>
#include <stdio.h>
#include <vga.h>
#include <errno.h>
#include "pmapi.h"

/*
 * Note: Observe that when writing the font to a file, the file to write is
 * opened after vga_init has been called (so that root permissions have been
 * given up). This means that there is no major security hole lurking here.
 */

unsigned char *regs;

void
main (int argc, char *argv[])
{
  int len = PM_getConsoleStateSize();

  vga_init ();
  if (argc == 1)
    {
      printf ("Save/restore textmode registers.\n");
      printf ("Syntax: restoretextmode option filename\n");
      printf ("	-r filename	Restore registers from file.\n");
      printf ("	-w filename	Write registers to file.\n");
      exit (0);
    }
  if (argv[1][0] != '-')
    {
      printf ("Must specify -r or -w.\n");
      exit (1);
    }
  regs = malloc(len);
  switch (argv[1][1])
    {
    case 'r':
    case 'w':
      if (argc != 3)
	{
	  printf ("Must specify filename.\n");
	  exit (1);
	}
      break;
    default:
      printf ("Invalid option. Must specify -r or -w.\n");
      exit (1);
    }
  if (argv[1][1] == 'r')
    {
      FILE *f;
      f = fopen (argv[2], "rb");
      if (f == NULL)
	{
	error:
	  perror ("restoretextmode");
	  exit (1);
	}
      if (1 != fread (regs, len, 1, f))
	{
	  if (errno)
	    goto error;
	  puts ("restoretextmode: input file corrupted.");
	  exit (1);
	}
      fclose (f);
    }
//  vga_setmode (G640x480x256);
  switch (argv[1][1])
    {
    case 'r':
      vga_settextmoderegs (regs);
      break;
    case 'w':
      vga_gettextmoderegs (regs);
      break;
    }
//  vga_setmode (TEXT);
  if (argv[1][1] == 'w')
    {
      FILE *f;
      f = fopen (argv[2], "wb");
      if (f == NULL)
	goto error;
      if (1 != fwrite (regs, len, 1, f))
	goto error;
      if (fclose (f))
	goto error;
    }
  free(regs);
  exit (0);
}
