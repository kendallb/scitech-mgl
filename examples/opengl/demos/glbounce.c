/* bounce.c */


/*
 * Bouncing ball demo.
 *
 * Brian Paul
 */



#include <math.h>
#include <stdio.h>
#include "GL/glut.h"
#include "mgraph.h"

#define COS(X)   cos( (X) * 3.14159/180.0 )
#define SIN(X)   sin( (X) * 3.14159/180.0 )


GLuint Ball;
GLenum Mode;
GLfloat Zrot = 0.0, Zstep = 4.0;
GLfloat Xpos = 0.0, Ypos = 1.0;
GLfloat Xvel = 0.1, Yvel = 0.0;
GLfloat Xmin=-4.0, Xmax=4.0;
GLfloat Ymin=-3.8, Ymax=4.0;
GLfloat G = -0.05;



static GLuint make_ball( void )
{
   GLuint list;
   GLfloat a, b;
   GLfloat da = 18.0, db = 18.0;
   GLfloat radius = 1.0;
   GLuint color;
   GLfloat x, y, z;

   list = glGenLists( 1 );

   glNewList( list, GL_COMPILE );

   color = 0;
   for (a=-90.0;a+da<=90.0;a+=da) {

      glBegin( GL_QUAD_STRIP );
      for (b=0.0;b<=360.0;b+=db) {

	 if (color) {
		glIndexf(MGL_RED);
	 }
	 else {
	    glIndexf(MGL_WHITE );
	 }

	 x = COS(b) * COS(a);
	 y = SIN(b) * COS(a);
	 z = SIN(a);
	 glVertex3f( x, y, z );

	 x = radius * COS(b) * COS(a+da);
	 y = radius * SIN(b) * COS(a+da);
	 z = radius * SIN(a+da);
	 glVertex3f( x, y, z );

	 color = 1-color;
      }
      glEnd();

   }

   glEndList();

   return list;
}



static void reshape( int width, int height )
{
   glViewport(0, 0, (GLint)width, (GLint)height);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho( -6.0, 6.0, -6.0, 6.0, -6.0, 6.0 );
   glMatrixMode(GL_MODELVIEW);
}


static void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
		case '\x1B':
			glutExit();
			break;
		default:
			return;
		}
}

static void draw( void )
{
   GLint i;

   glClear( GL_COLOR_BUFFER_BIT );
   glIndexf( MGL_CYAN );
   glBegin( GL_LINES );
   for (i=-5;i<=5;i++) {
      glVertex2i( i, -5 );   glVertex2i( i, 5 );
   }
   for (i=-5;i<=5;i++) {
      glVertex2i( -5,i );   glVertex2i( 5,i );
   }
   for (i=-5;i<=5;i++) {
      glVertex2i( i, -5 );  glVertex2f( i*1.15, -5.9 );
   }
   glVertex2f( -5.3, -5.35 );    glVertex2f( 5.3, -5.35 );
   glVertex2f( -5.75, -5.9 );     glVertex2f( 5.75, -5.9 );
   glEnd();


   glPushMatrix();
   glTranslatef( Xpos, Ypos, 0.0 );
   glScalef( 2.0, 2.0, 2.0 );
   glRotatef( 8.0, 0.0, 0.0, 1.0 );
   glRotatef( 90.0, 1.0, 0.0, 0.0 );
   glRotatef( Zrot, 0.0, 0.0, 1.0 );


   glCallList( Ball );

   glPopMatrix();

   glFlush();
   glutSwapBuffers();
}


static void idle( void )
{
   static float vel0 = -100.0;

   Zrot += Zstep;

   Xpos += Xvel;
   if (Xpos>=Xmax) {
      Xpos = Xmax;
      Xvel = -Xvel;
      Zstep = -Zstep;
   }
   if (Xpos<=Xmin) {
      Xpos = Xmin;
      Xvel = -Xvel;
      Zstep = -Zstep;
   }

   Ypos += Yvel;
   Yvel += G;
   if (Ypos<Ymin) {
      Ypos = Ymin;
      if (vel0==-100.0)  vel0 = fabs(Yvel);
      Yvel = vel0;
   }
   draw();
}



int main( int argc, char *argv[] )
{

	glutInitWindowSize(300, 300);
  	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_INDEX | GLUT_DOUBLE);
	glutCreateWindow("Bounce");

	Ball = make_ball();
	glCullFace( GL_BACK );
	glEnable( GL_CULL_FACE );
	glDisable( GL_DITHER );
	glShadeModel( GL_FLAT );
	glutDisplayFunc(draw);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	glutMainLoop();
	return 0;
}

