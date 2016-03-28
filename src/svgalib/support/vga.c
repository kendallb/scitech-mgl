#include <stdio.h>
#include <stdlib.h>
#include <dos.h>

/* January 1995, Scott Heavner (sdh@po.cwru.edu)

 * Changes to allow anyone to compile vga.c under the go32 Linux->dos cross compiler.
 * It should also work with DJGPP (the gcc port to msdos).  The cross compiler is
 * available at ftp://sunsite.unc.edu/pub/Linux/devel/msdos/go32crs.tar.gz.
 * 
 * I compiled it with:
 *
 *      go32gcc vga.c -lpc
 *      cat /usr/local/go32/bin/go32.exe a.out > vga.exe 
 */
#ifdef GO32
#include <pc.h>
#endif

/* VGA index register ports */
#define CRT_I   0x3D4		/* CRT Controller Index (mono: 0x3B4) */
#define ATT_IW  0x3C0		/* Attribute Controller Index & Data Write Register */
#define GRA_I   0x3CE		/* Graphics Controller Index */
#define SEQ_I   0x3C4		/* Sequencer Index */
#define PEL_IW  0x3C8		/* PEL Write Index */

/* VGA data register ports */
#define CRT_D   0x3D5		/* CRT Controller Data Register (mono: 0x3B5) */
#define ATT_R   0x3C1		/* Attribute Controller Data Read Register */
#define GRA_D   0x3CF		/* Graphics Controller Data Register */
#define SEQ_D   0x3C5		/* Sequencer Data Register */
#define MIS_R   0x3CC		/* Misc Output Read Register */
#define MIS_W   0x3C2		/* Misc Output Write Register */
#define IS1_R   0x3DA		/* Input Status Register 1 (mono: 0x3BA) */
#define PEL_D   0x3C9		/* PEL Data Register */

/* VGA indexes max counts */
#define CRT_C   24		/* 24 CRT Controller Registers */
#define ATT_C   21		/* 21 Attribute Controller Registers */
#define GRA_C   9		/* 9  Graphics Controller Registers */
#define SEQ_C   5		/* 5  Sequencer Registers */
#define MIS_C   1		/* 1  Misc Output Register */

/* VGA registers saving indexes */
#define CRT     0		/* CRT Controller Registers start */
#define ATT     CRT+CRT_C	/* Attribute Controller Registers start */
#define GRA     ATT+ATT_C	/* Graphics Controller Registers start */
#define SEQ     GRA+GRA_C	/* Sequencer Registers */
#define MIS     SEQ+SEQ_C	/* General Registers */
#define END     MIS+MIS_C	/* last */


unsigned char vga_regs[60];

#ifdef GO32

#define port_out(v,p) outportb(p,v)
#define port_in(p) inportb(p)

#else

void port_out(unsigned char value, unsigned short port)
{
    asm
    {
	mov dx, port
	 mov al, value
	 out dx, al
    }
} unsigned char port_in(unsigned short port)
{
    asm
    {
	mov dx, port
	 in al, dx
    } return (_AL);
}

#endif

main(int argc, char *argv[])
{
    union REGS cpu_regs;
    int i;
    unsigned char mode;

    if (argc != 2) {
	printf("Usage: getregs mode (mode must be hexadecimal)\n");
	exit(-1);
    }
    if (!sscanf(argv[1], "%x", &mode)) {
	printf("Usage: getregs mode (mode must be hexadecimal)\n");
	exit(-1);
    }
    cpu_regs.h.ah = 0x00;
    cpu_regs.h.al = mode;
    int86(0x10, &cpu_regs, &cpu_regs);

    /* get VGA register values */
    for (i = 0; i < CRT_C; i++) {
	port_out(i, CRT_I);
	vga_regs[CRT + i] = port_in(CRT_D);
    }
    for (i = 0; i < ATT_C; i++) {
	port_in(IS1_R);
	port_out(i, ATT_IW);
	vga_regs[ATT + i] = port_in(ATT_R);
    }
    for (i = 0; i < GRA_C; i++) {
	port_out(i, GRA_I);
	vga_regs[GRA + i] = port_in(GRA_D);
    }
    for (i = 0; i < SEQ_C; i++) {
	port_out(i, SEQ_I);
	vga_regs[SEQ + i] = port_in(SEQ_D);
    }
    vga_regs[MIS] = port_in(MIS_R);

    cpu_regs.h.ah = 0x00;
    cpu_regs.h.al = 0x03;
    int86(0x10, &cpu_regs, &cpu_regs);

    printf("/* BIOS mode 0x%02X */\n", mode);
    printf("static char regs[60] = {\n  ");
    for (i = 0; i < 12; i++)
	printf("0x%02X,", vga_regs[CRT + i]);
    printf("\n  ");
    for (i = 12; i < CRT_C; i++)
	printf("0x%02X,", vga_regs[CRT + i]);
    printf("\n  ");
    for (i = 0; i < 12; i++)
	printf("0x%02X,", vga_regs[ATT + i]);
    printf("\n  ");
    for (i = 12; i < ATT_C; i++)
	printf("0x%02X,", vga_regs[ATT + i]);
    printf("\n  ");
    for (i = 0; i < GRA_C; i++)
	printf("0x%02X,", vga_regs[GRA + i]);
    printf("\n  ");
    for (i = 0; i < SEQ_C; i++)
	printf("0x%02X,", vga_regs[SEQ + i]);
    printf("\n  ");
    printf("0x%02X", vga_regs[MIS]);
    printf("\n};\n");

}
