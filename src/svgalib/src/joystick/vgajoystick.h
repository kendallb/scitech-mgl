#ifndef VGAJOYSTICK_H
#define VGAJOYSTICK_H
/* Joystick interface modeled after svgalibs keyboard and mouse interfaces 
 * Copyright 1998 Daniel Engström <daniel.engstrom@riksnett.no> 
 * Partly based on code from 
 * joystick-0.7.3 Copyright (C) 1992, 1993 Author C. Smith
 * and 
 * joystick-1.0.6 Copyright (C) 1997 Vojtech Pavlik 
 *
 * Extension and modifications. Multiple joystick support, VC switching,
 * etc. Michael Weller <eowmob@exp-math.uni-essen.de>.
 */

/* event for joystick handlers */
#define JOY_EVENTBUTTONDOWN 1
#define JOY_EVENTBUTTONUP   2
#define JOY_EVENTAXIS       3

/* file is a struct FILE to output calibration instructions to,
 * set to NULL to skip calibration 
 */

typedef void (*__joystick_output) (const char *msg);

int joystick_init(int joydev, __joystick_output jo);

/* This is guaranteed not to collide with any user definition */
#define JOY_CALIB_STDOUT ((__joystick_output)(void *)joystick_init)

void joystick_close(int joydev);

/* polls the joystick and calls the eventhandler */
int joystick_update(void);

typedef void (*__joystick_handler) (int event, int number, char value, int joydev); 
 /* event - event type; see above
 * number - the axis or button number for this event 0=x axis or button 1, etc.
 * value - value for axis events -128 - 0 - +127
 */
void joystick_sethandler(int joydev, __joystick_handler jh);
void joystick_setdefaulthandler(int joydev);

char joystick_getnumaxes(int joydev);
char joystick_getnumbuttons(int joydev);

/* querys the default handler if used */
char joystick_getaxis(int joydev, int a);
char joystick_getbutton(int joydev, int b);

#define joystick_button1(i) joystick_getbutton(i, 0)
#define joystick_button2(i) joystick_getbutton(i, 1)
#define joystick_button3(i) joystick_getbutton(i, 2)
#define joystick_button4(i) joystick_getbutton(i, 3)

#define joystick_getb1() joystick_getbutton(0, 0)
#define joystick_getb2() joystick_getbutton(0, 1)
#define joystick_getb3() joystick_getbutton(0, 2)
#define joystick_getb4() joystick_getbutton(0, 3)

#define joystick_x(i) joystick_getaxis(i, 0)
#define joystick_y(i) joystick_getaxis(i, 1)
#define joystick_z(i) joystick_getaxis(i, 2)

#define joystick_getx() joystick_getaxis(0, 0)
#define joystick_gety() joystick_getaxis(0, 1)
#define joystick_getz() joystick_getaxis(0, 2)

#endif
