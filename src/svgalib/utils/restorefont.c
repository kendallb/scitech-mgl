#include <stdlib.h>
#include <stdio.h>
#include <vga.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>

/*
 * Note: Observe that when writing the font to a file, the file to write is
 * opened after vga_init has been called (so that root permissions have been
 * given up). This means that there is no major security hole lurking here.
 */

void main(int argc, char *argv[])
{
    FILE *f;
    unsigned char *font;
    size_t font_size = 0x2000; /* buffer size in 1.2.11 and before */
    int can_set = 0;
    struct stat statbuf;

    if (argc == 1) {
	printf("Restore corrupted textmode font.\n");
	printf("Syntax: restorefont option filename\n");
	printf("	-r filename	Restore VGA font from file.\n");
	printf("	-w filename	Write current VGA font to file.\n");
	exit(0);
    }
    if (argv[1][0] != '-') {
	printf("Must specify -r or -w.\n");
	exit(1);
    }
    switch (argv[1][1]) {
    case 'r':
    case 'w':
	if (argc != 3) {
	    printf("Must specify filename.\n");
	    exit(1);
	}
	break;
    default:
	printf("Invalid option. Must specify -r or -w.\n");
	exit(1);
    }
    vga_disabledriverreport();
    vga_init();
    /* we are in TEXT mode. Check for availability of vga_ext_set: */
    if (vga_getmodeinfo(TEXT)->flags & HAVE_EXT_SET) {
	if (vga_ext_set(VGA_EXT_AVAILABLE, VGA_AVAIL_SET) &
		(1 << VGA_EXT_FONT_SIZE)) {
	    can_set = 1;
	    /* Query the preferred data size: */
	    font_size = vga_ext_set(VGA_EXT_FONT_SIZE, 0);
	}
    }

    /* We never need more than that memory: */
    font = malloc((size_t)font_size);
    if (!font) {
	puts("restorefont: out of memory.");
	goto ex_no_errno;
    }

    // vga_setmode(G640x480x256);
    switch (argv[1][1]) {
      case 'r':
	f = fopen(argv[2], "rb");
	if (f == NULL) {
	  error:
	    perror("restorefont");
	  ex_no_errno:
	    vga_setmode(TEXT);
	    exit(1);
	}
	if (fstat(fileno(f), &statbuf))
	    goto error;
	font_size = statbuf.st_size;
	/* Check for sensible sizes: */
	switch (font_size) {
	    case 0x2000:
	    case 0x2000 * 4:
	    case 0x2000 * 8:
		if (can_set)
	    	    vga_ext_set(VGA_EXT_FONT_SIZE, font_size);
		break;
	    default:
	    corrupt:
	        puts("restorefont: input file corrupted.");
	        goto ex_no_errno;
	}
		
	if (1 != fread(font, font_size, 1, f)) {
	    if (errno)
		goto error;
	    goto corrupt;
	}
	fclose(f);
	vga_puttextfont(font);
	break;
      case 'w':
	/* save as much as we have.. */
	if (can_set)
	    vga_ext_set(VGA_EXT_FONT_SIZE, font_size);
	vga_gettextfont(font);
	f = fopen(argv[2], "wb");
	if (f == NULL)
	    goto error;
	if (1 != fwrite(font, font_size, 1, f))
	    goto error;
	if (fclose(f))
	    goto error;
	break;
    }
    // vga_setmode(TEXT);
    exit(0);
}
