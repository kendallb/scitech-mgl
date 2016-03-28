/****************************************************************************
*
*                         SciTech Display Doctor
*
*               Copyright (C) 1991-2004 SciTech Software, Inc.
*                            All rights reserved.
*
*  ======================================================================
*  |REMOVAL OR MODIFICATION OF THIS HEADER IS STRICTLY PROHIBITED BY LAW|
*  |                                                                    |
*  |This copyrighted computer code is a proprietary trade secret of     |
*  |SciTech Software, Inc., located at 505 Wall Street, Chico, CA 95928 |
*  |USA (www.scitechsoft.com).  ANY UNAUTHORIZED POSSESSION, USE,       |
*  |VIEWING, COPYING, MODIFICATION OR DISSEMINATION OF THIS CODE IS     |
*  |STRICTLY PROHIBITED BY LAW.  Unless you have current, express       |
*  |written authorization from SciTech to possess or use this code, you |
*  |may be subject to civil and/or criminal penalties.                  |
*  |                                                                    |
*  |If you received this code in error or you would like to report      |
*  |improper use, please immediately contact SciTech Software, Inc. at  |
*  |530-894-8400.                                                       |
*  |                                                                    |
*  |REMOVAL OR MODIFICATION OF THIS HEADER IS STRICTLY PROHIBITED BY LAW|
*  ======================================================================
*
* Language:     ANSI C
* Environment:  IBM PC (Linux/i386)
*
* Description:  SVGAlib functions to read the /etc/vga/libvga.* config files.
*
****************************************************************************/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

#include <vga.h>
#include "driver.h"
#include "mouse/vgamouse.h"

static int allowoverride = 0;	/* Allow dangerous options in ENV-Var or in */
				/* the $HOME/.svgalibrc */
int mouse_type = -1;
int mouse_modem_ctl = 0;
int __svgalib_default_mode = 0;
int __svgalib_nosigint = 0;	/* Don't generate SIGINT in graphics mode */
char *mouse_device = "/dev/mouse";

static int color_text;		/* true if color text emulation */

/* Parse a string for options.. str is \0-terminated source,
   commands is an array of char ptrs (last one is NULL) containing commands
   to parse for. (if first char is ! case sensitive),
   func is called with ind the index of the detected command.
   func has to return the ptr to the next unhandled token returned by strtok(NULL," ").
   Use strtok(NULL," ") to get the next token from the file..
   mode is 1 when reading from conffile and 0 when parsing the env-vars. This is to
   allow disabling of dangerous (hardware damaging) options when reading the ENV-Vars
   of Joe user.
   Note: We use strtok, that is str is destroyed! */
static void parse_string(char *str, char **commands, char *(*func) (int ind, int mode), int mode)
{
    int index;
    register char *ptr, **curr;

    /*Pass one, delete comments,ensure only whitespace is ' ' */
    for (ptr = str; *ptr; ptr++) {
	if (*ptr == '#') {
	    while (*ptr && (*ptr != '\n')) {
		*ptr++ = ' ';
	    }
	    if (*ptr)
		*ptr = ' ';
	} else if (isspace(*ptr)) {
	    *ptr = ' ';
	}
    }
    /*Pass two, parse commands */
    ptr = strtok(str, " ");
    while (ptr) {
#ifdef DEBUG_CONF
	printf("Parsing: %s\n", ptr);
#endif
	for (curr = commands, index = 0; *curr; curr++, index++) {
#ifdef DEBUG_CONF
	    printf("Checking: %s\n", *curr);
#endif
	    if (**curr == '!') {
		if (!strcmp(*curr + 1, ptr)) {
		    ptr = (*func) (index, mode);
		    break;
		}
	    } else {
		if (!strcasecmp(*curr, ptr)) {
		    ptr = (*func) (index, mode);
		    break;
		}
	    }
	}
	if (!*curr)		/*unknow command */
	    ptr = strtok(NULL, " ");	/* skip silently til' next command */
    }
}

static void process_config_file(FILE *file, int mode, char **commands,
		 			char *(*func)(int ind, int mode)) {
 struct stat st;
 char *buf, *ptr;
 int i;

  fstat(fileno(file), &st);	/* Some error analysis may be fine here.. */
  if ( (buf = alloca(st.st_size + 1)) == 0) {	/* + a final \0 */
    puts("svgalib: out of mem while parsing config file !");
    return;
  }
  fread(buf, 1, st.st_size, file);
  for (i = 0, ptr = buf; i < st.st_size; i++, ptr++) {
    if (!*ptr)
      *ptr = ' ';			/* Erase any maybe embedded \0 */
    }
  *ptr = 0;					/* Trailing \0 */
  parse_string(buf, commands, func, mode);	/* parse config file */
}

/* This is a service function for drivers. Commands and func are as above.
   The following config files are parsed in this order:
    - /etc/vga/libvga.conf (#define SVGALIB_CONFIG_FILE)
    - ~/.svgalibrc
    - the file where env variavle SVGALIB_CONFIG_FILE points
    - the env variable SVGALIB_CONFIG (for compatibility, but I would remove
      it, we should be more flexible...  Opinions ?)
    - MW: I'd rather keep it, doesn't do too much harm and is sometimes nice
      to have.
*/
void __svgalib_read_options(char **commands, char *(*func) (int ind, int mode)) {
    FILE *file;
    char *buf = NULL, *ptr;
    int i;

    if ( (file = fopen(SVGALIB_CONFIG_FILE, "r")) != 0) {
#ifdef DEBUG_CONF
  printf("Processing config file \'%s\'\n", SVGALIB_CONFIG_FILE);
#endif
      process_config_file(file, 1, commands, func);
      fclose(file);
    } else {
	fprintf(stderr, "svgalib: Configuration file \'%s\' not found.\n", SVGALIB_CONFIG_FILE);
    }

    if ( (ptr = getenv("HOME")) != 0) {
      char *filename;

      filename = alloca(strlen(ptr) + 20);
      if (!filename) {
	puts("svgalib: out of mem while parsing SVGALIB_CONFIG_FILE !");
      } else {
	strcpy(filename, ptr);
	strcat(filename, "/.svgalibrc");
	if ( (file = fopen(filename, "r")) != 0) {
#ifdef DEBUG_CONF
	  printf("Processing config file \'%s\'\n", filename);
#endif
	  process_config_file(file, allowoverride, commands, func);
	  fclose(file);
	}
      }
    }

    if ( (ptr = getenv("SVGALIB_CONFIG_FILE")) != 0) {
      if ( (file = fopen(ptr, "r")) != 0) {
#ifdef DEBUG_CONF
  printf("Processing config file \'%s\'\n", ptr);
#endif
	process_config_file(file, allowoverride, commands, func);
	fclose(file);
      } else {
        fprintf(stderr, "svgalib: warning: config file \'%s\', pointed to by SVGALIB_CONFIG_FILE, not found !\n", ptr);
      }
    }

    if ( (ptr = getenv("SVGALIB_CONFIG")) != 0  &&  (i = strlen(ptr)) != 0) {
      buf = alloca(i + 1);
      if (!buf) {
	puts("svgalib: out of mem while parsing SVGALIB_CONFIG !");
      } else {
	strcpy(buf, ptr);		/* Copy for safety and strtok!! */
#ifdef DEBUG_CONF
	puts("Parsing env variable \'SVGALIB_CONFIG\'");
#endif
	parse_string(buf, commands, func, allowoverride);
      }
    }
}

/* Configuration file, mouse interface, initialization. */

/* We leave the same keywords so that users can reuse their existing libvga.conf
   files and keep their mouse/joystick settings */
/* What are these m0 m1 m... things ? Shouldn't they be removed ? */
static char *vga_conf_commands[] = {
    "mouse", "monitor", "!m", "!M", "chipset", "overrideenable", "!m0", "!m1", "!m2", "!m3",
    "!m4", "!m9", "!M0", "!M1", "!M2", "!M3", "!M4", "!M5", "!M6", "nolinear",
    "linear", "!C0", "!C1", "!C2", "!C3", "!C4", "!C5", "!C6", "!C7", "!C8", "!C9",
    "!c0", "!c1", "monotext", "colortext", "!m5",
    "leavedtr", "cleardtr", "setdtr", "leaverts", "clearrts",
    "setrts", "grayscale", "horizsync", "vertrefresh", "modeline",
    "security","mdev", "default_mode", "nosigint", "sigint",
    "joystick0", "joystick1", "joystick2", "joystick3",
    "TextProg",
    NULL};

static char *conf_mousenames[] =
{
  "Microsoft", "MouseSystems", "MMSeries", "Logitech", "Busmouse", "PS2",
    "MouseMan", "gpm", "Spaceball", "none", "IntelliMouse", "IMPS2", NULL};

static int check_digit(char *ptr, char *digits)
{
    if (ptr == NULL)
	return 0;
    return strlen(ptr) == strspn(ptr, digits);
}

static char *process_option(int command, int mode)
{
    static char digits[] = ".0123456789";
    char *ptr, **tabptr; //, *ptb;
    int i;
    float f;

#ifdef DEBUG_CONF
    printf("command %d detected.\n", command);
#endif
    switch (command) {
    case 5:
#ifdef DEBUG_CONF
	puts("Allow override");
#endif
	break;
    case 0:			/* mouse */
    case 2:			/* m */
	ptr = strtok(NULL, " ");
	if (ptr == NULL)
	    goto inv_mouse;
	if (check_digit(ptr, digits + 1)) {	/* It is a number.. */
	    i = atoi(ptr);
	    if ((i < 0) || (i > 9))
		goto inv_mouse;
	    mouse_type = i;
	} else {		/* parse for symbolic name.. */
	    for (i = 0, tabptr = conf_mousenames; *tabptr; tabptr++, i++) {
		if (!strcasecmp(ptr, *tabptr)) {
		    mouse_type = i;
		    goto leave;
		}
	    }
	  inv_mouse:
	    printf("svgalib: Illegal mouse setting: {mouse|m} %s\n"
		   "Correct usage: {mouse|m} mousetype\n"
		   "where mousetype is one of 0, 1, 2, 3, 4, 5, 6, 7, 9,\n",
		   (ptr != NULL) ? ptr : "");
	    for (tabptr = conf_mousenames, i = 0; *tabptr; tabptr++, i++) {
		if (i == MOUSE_NONE)
		    continue;
		printf("%s, ", *tabptr);
	    }
	    puts("or none.");
	    return ptr;		/* Allow a second parse of str */
	}
	break;
    case 1:			/* monitor */
    case 3:			/* M */
	ptr = strtok(NULL, " ");
	if (check_digit(ptr, digits + 1)) {	/* It is an int.. */
	    i = atoi(ptr);
	    if (i < 7) {
		command = i + 12;
		goto monnum;
	    } else {
		f = i;
		goto monkhz;
	    }
	} else if (check_digit(ptr, digits)) {	/* It is a float.. */
	    f = atof(ptr);
	  monkhz:
	    if (!mode)
		goto mon_deny;
	    //	    __svgalib_horizsync.max = f * 1000.0f;
	} else {
	    printf("svgalib: Illegal monitor setting: {monitor|M} %s\n"
		   "Correct usage: {monitor|M} monitortype\n"
		   "where monitortype is one of 0, 1, 2, 3, 4, 5, 6, or\n"
		   "maximal horz. scan frequency in khz.\n"
		   "Example: monitor 36.5\n",
		   (ptr != NULL) ? ptr : "");
	    return ptr;		/* Allow a second parse of str */
	}
	break;
    case 4:			/* chipset */
	ptr = strtok(NULL, " ");
	break;
    case 6:			/* oldstyle config: m0-m4 */
    case 7:
    case 8:
    case 9:
    case 10:
	mouse_type = command - 6;
	break;
    case 11:			/* m9 */
	mouse_type = MOUSE_NONE;
	break;
    case 12:			/* oldstyle config: M0-M6 */
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
      monnum:
	if (!mode) {
	  mon_deny:
	    puts("Monitor setting from environment denied.");
	    break;
	}
	break;
    case 19:			/*nolinear */
	break;
    case 20:			/*linear */
	break;
    case 21:			/* oldstyle chipset C0 - C9 */
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
    case 30:

	break;
    case 31:			/* c0-c1 color-text selection */
	if (!mode) {
	  coltexdeny:
	    puts("Color/mono text selection from environment denied.");
	    break;
	}
	color_text = 0;
	break;
    case 32:
	if (!mode) {
	    puts("Color/mono text selection from environment denied.");
	    break;
	}
	color_text = 1;
	break;
    case 33:
    case 34:
	if (!mode)
	    goto coltexdeny;
	color_text = command - 32;
	break;
    case 35:			/* Mouse type 5 - "PS2". */
	mouse_type = 5;
	break;
    case 36:
	mouse_modem_ctl &= ~(MOUSE_CHG_DTR | MOUSE_DTR_HIGH);
	break;
    case 37:
	mouse_modem_ctl &= ~MOUSE_DTR_HIGH;
	mouse_modem_ctl |= MOUSE_CHG_DTR;
	break;
    case 38:
	mouse_modem_ctl |= (MOUSE_CHG_RTS | MOUSE_RTS_HIGH);
	break;
    case 39:
	mouse_modem_ctl &= ~(MOUSE_CHG_RTS | MOUSE_RTS_HIGH);
	break;
    case 40:
	mouse_modem_ctl &= ~MOUSE_RTS_HIGH;
	mouse_modem_ctl |= MOUSE_CHG_RTS;
	break;
    case 41:
	mouse_modem_ctl |= (MOUSE_CHG_RTS | MOUSE_RTS_HIGH);
	break;
    case 42:			/* grayscale */

	break;
    case 43:			/* horizsync */
	ptr = strtok(NULL, " ");
	if (check_digit(ptr, digits)) {		/* It is a float.. */
	    f = atof(ptr);
	    if (!mode)
		goto mon_deny;
	    //	    __svgalib_horizsync.min = f * 1000;
	} else
	    goto hs_bad;

	ptr = strtok(NULL, " ");
	if (check_digit(ptr, digits)) {		/* It is a float.. */
	    f = atof(ptr);
	    if (!mode)
		goto mon_deny;
	    // __svgalib_horizsync.max = f * 1000;
	} else {
	  hs_bad:
	    printf("svgalib: Illegal HorizSync setting.\n"
		   "Correct usage: HorizSync min_kHz max_kHz\n"
		   "Example: HorizSync 31.5 36.5\n");
	}
	break;
    case 44:			/* vertrefresh */
	ptr = strtok(NULL, " ");
	if (check_digit(ptr, digits)) {		/* It is a float.. */
	    f = atof(ptr);
	    if (!mode)
		goto mon_deny;
	    // __svgalib_vertrefresh.min = f;
	} else
	    goto vr_bad;

	ptr = strtok(NULL, " ");
	if (check_digit(ptr, digits)) {		/* It is a float.. */
	    f = atof(ptr);
	    if (!mode)
		goto mon_deny;
	    // __svgalib_vertrefresh.max = f;
	} else {
	  vr_bad:
	    printf("svgalib: Illegal VertRefresh setting.\n"
		   "Correct usage: VertRefresh min_Hz max_Hz\n"
		   "Example: VertRefresh 50 70\n");
	}
	break;
    case 45:{			/* modeline */
      //	    MonitorModeTiming mmt;
	    const struct {
		char *name;
		int val;
	    } options[7]; /* = { */
/* 		{ */
/* 		    "-hsync", NHSYNC */
/* 		}, */
/* 		{ */
/* 		    "+hsync", PHSYNC */
/* 		}, */
/* 		{ */
/* 		    "-vsync", NVSYNC */
/* 		}, */
/* 		{ */
/* 		    "+vsync", PVSYNC */
/* 		}, */
/* 		{ */
/* 		    "interlace", INTERLACED */
/* 		}, */
/* 		{ */
/* 		    "interlaced", INTERLACED */
/* 		}, */
/* 		{ */
/* 		    "doublescan", DOUBLESCAN */
/* 		} */
/* 	    }; */
#define ML_NR_OPTS (sizeof(options)/sizeof(*options))

	    /* Skip the name of the mode */
	    ptr = strtok(NULL, " ");
	    if (!ptr)
		break;

	    ptr = strtok(NULL, " ");
	    if (!ptr)
		break;
	    //	    mmt.pixelClock = atof(ptr) * 1000;

#define ML_GETINT(x) \
	ptr = strtok(NULL, " "); if(!ptr) break;
//	mmt.##x = atoi(ptr);

	    ML_GETINT(HDisplay);
	    ML_GETINT(HSyncStart);
	    ML_GETINT(HSyncEnd);
	    ML_GETINT(HTotal);
	    ML_GETINT(VDisplay);
	    ML_GETINT(VSyncStart);
	    ML_GETINT(VSyncEnd);
	    ML_GETINT(VTotal);
	    //	    mmt.flags = 0;
	    while ((ptr = strtok(NULL, " "))) {
		for (i = 0; i < ML_NR_OPTS; i++)
/* 		    if (!strcasecmp(ptr, options[i].name)) */
/* 			mmt.flags |= options[i].val; */
		if (i == ML_NR_OPTS)
		    break;
	    }
#undef ML_GETINT
#undef ML_NR_OPTS

	    return ptr;
	
	}
    case 46:
	if (!mode) {
	    puts("Security setting from environment denied.");
	    break;
	}
	if ( (ptr = strtok( NULL, " ")) ) {
	    if (!strcasecmp("revoke-all-privs", ptr)) {
	      //		 __svgalib_security_revokeallprivs = 1;
		 break;
	    } else if (!strcasecmp("compat", ptr)) {
	      //  __svgalib_security_revokeallprivs = 0;
		 break;
	    }
	}
	puts("svgalib: Unknown security options\n");
	break;
    case 47:
	ptr = strtok(NULL," ");
	if (ptr) {
	    mouse_device = strdup(ptr);
	    if (mouse_device == NULL) {
	      nomem:
		puts("svgalib: Fatal error: out of memory.");
		exit(1);
	    }
	} else
	    goto param_needed;
	break;
    case 48:		/* default_mode */
	if ( (ptr = strtok(NULL, " ")) != 0) {
	 int mode = vga_getmodenumber(ptr);
	  if (mode != -1) {
	    __svgalib_default_mode = mode;
	  } else {
	    printf("svgalib: config: illegal mode \'%s\' for \'%s\'\n",
	   			  ptr, vga_conf_commands[command]);
	  }
	} else {
  param_needed:
  	  printf("svgalib: config: \'%s\' requires parameter(s)",
  	  				vga_conf_commands[command]);
	  break;
	}
	break;
    case 49: /* nosigint */
      	__svgalib_nosigint = 1;
	break;
    case 50: /* sigint */
      	__svgalib_nosigint = 0;
	break;
    case 51: /* joystick0 */
    case 52: /* joystick1 */
    case 53: /* joystick2 */
    case 54: /* joystick3 */
	if (! (ptr = strtok(NULL, " ")) )
		goto param_needed;
#ifndef SVGA_AOUT
	if (__joystick_devicenames[command - 51])
	    free(__joystick_devicenames[command - 51]);
	__joystick_devicenames[command - 51] = strdup(ptr);
	if (!__joystick_devicenames[command - 51])
	    goto nomem;
#else
	printf("svgalib: No joystick support in a.out version.\n");
#endif
	break;
    case 55: /* TextProg */
	ptr = strtok(NULL," ");
        if(ptr==NULL)break;
/*         __svgalib_textprog|=2; */
/* 	__svgalib_TextProg = strdup(ptr); */
/* 	if (!__svgalib_TextProg) */
/* 	    goto nomem; */
        i=1;
        while(((ptr=strtok(NULL," "))!=NULL) /* && */
/* 	       (i< ((sizeof(__svgalib_TextProg_argv) / sizeof(char *)) + 1)) */ &&
	       strcmp(ptr,"END")){
/* 	   __svgalib_TextProg_argv[i]=strdup(ptr); */
/* 	   if (!__svgalib_TextProg_argv[i]) */
/* 	       goto nomem; */
	   i++;
        };
/*         __svgalib_TextProg_argv[i]=NULL; */
/*         ptb=strrchr(__svgalib_TextProg,'/'); */
/*         __svgalib_TextProg_argv[0]=ptb?ptb + 1:__svgalib_TextProg; */
/*        printf("%s\n",__svgalib_TextProg);
        for(j=0;j<i;j++)printf("%i:%s\n",j,__svgalib_TextProg_argv[j]);
*/        break;
    }
  leave:
    return strtok(NULL, " ");
}

void __svgalib_readconfigfile(void)
{
  static int configfileread = 0;
  if (configfileread)
    return;
  configfileread = 1;
  mouse_type = -1;
  __svgalib_read_options(vga_conf_commands, process_option);
  if (mouse_type == -1) {
    mouse_type = MOUSE_MICROSOFT;	/* Default. */
    puts("svgalib: Assuming Microsoft mouse.");
  }
}
