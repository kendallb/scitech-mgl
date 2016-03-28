/* Joystick interface modeled after svgalibs keyboard and mouse interfaces 
 * Copyright 1998 Daniel Engström <daniel.engstrom@riksnett.no> 
 * Partly based on code from 
 * joystick-0.7.3 Copyright (C) 1992, 1993 Author C. Smith
 * and 
 * joystick-1.0.6 Copyright (C) 1997 Vojtech Pavlik 
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "joydev.h"
#include "vgajoystick.h"
#include <driver.h>

#undef DEBUG

#ifndef SVGA_AOUT

/* forward declarations */
static void joystick_defaulthandler(int event, int num, char val, int joydev);
static void joystick_initdefhandler(int joydev, int buttons, int axes);

/* global variables */
/* Do not just change the sizes, check vga.c too, 4 is a minimum */
#define NUM_JOYSTICKS 4

char *__joystick_devicenames[NUM_JOYSTICKS] = { NULL, NULL, NULL, NULL };
static char *defnames[NUM_JOYSTICKS] = {
    "/dev/js0", "/dev/js1", "/dev/js2", "/dev/js3"
};

typedef struct {
	int (*update)(int);
	void (*handler)(int, int, char, int);
	int (*flip_vc)(int, int);
	char *dh_buttons;
	char *dh_axes;
	char axes, buttons;
	union t{
	    struct {
		int b, x, y;
		int xdif, ydif;
   		struct JS_DATA_TYPE caldata;
	    } prot0;
	    struct {
   		struct js_corr corrdata[4];
	    } prot1;
	} p;
} joydat_t;

static struct {
	int fd;
	joydat_t *joydata;
} joydesc[NUM_JOYSTICKS] = {
	{-1, NULL},
	{-1, NULL},
	{-1, NULL},
	{-1, NULL},
};

#define PROT0_TIMELIMIT 5

/* NULL func to be used before we know which one to use */
/* Update functions return bit 1 set if further events might be pending, bit 0 set
   if status changed... */

static int joystick_updx(int joydev)
{
   return 0;
}

/* For old version 0.x joystick drivers */
static int joystick_upd0(int joydev)
{
   int retval = 0;
   struct JS_DATA_TYPE js;
   
   if (read(joydesc[joydev].fd, &js, sizeof(struct JS_DATA_TYPE)) != 
       sizeof(struct JS_DATA_TYPE))
     return retval;
   
   if (js.buttons != joydesc[joydev].joydata->p.prot0.b)
     {
	int bmask=1, but;
	
	for (but=0; but<4; but++)
	  {
	     if ((js.buttons & bmask) != (joydesc[joydev].joydata->p.prot0.b & bmask)) {
	       joydesc[joydev].joydata->handler((js.buttons & bmask)?
				JOY_EVENTBUTTONDOWN:
				JOY_EVENTBUTTONUP, 
				but, 0, joydev);
	     }
	     bmask=bmask<<1;
	  }
	joydesc[joydev].joydata->p.prot0.b=js.buttons;
	retval = 1;
     }
   
   if (js.x != joydesc[joydev].joydata->p.prot0.x)
     {
	joydesc[joydev].joydata->p.prot0.x = js.x;
	js.x -= joydesc[joydev].joydata->p.prot0.xdif;
	if (js.x < -128)
	    js.x = -128;
	else if (js.x > 127)
	    js.x = 127;
	joydesc[joydev].joydata->handler(JOY_EVENTAXIS, 0, (char)js.x, joydev);
	retval = 1;
     }
   
   if (js.y != joydesc[joydev].joydata->p.prot0.y)
     {
	joydesc[joydev].joydata->p.prot0.y = js.y;
	js.y -= joydesc[joydev].joydata->p.prot0.ydif;
	if (js.y < -128)
	    js.y = -128;
	else if (js.y > 127)
	    js.y = 127;
	joydesc[joydev].joydata->handler(JOY_EVENTAXIS, 1, (char)js.y, joydev);
	retval = 1;
     }
   
   return retval;
}

static int joystick_flp0(int joydev, int acquire) {
    unsigned long tmpl;

#ifdef DEBUG
    printf("joyflp: %d, %d\n", joydev, acquire);
#endif

    if (!acquire) {
	/* close joystick device */
	if (joydesc[joydev].fd >= 0) {
	    close(joydesc[joydev].fd);
	    joydesc[joydev].fd = -1;
	}
    } else if (joydesc[joydev].fd < 0) {
#ifdef DEBUG
    puts("trying to reopen");
#endif
	/* (re)open joystick device */
   	if ((joydesc[joydev].fd = open(__joystick_devicenames[joydev] ?
	    __joystick_devicenames[joydev] : defnames[joydev], O_RDONLY|O_NONBLOCK)) < 0)
		return 1;

	tmpl = PROT0_TIMELIMIT;
	if (-1 == ioctl(joydesc[joydev].fd, JS_SET_TIMELIMIT, &tmpl))
	    return 1;

	if (-1 == ioctl(joydesc[joydev].fd, JS_SET_CAL, &joydesc[joydev].joydata->p.prot0.caldata))
	    return 1;
    }
    return 0;
}

/* For new, version 1.x+ joystick drivers */
static int joystick_upd1(int joydev)
{
   int retval = 0;
   struct js_event js;
	
   if (read(joydesc[joydev].fd, &js, sizeof(struct js_event)) != 
       sizeof(struct js_event))
     return retval;

   retval = 2; /* further events might be pending */

   if ((js.type & JS_EVENT_AXIS) == JS_EVENT_AXIS) {
     joydesc[joydev].joydata->handler(JOY_EVENTAXIS, js.number, (js.value-0x8000)>>8, joydev);
     retval = 3;
   }

   if ((js.type & JS_EVENT_BUTTON) == JS_EVENT_BUTTON)
     {
	if (js.value)
	  joydesc[joydev].joydata->handler(JOY_EVENTBUTTONDOWN, js.number, 0, joydev);
	else
	  joydesc[joydev].joydata->handler(JOY_EVENTBUTTONUP, js.number, 0, joydev);
        retval = 3;
     }
   
   return retval;
}

static int joystick_flp1(int joydev, int acquire) {
    if (!acquire) {
	/* close joystick device */
	if (joydesc[joydev].fd >= 0) {
	    close(joydesc[joydev].fd);
	    joydesc[joydev].fd = -1;
	}
    } else if (joydesc[joydev].fd < 0) {
	/* (re)open joystick device */
   	if ((joydesc[joydev].fd = open(__joystick_devicenames[joydev] ?
	    __joystick_devicenames[joydev] : defnames[joydev], O_RDONLY|O_NONBLOCK)) < 0)
		return 1;

	if (-1 == ioctl(joydesc[joydev].fd, JSIOCSCORR,
			&joydesc[joydev].joydata->p.prot1.corrdata))
	    return 1;
    }
    return 0;
}

/* calibration routine for version 0.x protocol */
static int jscal0(int joydev, __joystick_output jo)
{
   char msg[100];
   int tmp;
   long tmpl;
   struct JS_DATA_TYPE js_data;
   
   tmpl = PROT0_TIMELIMIT;
   
   if (-1 == ioctl(joydesc[joydev].fd, JS_SET_TIMELIMIT, &tmpl)) {
	if (__svgalib_driver_report)
	    printf("svgalib, joystick%d: Failed to set timelimit\n", joydev);
	return -1;
   }
   
   if (-1 == ioctl(joydesc[joydev].fd, JS_GET_CAL, &js_data)) {
	if (__svgalib_driver_report)
	    printf("svgalib, joystick%d: Failed to read calibration data\n", joydev);
	return -1;
   }
   if (__svgalib_driver_report)
	printf("svgalib, joystick%d: Current correction: %d , %d\n", joydev,
		js_data.x, js_data.y);

   sprintf(msg, "Move Joystick %d to lower right corner and press either button...\n", joydev);
   jo(msg);
   
   while ((read (joydesc[joydev].fd, &js_data, JS_RETURN) > 0) && js_data.buttons == 0x00)
     usleep(100);
   
   for (tmp = 0; js_data.x > 0xff; tmp++, js_data.x = js_data.x >> 1);
   js_data.x = tmp;
   for (tmp = 0; js_data.y > 0xff; tmp++, js_data.y = js_data.y >> 1);
   js_data.y = tmp;

   if (__svgalib_driver_report)
	printf("svgalib, joystick%d: Setting correction: %d , %d\n", joydev,
		js_data.x, js_data.y);
   
   if (-1 == ioctl(joydesc[joydev].fd, JS_SET_CAL, &js_data))
     {
	if (__svgalib_driver_report)
	    printf("svgalib, joystick%d: Failed to set calibration data\n", joydev);
	return -1;
     }

   sprintf(msg, "Center Joystick %d and press either button...\n", joydev);
   jo(msg);
   
   while ((read (joydesc[joydev].fd, &js_data, JS_RETURN) > 0) && js_data.buttons)
     usleep(100);
   while ((read (joydesc[joydev].fd, &js_data, JS_RETURN) > 0) && !js_data.buttons)
     usleep(100);
   if (__svgalib_driver_report)
	printf("svgalib, joystick%d: Setting center offset: %d , %d\n", 
	       joydev,	js_data.x, js_data.y);
   joydesc[joydev].joydata->p.prot0.xdif = js_data.x;
   joydesc[joydev].joydata->p.prot0.ydif = js_data.y;

   sprintf(msg, "Joystick %d recalibrated.\n", joydev);
   jo(msg);
   
   return joydesc[joydev].fd;
}

/* Definitions and data types for jscal1() and friends */
#define NUM_POS 3

struct js_info 
{
   int time;
   int buttons;
   int axis[4];
};

struct correction_data 
{
   int cmin[NUM_POS];
   int cmax[NUM_POS];
}; 

/* support routine for jscal1() 
 * Waits for a joystick event to happen 
 * and updates struct js_info *s accordingly */
static int wait_for_event(int fd, struct js_info *s)
{
   struct js_event ev;
   
   if (read(fd, &ev, sizeof(struct js_event)) 
       != sizeof(struct js_event))
     return 0;
     
   s->time = ev.time;
   
   switch (ev.type & ~JS_EVENT_INIT) 
     {
      case JS_EVENT_AXIS: 
	s->axis[ev.number]=ev.value;
	break;
      case JS_EVENT_BUTTON:
	s->buttons = (s->buttons & ~(1 << ev.number)) | ev.value << ev.number;
     }	
   return sizeof(struct js_event);
}

/* calibration routine for 1.x protcol */
static int jscal1(int joydev, __joystick_output jo)
{
   int i, j, t;
   struct correction_data corda[4]; 
   struct js_corr corr[4];
   struct js_info js;
   const char corr_coef_num[] = {0,6};
   char msg[200];
   
   static const char *pos_name[] = {"minimum", "center", "maximum"};
   static const char *corr_name[] = {"none (raw)", "broken line"};

   js.buttons=0;
   
   if (-1 == ioctl(joydesc[joydev].fd, JSIOCGCORR, &corr)) 
     {
	if (__svgalib_driver_report)
	   printf("svgalib, joystick %d: error getting correction\n", joydev);
	return -1;
     }

   for (i=0; i<3; i++) 
     {
	corr[i].type = JS_CORR_NONE;
	corr[i].prec = 0;
     }
   
   for(i=0; i < joydesc[joydev].joydata->axes; i++)
     js.axis[i]=0;

   for(i=0; i < joydesc[joydev].joydata->axes; i++)
     while(0==js.axis[i])
       wait_for_event(joydesc[joydev].fd, &js);
   
   
   if (__svgalib_driver_report)
     printf("svgalib, joystick %d: Setting correction to: %s\n", joydev, corr_name[JS_CORR_NONE]);
   
   if (-1 == ioctl(joydesc[joydev].fd, JSIOCSCORR, &corr)) 
     {
   	if (__svgalib_driver_report)
	  printf("svgalib, joystick %d: error setting correction\n", joydev);
	return -1;
     }
   
   
   for (i=0; i<NUM_POS; i++) 
     {
	do 
	  {
	     while(js.buttons)
	       wait_for_event(joydesc[joydev].fd, &js);

	     sprintf(msg, "Position all joystick axes at their %s positions and push any button.\n"
	     	"Move them slightly around then, still holding the button.\n", pos_name[i]);
	     jo(msg);

	     while (!js.buttons) 
	       {
		  msg[0] = 0;
		  for(j=0; j < joydesc[joydev].joydata->axes; j++)
		    sprintf(strchr(msg, 0), "Axis %d:%5d ", j, js.axis[j]);
		  strcat(msg, "\r");
	     	  jo(msg);
		  
		  wait_for_event(joydesc[joydev].fd, &js);
	       }
	     jo("\nHold ... ");
	     for (j = 0; j < joydesc[joydev].joydata->axes; j++) 
	       {
		  corda[j].cmin[i] = js.axis[j];
		  corda[j].cmax[i] = 0;
	       }
	     t = js.time;
	     while (js.time < t+2000 && js.buttons) 
	       {
		  for (j = 0; j < joydesc[joydev].joydata->axes; j++) 
		    {
		       if (js.axis[j] < corda[j].cmin[i]) 
			 corda[j].cmin[i] = js.axis[j];
		       if (js.axis[j] > corda[j].cmax[i]) 
			 corda[j].cmax[i] = js.axis[j];
		    }
		wait_for_event(joydesc[joydev].fd, &js);
	       }
	     if (js.time < t+2000) {
 		jo("released too soon. Try again:\n\n");
	     }
	  } while (js.time < t+2000);
	jo("OK.\n");
     }
   
   for (j = 0; j < joydesc[joydev].joydata->axes; j++) 
     {
	int temp;
	
	/* This used to use fp math, not any more */
	corr[j].coef[0] = corda[j].cmin[1];
	corr[j].coef[1] = corda[j].cmax[1];
	corr[j].coef[2] = (temp=32768 / (corda[j].cmin[1] - 
				      corda[j].cmax[0]))*16384;
	corr[j].coef[3] = -temp * corda[j].cmax[0];
	corr[j].coef[4] = (temp=32767 / (corda[j].cmin[2] - 
				      corda[j].cmax[1]))*16384;
	corr[j].coef[5] = -temp * corda[j].cmin[1] + 32768;
	
	
	corr[j].type = JS_CORR_BROKEN;
	corr[j].prec = 10;
     }

   if (__svgalib_driver_report)
     {
	printf("svgalib, joystick %d: Setting correction to: %s\n", joydev,
		 corr_name[(int)corr[0].type]);
	for (i = 0; i < joydesc[joydev].joydata->axes; i++) 
	  {
	     printf("  Coefs for axis %d:", i);
	     for(j = 0; j < corr_coef_num[(int)corr[i].type]; j++) 
	       {
		  printf(" %d", corr[i].coef[j]); 
		  if (j < corr_coef_num[(int)corr[i].type] - 1) putchar(',');
	       }
	     puts("");
	  }
	puts("");
     }
   
   if (-1 == ioctl(joydesc[joydev].fd, JSIOCSCORR, &corr)) 
     {
   	if (__svgalib_driver_report)
	  printf("svgalib, joystick %d: error setting correction\n", joydev);
	return -1;
     }
   
   return joydesc[joydev].fd;
}

int joystick_update(void) {
   int i, retval = 0, status, mask;

   for (i = 0, mask = 1; i < NUM_JOYSTICKS; i++, mask <<= 1) {
	if ((joydesc[i].fd >= 0) && joydesc[i].joydata) {
	    do {
		status = joydesc[i].joydata->update(i);
		if (status & 1)
		    retval |= mask;
	    } while(status & 2);
	}
   }
   return retval & 1;
}

/* temporarily release joystick devices.. */
void __joystick_flip_vc(int acquire) {
   int i;

   for (i = 0; i < NUM_JOYSTICKS; i++)
	if (joydesc[i].joydata && joydesc[i].joydata->flip_vc)
	    if (joydesc[i].joydata->flip_vc(i, acquire)) {
		puts("svgalib: Fatal, cannot reopen joystick after VC switch.\n");
		exit(1);
	    }
}

static void joy_stdout(const char *msg)
{
   fputs(msg, stdout);
   fflush(stdout);
}

int joystick_init(int joydev, __joystick_output jo)
{
   if ((joydev < 0) || (joydev >= NUM_JOYSTICKS))
	return -1;
   
   if (jo == JOY_CALIB_STDOUT) 
	jo = joy_stdout;

   joystick_close(joydev);

   if ((joydesc[joydev].fd = open(__joystick_devicenames[joydev] ?
	 __joystick_devicenames[joydev] : defnames[joydev], O_RDONLY|O_NONBLOCK)) < 0)
     {
	return -1;
     }
   else
     {
	u_int32_t version;

	if (-1 == ioctl(joydesc[joydev].fd, JSIOCGVERSION, &version)) 
	  {
	     if (__svgalib_driver_report)
	       printf("svgalib: Initializing joystick %d: assuming old 0.x driver protocol\n", joydev);
	     
	     /* Old 0.x protocol */
	     joystick_initdefhandler(joydev, 4, 2);
	     joydesc[joydev].joydata->update = joystick_upd0;
	     joydesc[joydev].joydata->flip_vc = joystick_flp0;
	     joydesc[joydev].joydata->p.prot0.b = 0;
	     joydesc[joydev].joydata->p.prot0.x = 0;
	     joydesc[joydev].joydata->p.prot0.y = 0;
	     joydesc[joydev].joydata->p.prot0.xdif = 0x80;
	     joydesc[joydev].joydata->p.prot0.ydif = 0x80;
	     
	     if (__svgalib_driver_report)
	       printf("   assuming %d axes and %d buttons\n",
		      joydesc[joydev].joydata->axes, joydesc[joydev].joydata->buttons);
	     
	     /* Now calibrate ... */
	     if (jo)
		jscal0(joydev, jo);

	     if (-1 == ioctl(joydesc[joydev].fd, JS_GET_CAL,
			 &joydesc[joydev].joydata->p.prot0.caldata)) {
	       if (__svgalib_driver_report)
		  printf("svgalib, joystick%d: Failed to read calibration data\n", joydev);
	       joystick_close(joydev);
	       return -1;
	     }
	     return 1;
	  }
	else
	  {
	     char axes, buttons;

	     if (__svgalib_driver_report)
	       printf("svgalib: Initializing joystick %d: driver version %x.%x.%x (new protocol)\n",
		      joydev,
		      (version & 0xff0000) >> 16, (version & 0xff00)>> 8,
		      version & 0xff);
	     
	     if (-1 == ioctl(joydesc[joydev].fd, JSIOCGAXES, &axes)) 
	       {
	          if (__svgalib_driver_report)
		    printf("svgalib, joystick%d: error getting number of axes\n", joydev);
	          joystick_close(joydev);
		  return -1;
	       }
	     
	     if (-1 == ioctl(joydesc[joydev].fd, JSIOCGBUTTONS, &buttons)) 
	       {
	          if (__svgalib_driver_report)
		    printf("svgalib, joystick%d: error getting number of buttons\n", joydev);
	          joystick_close(joydev);
		  return -1;
	       }

	     if (__svgalib_driver_report)
	       printf("joystick %d has %d axes and %d buttons\n", joydev, axes, buttons);
	     joystick_initdefhandler(joydev, buttons, axes);
	     
	     /* Now calibrate ... */
	     if (jo)
		jscal1(joydev, jo);

	     if (-1 == ioctl(joydesc[joydev].fd, JSIOCGCORR,
			&joydesc[joydev].joydata->p.prot1.corrdata)) {
	       if (__svgalib_driver_report)
		  printf("svgalib, joystick%d: Failed to read calibration data\n", joydev);
	       joystick_close(joydev);
	       return -1;
	     }
	     joydesc[joydev].joydata->update = joystick_upd1;
	     joydesc[joydev].joydata->flip_vc = joystick_flp1;
	     return 1;
	  }
     }  
   return -1;
}

void joystick_close(int joydev)
{
   if (joydev >= NUM_JOYSTICKS)
	return;
   if (joydev < 0) {
	for (joydev = 0; joydev < NUM_JOYSTICKS; joydev++)
	     joystick_close(joydev);
	return;
   }
   if (joydesc[joydev].fd >= 0)
	close(joydesc[joydev].fd);
   joydesc[joydev].fd = -1;
   if (joydesc[joydev].joydata) {
	if (joydesc[joydev].joydata->dh_buttons)
	    free(joydesc[joydev].joydata->dh_buttons);
	if (joydesc[joydev].joydata->dh_axes)
	    free(joydesc[joydev].joydata->dh_axes);
	free(joydesc[joydev].joydata);
	joydesc[joydev].joydata = NULL;
   }
}

char joystick_getnumaxes(int joydev)
{
   if ((joydev >= 0) && (joydev < NUM_JOYSTICKS) && joydesc[joydev].joydata)
	return joydesc[joydev].joydata->axes;
   return 0;
}

char joystick_getnumbuttons(int joydev)
{
   if ((joydev >= 0) && (joydev < NUM_JOYSTICKS) && joydesc[joydev].joydata)
	return joydesc[joydev].joydata->buttons;
   return 0;
}

static void joystick_initdefhandler(int joydev, int buttons, int axes)
{
   joydesc[joydev].joydata = malloc(sizeof(joydat_t));
   if (!joydesc[joydev].joydata) {
     nomem:
	printf("svgalib: Fatal, out of memory\n");
	exit(1);
   }
   joydesc[joydev].joydata->axes = axes;
   joydesc[joydev].joydata->buttons = buttons;
   joydesc[joydev].joydata->dh_buttons = malloc(buttons * sizeof(int));
   joydesc[joydev].joydata->dh_axes = malloc(axes * sizeof(int));
  
   if ((!joydesc[joydev].joydata->dh_buttons) ||
	(!joydesc[joydev].joydata->dh_axes))
	    goto nomem; 
  
   memset(joydesc[joydev].joydata->dh_buttons, 0, buttons * sizeof(int));
   memset(joydesc[joydev].joydata->dh_axes, 0, axes * sizeof(int));
   joydesc[joydev].joydata->handler = joystick_defaulthandler;
   joydesc[joydev].joydata->update = joystick_updx;
   joydesc[joydev].joydata->flip_vc = NULL;
}

static void joystick_defaulthandler(int event, int num, char val, int joydev)
{
#ifdef DEBUG
   printf("%d: %d %d %d\n", joydev, event, num, val);
#endif
   switch (event)
     {
      case JOY_EVENTAXIS:
	joydesc[joydev].joydata->dh_axes[num]=val;
	break;

      case JOY_EVENTBUTTONDOWN:
	joydesc[joydev].joydata->dh_buttons[num]=1;
	break;

      case JOY_EVENTBUTTONUP:
	joydesc[joydev].joydata->dh_buttons[num]=0;
	break;	
     }
}

void joystick_sethandler(int joydev, __joystick_handler jh)
{
   if (joydev >= NUM_JOYSTICKS)
	return;
   if (joydev < 0) {
	for (joydev = 0; joydev < NUM_JOYSTICKS; joydev++)
	     joystick_sethandler(joydev, jh);
	return;
   }
   if (joydesc[joydev].joydata)
	joydesc[joydev].joydata->handler=jh;
}

void joystick_setdefaulthandler(int joydev)
{
   joystick_sethandler(joydev, joystick_defaulthandler);
}

char joystick_getaxis(int joydev, int a)
{
   if ((joydev >= 0) && (joydev < NUM_JOYSTICKS) && joydesc[joydev].joydata &&
		(a >= 0) && (a < joydesc[joydev].joydata->axes))
	return joydesc[joydev].joydata->dh_axes[a];
   return 0;
}

char joystick_getbutton(int joydev, int b)
{
   if ((joydev >= 0) && (joydev < NUM_JOYSTICKS) && joydesc[joydev].joydata &&
		(b >= 0) && (b < joydesc[joydev].joydata->buttons))
	return joydesc[joydev].joydata->dh_buttons[b];
   return 0;
}

#endif
