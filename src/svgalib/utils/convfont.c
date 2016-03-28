/* Convert standard binary font to codepage format */


#include <stdio.h>
#include <stdlib.h>


int fontheight;
int sfontsize;
int font_nuchars;
unsigned char sfontbuf[32 * 256];
unsigned char tfontbuf[32 * 256];
FILE *sf;
FILE *tf;


void
main (int argc, char **argv)
{
  int i;
  if (argc != 4)
    {
      printf ("Syntax: convfont fontfile fontheight vgafontfile\n");
      printf (
      "\nconvfont - convert standard format binary font to codepage format\n"
	       "The converted font is written to vgafontfile.\n");
      printf (
	       "A binary font file of any number of characters up to 256 can be used, although\n"
	       "at least defining the first 128 characters is a good idea. The fontheight\n"
	       "should be in the range 1-32.\n"
	);
      exit (1);
    }
  if ((sf = fopen (argv[1], "rb")) == NULL)
    {
      printf ("convfont: Unable to open file.\n");
      exit (1);
    }
  if ((tf = fopen (argv[3], "wb")) == NULL)
    {
      printf ("convfont: Unable to create file.\n");
      exit (1);
    }
  fontheight = atoi (argv[2]);
  if (fontheight < 1 || fontheight > 32)
    {
      printf ("convfont: Invalid fontheight.\n");
      exit (1);
    }

  fseek (sf, 0, SEEK_END);
  sfontsize = ftell (sf);
  fseek (sf, 0, SEEK_SET);
  font_nuchars = sfontsize / fontheight;
  printf ("Converting %d characters\n", font_nuchars);
  if (font_nuchars < 1 || font_nuchars > 256)
    {
      printf ("convfont: Invalid number of characters in font.\n");
      exit (1);
    }
  fread (sfontbuf, 1, sfontsize, sf);
  fclose (sf);
  for (i = 0; i < font_nuchars; i++)
    {
      int j;

      for (j = 0; j < fontheight; j++)
	tfontbuf[i * 32 + j] =
	  sfontbuf[i * fontheight + j];

      for (j = 0; j < 32 - fontheight; j++)
	tfontbuf[i * 32 + fontheight] = 0;
    }
  /* clear remaining characters */
  for (i = font_nuchars * 32; i < 32 * 256; i++)
    tfontbuf[i] = 0;
  printf ("Writing font file.\n");
  fwrite (tfontbuf, 1, 32 * 256, tf);
  fclose (tf);
  exit (0);
}
