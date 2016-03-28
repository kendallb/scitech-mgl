/*
 * Main.c
 *
 * This file is part of the openGL-logo demo.
 * (c) Henk Kok (kok@wins.uva.nl)
 *
 * Copying, redistributing, etc is permitted as long as this copyright
 * notice and the Dutch variable names :) stay in tact.
 *
 * Modified for stereo display by Dave Milici, ironic@wenet.net,
 * from source code posted in GLUT public archive examples at www.OpenGL.org.
 *
 */

/* Define STEREO for stereo display support; otherwise default display. */
/* #define STEREO /* makefile option */

#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#ifdef STEREO
int stereo = 1;					/* stereo view on/off ? */
int viewpt = +1;				/* viewpt left = +1, center = 0, right = -1 */
GLdouble dxViewpt = 1.900;		/* viewpt perspective shift */
GLdouble dxAdjust = 0.010;		/* projection adjustment shift */
#endif

GLfloat lightpos[4] = { 1.0, 1.0, 1.0, 0.0 };
GLfloat lightamb[4] = { 0.3, 0.3, 0.3, 1.0 };
GLfloat lightdif[4] = { 0.8, 0.8, 0.8, 1.0 };
float speed=0, progress = 1;
void SetCamera(void);

extern void randomize(void);
extern void def_logo(void);
extern void draw_logo(void);

void do_display (void)
{
#ifdef STEREO
	/* enclose original rendering code in 2-pass loop */
	int i;
	for (i=0; i<2; i++)
	{
	/* select left, right, or default back buffer for drawing */
	glDrawBuffer(stereo ? ((i==0) ? GL_BACK_LEFT : GL_BACK_RIGHT) : GL_BACK);
	/* select left, right, or default central viewpt for camera */
	viewpt = (stereo) ? ((i==0) ? +1 : -1) : 0;
#endif

	SetCamera();
	draw_logo();
	glFlush();

#ifdef STEREO
	/* render twice for stereo, or only once for mono */
	if (!stereo)
		break;
	}
#endif

	glutSwapBuffers();
}

void display(void)
{
#ifdef STEREO
	/* select both left and right back buffers to be cleared in stereo mode */
	glDrawBuffer(GL_BACK);
#endif
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	do_display();
}

void myinit (void)
{
	glShadeModel (GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightamb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightdif);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glColor3f(1.0, 1.0, 1.0);
#ifdef STEREO
	/* select gray instead of black to reduce background contrast in stereo */
	glClearColor(0.2, 0.2, 0.2, 1.0);
#else
	glClearColor(0.0, 0.0, 0.0, 1.0);
#endif
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_NORMALIZE);
	def_logo();
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
}

/* ARGSUSED1 */
void parsekey(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27: glutExit(); break;
	case 13: break;
	case ' ': progress = 1; randomize(); break;
#ifdef STEREO
	/* update stereo parallax settings */
	case '+': dxViewpt += 0.100; break;
	case '-': dxViewpt -= 0.100; break;
	case '*': dxAdjust += 0.0001; break;
	case '/': dxAdjust -= 0.0001; break;
	case 's': stereo = (stereo) ? 0 : 1; break;
#endif
	}
}

/* ARGSUSED1 */
void parsekey_special(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:		break;
	case GLUT_KEY_DOWN:		break;
	case GLUT_KEY_RIGHT:	break;
	case GLUT_KEY_LEFT:		break;
	}
}

void Animate(void)
{
	speed = -0.95*speed + progress*0.05;
	if (progress > 0.0 && speed < 0.0003)
		speed = 0.0003;
	if (speed > 0.01)
		speed = 0.01;
	progress = progress - speed;
	if (progress < 0.0)
	{
		progress = 0.0;
		speed = 0;
	}
	glutPostRedisplay();
}

void myReshape(int w, int h)
{
	glMatrixMode (GL_MODELVIEW);
	glViewport (0, 0, w, h);
	glLoadIdentity();
	SetCamera();
}

void SetCamera(void)
{
#ifdef STEREO
	/* shift perspective projection leftward or rightward (or not at all) */
	GLdouble dx1 = (GLdouble)viewpt * dxViewpt;
	GLdouble dx2 = (GLdouble)viewpt * dxAdjust;
#endif

	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
#ifdef STEREO
	glFrustum (-0.1333+dx2, 0.1333+dx2, -0.1, 0.1, 0.2, 150.0);
	glTranslated (dx1, 0.0, 0.0);
#else
	glFrustum (-0.1333, 0.1333, -0.1, 0.1, 0.2, 150.0);
#endif
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0,1.5,2, 0,1.5,0, 0,1,0);
	glTranslatef(0.0, -8.0, -45.0);
	glRotatef(-progress*720, 0.0, 1.0, 0.0);
}

int main(int argc, char *argv[])
{
#ifdef STEREO
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGB | GLUT_DOUBLE | GLUT_STEREO | GLUT_MULTISAMPLE);
#else
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGB | GLUT_DOUBLE | GLUT_MULTISAMPLE);
#endif
	glutInitWindowPosition(200, 0);
	glutInitWindowSize(640, 480);
	glutCreateWindow("Rotating OpenGL Logo");
	glutDisplayFunc(display);
	glutKeyboardFunc(parsekey);
	glutSpecialFunc(parsekey_special);
	glutReshapeFunc(myReshape);
	glutIdleFunc(Animate);
	randomize();
	myinit();
	glutSwapBuffers();
	glutMainLoop();
	return 0;             /* ANSI C requires main to return int. */
}
