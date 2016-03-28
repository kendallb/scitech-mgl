/*
 *  joytest.c 1.0 
 *  Cpoyright (C) 1998 Daniel Engström <daniel.engstrom@riksnett.no>
 *
 */

/*
 *  This is a simple joystick test program and an example
 *  how to write programs using the vgajoystick library routines  
 */

/*
 *  usage: joytest <number>
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <vgajoystick.h>

char *axe_name[] = { "X", "Y", "Z", };

int main (int argc, char **argv)
{
   int joystick, axes, buttons, i;

   if (argc != 2) {
      fprintf(stderr, "usage: joytest <number>\n");
      exit (1);
   }

   joystick = atoi(argv[1]);

   if (joystick_init(joystick, JOY_CALIB_STDOUT) < 0) 
	exit(1);
  
   axes = joystick_getnumaxes(joystick); 
   buttons = joystick_getnumbuttons(joystick); 
   printf("Joystick %d has %d axes and %d buttons.\n", joystick, axes, buttons);

   if (axes > 3)
	axes = 3;

   printf("Press enter to start testing ... (interrupt to exit)\n");
   getchar();
   for(;;)
     {
	while(!joystick_update());

        for (i = 0; i < buttons; i++)
	    printf("B%d: %s ", i, joystick_getbutton(joystick, i) ? "down" : "up  ");
        for (i = 0; i < axes; i++)
	    printf("%s: %4d ", axe_name[i], joystick_getaxis(joystick, i));
        putchar('\n');
        usleep(100);
   }

   joystick_close(joystick);
   return 0;
}
