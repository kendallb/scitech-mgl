/****************************************************************************
*
*                 High Speed Fixed/Floating Point Library
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
* Language:     C++ 3.0
* Environment:  any
*
* Description:  Test program for the matrix classes.
*
****************************************************************************/

#include "fx/xform2d.h"
#include "fx/xform3d.h"
#include "ztimer.h"
#include <iostream.h>
#include <iomanip.h>
#include <dos.h>
#include <conio.h>

#define MAXITER 1000
#define TEST3D

// Macros to aid in timing member functions

#define time_start()                                                    \
    m.identity();                                                       \
    LZTimerOn();                                                        \
    for (i = 0; i < MAXITER; i++)

#define time_start2()                                                   \
    m.identity();                                                       \
    LZTimerOn();                                                        \
    for (i = 0; i < MAXITER/2; i++)

#define time_end(name)                                                  \
    LZTimerOff();                                                       \
    cout << "Time for " << MAXITER << " ";                              \
    cout << setw(20) << #name << "'s:\t" << LZTimerCount() << endl

#ifdef  TEST2D

real    data[] = {REAL(1), REAL(2), REAL(3),
                  REAL(4), REAL(5), REAL(6)};

void test_it(void)
{
    FXForm2d    m1(data),m2,m3,m4,m5,m6,m7;

    cout << m1 << endl;
    cout << m2.concat(m1,m1) << endl;

    m1.identity();
    m1.scale(REAL(10),REAL(20));
    m2.identity();
    m2.scaleAbout(REAL(10),REAL(20),FXPoint2d(REAL(1),REAL(2)));
    m3.identity();
    m3.translate(REAL(10),REAL(20));
    m4.identity();
    m4.rotate(REAL(45));
    m5.identity();
    m5.rotateAbout(REAL(45),FXPoint2d(REAL(1),REAL(2)));
    m6.viewToWorld(REAL(10),REAL(10),REAL(-10),REAL(10),REAL(-200),REAL(200));
    m5.inverse(m7);

    cout << m1 << m2 << m3 << m4 << m5 << m7;

    getch();

    FXPoint2d p(REAL(1),REAL(1)),p_dash;

    cout << "map:\n";
    cout << p << " -> ";    m1.map(p_dash,p);   cout << p_dash << endl;
    cout << p << " -> ";    m2.map(p_dash,p);   cout << p_dash << endl;
    cout << p << " -> ";    m3.map(p_dash,p);   cout << p_dash << endl;
    cout << p << " -> ";    m4.map(p_dash,p);   cout << p_dash << endl;
    cout << p << " -> ";    m5.map(p_dash,p);   cout << p_dash << endl;
    cout << p_dash << " -> ";   m7.map(p,p_dash);   cout << p << endl;
}

void time_it(void)
{
    int         i;
    FXForm2d    m,m1,m2,m3;
    FXPoint2d   p1,p2(REAL(123.456),REAL(456.683));
    FXVec2d v1,v2(REAL(123.456),REAL(456.683));

    time_start2() {
        m.scale(REAL(10),REAL(20));
        m.scale(REAL(.1),REAL(.05));
        } time_end(scale);
    time_start2() {
        m.scaleAbout(REAL(10),REAL(20),FXPoint2d(REAL(1),REAL(2)));
        m.scaleAbout(REAL(.1),REAL(.05),FXPoint2d(REAL(1),REAL(2)));
        } time_end(scaleAbout);
    time_start2() {
        m.translate(REAL(10),REAL(20));
        m.translate(REAL(-10),REAL(-20));
        } time_end(translate);
    time_start() {
        m.rotate(REAL(45));
        } time_end(rotate);
    time_start() {
        m.rotateAbout(REAL(45),FXPoint2d(REAL(1),REAL(2)));
        } time_end(rotateAbout);

    m.viewToWorld(REAL(10),REAL(10),REAL(-10),REAL(10),REAL(-200),REAL(200));

    time_start()    m.map(p1,p2);   time_end(map_p);
    time_start()    m.map(v1,v2);   time_end(map_v);

    m1.rotateAbout(REAL(45),FXPoint2d(REAL(1),REAL(2)));
    m2.viewToWorld(REAL(10),REAL(10),REAL(-10),REAL(10),REAL(-200),REAL(200));

    time_start()    m.concat(m1,m2);    time_end(concat);
}

#endif

#ifdef  TEST3D

real    data[] = {REAL(1),  REAL(2),  REAL(3),  REAL(4),
                  REAL(5),  REAL(6),  REAL(7),  REAL(8),
                  REAL(9),  REAL(10), REAL(11), REAL(12),
                  REAL(13), REAL(14), REAL(15), REAL (16)};

FXVec3d VPN(REAL(0.5), REAL(0.25), REAL(1));
FXVec3d VUP(REAL(0), REAL(1), REAL(0));
FXVec3d VRP(REAL(2), REAL(1), REAL(4));
FXVec3d PRP(REAL(0), REAL(0), REAL(5));

real    F = REAL(1);
real    B = REAL(-1);
real    vmin = REAL(-0.75);
real    vmax = REAL(0.75);
real    umin = REAL(-0.75);
real    umax = REAL(0.75);
real    NDC_maxx = REAL(639);
real    NDC_maxy = REAL(479);

void test_it(void)
{
    FXForm3d    m1(data),m2,m3,m4,m5,m6,m7;
    FXVec3d     axis(REAL(1),REAL(1),REAL(1));

    cout << m1 << endl;
    cout << m2.concat(m1,m1) << endl;

    m1.identity();
    m1.scale(REAL(10),REAL(20),REAL(30));
    m2.identity();
    m2.translate(REAL(10),REAL(20),REAL(30));
    m3.identity();
    m3.rotatex(REAL(30));
    m4.identity();
    m4.rotatey(REAL(30));
    m5.identity();
    m5.rotatez(REAL(30));
    m6.identity();
    m6.rotate(REAL(30),axis);

    cout << m1 << m2 << m3 << m4 << m5 << m6;
    getch();

    cout << "View mapping: \n";

    // Set up a genuine parallel viewing transform

    m1.viewOrientation(VRP,VPN,VUP);
    m2.viewMapping(umin,vmin,umax,vmax,FXForm3dParallel,PRP,F,B,
        NDC_maxx,NDC_maxy,1000,true);
    m3.concat(m2,m1);

    cout << m1 << endl;
    cout << m2 << endl;
    cout << m3 << endl;

    FXPoint3d p(REAL(1),REAL(1),REAL(1)),p_dash;
    FXPoint4d p4(REAL(1),REAL(1),REAL(1)),p4_dash;

    cout << "map:\n";
    cout << p << " -> ";    m3.map(p_dash,p);   cout << p_dash << endl;
    cout << p << " -> ";    m3.map(p4_dash,p);  cout << p4_dash << endl;
    cout << p4 << " -> ";   m3.map(p4_dash,p4); cout << p4_dash << endl;

    m3.inverse(m4);
    cout << m4 << endl;

    cout << "map inverse:\n";
    cout << p_dash << " -> ";   m4.map(p,p_dash);   cout << p << endl;
    cout << p4_dash << " -> ";  m4.map(p4,p4_dash); cout << p4 << endl;

    getch();

    // Set up a genuine perspective viewing transform

    m2.viewMapping(umin,vmin,umax,vmax,FXForm3dPerspective,PRP,F,B,
        NDC_maxx,NDC_maxy,1000,true);
    m3.concat(m2,m1);

    cout << m1 << endl;
    cout << m2 << endl;
    cout << m3 << endl;

    p = FXPoint3d(REAL(1),REAL(1),REAL(1));
    p4 = FXPoint4d(REAL(1),REAL(1),REAL(1),REAL(1));
    cout << "map:\n";
    cout << p << " -> ";    m3.map(p_dash,p);   cout << p_dash << endl;
    cout << p << " -> ";    m3.map(p4_dash,p);  cout << p4_dash << endl;
    cout << p4 << " -> ";   m3.map(p4_dash,p4); cout << p4_dash << endl;

    m3.inverse(m4);
    cout << m4 << endl;

    cout << "map inverse:\n";
    cout << p_dash << " -> ";   m4.map(p,p_dash);   cout << p << endl;
    cout << p4_dash << " -> ";  m4.map(p4,p4_dash); cout << p4 << endl;
}

void time_it(void)
{
    int         i;
    FXForm3d    m,m1,m2;
    FXVec3d     axis(REAL(1),REAL(1),REAL(1));
    FXPoint3d   p(REAL(1),REAL(1),REAL(1)),p_dash;
    FXPoint4d   p4(REAL(1),REAL(1),REAL(1)),p4_dash;

    time_start2() {
        m.scale(REAL(10),REAL(20),REAL(30));
        m.scale(REAL(.1),REAL(.05),REAL(1.0/30));
        } time_end(scale);
    time_start2() {
        m.translate(REAL(10),REAL(20),REAL(30));
        m.translate(REAL(-10),REAL(-20),REAL(-30));
        } time_end(translate);
    time_start() {
        m.rotatex(REAL(30));
        } time_end(rotatex);
    time_start() {
        m.rotatey(REAL(30));
        } time_end(rotatey);
    time_start() {
        m.rotatez(REAL(30));
        } time_end(rotatez);
    time_start() {
        m.rotate(REAL(30),axis);
        } time_end(rotate);

    cout << "\n Parallel projections:\n\n";

    time_start() m.viewOrientation(VRP,VPN,VUP);    time_end(viewOrientaton);
    m1 = m;
    time_start() {
        m.viewMapping(umin,vmin,umax,vmax,FXForm3dParallel,PRP,F,B,
            NDC_maxx,NDC_maxy,1000,true);
        } time_end(viewMapping);
    m2 = m;
    time_start() m.concat(m2,m1);   time_end(concat);
    time_start() m.inverse(m2);     time_end(inverse);

    time_start() m.map(p_dash,p);   time_end(map3d3d);
    time_start() m.map(p4_dash,p);  time_end(map3d4d);
    time_start() m.map(p4_dash,p4); time_end(map4d4d);

    cout << "\n Perspective projections:\n\n";

    time_start() m.viewOrientation(VRP,VPN,VUP);    time_end(viewOrientaton);
    m1 = m;
    time_start() {
        m.viewMapping(umin,vmin,umax,vmax,FXForm3dPerspective,PRP,F,B,
            NDC_maxx,NDC_maxy,1000,true);
        } time_end(viewMapping);
    m2 = m;
    time_start() m.concat(m2,m1);   time_end(concat);
    time_start() m.inverse(m2);     time_end(inverse);

    time_start() m.map(p_dash,p);   time_end(map3d3d);
    time_start() m.map(p4_dash,p);  time_end(map3d4d);
    time_start() m.map(p4_dash,p4); time_end(map4d4d);
}

#endif

void main(void)
{
    ZTimerInit();
    test_it();
    getch();
    time_it();
    cout << endl;
}
