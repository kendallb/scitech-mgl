/****************************************************************************
*
*  ========================================================================
*
*   Copyright (C) 1991-2004 SciTech Software, Inc. All rights reserved.
*
*   This file may be distributed and/or modified under the terms of the
*   GNU General Public License version 2.0 as published by the Free
*   Software Foundation and appearing in the file LICENSE.GPL included
*   in the packaging of this file.
*
*   Licensees holding a valid Commercial License for this product from
*   SciTech Software, Inc. may use this file in accordance with the
*   Commercial License Agreement provided with the Software.
*
*   This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING
*   THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
*   PURPOSE.
*
*   See http://www.scitechsoft.com/license/ for information about
*   the licensing options available and how to purchase a Commercial
*   License Agreement.
*
*   Contact license@scitechsoft.com if any conditions of this licensing
*   are not clear to you, or you have questions about licensing options.
*
*  ========================================================================
*
* Description:  GLUT main implementation code for SGI's SkyFly
*				demonstration program.
*
*				Adapted from SciTech Game Framework version for use with
*				any GLUT implementation, including Mesa GLUT32, for testing
*				SciTech GLDirect OpenGL driver. (DaveM)
*
*				Modified for stereoscopic displays implementing
*				GL_STEREO quad-buffered page-flipping by Dave Milici.
*
****************************************************************************/

#ifndef NO_GL_MACROS
#define NO_GL_MACROS			/* insure static link with OpenGL lib */
#endif

#include "GL/glut.h"			/* insure correct INCLUDE path for GLUT */
#include "GL/gl.h"				/* insure correct INCLUDE path for GL */
#include "skyfly.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stereo.h"				/* stereo3d code additions */
#ifdef	__WINDOWS__
#undef	WINGDIAPI
#undef	APIENTRY
#undef	STRICT
#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

/*---------------------------- Global Variables ---------------------------*/

/* Is OpenGL hardware accelerator being used ? */
GLboolean   bHardwareAccelerator = GL_TRUE;

/* Navigational control automatic or manual ? */
int			_autopilot = GL_FALSE;

/* Palette for color-indexed mode, if supported */
typedef struct {
	GLfloat red;
	GLfloat green;
	GLfloat blue;
} palette_t;
palette_t pal[256];

#ifdef STEREO
/* Control vars for stereo3d display */
GLboolean bStereoRequested = GL_FALSE;
GLboolean bStereoEnabled = GL_FALSE;
/* Stereo3d parallax parameters */
GLfloat fDxViewpoint;
GLfloat fDxImageShift;
/* Stereo3d frustum parameters */
GLfloat fFrustumWidth2;
GLfloat fFrustumHeight2;
GLfloat fFrustumZnear;
GLfloat fFrustumZfar;
/* Stereo3d projection matrixes */
GLfloat mProjectionLeft[16];
GLfloat mProjectionRight[16];
#endif

/* Event handling globals used in fly.c */

static int buttons[BUTCOUNT];
static int mouse_x, mouse_y;

/* Functions in skyfly.c */

void init_misc(void);
void init_skyfly(void);
void sim_singlechannel(void);
void cull_proc(void);
void draw_proc(void);
void set_fog(int enable);
void set_dither(int enable);

#define SKY_R 0.23f
#define SKY_G 0.35f
#define SKY_B 0.78f

#define TERR_DARK_R 0.27f
#define TERR_DARK_G 0.18f
#define TERR_DARK_B 0.00f

#define TERR_LITE_R 0.24f
#define TERR_LITE_G 0.53f
#define TERR_LITE_B 0.05f

/*------------------------------ Implementation ---------------------------*/

int Xgetbutton(int button)
{
	int b;
	if (button < 0 || button >= BUTCOUNT)
		return -1;
	b = buttons[button];
	if (button < LEFTMOUSE)
		buttons[button] = 0;
	return b;
}

int Xgetvaluator(int val)
{
    switch (val) {
		case MOUSEX:
			return mouse_x;
		case MOUSEY:
			return mouse_y;
		default:
			return -1;
    	}
}

void setPaletteIndex(int i, GLfloat r, GLfloat g, GLfloat b)
{
	/* Load normalized GLfloats as-is for use with GLUT */
	pal[i].red 	= r;
	pal[i].green= g;
	pal[i].blue = b;
}

void init_cmap(void)
{
	int 		ii, jj, color;
    GLfloat 	r0, g0, b0, r1, g1, b1;

    /* Set up color map */
	color = 10;
	memset(pal,0,sizeof(pal));

    /* Sky colors */
    sky_base = color;
    r0 = SKY_R; r1 = 1.0f;
    g0 = SKY_G; g1 = 1.0f;
    b0 = SKY_B; b1 = 1.0f;
    for (ii = 0; ii < SKY_COLORS; ii++) {
        GLfloat p, r, g, b;
        p = (GLfloat) ii / (SKY_COLORS-1);
        r = r0 + p * (r1 - r0);
        g = g0 + p * (g1 - g0);
        b = b0 + p * (b1 - b0);
        for (jj = 0; jj < FOG_LEVELS; jj++) {
            GLfloat fp, fr, fg, fb;
            fp = (FOG_LEVELS > 1) ? (GLfloat) jj / (FOG_LEVELS-1) : 0.0f;
			fr = r + fp * (fog_params[0] - r);
			fg = g + fp * (fog_params[1] - g);
			fb = b + fp * (fog_params[2] - b);
			setPaletteIndex(sky_base + (ii*FOG_LEVELS) + jj, fr, fg, fb);
        }
    }
    color += (SKY_COLORS * FOG_LEVELS);

    /* Terrain colors */
    terr_base = color;
    r0 = TERR_DARK_R;   r1 = TERR_LITE_R;
    g0 = TERR_DARK_G;   g1 = TERR_LITE_G;
    b0 = TERR_DARK_B;   b1 = TERR_LITE_B;
    for (ii = 0; ii < TERR_COLORS; ii++) {
        GLfloat p, r, g, b;
        p = (GLfloat) ii / (TERR_COLORS-1);
        r = r0 + p * (r1 - r0);
        g = g0 + p * (g1 - g0);
        b = b0 + p * (b1 - b0);
        for (jj = 0; jj < FOG_LEVELS; jj++) {
            GLfloat fp, fr, fg, fb;
            fp = (FOG_LEVELS > 1) ? (GLfloat) jj / (FOG_LEVELS-1) : 0.0f;
			fr = r + fp * (fog_params[0] - r);
			fg = g + fp * (fog_params[1] - g);
			fb = b + fp * (fog_params[2] - b);
            setPaletteIndex(terr_base + (ii*FOG_LEVELS) + jj, fr, fg, fb);
        }
	}
    color += (TERR_COLORS * FOG_LEVELS);

    /* Plane colors */
    plane_colors[0] = color;
    plane_colors[1] = color + (PLANE_COLORS/2);
    plane_colors[2] = color + (PLANE_COLORS-1);
    r0 = 0.4; r1 = 0.8;
    g0 = 0.4; g1 = 0.8;
	b0 = 0.1; b1 = 0.1;
	for (ii = 0; ii < PLANE_COLORS; ii++) {
		GLfloat p, r, g, b;
		p = (GLfloat) ii / (PLANE_COLORS);
		r = r0 + p * (r1 - r0);
		g = g0 + p * (g1 - g0);
		b = b0 + p * (b1 - b0);
		setPaletteIndex(plane_colors[0] + ii, r, g, b);
	}
	color += PLANE_COLORS;

	/* Use GLUT to set palette */
	for (ii = 0; ii < 256; ii++) {
		glutSetColor(ii, pal[ii].red, pal[ii].green, pal[ii].blue);
		}
}

static GLboolean checkExtension(const char *name)
{
	const char *p = (const char *) glGetString(GL_EXTENSIONS);

	while (p = strstr(p, name)) {
		const char *q = p + strlen(name);

		if (*q == ' ' || *q == '\0') {
			return GL_TRUE;
		}
		p = q;
	}
	return GL_FALSE;
}

void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
}

void reinit(void)
{
	char  	title[256];
	int		i;

	rgbmode = glutGet(GLUT_WINDOW_RGBA);
	fog = !rgbmode;
	dither = !rgbmode;
	mipmap = GL_FALSE;
	if (bHardwareAccelerator) {
		/* Enable dithering and mipmapping for hardware 3D modes */
		dither = GL_TRUE;
		mipmap = GL_TRUE;
#if 0	// fog quality varies widely among OpenGL implementations
		fog = GL_TRUE;
#endif
		}

#ifdef  STEREO
	/* Check if GL_STEREO quad-buffered page-flipping enabled ? */
	glGetBooleanv(GL_STEREO, &bStereoEnabled);
#endif

	sprintf(title, "OpenGL SkyFly (%s %s %s)",
		glGetString(GL_VENDOR),
		glGetString(GL_RENDERER),
		glGetString(GL_VERSION));
	glutSetWindowTitle(title);

	Wxsize = glutGet(GLUT_WINDOW_WIDTH);
	Wysize = glutGet(GLUT_WINDOW_HEIGHT);
	glViewport(0,0,Wxsize,Wysize);

	/* Init keyboard and mouse event vars */
	for(i = 0; i < BUTCOUNT; i++)
		buttons[i] = 0;
	mouse_x = Wxsize * 0.4;
	mouse_y = Wysize/2;
	init_misc();
	if (!rgbmode)
		init_cmap();
	init_skyfly();

	/* Set default mouse position */
	mouse_x = Wxsize * 0.1;
	mouse_y = Wysize/2;
}

void gameLogic(void)
{
	sim_singlechannel();
	glutPostRedisplay();
}

void draw(void)
{
#ifdef 	STEREO
	int 	i;

	/* enclose original rendering code in 2-pass loop */
	for (i=0; i<2; i++)
	{
	/* adjust projection matrix for stereo viewpoint */
	if (bStereoEnabled) {
		glDrawBuffer((i==0) ? GL_BACK_LEFT : GL_BACK_RIGHT);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadMatrixf((i==0) ? &mProjectionLeft[0] : &mProjectionRight[0]);
		glMatrixMode(GL_MODELVIEW);
		}
#endif

	/* Draw the frame */
	cull_proc();
	draw_proc();

#ifdef 	STEREO
	/* render twice for stereo, or once for mono */
	if (bStereoEnabled) {
		glFlush();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		}
	else
		break;
	} /* for loop */
#endif

	/* Swap the display buffers */
	glutSwapBuffers();
}

extern int _frame;
extern float Speed;
void init_positions(void);

void extkeydown(int key, int x, int y)
{
	switch (key) {
		case GLUT_KEY_LEFT:
			buttons[LEFTARROWKEY] = 1;
			break;
		case GLUT_KEY_RIGHT:
			buttons[RIGHTARROWKEY] = 1;
			break;
		case GLUT_KEY_UP:
			buttons[UPARROWKEY] = 1;
			break;
		case GLUT_KEY_DOWN:
			buttons[DOWNARROWKEY] = 1;
			break;
		case GLUT_KEY_PAGE_DOWN:
			buttons[PAGEDOWNKEY] = 1;
			break;
		case GLUT_KEY_PAGE_UP:
			buttons[PAGEUPKEY] = 1;
			break;
		}
}

void keydown(unsigned char key, int x, int y)
{
			switch (key) {
				case 0x1B:
					glutExit();
					break;
				case 'r':
					buttons[RKEY] = 1;
					break;
				case '.':
					buttons[PERIODKEY] = 1;
					break;
				case ' ':
					buttons[SPACEKEY] = 1;
					break;
				case 'f':
					set_fog(!fog);
					break;
				case 'd':
					set_dither(!dither);
					break;
				case 'v':
					/* VSync toggle not supported */
					break;
				case 'a':
					_autopilot = !_autopilot;
					_frame = 0;
					mouse_x = Wxsize * 0.1;
					mouse_y = Wysize/2;
					init_positions();
					if (_autopilot) {
						Speed = 1.0f;
						}
					else {
						Speed = 0.3f;
						}
					break;
#ifdef 	STEREO	/* adjust stereo3d parallax settings */
				case '+':
					fDxViewpoint += 0.01;
					calcStereoMatrix();
					break;
				case '-':
					fDxViewpoint -= 0.01;
					if (fDxViewpoint < 0.0)
						fDxViewpoint = 0.0;
					calcStereoMatrix();
					break;
				case '*':
					fDxImageShift += 0.0005;
					calcStereoMatrix();
					break;
				case '/':
					fDxImageShift -= 0.0005;
					calcStereoMatrix();
					break;
#endif
				}
}

void mousedown(int button, int state, int x, int y)
{
	if (_autopilot) {
		mouse_x = Wxsize * 0.1;
		mouse_y = Wysize/2;
		}
	else {
		mouse_x = x;
		mouse_y = y;
		}

	if (button == GLUT_LEFT_BUTTON)
		buttons[LEFTMOUSE] = (state == GLUT_DOWN) ? 1 : 0;
	if (button == GLUT_RIGHT_BUTTON)
		buttons[RIGHTMOUSE] = (state == GLUT_DOWN) ? 1 : 0;
}

void mousemove(int x, int y)
{
	if (_autopilot) {
		mouse_x = Wxsize * 0.1;
		mouse_y = Wysize/2;
		}
	else {
		mouse_x = x;
		mouse_y = y;
		}
}

int main(int argc, char *argv[])
{
	int	xRes = 640,yRes = 480,bits = 16;
	int mode = GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH;

#ifdef STEREO
#ifdef STEREO_DEFAULT
	mode |= GLUT_STEREO;
	bStereoRequested = GL_TRUE;
#endif
#endif

	glutInit(&argc, &*argv);
	glutInitDisplayMode(mode);
	glutInitWindowSize(xRes, yRes);
	glutCreateWindow("OpenGL SkyFly");
	glutDisplayFunc(draw);
	glutIdleFunc(gameLogic);
	glutKeyboardFunc(keydown);
	glutSpecialFunc(extkeydown);
	glutMouseFunc(mousedown);
	glutMotionFunc(mousemove);
	glutReshapeFunc(reshape);

	reinit();
	glutMainLoop();

	return 0;             /* ANSI C requires main to return int. */
}
