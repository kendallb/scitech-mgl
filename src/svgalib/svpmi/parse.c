
/*
 * This is a quickly hacked program to convert an SVPMI (Super VGA Protected
 * Mode Interface) data file to an svgalib driver. Feedback is
 * very welcome.
 *
 * Initial version (Mar 94 HH). Doesn't do much yet.
 * Assumes textmode is last defined mode.
 * End with ctrl-c. Correct resulting files by hand.
 * (add "}" to modetable.svpmi)
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>


void parse ();


void
main ()
{
  parse ();
}


/* Return pointer to line red from stdin, end marked by CR or CR/LF.
 * Initial spaces are removed. */

char *
getline ()
{
  static char linebuf[128];
  int i, length, spaces;
  i = 0;
  for (;;)
    {
      int c;
      c = fgetc (stdin);
      if (feof (stdin))
	return NULL;
      if (c == 13)
	continue;		/* Skip. */
      if (c == 10)
	break;
      linebuf[i] = c;
      i++;
    }
  length = i;
  linebuf[i] = 0;
  /* Remove initial spaces. */
  spaces = 0;
  i = 0;
  while (i < length)
    {
      if (linebuf[i] != ' ')
	break;
      i++;
      spaces++;
    }
  return linebuf + spaces;
}


/* Skip lines until left side of line matches string s. */

char *
getthisline (char *s)
{
  char buf[128];
  int n;
  n = strlen (s);
  for (;;)
    {
      char *line;
      line = getline ();
      if (strncmp (line, s, n) == 0)
	return line;
    }
}


/* Get the (n + 1)th word delimited by ' ' and ';' in string s. */

char *
getword (char *s, int n)
{
  int i;
  char *word;
  int mode, wcount;
  word = s;
  mode = 0;			/* Whitespace. */
  wcount = 0;
  i = 0;
  for (i = 0; s[i] != 0; i++)
    {
      if (s[i] == ' ' || s[i] == ';')
	if (mode == 0)
	  continue;
	else
	  {
	    s[i] = 0;
	    if (wcount == n)
	      return word;
	    wcount++;
	    mode = 0;
	  }
      else if (mode == 1)
	continue;
      else
	{
	  word = &s[i];
	  mode = 1;
	}
    }
  return NULL;
}


/* Write lines to file until left part matches string s. */

void
writetofileuntilthisline (char *s, FILE * f)
{
  int n;
  n = strlen (s);
  for (;;)
    {
      char *line;
      line = getline ();
      if (strncmp (line, s, n) == 0)
	break;
      fprintf (f, "%s\n", line);
    }
}


void
writetofileuntilend (FILE * f)
{
  for (;;)
    {
      char *line;
      line = getline ();
      if (line == NULL)
	return;
      fprintf (f, "%s\n", line);
    }
}


void
parse ()
{
  char *line;
  char s[128];
  char modename[40];
  int modenumber;
  FILE *f, *g;
  printf ("SVPMI to svgalib driver converter.\n\n");

  /* Read header. */
  getthisline ("[ADAPTER]");
  line = getline ();
  printf ("Graphics Adapter string: %s\n", line);

  /* Read modes. */
  modenumber = 0;
  g = fopen ("modetable.svpmi", "wb");
  f = fopen ("modes.svpmi", "wb");
  fprintf (g, "/* svgalib SVPMI mode table. */\n\n");
  fprintf (g, "static svpmi_modeentry svpmi_modes[] = {\n");

  for (;;)
    {
      int XResolution;		/* SVPMI modeinfo fields. */
      int YResolution;
      int BitsPerPixel;
      int BytesPerScanline;
      int WinAGranularity;
      int WinASize;
      int WinABase;
      int ModeAttributes;
      getthisline ("[MODEINFO]");
      line = getthisline ("ModeAttributes");
      ModeAttributes = atoi (getword (line, 2));
      line = getthisline ("WinAGranularity");
      WinAGranularity = atoi (getword (line, 2));
      line = getthisline ("WinASize");
      WinASize = atoi (getword (line, 2));
      line = getthisline ("WinABase");
      WinABase = atoi (getword (line, 2));
#if 0
      if (WinABase != 0xa0000)
	{
	  printf ("Window is not at 0xa0000.\n");
	  exit (-1);
	}
#endif
      line = getthisline ("BytesPerScanline");
      BytesPerScanline = atoi (getword (line, 2));
      line = getthisline ("XResolution");
      XResolution = atoi (getword (line, 2));
      line = getthisline ("YResolution");
      YResolution = atoi (getword (line, 2));
      line = getthisline ("BitsPerPixel");
      BitsPerPixel = atoi (getword (line, 2));

      if (ModeAttributes == 0x07)
	{
	  /* Text mode. */
	  printf ("Textmode found.\n");
	  getthisline ("[SETMODE]");
	  fprintf (f, "static void svpmi_setmode_text() {\n");
	  writetofileuntilend (f);
	  fprintf (f, "}\n\n\n");
	  fprintf (g, "}\n");
	  fclose (f);
	  fclose (g);
	  exit (0);
	}

      printf ("Mode found: %d x %d, %dbpp  %d bpl  gran A %d\n",
	      XResolution, YResolution, BitsPerPixel,
	      BytesPerScanline, WinAGranularity);

      sprintf (modename, "%dx%dx%d", XResolution, YResolution,
	       1 << BitsPerPixel);

      getthisline ("[SETMODE]");
      fprintf (f, "static void svpmi_setmode_%s() {\n", modename);
      writetofileuntilthisline ("[SETCOLOR]", f);
      fprintf (f, "}\n\n\n");
      getthisline ("[SETWINDOW]");
      fprintf (f, "static void svpmi_setwindow_%s( int r0 ) {\n", modename);
      writetofileuntilthisline ("[MODE]", f);
      fprintf (f, "}\n\n\n");

      fprintf (g, "{ %d, %d, %d, %d, %d, svpmi_setmode_%s, svpmi_setwindow_%s },\n",
	       XResolution, YResolution, BitsPerPixel,
	       BytesPerScanline, WinAGranularity, modename, modename);

      fflush (f);
      fflush (g);

      modenumber++;
    }
  fprintf (g, "}\n");
  fclose (f);
  fclose (g);
}
