/* Joystick interface modeled after svgalibs keyboard and mouse interfaces 
 * Copyright 1998 Daniel Engström <daniel.engstrom@riksnett.no> 
 * Partly based on code from 
 * joystick-0.7.3 Copyright (C) 1992, 1993 Author C. Smith
 * and 
 * joystick-1.0.6 Copyright (C) 1997 Vojtech Pavlik 
 */

#include <sys/ioctl.h> /* for _IOR(x) and _IOW(x) macros */ 
#include <sys/types.h>
/*
 * IOCTL commands for joystick driver
 */
				/* get driver version */
#define JSIOCGVERSION		_IOR('j', 0x01, u_int32_t)

				/* get number of axes */
#define JSIOCGAXES		_IOR('j', 0x11, u_int8_t)
				/* get number of buttons */
#define JSIOCGBUTTONS		_IOR('j', 0x12, u_int8_t)

		                /* set correction values */
#define JSIOCSCORR		_IOW('j', 0x21, struct js_corr[4])
		                /* get correction values */
#define JSIOCGCORR		_IOR('j', 0x22, struct js_corr[4])

/*
 * Types and constants for get/set correction
 */

#define JS_CORR_NONE		0x00		/* returns raw values */
#define JS_CORR_BROKEN		0x01		/* broken line */

struct js_corr {
	int32_t coef[8];
	u_int16_t prec;
	u_int16_t type;
};

/*
 * Types and constants for reading from /dev/js
 */

#define JS_EVENT_BUTTON		0x01	/* button pressed/released */
#define JS_EVENT_AXIS		0x02	/* joystick moved */
#define JS_EVENT_INIT		0x80	/* initial state of device */

struct js_event {
        u_int32_t time;		/* time when event happened in miliseconds 
				 * since open */
        u_int16_t value;	/* new value */
        u_int8_t  type;		/* type of event, see above */
        u_int8_t  number;	/* axis/button number */
};

/*
 * Backward (version 0.x) compatibility definitions
 */

#define JS_RETURN 	sizeof(struct JS_DATA_TYPE)
#define JS_TRUE 	1
#define JS_FALSE 	0
#define JS_X_0		0x01		/* bit mask for x-axis js0 */
#define JS_Y_0		0x02		/* bit mask for y-axis js0 */
#define JS_X_1		0x04		/* bit mask for x-axis js1 */
#define JS_Y_1		0x08		/* bit mask for y-axis js1 */
#define JS_MAX 		2		/* max number of joysticks */

#define JS_SET_CAL 0x01		/*ioctl cmd to set joystick correction factor*/
#define JS_GET_CAL 0x02		/*ioctl cmd to get joystick correction factor*/
#define JS_SET_TIMEOUT 0x03	/*ioctl cmd to set maximum number of iterations
				  to wait for a timeout*/
#define JS_GET_TIMEOUT		0x04	/*as above, to get*/
#define JS_SET_TIMELIMIT	0x05	/*set data retention time*/
#define JS_GET_TIMELIMIT	0x06	/*get data retention time*/
#define JS_GET_ALL		0x07	/*get the whole JS_DATA[minor] struct*/
#define JS_SET_ALL		0x08	/*set the whole JS_DATA[minor] struct
						  except JS_BUSY!*/

/* version 0.x struct */
struct JS_DATA_TYPE {
	int buttons;		/* immediate button state */
	int x;                  /* immediate x axis value */
	int y;                  /* immediate y axis value */
};
