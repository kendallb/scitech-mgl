/*
 * Morph - Morphing polyhedron
 *
 * History summary:
 * Originally based on Morph3D demo.
 * Modified for Win32 stereo support for GloriaXL board by ELSA.
 * Improved stereoscopic display effect by Bob Akka, StereoGraphics.
 * Re-adapted for GLUT framework by Dave Milici, Ironic Research Labs.
 *
 */

/* Define STEREO for stereoscopic display support */
/* #define STEREO /* makefile option */

/* ##### INCLUDES 	*/

#include <gl\glut.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* ##### DEFINES 	*/

#define XVAL 0.525731112119133606f
#define ZVAL 0.850650808352039932f

#define MORPH_MIN 0.25f
#define MORPH_MAX 1.5f

#define EYE_OFFSET  0.08f  /* default viewpoint separation */
#define EYE_ADJUST -0.04f  /* default horizontal image shift adjustment */

#define PULL_BACK 2.2f     /* z distance from object center */

/* ##### TYPEDEFS	*/

#ifndef POINT
typedef struct {int x, y;} _POINT;
#endif

/* ##### VARIABLES */

/* App run-time data */

GLboolean bStereo = GL_FALSE;
GLboolean bRotation = GL_TRUE;

GLfloat eyeOffset = EYE_OFFSET;
GLfloat eyeAdjust = EYE_ADJUST;

int nWidth = 480;
int nHeight = 360;
float aspectViewport = 1.0f;

int bLButtonDown = GL_FALSE;
int bRButtonDown = GL_FALSE;

_POINT ptLast;
_POINT ptCurrent;

GLfloat   angleX = 0.0f;
GLfloat   angleY = 0.0f;
GLfloat   angleZ = 0.0f;
GLfloat   translateZ = 0.0f;
GLfloat   incZ = 0.005f;

/* Object data */

GLfloat   factorMorph = 1.0;
GLfloat   incMorph    = 0.015f;

static GLfloat vIco[12][3] =
{
  {  -XVAL, 0.0f,    ZVAL},
  {   XVAL, 0.0f,    ZVAL},
  {  -XVAL, 0.0f,   -ZVAL},
  {   XVAL, 0.0f,   -ZVAL},
  {0.0f,    ZVAL,    XVAL},
  {0.0f,    ZVAL,   -XVAL},
  {0.0f,   -ZVAL,    XVAL},
  {0.0f,   -ZVAL,   -XVAL},
  {   ZVAL,    XVAL, 0.0f},
  {  -ZVAL,    XVAL, 0.0f},
  {   ZVAL,   -XVAL, 0.0f},
  {  -ZVAL,   -XVAL, 0.0f}
};

static int idxIco[20][3] =
{
  { 0,  1,  4}, /*  0 */
  { 0,  4,  9}, /*  1 */
  { 0,  9, 11}, /*  2 */
  { 0,  6,  1}, /*  3 */
  { 0, 11,  6}, /*  4 */
  { 1,  6, 10}, /*  5 */
  { 1, 10,  8}, /*  6 */
  { 1,  8,  4}, /*  7 */
  { 2,  3,  7}, /*  8 */
  { 2,  5,  3}, /*  9 */
  { 2,  9,  5}, /* 10 */
  { 2, 11,  9}, /* 11 */
  { 2,  7, 11}, /* 12 */
  { 3,  5,  8}, /* 13 */
  { 3,  8, 10}, /* 14 */
  { 3, 10,  7}, /* 15 */
  { 4,  5,  9}, /* 16 */
  { 4,  8,  5}, /* 17 */
  { 6,  7, 10}, /* 18 */
  { 6, 11,  7}  /* 19 */
};

static GLfloat vDode[20][3];

static int idxDode[12][5] =
{
  { 0,  1,  2,  4,  3}, /*  0 */
  { 0,  3,  5,  6,  7}, /*  1 */
  { 9,  8, 12, 11, 10}, /*  2 */
  { 9, 13, 14, 15,  8}, /*  3 */
  { 0,  7, 17, 16,  1}, /*  4 */
  { 9, 10, 16, 17, 13}, /*  5 */
  {18,  5,  3,  4, 19}, /*  6 */
  {15, 18, 19, 12,  8}, /*  7 */
  { 6, 14, 13, 17,  7}, /*  8 */
  { 1, 16, 10, 11,  2}, /*  9 */
  { 5, 18, 15, 14,  6}, /* 10 */
  { 2, 11, 12, 19,  4}  /* 11 */
};

/* #################### PROTOTYPES 	*/

void Init(void);
void Display(void);
void ReshapeHandler(int w, int h);

void MaterialCreate(void);
void LightCreate(void);
void Icosahedron(void);

void InitDodecahedron(void);
void Dodecahedron(void);
void DodecahedronMorph(void);

/* #################### FUNCTIONS 	*/

void TriangleNormalVector(GLfloat a[3], GLfloat b[3], GLfloat c[3], GLfloat n[3])
{
  int i;
  GLfloat d;
  GLfloat v1[3];
  GLfloat v2[3];

  for (i = 0; i < 3; i++)
  {
	v1[i] =  a[i] - b[i];
	v2[i] =  b[i] - c[i];
  }

  n[0] = v1[1] * v2[2] - v1[2] * v2[1];
  n[1] = v1[2] * v2[0] - v1[0] * v2[2];
  n[2] = v1[0] * v2[1] - v1[1] * v2[0];

  d = (GLfloat) sqrt(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
  if (fabs(d) > 1.0e-8)
  {
    d = 1.0f / d;
    n[0] *= d;
	n[1] *= d;
    n[2] *= d;
  }
}

void Icosahedron(void)
{
  int i;
  GLfloat norm[3];

  glNewList(1, GL_COMPILE);
    glBegin(GL_TRIANGLES);
	for (i = 0; i < 20; i++)
	{
	  TriangleNormalVector(vIco[idxIco[i][0]], vIco[idxIco[i][1]], vIco[idxIco[i][2]], norm);
      glNormal3fv(norm);
      glVertex3fv(vIco[idxIco[i][0]]);
      glVertex3fv(vIco[idxIco[i][1]]);
	  glVertex3fv(vIco[idxIco[i][2]]);
    }
    glEnd();
  glEndList();
}

void InitDodecahedron(void)
{
  int     i;
  GLfloat n[3];
  GLfloat d;

  for (i = 0; i < 20; i++)
  {
    n[0] = vIco[idxIco[i][0]][0] + vIco[idxIco[i][1]][0] + vIco[idxIco[i][2]][0];
	n[1] = vIco[idxIco[i][0]][1] + vIco[idxIco[i][1]][1] + vIco[idxIco[i][2]][1];
    n[2] = vIco[idxIco[i][0]][2] + vIco[idxIco[i][1]][2] + vIco[idxIco[i][2]][2];

    d = (GLfloat) sqrt(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
    if (fabs(d) > 1.0e-8)
    {
      d = 1.0f / d;
      n[0] *= d;
      n[1] *= d;
      n[2] *= d;
    }
    vDode[i][0] = n[0];
    vDode[i][1] = n[1];
    vDode[i][2] = n[2];
  }
}

void Dodecahedron(void)
{
  int i;

  for (i = 0; i < 12; i++)
  {
	glBegin(GL_POLYGON);
	  glNormal3fv(vIco[i]);                /* Icosahedron coordinate is exactly the normal vector. */
      glVertex3fv(vDode[idxDode[i][0]]);
      glVertex3fv(vDode[idxDode[i][1]]);
      glVertex3fv(vDode[idxDode[i][2]]);
	  glVertex3fv(vDode[idxDode[i][3]]);
      glVertex3fv(vDode[idxDode[i][4]]);
    glEnd();
  }
}

void DodecahedronMorph(void)
{
  int i;
  GLfloat v[3];
  GLfloat norm[3];

  for (i = 0; i < 12; i++)
  {
    glBegin(GL_TRIANGLE_FAN);
      v[0] = factorMorph * vIco[i][0];
	  v[1] = factorMorph * vIco[i][1];
      v[2] = factorMorph * vIco[i][2];
      TriangleNormalVector(v, vDode[idxDode[i][0]], vDode[idxDode[i][1]], norm);
      glNormal3fv(norm);
      glVertex3fv(v); /* center */
      glVertex3fv(vDode[idxDode[i][0]]);
      glVertex3fv(vDode[idxDode[i][1]]);
      TriangleNormalVector(v, vDode[idxDode[i][1]], vDode[idxDode[i][2]], norm);
      glNormal3fv(norm);
      glVertex3fv(vDode[idxDode[i][2]]);
      TriangleNormalVector(v, vDode[idxDode[i][2]], vDode[idxDode[i][3]], norm);
      glNormal3fv(norm);
      glVertex3fv(vDode[idxDode[i][3]]);
      TriangleNormalVector(v, vDode[idxDode[i][3]], vDode[idxDode[i][4]], norm);
      glNormal3fv(norm);
      glVertex3fv(vDode[idxDode[i][4]]);
      TriangleNormalVector(v, vDode[idxDode[i][4]], vDode[idxDode[i][0]], norm);
      glNormal3fv(norm);
      glVertex3fv(vDode[idxDode[i][0]]);
    glEnd();
  }
}

void MaterialCreate(void)
{
  GLfloat ambientGold[]  = {0.1f, 0.05f, 0.0f, 1.0f};
  GLfloat diffuseGold[]  = {0.65f, 0.55f, 0.15f, 1.0f};
  GLfloat specularGold[] = {0.85f, 0.75f, 0.45f, 1.0f};

  GLfloat ambientSilver[]  = {0.1f, 0.1f, 0.1f, 1.0f};
  GLfloat diffuseSilver[]  = {0.6f, 0.6f, 0.6f, 1.0f};
  GLfloat specularSilver[] = {0.9f, 0.9f, 0.9f, 1.0f};

  GLfloat shininess = 100.0f;

  glMaterialfv(GL_FRONT, GL_AMBIENT, ambientGold);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseGold);
  glMaterialfv(GL_FRONT, GL_SPECULAR, specularGold);
  glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

/* Create directional light */
void LightCreate(void)
{
  GLfloat light0_ambient[]  = {1.0f, 1.0f, 1.0f, 1.0f};
  GLfloat light0_diffuse[]  = {1.0f, 1.0f, 1.0f, 1.0f};
  GLfloat light0_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
  GLfloat light0_position[] = {-5.0f, 5.0f, 5.0f, 1.0f};

  GLfloat light1_ambient[]  = {0.5f, 0.5f, 0.5f, 1.0f};
  GLfloat light1_diffuse[]  = {0.5f, 0.5f, 0.5f, 1.0f};
  GLfloat light1_specular[] = {0.5f, 0.5f, 0.5f, 1.0f};
  GLfloat light1_position[] = {5.0f, 0.0f, 0.0f, 1.0f};

  glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
  glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

  glLightfv(GL_LIGHT1, GL_AMBIENT, light0_ambient);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, light0_diffuse);
  glLightfv(GL_LIGHT1, GL_SPECULAR, light0_specular);
  glLightfv(GL_LIGHT1, GL_POSITION, light1_position);

  glEnable(GL_LIGHTING);

  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);
}

void Init(void)
{
#ifdef STEREO
  /* Query stereo support */
  glGetBooleanv(GL_STEREO, &bStereo);

  /* Changed from black (to reduce ghosting) by Akka */
  glClearColor(0.3f, 0.3f, 0.3f, 0.3f);
#endif

  LightCreate();
  Icosahedron();
  MaterialCreate();

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  glFrontFace(GL_CCW);  /* default */
  glCullFace(GL_BACK);  /* default */
  glEnable(GL_CULL_FACE);
  glShadeModel(GL_FLAT);

  InitDodecahedron();
}

/* GLUT framework functions: */

void Display(void)
{
	float frustumAdjust = 0.0f;
	int i;

	/* Clear both back buffers in one step. */
	glDrawBuffer(GL_BACK);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#ifdef STEREO
	/* Enclosed original rendering code in 2-pass loop. (DaveM) */
	/* We will render twice for stereo, or only once for mono. */
	for (i=0; i<2; i++)
	{
		/* Draw the image for the left or right eye. */
		if (bStereo)
			glDrawBuffer(i ? GL_BACK_RIGHT : GL_BACK_LEFT);
#endif

		/* Setup the projection matrix for the perspective view. */
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

#ifdef STEREO
		/* Shift the frustum left or right (stereo), or center (mono). */
        /* Frustum adjustment calculation added by Akka. */
        if (bStereo)
            frustumAdjust = (i)
               ? -eyeAdjust * eyeOffset / EYE_OFFSET
               : eyeAdjust * eyeOffset / EYE_OFFSET;
#endif
		glFrustum(-aspectViewport * 0.75 - frustumAdjust,
			 aspectViewport * 0.75 - frustumAdjust,
			-0.75, 0.75, 0.65, 4.0);

#ifdef STEREO
		/* Shift the viewpoint left or right (stereo only). */
		if (bStereo)
			glTranslatef((i ? -eyeOffset : eyeOffset), 0.0f, 0.0f);
#endif
		glTranslatef(0.0f, 0.0f, -PULL_BACK);

		/* Setup the transformation matrix for the object. */
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glRotatef(angleZ, 0.0f, 0.0f, 1.0f);
		glRotatef(angleY, 0.0f, 1.0f, 0.0f);
		glRotatef(angleX, 1.0f, 0.0f, 0.0f);
		glTranslatef(0.0f, 0.0f, translateZ);

        /* Draw the object with in the current morph state */
		DodecahedronMorph();
		glCallList(1);

		/* Render GL commands for selected draw buffer. */
		glFlush();

#ifdef STEREO
		/* Render only once if mono view. */
		if (!bStereo)
			break;
	}
#endif

	/* Update stereo buffers for display. */
	glutSwapBuffers();
}

void UpdateHandler(void)
{
  /* Update morph parameters */
  if (bRotation)  {
	  angleX += 0.6f;
	  if (angleX > 360.0f)
		angleX -= 360.0f;
		  angleY += 1.0f;
	  if (angleY > 360.0f)
		angleY -= 360.0f;
		  angleZ += 0.4f;
	  if (angleZ > 360.0f)
		angleZ -= 360.0f;
		  translateZ += incZ;
	  if (translateZ > 0.5f)
	  {
		translateZ = 0.5f;
		incZ = -incZ;
	  }
	  else if (translateZ < -0.5f)
	  {
		translateZ = -0.5f;
		incZ = -incZ;
	  }
	  factorMorph += incMorph;
	  if (factorMorph > MORPH_MAX)
	  {
		factorMorph = MORPH_MAX;
		incMorph = -incMorph;
	  }
	  else if (factorMorph < MORPH_MIN)
	  {
		factorMorph = MORPH_MIN;
		incMorph = -incMorph;
	  }
  }
  glutPostRedisplay();
}

void ReshapeHandler(int w, int h)
{
  /* Update viewport parameters */
  nWidth = w;
  nHeight = h;
  glViewport(0, 0, nWidth, nHeight);
  if (nHeight > 0)
	aspectViewport = (float) nWidth / (float) nHeight;
  else
	aspectViewport = 1.0f;
  glutPostRedisplay();
}

void KeyDownHandler(unsigned char key, int x, int y)
{
  /* Handle keyboard commands */
  switch (key)
  {
	case 0x1B:
      glutExit();
	  break;

	case '1':
	  glEnable(GL_CULL_FACE);
	  break;

	case '2':
	  glDisable(GL_CULL_FACE);
	  break;

	case ' ':
	  bRotation = !bRotation;
      glutPostRedisplay();
	  break;

#ifdef STEREO
	case 's':
	  bStereo = !bStereo;
      glutPostRedisplay();
	  break;

	case '\r':
	  eyeAdjust = EYE_ADJUST;
	  eyeOffset = EYE_OFFSET;
	  glutPostRedisplay();
	  break;

    case '+':
      eyeOffset += 0.01f;
      glutPostRedisplay();
      break;

    case '-':
      eyeOffset -= 0.01f;
	  if (eyeOffset < 0.0f)
		  eyeOffset = 0.0f;
      glutPostRedisplay();
      break;

    case '*':
      eyeAdjust += 0.001f;
	  if (eyeAdjust > 0.0f)
		  eyeAdjust = 0.0f;
      glutPostRedisplay();
      break;

    case '/':
      eyeAdjust -= 0.001f;
      glutPostRedisplay();
      break;
#endif
  }
}

void MouseDownHandler(int button, int state, int x, int y)
{
  /* Handle mouse buttons */
  if (button == GLUT_LEFT_BUTTON)
	bLButtonDown = (state == GLUT_DOWN) ? GL_TRUE : GL_FALSE;
  if (button == GLUT_RIGHT_BUTTON)
	bRButtonDown = (state == GLUT_DOWN) ? GL_TRUE : GL_FALSE;

  if (bLButtonDown || bRButtonDown) {
	ptLast.x = x;
	ptLast.y = y;
  }
}

void MouseMoveHandler(int x, int y)
{
  int offset;

  /* Handle mouse movement while dragging */
  if (bLButtonDown)
  {
	ptCurrent.x = x;
	ptCurrent.y = y;
	offset = ptCurrent.x - ptLast.x;
#ifdef STEREO
    /* Update stereo parallax parameters */
	if (nWidth > 0)
	{
	  /* changed by Akka; originally += 0.2f ... */
	  eyeOffset += 0.1f * (float) offset / (float) nWidth;
	  if (eyeOffset < 0.0f)
		  eyeOffset = 0.0f;
	  if (eyeOffset > EYE_OFFSET * 4)
		  eyeOffset = EYE_OFFSET * 4;
	}
#endif
	ptLast = ptCurrent;
	glutPostRedisplay();
  }
  else if (bRButtonDown)
  {
	ptCurrent.x = x;
	ptCurrent.y = y;
	offset = ptCurrent.x - ptLast.x;
#ifdef STEREO
	if (nWidth > 0)
	{
	  /* changed by Akka; originally += 0.2f ... */
	  eyeAdjust += 0.02f * (float) offset / (float) nWidth;
	  if (eyeAdjust > 0.0f)
		  eyeAdjust = 0.0f;
	  if (eyeAdjust < EYE_ADJUST * 2)
		  eyeAdjust = EYE_ADJUST * 2;
	}
#endif
	ptLast = ptCurrent;
	glutPostRedisplay();
  }
}

int main(int argc, char *argv[])
{
	glutInit(&argc, &*argv);
#ifdef STEREO
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_STEREO | GLUT_DEPTH);
#else
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#endif
	glutInitWindowSize(480, 360);
	glutCreateWindow("Morph");

	glutDisplayFunc(Display);
	glutIdleFunc(UpdateHandler);
	glutKeyboardFunc(KeyDownHandler);
	glutMouseFunc(MouseDownHandler);
	glutMotionFunc(MouseMoveHandler);
	glutReshapeFunc(ReshapeHandler);

	Init();
	glutMainLoop();

	return 0;             /* ANSI C requires main to return int. */
}
