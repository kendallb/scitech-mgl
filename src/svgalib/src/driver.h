#ifndef __DRIVER_H__
#define __DRIVER_H__

#define MAX_REGS 128000

void __svgalib_read_options(char **commands, char *(*func) (int ind, int mode));
void __joystick_flip_vc(int acquire);

extern char *__joystick_devicenames[4];
extern int __svgalib_driver_report;

#endif
