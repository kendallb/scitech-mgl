/****************************************************************************
*
*                    SciTech SNAP Graphics Architecture
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
* Language:     ANSI C
* Environment:  Any 32-bit protected mode environment
*
* Description:  Module to implement the moth OpenGL sample program.
*               Original code written by Robert Doyle, Naval Research
*               Laboratory, Washington, DC. Scene objects are built into
*               display lists in the 'myInit' function (look for three rows
*               of I's). Objects are assembled and motion described in the
*               'display' function (look for three rows of $'s).
*
****************************************************************************/

#include "gatest.h"

/*---------------------------- Global Variables ---------------------------*/

static GLfloat wall_color[] = {1.0, 1.0, 1.0, 1.0};
static GLfloat wall_ambient[] = {0.5, 0.5, 0.5, 1.0};
static GLfloat floor_color[] = {0.5, 1.0, 0.5, 0.5};
static GLfloat column_color[] = {1.0, 0.0, 0.0, 1.0};
static GLfloat column_ambient[] = {0.25, 0.0, 0.0, 1.0};

static GLfloat panel_color[] = {0.1, 0.1, 1.0, 1.0};
static GLfloat panel_ambient[] = {0.01, 0.01, 0.3, 1.0};

static GLfloat lamp_ambient[] = {1.0, 1.0, 1.0, 1.0};
static GLfloat lamp_diffuse[] = {1.0, 1.0, 1.0, 1.0};
static GLfloat lamp_specular[] = {1.0, 1.0, 1.0, 1.0};
static GLfloat lamp_post_diffuse[] = {0.8, 0.0, 0.0, 1.0};
static GLfloat lamp_post_specular[] = {0.8, 0.0, 0.0, 1.0};
static GLfloat lamp_post_ambient[] = {0.25, 0.0, 0.0, 1.0};

static GLfloat satellite_diffuse[] = {1.0, 0.69, 0.0, 1.0};
static GLfloat satellite_shiny[] = {128.0};
static GLfloat satellite_specular[] = {1.0, 1.0, 1.0, 1.0};
static GLfloat satellite_ambient[] = {0.37, 0.25, 0.0, 1.0};

static GLfloat cube_specular[] = {0.9, 0.9, 0.9, 1.0};
static GLfloat cube_ambient[] = {0.1, 0.1, 0.1, 1.0};

static GLfloat shadow_ambient[] = {0.0, 0.0, 0.0, 1.0};
static GLfloat shadow_diffuse[] = {0.0, 0.0, 0.0, 0.3};
static GLfloat shadow_shiny[] = {0.0};
static GLfloat shadow_specular[] = {0.0, 0.0, 0.0, 1.0};

static GLuint column = 3;
static GLuint ground = 30;
static GLuint left_wall = 40;
static GLuint right_wall = 50;
static GLuint four_columns = 7;
static GLuint two_columns = 32;
static GLuint satellite1 = 301;
static GLuint satellite2 = 302;
static GLuint panel1 = 303;
static GLuint panel2 = 304;

static GLfloat Tx = -0.01 * 5;
static GLfloat Ty = -0.01 * 5;
static GLfloat Tz = -0.02 * 5;
static GLfloat mvt_x = -15.0;
static GLfloat mvt_y = -15.0;
static GLfloat mvt_z = -30.7;

static GLfloat Rx = 0.1 * 5;
static GLfloat mvr_d = 150.0;
static GLfloat mvr_x = 1.0;
static GLfloat mvr_y = -1.0;
static GLfloat mvr_z = -1.0;

static GLfloat cubeXform[4][4];
static GLfloat four_columnsXform[4][4];

static float shadowMat_ground[4][4];
static float shadowMat_left[4][4];
static float shadowMat_back[4][4];
static float shadowMat_column[4][4];
static float shadowMat_right[4][4];

static float shadowMat1_ground[4][4];
static float shadowMat1_left[4][4];
static float shadowMat1_back[4][4];
static float shadowMat1_right[4][4];

static int tick = -1;

static float lightPos[4] = {1.0, 2.5, 3.0, 1.0};

static float light1Pos[4] = {0.0, 1.6, -5.0, 1.0};
static float light1Amb[4] = {1.0, 1.0, 1.0, 1.0};
static float light1Diff[4] = {1.0, 1.0, 1.0, 1.0};
static float light1Spec[4] = {1.0, 1.0, 1.0, 1.0};

static float leftPlane[4] = {1.0, 0.0, 0.0, 4.88}; /* X = -4.88 */
static float rightPlane[4] = {-1.0, 0.0, 0.0, 4.88}; /* X = 4.98 */
static float groundPlane[4] = {0.0, 1.0, 0.0, 1.450}; /* Y = -1.480 */
static float columnPlane[4] = {0.0, 0.0, 1.0, 0.899}; /* Z = -0.899 */
static float backPlane[4] = {0.0, 0.0, 1.0, 8.98}; /* Z = -8.98 */

#define S 0.7071
#define NS 0.382683
#define NC 0.923880

/* satellite body. */
static float oct_vertices[8][3][4] =
{
    {
        {0.0, 0.0, 0.0, 1.0},
        {0.0, 1.0, 0.0, 1.0},
        {-S, S, 0.0, 1.0}},

    {
        {0.0, 0.0, 0.0, 1.0},
        {-S, S, 0.0, 1.0},
        {-1.0, 0.0, 0.0, 1.0}},

    {
        {0.0, 0.0, 0.0, 1.0},
        {-1.0, 0.0, 0.0, 1.0},
        {-S, -S, 0.0, 1.0}},

    {
        {0.0, 0.0, 0.0, 1.0},
        {-S, -S, 0.0, 1.0},
        {0.0, -1.0, 0.0, 1.0}},

    {
        {0.0, 0.0, 0.0, 1.0},
        {0.0, -1.0, 0.0, 1.0},
        {S, -S, 0.0, 1.0}},

    {

        {0.0, 0.0, 0.0, 1.0},
        {S, -S, 0.0, 1.0},
        {1.0, 0.0, 0.0, 1.0}},

    {
        {0.0, 0.0, 0.0, 1.0},
        {1.0, 0.0, 0.0, 1.0},
        {S, S, 0.0, 1.0}},

    {
        {0.0, 0.0, 0.0, 1.0},
        {S, S, 0.0, 1.0},
        {0.0, 1.0, 0.0, 1.0}}

};

static float oct_side_vertices[8][4][4] =
{
    {
        {-S, S, 0.0, 1.0},
        {0.0, 1.0, 0.0, 1.0},
        {0.0, 1.0, -1.0, 1.0},
        {-S, S, -1.0, 1.0}},

    {
        {-1.0, 0.0, 0.0, 1.0},
        {-S, S, 0.0, 1.0},
        {-S, S, -1.0, 1.0},
        {-1.0, 0.0, -1.0, 1.0}},

    {
        {-S, -S, 0.0, 1.0},
        {-1.0, 0.0, 0.0, 1.0},
        {-1.0, 0.0, -1.0, 1.0},
        {-S, -S, -1.0, 1.0}},

    {
        {0.0, -1.0, 0.0, 1.0},
        {-S, -S, 0.0, 1.0},
        {-S, -S, -1.0, 1.0},
        {0.0, -1.0, -1.0, 1.0}},

    {
        {S, -S, 0.0, 1.0},
        {0.0, -1.0, 0.0, 1.0},
        {0.0, -1.0, -1.0, 1.0},
        {S, -S, -1.0, 1.0}},

    {
        {1.0, 0.0, 0.0, 1.0},
        {S, -S, 0.0, 1.0},
        {S, -S, -1.0, 1.0},
        {1.0, 0.0, -1.0, 1.0}},

    {
        {S, S, 0.0, 1.0},
        {1.0, 0.0, 0.0, 1.0},
        {1.0, 0.0, -1.0, 1.0},
        {S, S, -1.0, 1.0}},

    {
        {0.0, 1.0, 0.0, 1.0},
        {S, S, 0.0, 1.0},
        {S, S, -1.0, 1.0},
        {0.0, 1.0, -1.0, 1.0}}

};

static float oct_side_normals[8][3] =
{
    {-NS, NC, 0.0},
    {-NC, NS, 0.0},
    {-NC, -NS, 0.0},
    {-NS, -NC, 0.0},
    {NS, -NC, 0.0},
    {NC, -NS, 0.0},
    {NC, NS, 0.0},
    {NS, NC, 0.0}

};

static float cube_vertexes[6][4][4] =
{
    {
        {-1.0, -1.0, -1.0, 1.0},
        {-1.0, -1.0, 1.0, 1.0},
        {-1.0, 1.0, 1.0, 1.0},
        {-1.0, 1.0, -1.0, 1.0}},

    {
        {1.0, 1.0, 1.0, 1.0},
        {1.0, -1.0, 1.0, 1.0},
        {1.0, -1.0, -1.0, 1.0},
        {1.0, 1.0, -1.0, 1.0}},

    {
        {-1.0, -1.0, -1.0, 1.0},
        {1.0, -1.0, -1.0, 1.0},
        {1.0, -1.0, 1.0, 1.0},
        {-1.0, -1.0, 1.0, 1.0}},

    {
        {1.0, 1.0, 1.0, 1.0},
        {1.0, 1.0, -1.0, 1.0},
        {-1.0, 1.0, -1.0, 1.0},
        {-1.0, 1.0, 1.0, 1.0}},

    {
        {-1.0, -1.0, -1.0, 1.0},
        {-1.0, 1.0, -1.0, 1.0},
        {1.0, 1.0, -1.0, 1.0},
        {1.0, -1.0, -1.0, 1.0}},

    {
        {1.0, 1.0, 1.0, 1.0},
        {-1.0, 1.0, 1.0, 1.0},
        {-1.0, -1.0, 1.0, 1.0},
        {1.0, -1.0, 1.0, 1.0}}
};

static float cube_normals[6][4] =
{
    {-1.0, 0.0, 0.0, 0.0},
    {1.0, 0.0, 0.0, 0.0},
    {0.0, -1.0, 0.0, 0.0},
    {0.0, 1.0, 0.0, 0.0},
    {0.0, 0.0, -1.0, 0.0},
    {0.0, 0.0, 1.0, 0.0}
};

static GLUquadricObj *quadObj;

/*----------------------------- Implementation ----------------------------*/

/* DRAW CUBE */

static void drawCube(
    GLfloat color[4],
    GLfloat ambient[4])
{
    int i;

    glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);

    for (i = 0; i < 6; ++i) {
        glNormal3fv(&cube_normals[i][0]);
        glBegin(GL_POLYGON);
        glVertex4fv(&cube_vertexes[i][0][0]);
        glVertex4fv(&cube_vertexes[i][1][0]);
        glVertex4fv(&cube_vertexes[i][2][0]);
        glVertex4fv(&cube_vertexes[i][3][0]);
        glEnd();
        }
}

/* DRAW OCTOGON TOP */

static void drawOct(void)
{
    int i;

    for (i = 0; i < 8; ++i) {
        glNormal3f(0.0, 0.0, 1.0);
        glBegin(GL_TRIANGLE_FAN);
        glVertex4fv(&oct_vertices[i][0][0]);
        glVertex4fv(&oct_vertices[i][1][0]);
        glVertex4fv(&oct_vertices[i][2][0]);
        glEnd();
        }
}

/* DRAW OCTOGON SIDES */

static void drawOctSides(void)
{
    int i;

    for (i = 0; i < 8; ++i) {
        glNormal3fv(&oct_side_normals[i][0]);
        glBegin(GL_POLYGON);
        glVertex4fv(&oct_side_vertices[i][0][0]);
        glVertex4fv(&oct_side_vertices[i][1][0]);
        glVertex4fv(&oct_side_vertices[i][2][0]);
        glVertex4fv(&oct_side_vertices[i][3][0]);
        glEnd();
        }
}

/* DRAW SATELLITE BODY */

static void drawSatellite(
    GLfloat diffuse[4],
    GLfloat ambient[4],
    GLfloat specular[4],
    GLfloat shiny[1])
{
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, shiny);

    glPushMatrix();
    glScalef(0.3, 0.3, 0.9);
    glPushMatrix();
    drawOctSides();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.0, 0.0, 0.0);
    drawOct();
    glPopMatrix();
    glPushMatrix();
    glRotatef(180, 1.0, 0.0, 0.0);
    glTranslatef(0.0, 0.0, 1.0);
    drawOct();
    glPopMatrix();
    glPopMatrix();
}

/* DRAW SOLAR PANELS */

static void drawPanels(
    GLfloat color[4],
    GLfloat ambient[4])
{
    glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);

    glPushMatrix();
    glTranslatef(0.95, 0.0, -0.45);
    glRotatef(45.0, 1.0, 0.0, 0.0);
    glScalef(0.65, 0.20, 0.02);
    drawCube(color, ambient);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.95, 0.0, -0.45);
    glRotatef(45.0, 1.0, 0.0, 0.0);
    glScalef(0.65, 0.20, 0.02);
    drawCube(color, ambient);
    glPopMatrix();
}

/* DRAW FLOOR */

void drawFloor(GLfloat f_color[4], GLfloat ambient[4])
{

    glMaterialfv(GL_FRONT, GL_DIFFUSE, f_color);
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);

    glBegin (GL_QUADS);
    glVertex3f (-1.0, -1.0, 0.0);
    glVertex3f (1.0, -1.0, 0.0);
    glVertex3f (1.0, 1.0, 0.0);
    glVertex3f (-1.0, 1.0, 0.0);
    glEnd();
}

/* DRAW GROUND */

/* Ground coordinates are in drawGround() below. Subdivision */
/* of triangles id done by subDivide(). */

void subDivide(
    float u1[3],
    float u2[3],
    float u3[3],
    int depth)
{
    GLfloat u12[3];
    GLfloat u23[3];
    GLfloat u31[3];

    GLint i;

    if (depth == 0) {
        glBegin (GL_POLYGON);
        glNormal3f (0.0, 0.0, 1.0); glVertex3fv(u1);
        glNormal3f (0.0, 0.0, 1.0); glVertex3fv(u2);
        glNormal3f (0.0, 0.0, 1.0); glVertex3fv(u3);
        glEnd();
        return;
        }

    for (i = 0; i < 3; i++) {
        u12[i] = (u1[i] + u2[i]) / 2.0;
        u23[i] = (u2[i] + u3[i]) / 2.0;
        u31[i] = (u3[i] + u1[i]) / 2.0;
        }

    subDivide(u1, u12, u31, depth - 1);
    subDivide(u2, u23, u12, depth - 1);
    subDivide(u3, u31, u23, depth - 1);
    subDivide(u12, u23, u31, depth - 1);
}

void drawGround(void)
{
/* Use two subdivided triangles for the unscaled 1X1 square. */
/* Subdivide to this depth: */

    GLint maxdepth = 2;

/* Coordinates of first triangle: */

    GLfloat u1[] = {-1.0, -1.0, 0.0};
    GLfloat u2[] = {1.0, -1.0, 0.0};
    GLfloat u3[] = {1.0, 1.0, 0.0};

/* Coordinates of second triangle: */

    GLfloat v1[] = {-1.0, -1.0, 0.0};
    GLfloat v2[] = {1.0, 1.0, 0.0};
    GLfloat v3[] = {-1.0, 1.0, 0.0};

    subDivide(u1, u2, u3, maxdepth);
    subDivide(v1, v2, v3, maxdepth);
}

/* Matrix for shadow. From Mark Kilgard's "scube". */

static void myShadowMatrix(
    float ground[4],
    float light[4],
    float shadowMat[4][4])
{
    float dot;

    dot = ground[0] * light[0] +
          ground[1] * light[1] +
          ground[2] * light[2] +
          ground[3] * light[3];

    shadowMat[0][0] = dot - light[0] * ground[0];
    shadowMat[1][0] = 0.0 - light[0] * ground[1];
    shadowMat[2][0] = 0.0 - light[0] * ground[2];
    shadowMat[3][0] = 0.0 - light[0] * ground[3];

    shadowMat[0][1] = 0.0 - light[1] * ground[0];
    shadowMat[1][1] = dot - light[1] * ground[1];
    shadowMat[2][1] = 0.0 - light[1] * ground[2];
    shadowMat[3][1] = 0.0 - light[1] * ground[3];

    shadowMat[0][2] = 0.0 - light[2] * ground[0];
    shadowMat[1][2] = 0.0 - light[2] * ground[1];
    shadowMat[2][2] = dot - light[2] * ground[2];
    shadowMat[3][2] = 0.0 - light[2] * ground[3];

    shadowMat[0][3] = 0.0 - light[3] * ground[0];
    shadowMat[1][3] = 0.0 - light[3] * ground[1];
    shadowMat[2][3] = 0.0 - light[3] * ground[2];
    shadowMat[3][3] = dot - light[3] * ground[3];
}

void display(void)
{
    glPushMatrix();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    /* Rotation and Translation of Entire Scene */
    if (mvt_x < 0 && mvt_y < 0) {
        glTranslatef(mvt_x ,mvt_y ,mvt_z );
        mvt_x = mvt_x - Tx;
        mvt_y = mvt_y - Ty;
        mvt_z = mvt_z - Tz;
        glRotatef(mvr_d, mvr_x, mvr_y, mvr_z);
        mvr_d = mvr_d - Rx;
        }
    else {
        glTranslatef(0.0, 0.0 ,mvt_z);
        }

    glPushMatrix();
    glLightfv(GL_LIGHT1, GL_POSITION, light1Pos);
    glPopMatrix();

    /* Draw Floor */
    glPushMatrix();
    glCallList(ground);
    glPopMatrix();

    /* Draw Lamp Post amd Lamp */
    glPushMatrix();
    glCallList(21);
    glPopMatrix();
    glPushMatrix();
    glCallList(22);
    glPopMatrix();
    glPushMatrix();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glCallList(501);
    glDisable(GL_BLEND);
    glPopMatrix();

    /* Draw Left Wall */
    glCallList(left_wall);

    /* Draw Right Wall */
    glCallList(right_wall);

    /* Draw Columns */
    glCallList(four_columns);
    glPushMatrix();
    glTranslatef(0.0, 0.0, -9.0);
    glCallList(four_columns);
    glPopMatrix();
    glCallList(two_columns);

    /* CUBE */
    glMaterialf(GL_FRONT, GL_SHININESS, 99.0);
    glMaterialfv(GL_FRONT, GL_SPECULAR, cube_specular);

    glPushMatrix();
    glTranslatef(0.0, 0.0, -5.0);
    glRotatef((360.0 / (30 * 2)) * tick, 0, 1, 0);
    glPushMatrix();
    glTranslatef(0.0, 0.2, 2.0);
    glRotatef((360.0 / (30 * 1)) * tick, 1, 0, 0);
    glRotatef((360.0 / (30 * 2)) * tick, 0, 1, 0);
    glRotatef((360.0 / (30 * 4)) * tick, 0, 0, 1);

    glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *) cubeXform);

    glCallList(satellite1);
    glCallList(panel1);

    glPopMatrix();
    glPopMatrix();

    glMaterialf(GL_FRONT, GL_SHININESS, 0.0);
    glMaterialfv(GL_FRONT, GL_SPECULAR, shadow_specular);

    /* CUBE SHADOWS */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPushMatrix();
    glMultMatrixf((const GLfloat *) shadowMat1_ground);
    glRotatef(-mvr_d, mvr_x, mvr_y, mvr_z);
    glTranslatef(-mvt_x, -mvt_y, -mvt_z);      /* correct for modelview matrix */
    glMultMatrixf((const GLfloat *) cubeXform);

    glCallList(satellite2);
    glTranslatef(0.0, -.040, 0.0);
    glCallList(panel2);
    glPopMatrix();

    /* Shadow left wall only if cube is in front of left wall. */
    if ((tick*6) >= 220 && (tick*6) <= 320) {
        glPushMatrix();
        glMultMatrixf((const GLfloat *) shadowMat1_left);
        glRotatef(-mvr_d, mvr_x, mvr_y, mvr_z);
        glTranslatef(-mvt_x, -mvt_y, -mvt_z);   /* correct for modelview matrix */
        glMultMatrixf((const GLfloat *) cubeXform);
        drawSatellite(shadow_diffuse, shadow_ambient, shadow_specular, shadow_shiny);      /* draw left shadow */
        drawPanels(shadow_diffuse, shadow_ambient);
        glPopMatrix();
        }

    /* Shadow back wall only if cube is in front of back wall. */
    if ((tick*6) >= 125 && (tick*6) <= 330) {
        glPushMatrix();
        glMultMatrixf((const GLfloat *) shadowMat1_back);
        glRotatef(-mvr_d, mvr_x, mvr_y, mvr_z);
        glTranslatef(-mvt_x, -mvt_y, -mvt_z);   /* correct for modelview matrix */
        glMultMatrixf((const GLfloat *) cubeXform);
        drawSatellite(shadow_diffuse, shadow_ambient, shadow_specular, shadow_shiny);      /* draw back wall shadow */
        drawPanels(shadow_diffuse, shadow_ambient);
        glPopMatrix();
        }

    /* Shadow right wall only if cube is in front of right wall.  */
    if ((tick*6) >= 40 && (tick*6) <= 145) {
        glPushMatrix();
        glMultMatrixf((const GLfloat *) shadowMat1_right);
        glRotatef(-mvr_d, mvr_x, mvr_y, mvr_z);
        glTranslatef(-mvt_x, -mvt_y, -mvt_z);   /* correct for modelview matrix */
        glMultMatrixf((const GLfloat *) cubeXform);
        drawSatellite(shadow_diffuse, shadow_ambient, shadow_specular, shadow_shiny);      /* draw right wall shadow */
        drawPanels(shadow_diffuse, shadow_ambient);
        glPopMatrix();
        }

    /* Clean up and clear the matrix stack */
    glDisable(GL_BLEND);
    glPopMatrix();
}

void makeSolidSphere(
    GLdouble radius,
    GLint slices,
    GLint stacks)
{
    if(!quadObj) {
        quadObj = gluNewQuadric();
        if (!quadObj)
            PM_fatalError("out of memory");
        }
    gluQuadricDrawStyle(quadObj, GLU_FILL);
    gluQuadricNormals(quadObj, GLU_SMOOTH);
    /* If we ever changed/used the texture or orientation state
     of quadObj, we'd need to change it to the defaults here
     with gluQuadricTexture and/or gluQuadricOrientation. */
    gluSphere(quadObj, radius, slices, stacks);
}

void myInit(void)
{
    /* Initialise scene variables */
    tick = -1;
    mvt_x = -15.0;
    mvt_y = -15.0;
    mvt_z = -30.7;
    mvr_d = 150.0;
    mvr_x = 1.0;
    mvr_y = -1.0;
    mvr_z = -1.0;

    /* Initial light position is declared in the display function */
    glLightfv(GL_LIGHT1, GL_AMBIENT, light1Amb);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1Diff);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1Spec);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHTING);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.07); /* try 0.07 w/ 24 bit color */

    /* Shadow Matrices For Floor, Left Wall, Back Wall, and Right Wall */

    /* For light0 */
    myShadowMatrix(groundPlane, lightPos, shadowMat_ground);
    myShadowMatrix(leftPlane, lightPos, shadowMat_left);
    myShadowMatrix(columnPlane, lightPos, shadowMat_column);
    myShadowMatrix(backPlane, lightPos, shadowMat_back);
    myShadowMatrix(rightPlane, lightPos, shadowMat_right);

    /* For light1 */
    myShadowMatrix(groundPlane, light1Pos, shadowMat1_ground);
    myShadowMatrix(leftPlane, light1Pos, shadowMat1_left);
    myShadowMatrix(backPlane, light1Pos, shadowMat1_back);
    myShadowMatrix(rightPlane, light1Pos, shadowMat1_right);

    /* Make Satellite Body and Shadow */
    glNewList(satellite1, GL_COMPILE);
    glPushMatrix();
    drawSatellite(satellite_diffuse, satellite_ambient, satellite_specular, satellite_shiny);
    glPopMatrix();
    glEndList();
    glNewList(satellite2, GL_COMPILE);
    glPushMatrix();
    drawSatellite(shadow_diffuse, shadow_ambient, shadow_specular, shadow_shiny);
    glPopMatrix();
    glEndList();

    /* Make Solar Panels and Shadows */
    glNewList(panel1, GL_COMPILE);
    glPushMatrix();
    drawPanels(panel_color, panel_ambient);
    glPopMatrix();
    glEndList();

    glNewList(panel2, GL_COMPILE);
    glPushMatrix();
    drawPanels(shadow_diffuse, shadow_ambient);
    glPopMatrix();
    glEndList();

    /* Make Floor */
    glNewList(ground, GL_COMPILE);
    glPushMatrix();
    glPushAttrib(GL_LIGHTING_BIT);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, floor_color);
    glMaterialfv(GL_FRONT, GL_AMBIENT, shadow_ambient);
    glTranslatef(0.0, -1.5, -5.0);
    glRotatef(-90.0, 1, 0, 0);
    glScalef(5.0, 5.0, 1.0);
    drawGround();  /* draw ground */
    glPopAttrib();
    glPopMatrix();
    glEndList();

    /* Make Lamp Post and Lamp */
    glNewList(21, GL_COMPILE);
    glPushMatrix();
    glPushAttrib(GL_LIGHTING_BIT);
    glMaterialfv(GL_FRONT, GL_AMBIENT, lamp_post_specular);
    glTranslatef(0.0, -0.1, -5.0);
    glScalef(0.07, 1.45, 0.07);
    drawCube(lamp_post_diffuse, lamp_post_ambient);  /* draw lamp post */
    glPopAttrib();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.0, -1.45, -5.0);
    glScalef(0.3, 0.05, 0.3);
    drawCube(wall_color, cube_ambient);  /* draw lamp post base */
    glPopMatrix();
    glEndList();

    glNewList(22, GL_COMPILE);
    glPushMatrix();
    glPushAttrib(GL_LIGHTING_BIT);
    glMaterialfv(GL_FRONT, GL_AMBIENT, lamp_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, lamp_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, lamp_specular);
    glTranslatef(0.0, 1.6, -5.0);
    makeSolidSphere(0.3, 20.0, 20.0);   /* draw lamp */
    glPopAttrib();
    glPopMatrix();
    glEndList();

    /* Lamp post base shadow */
    glNewList(501, GL_COMPILE);
    glPushMatrix();
    glPushAttrib(GL_LIGHTING_BIT);
    glMaterialfv(GL_FRONT, GL_AMBIENT, shadow_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, shadow_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, shadow_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, shadow_shiny);
    glTranslatef(0.0, -1.49, -5.0);
    glRotatef(-90.0, 1.0, 0.0, 0.0);
    glScalef(0.7, 0.7, 1.0);
    drawOct();
    glPopAttrib();
    glPopMatrix();
    glEndList();

    /* Make Left Wall */
    glNewList(left_wall, GL_COMPILE);
    glPushMatrix();
    glPushAttrib(GL_LIGHTING_BIT);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, wall_color);
    glMaterialfv(GL_FRONT, GL_AMBIENT, wall_ambient);
    glTranslatef(0.0, -1.5, 0.0);
    glTranslatef(0.0, 1.2, 0.0);
    glTranslatef(0.0, 0.0, -5.0);
    glTranslatef(-5.0, 0.0, 0.0);
    glRotatef(90.0, 0, 1, 0);
    glScalef(4.5, 1.2, 1.0);
    glNormal3f (0.0, 0.0, 1.0);
    drawGround();  /* draw left wall */
    glPopAttrib();
    glPopMatrix();
    glEndList();

    /* Make Right Wall */
    glNewList(right_wall, GL_COMPILE);
    glPushMatrix();
    glPushAttrib(GL_LIGHTING_BIT);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, wall_color);
    glMaterialfv(GL_FRONT, GL_AMBIENT, wall_ambient);
    glTranslatef(0.0, -1.5, 0.0);
    glTranslatef(0.0, 1.2, 0.0);

    glTranslatef(0.0, 0.0, -5.0);
    glTranslatef(5.0, 0.0, 0.0);
    glRotatef(270.0, 0, 1, 0);

    glScalef(4.5, 1.2, 1.0);
    glNormal3f (0.0, 0.0, 1.0);
    drawGround();  /* draw right wall */
    glPopAttrib();
    glPopMatrix();
    glEndList();

    /* Build Columns */
    glPushMatrix();
    glNewList(1, GL_COMPILE);
    glPushMatrix();
    glScalef(0.4, 1.4, 0.4);
    drawCube(column_color, column_ambient);  /* draw column1 */
    glPopMatrix();
    glEndList();

    glNewList(2, GL_COMPILE);
    glPushMatrix();
    glTranslatef(0.0, -1.45, 0.0);
    glScalef(0.5, 0.1, 0.5);
    drawCube(wall_color, cube_ambient); /* draw base */
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.0, 1.45, 0.0);
    glScalef(0.5, 0.1, 0.5);
    drawCube(wall_color, cube_ambient); /* draw top */
    glPopMatrix();
    glEndList();
    glPopMatrix();

    glNewList(column, GL_COMPILE);
    glPushMatrix();
    glCallList(1);
    glCallList(2);
    glPopMatrix();
    glEndList();

    /* Place columns at front of scene */
    glNewList(4, GL_COMPILE);
    glPushMatrix();
    glTranslatef(-5.0, 0.0, -0.5);
    glCallList(column);
    glPopMatrix();
    glEndList();

    glNewList(5, GL_COMPILE);
    glPushMatrix();
    glTranslatef(-1.75, 0.0, -0.5);
    glCallList(column);
    glPopMatrix();
    glEndList();

    glNewList(6, GL_COMPILE);
    glPushMatrix();
    glTranslatef(1.75, 0.0, -0.5);
    glCallList(column);
    glPopMatrix();
    glEndList();

    glNewList(17, GL_COMPILE);
    glPushMatrix();
    glTranslatef(5.0, 0.0, -0.5);
    glCallList(column);
    glPopMatrix();
    glEndList();

    /* Get the modelview matrix once */
    glPushMatrix();
    glRotatef(-mvr_d, mvr_x, mvr_y, mvr_z);
    glTranslatef(-mvt_x, -mvt_y, -mvt_z);
    glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat *) four_columnsXform);
    glPopMatrix();

    glNewList(four_columns, GL_COMPILE);
    glPushMatrix();
    glCallList(4);
    glCallList(5);
    glCallList(6);
    glCallList(17);
    glPopMatrix();
    glEndList();

    /* Make two columns for sides of scene */
    glNewList(two_columns, GL_COMPILE);
    glPushMatrix();
    glRotatef(90.0, 0.0, 1.0, 0.0);
    glTranslatef(5.0, 0.0, -5.0);
    glPushMatrix();
    glTranslatef(0.0, 0.0, -0.3);
    glCallList(column);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.0, 0.0, 10.3);
    glCallList(column);
    glPopMatrix();
    glPopMatrix();
    glEndList();

    /* Make shadows */
    glPushMatrix();
    glNewList(8, GL_COMPILE);
    glPushMatrix();
    glScalef(0.4, 1.4, 0.4);
    drawCube(shadow_diffuse, shadow_ambient);  /* draw column1 */
    glPopMatrix();
    glEndList();

    glNewList(9, GL_COMPILE);
    glPushMatrix();
    glTranslatef(0.0, -1.45, 0.0);
    glScalef(0.5, 0.1, 0.5);
    drawCube(shadow_diffuse, shadow_ambient); /* draw base. */
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.0, 1.45, 0.0);
    glScalef(0.5, 0.1, 0.5);
    drawCube(shadow_diffuse, shadow_ambient); /* draw top. */
    glPopMatrix();
    glEndList();
    glPopMatrix();

    glNewList(10, GL_COMPILE);
    glPushMatrix();
    glCallList(8);
    glCallList(9);
    glPopMatrix();
    glEndList();

    glNewList(11, GL_COMPILE);
    glPushMatrix();
    glTranslatef(-5.0, 0.0, -0.5);
    glCallList(10);
    glPopMatrix();
    glEndList();

    glNewList(12, GL_COMPILE);
    glPushMatrix();
    glTranslatef(-1.75, 0.0, -0.5);
    glCallList(10);
    glPopMatrix();
    glEndList();

    glNewList(13, GL_COMPILE);
    glPushMatrix();
    glTranslatef(1.75, 0.0, -0.5 );
    glCallList(10);
    glPopMatrix();
    glEndList();

    glNewList(14, GL_COMPILE);
    glPushMatrix();
    glTranslatef(5.0, 0.0, -0.5 );
    glCallList(10);
    glPopMatrix();
    glEndList();

    glNewList(15, GL_COMPILE);
    glPushMatrix();
    glCallList(11);
    glCallList(12);
    glCallList(13);
    glCallList(14);
    glPopMatrix();
    glEndList();

    glNewList(100, GL_COMPILE);
    glPushMatrix();
    glMultMatrixf((const GLfloat *) shadowMat_ground);
    glTranslatef(-mvt_x, -mvt_y, -mvt_z); /* correct for modelview matrix */
    glRotatef(-mvr_d, mvr_x, mvr_y, mvr_z);
    glMultMatrixf((const GLfloat *) four_columnsXform);
    glCallList(15);
    glPopMatrix();
    glEndList();

    glNewList(101, GL_COMPILE);
    glPushMatrix();
    glMultMatrixf((const GLfloat *) shadowMat_left);
    glTranslatef(-mvt_x, -mvt_y, -mvt_z); /* correct for modelview matrix */
    glRotatef(-mvr_d, mvr_x, mvr_y, mvr_z);
    glMultMatrixf((const GLfloat *) four_columnsXform);
    glCallList(15);
    glPopMatrix();
    glEndList();

    glNewList(102, GL_COMPILE);
    glPushMatrix();
    glMultMatrixf((const GLfloat *) shadowMat_back);
    glRotatef(-mvr_d, mvr_x, mvr_y, mvr_z);
    glTranslatef(-mvt_x, -mvt_y, -mvt_z); /* correct for modelview matrix */
    glMultMatrixf((const GLfloat *) four_columnsXform);
    glCallList(15);
    glPopMatrix();
    glEndList();

    glNewList(103, GL_COMPILE);
    glPushMatrix();
    glMultMatrixf((const GLfloat *) shadowMat_right);
    glRotatef(-mvr_d, mvr_x, mvr_y, mvr_z);
    glTranslatef(-mvt_x, -mvt_y, -mvt_z); /* correct for modelview matrix */
    glMultMatrixf((const GLfloat *) four_columnsXform);
    glCallList(15);
    glPopMatrix();
    glEndList();
}

static void myReshape(
    int w,
    int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-.9, .9, -.9, .9, 1.0, 35.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_NORMALIZE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glShadeModel(GL_SMOOTH);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
}

int mothTest(
    GA_HGLRC glrc,
    int width,
    int height)
{
    ibool   done = false;
    int     initPageCount = 5;
    int     fpsRate = 0,key = 0,waitVRT = gaWaitVRT,pageCount = initPageCount;
    ulong   lastCount = 0,newCount;

    myInit();
    myReshape(width,height);
    LZTimerOn();
    while (!done) {
        tick++;
        if (tick >= 60)
            tick = 0;
        display();
        gmoveto(0,0);
        gprintf("%d x %d %d bit %s (%d.%d fps)",(int)maxX+1,(int)maxY+1,
            (int)modeInfo.BitsPerPixel,
            (cntMode & gaLinearBuffer) ? "Linear" : "Banked",
            fpsRate / 10, fpsRate % 10);
        if (softwareOnly)
            gprintf("Rendering to system memory back buffer");
        switch (waitVRT) {
            case gaTripleBuffer:
                gprintf("Triple buffering - should be no flicker");
                gprintf("Frame rate *must* max at refresh rate");
                break;
            case gaWaitVRT:
                gprintf("Double buffering - should be no flicker");
                gprintf("Frame rate *must* lock to multiple of refresh");
                break;
            default:
                gprintf("Page flipping (no wait) - may flicker");
                gprintf("Frame rate *must* max at hardware limit");
                break;
            }
        glFuncs.SwapBuffers(glrc,waitVRT);
        if (EVT_kbhit()) {
            key = EVT_getch();                /* Swallow keypress */
            if (key == 'v' || key == 'V') {
                waitVRT -= 1;
                if (modeInfo.Attributes & gaHaveTripleBuffer) {
                    if (waitVRT < gaTripleBuffer)
                        waitVRT = gaDontWait;
                    }
                else {
                    if (waitVRT < gaWaitVRT)
                        waitVRT = gaDontWait;
                    }
                }
            else
                break;
            }

        /* Compute the frames per second rate after going through a large
         * number of pages.
         */
        if (--pageCount == 0) {
            newCount = LZTimerLap();
            fpsRate = (int)(10000000L / (newCount - lastCount)) * initPageCount;
            lastCount = newCount;
            pageCount = initPageCount;
            }
        }
    LZTimerOff();
    if (quadObj) {
        gluDeleteQuadric(quadObj);
        quadObj = NULL; // and this is IMPORTANT!
        }
    return key;
}

