/* isosurf.c */

/*
 * Mesa 2.0 version
 *
 * Display an isosurface of 3-D wind speed volume.  Use arrow keys to
 * rotate, S toggles smooth shading, L toggles lighting
 * Brian Paul
 */




#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "GL/glut.h"


GLboolean speed_test = GL_FALSE;
GLboolean use_vertex_arrays = GL_FALSE;

GLboolean doubleBuffer = GL_TRUE;

GLboolean smooth = GL_TRUE;
GLboolean lighting = GL_TRUE;



#define MAXVERTS 10000

static GLfloat verts[MAXVERTS][3];
static GLfloat norms[MAXVERTS][3];
static GLint numverts;

static GLfloat xrot;
static GLfloat yrot;



static void read_surface( char *filename )
{
   FILE *f;

   f = fopen(filename,"r");
   if (!f) {
      printf("couldn't read %s\n", filename);
      exit(1);
   }

   numverts = 0;
   while (!feof(f) && numverts<MAXVERTS) {
      fscanf( f, "%f %f %f  %f %f %f",
	      &verts[numverts][0], &verts[numverts][1], &verts[numverts][2],
	      &norms[numverts][0], &norms[numverts][1], &norms[numverts][2] );
      numverts++;
   }
   numverts--;

   printf("%d vertices, %d triangles\n", numverts, numverts-2);
   fclose(f);
}



static void draw_surface( void )
{
   GLuint i;

#ifdef GL_EXT_vertex_array
   if (use_vertex_arrays) {
      glDrawArrays( GL_TRIANGLE_STRIP, 0, numverts );
   }
   else {
#endif
      glBegin( GL_TRIANGLE_STRIP );
      for (i=0;i<numverts;i++) {
         glNormal3fv( norms[i] );
         glVertex3fv( verts[i] );
      }
      glEnd();
#ifdef GL_EXT_vertex_array
   }
#endif
}



static void draw1(void)
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glPushMatrix();
    glRotatef( yrot, 0.0, 1.0, 0.0 );
    glRotatef( xrot, 1.0, 0.0, 0.0 );

    draw_surface();

    glPopMatrix();

    glFlush();
    if (doubleBuffer) {
		glutSwapBuffers();
		}
}


static void draw(void)
{
	if (speed_test) {
		for (xrot=0.0;xrot<=360.0;xrot+=10.0) {
			draw1();
			}
		glutExit();
		}
	else {
		draw1();
		}
}


static void InitMaterials(void)
{
    static float ambient[] = {0.1, 0.1, 0.1, 1.0};
    static float diffuse[] = {0.5, 1.0, 1.0, 1.0};
    static float position0[] = {0.0, 0.0, 20.0, 0.0};
    static float position1[] = {0.0, 0.0, -20.0, 0.0};
    static float front_mat_shininess[] = {60.0};
    static float front_mat_specular[] = {0.2, 0.2, 0.2, 1.0};
    static float front_mat_diffuse[] = {0.5, 0.28, 0.38, 1.0};
    /*
    static float back_mat_shininess[] = {60.0};
    static float back_mat_specular[] = {0.5, 0.5, 0.2, 1.0};
    static float back_mat_diffuse[] = {1.0, 1.0, 0.2, 1.0};
    */
    static float lmodel_ambient[] = {1.0, 1.0, 1.0, 1.0};
    static float lmodel_twoside[] = {GL_FALSE};

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, position0);
    glEnable(GL_LIGHT0);
    
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT1, GL_POSITION, position1);
    glEnable(GL_LIGHT1);
    
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
    glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);
    glEnable(GL_LIGHTING);

    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, front_mat_shininess);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, front_mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, front_mat_diffuse);
}


static void Init(void)
{
   glClearColor(0.0, 0.0, 0.0, 0.0);

   glShadeModel(GL_SMOOTH);
   glEnable(GL_DEPTH_TEST);

   InitMaterials();

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glFrustum( -1.0, 1.0, -1.0, 1.0, 5, 25 );

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glTranslatef( 0.0, 0.0, -6.0 );

#ifdef GL_EXT_vertex_array
   if (use_vertex_arrays) {
      glVertexPointer( 3, GL_FLOAT, 0, verts );
      glNormalPointer( GL_FLOAT, 0, norms );
      glEnable( GL_VERTEX_ARRAY_EXT );
      glEnable( GL_NORMAL_ARRAY_EXT );
   }
#endif
}



static void Reshape(int width, int height)
{
	glViewport(0, 0, (GLint)width, (GLint)height);
}

static void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
		case 's':
			smooth = !smooth;
			if (smooth)
				glShadeModel(GL_SMOOTH);
			else
				glShadeModel(GL_FLAT);
			break;
		case 'l':
			lighting = !lighting;
			if (lighting)
				glEnable(GL_LIGHTING);
			else
				glDisable(GL_LIGHTING);
			break;
		case '\x1B':
			glutExit();
			break;
		default:
			return;
		}
	glutPostRedisplay();
}

static void special(int key, int x, int y)
{
	switch (key) {
		case GLUT_KEY_LEFT:
			yrot -= 15.0;
			break;
		case GLUT_KEY_RIGHT:
			yrot += 15.0;
			break;
		case GLUT_KEY_UP:
			xrot += 15.0;
			break;
		case GLUT_KEY_DOWN:
			xrot -= 15.0;
			break;
		default:
			return;
		}
	glutPostRedisplay();
}


static GLenum Args(int argc, char **argv)
{
   GLint i;

   for (i = 1; i < argc; i++) {
      if (strcmp(argv[i], "-sb") == 0) {
         doubleBuffer = GL_FALSE;
      }
      else if (strcmp(argv[i], "-db") == 0) {
         doubleBuffer = GL_TRUE;
      }
      else if (strcmp(argv[i], "-speed") == 0) {
         speed_test = GL_TRUE;
         doubleBuffer = GL_TRUE;
      }
      else if (strcmp(argv[i], "-va") == 0) {
         use_vertex_arrays = GL_TRUE;
      }
      else {
         printf("%s (Bad option).\n", argv[i]);
         return GL_FALSE;
      }
   }

   return GL_TRUE;
}



void main(int argc, char **argv)
{
	GLenum type;
	char *extensions;

	read_surface( "isosurf.dat" );

	if (Args(argc, argv) == GL_FALSE) {
		exit(1);
		}

	glutInitWindowSize(400, 400);
	glutInit(&argc, argv);

	type = GLUT_DEPTH;
	type |= GLUT_RGB;
	type |= (doubleBuffer) ? GLUT_DOUBLE : GLUT_SINGLE;
	glutInitDisplayMode(type);
	glutCreateWindow("Isosurface");

	/* Make sure server supports the vertex array extension */
	extensions = (char *) glGetString( GL_EXTENSIONS );
	if (!strstr( extensions, "GL_EXT_vertex_array" )) {
		use_vertex_arrays = GL_FALSE;
		}

	Init();
	glutDisplayFunc(draw);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutReshapeFunc(Reshape);
	glutMainLoop();
}
