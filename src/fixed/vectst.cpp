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
* Description:  Test program for the vector classes. Times each of the
*               operations.
*
****************************************************************************/

#include "fx/vec2d.h"
#include "fx/vec3d.h"
#include "fx/vec4d.h"
#include "fx/color.h"
#include "ztimer.h"
#include <iostream.h>
#include <dos.h>
#include <stdlib.h>

#ifdef  DOUBLE
#define MAXITER 500
#else
#define MAXITER 1000
#endif
#define TEST3D

real    a1Real[MAXITER];
int     test = 0;

#ifdef  TEST2D
FXVec2d a1Vec[MAXITER];
FXVec2d a2Vec[MAXITER];
FXVec2d a3Vec[MAXITER];

FXVec2d v1;
FXVec2d v3(REAL(10.765),REAL(20.143));
FXVec2d v2 = v3;
real    scale = REAL(1.56);
int     iscale = 5;

#endif

#ifdef  TEST3D
FXVec3d a1Vec[MAXITER];
FXVec3d a2Vec[MAXITER];
FXVec3d a3Vec[MAXITER];

FXVec3d v1;
FXVec3d v3(REAL(10.765),REAL(20.143),REAL(16.1));
FXVec3d v2 = v3;
real    scale = REAL(1.56);
int     iscale = 5;

#endif

#ifdef  TEST4D
FXVec4d a1Vec[MAXITER];
FXVec4d a2Vec[MAXITER];
FXVec4d a3Vec[MAXITER];

FXVec4d v1;
FXVec4d v3(REAL(10.765),REAL(20.143),REAL(16.1),REAL(2));
FXVec4d v2 = v3;
real    scale = REAL(1.56);
int     iscale = 5;

#endif

// Test all of the vector member functions

void test_operations(void)
{
    cout << "v1 = " << v1 << endl;
    cout << "v2 = " << v2 << endl;
    cout << "v3 = " << v3 << endl;

    // Arithmetic operators
    v1 = v2 + v3;
    cout << "v2 + v3 = " << v1 << endl;

    v1 = v2 - v3;
    cout << "v2 - v3 = " << v1 << endl;

    v1 = v2 * v3;
    cout << "v2 * v3 = " << v1 << endl;

    v1 = v2 * scale;
    cout << "v2 * " << FXrealToDbl(scale) << " = " << v1 << endl;

    v1 = scale * v2;
    cout << FXrealToDbl(scale) << " * v2 = " << v1 << endl;

    v1 = v2 * iscale;
    cout << "v2 * " << iscale << " = " << v1 << endl;

    v1 = iscale * v2;
    cout << iscale << " * v2 = " << v1 << endl;

    v1 = v2 / scale;
    cout << "v2 / " << FXrealToDbl(scale) << " = " << v1 << endl;

    v1 = v2 / iscale;
    cout << "v2 / " << iscale << " = " << v1 << endl;

    // Faster methods
    v1 = v2;
    v1 += v3;
    cout << "v2 += v3 = " << v1 << endl;

    v1 = v2;
    v1 -= v3;
    cout << "v2 -= v3 = " << v1 << endl;

    v1 = v2;
    v1 *= v3;
    cout << "v2 *= v3 = " << v1 << endl;

    v1 = v2;
    v1 *= scale;
    cout << "v2 *= " << FXrealToDbl(scale) << " = " << v1 << endl;

    v1 = v2;
    v1 *= iscale;
    cout << "v2 *= " << iscale << " = " << v1 << endl;

    v1 = v2;
    v1 /= scale;
    cout << "v2 /= " << FXrealToDbl(scale) << " = " << v1 << endl;

    v1 = v2;
    v1 /= iscale;
    cout << "v2 /= " << iscale << " = " << v1 << endl;

    // Negation methods
    v1 = -v2;
    cout << "-v2 = " << v1 << endl;

    v1 = v2;
    v1.negate();
    cout << "v2.negate() = " << v1 << endl;

    // Miscellaneous
#ifndef TEST4D
    v3 = v2 / 2;
    cout << "v3 = " << v3 << endl;
    scale = v2 % v3;
    cout << "v2 . v3 = " << FXrealToDbl(scale) << endl;

#ifdef  TEST3D
    v2.x = v2.x / 2;
    v1 = v2 ^ v3;
    cout << "v2 x v3 = " << v1 << endl;
#endif

    v1 = ~v2;
    cout << "~v2 = " << v1 << endl;

    v1 = v2;
    v1.normalise();
    cout << "v2.normalise() = " << v1 << endl;

    v2.perp(v1);
    cout << "v2.perp() = " << v1 << endl;

    cout << "v1 = " << v1 << ", v1.isZero() = " << v1.isZero() << endl;
    v1.x = v1.y = 0;
    cout << "v1 = " << v1 << ", v1.isZero() = " << v1.isZero() << endl;

    cout << "v1 == v2 (no) = " << (v1 == v2) << endl;
    v1 = (v2 * REAL(30)) / REAL(30);
    cout << "v1 = " << v1 << ", v2 = " << v2 << endl;
    cout << "v1 == v2 (yes) = " << (v1 == v2) << endl;

    v3.x = v3.y = 0;
    interpolate(v1,v2,v3,REAL(0.5));
    cout << "interpolate(v1,v2,v3,0.5) = " << v1 << endl;

    v3.x = REAL(4);
    v3.y = REAL(5);
    combine(v1,v2,REAL(0.1),v3,REAL(0.2));
    cout << "combine(v1,v2,0.1,v3,0.2) = " << v1 << endl;

    scaleAndAdd(v1,v2,v3,REAL(0.2));
    cout << "scaleAndAdd(v1,v2,v3,0.2) = " << v1 << endl;

    scale = distance(v2,v3);
    cout << "distance(v2,v3) = " << v1 << endl;
#endif

#ifdef  TEST4D
    cout << "v2.homogenise = " << v2.homogenise() << endl;

    FXPoint3d p3d;
    v2.to3d(p3d);
    cout << "v2.to3d = " << p3d << endl;
#endif
}

//---------------------------------------------------------------------------
// Set up convienience macros for timing.
//---------------------------------------------------------------------------

// Define a macro to time a particular binary operation

#define time_binary(op,type)                                            \
    LZTimerOn();                                                        \
    for (i = 0; i < MAXITER; i++)                                       \
        a3Vec[i] = a1Vec[i] op a2Vec[i];                                \
    LZTimerOff();                                                       \
    cout << "Time for " << MAXITER << " " << #type << " binary ";       \
    cout << #op << "'s:\t" << LZTimerCount() << endl;

#define time_binary2(op,type,scale)                                     \
    LZTimerOn();                                                        \
    for (i = 0; i < MAXITER; i++)                                       \
        a3Vec[i] = a1Vec[i] op scale;                                   \
    LZTimerOff();                                                       \
    cout << "Time for " << MAXITER << " " << #type << " binary2 ";      \
    cout << #op << "'s:\t" << LZTimerCount() << endl;

// Define a macro to time a particular unary operation

#define time_unary(op,type)                                             \
    LZTimerOn();                                                        \
    for (i = 0; i < MAXITER; i++)                                       \
        a3Vec[i] = op a1Vec[i];                                         \
    LZTimerOff();                                                       \
    cout << "Time for " << MAXITER << " " << #type << " unary ";        \
    cout << #op << "'s:\t" << LZTimerCount() << endl;

#define time_unary2(op,type)                                            \
    LZTimerOn();                                                        \
    for (i = 0; i < MAXITER; i++)                                       \
        a3Vec[i] op a1Vec[i];                                           \
    LZTimerOff();                                                       \
    cout << "Time for " << MAXITER << " " << #type << " unary2 ";       \
    cout << #op << "'s:\t" << LZTimerCount() << endl;

#define time_unary3(op,type,scale)                                      \
    LZTimerOn();                                                        \
    for (i = 0; i < MAXITER; i++)                                       \
        a3Vec[i] op scale;                                              \
    LZTimerOff();                                                       \
    cout << "Time for " << MAXITER << " " << #type << " unary3 ";       \
    cout << #op << "'s:\t" << LZTimerCount() << endl;

// Define a macro to time a member function

#define time_member(member,type)                                        \
    LZTimerOn();                                                        \
    for (i = 0; i < MAXITER; i++)                                       \
        a1Vec[i].##member##();                                              \
    LZTimerOff();                                                       \
    cout << "Time for " << MAXITER << " " << #type << " ";\
    cout << #member << "'s:\t" << LZTimerCount() << endl;

#ifdef  TEST2D

void fillVectors(void)
{
    int     i;

    for (i = 0; i < MAXITER; i++) {
        a1Vec[i].x = FXdblToReal((rand() * 10.0) / RAND_MAX);
        a1Vec[i].y = FXdblToReal((rand() * 10.0) / RAND_MAX);
        a2Vec[i].x = FXdblToReal((rand() * 10.0) / RAND_MAX);
        a2Vec[i].y = FXdblToReal((rand() * 10.0) / RAND_MAX);
        }
}

#endif  // TEST2D

#ifdef  TEST3D

void fillVectors(void)
{
    int     i;

    for (i = 0; i < MAXITER; i++) {
        a1Vec[i].x = FXdblToReal((rand() * 10.0) / RAND_MAX);
        a1Vec[i].y = FXdblToReal((rand() * 10.0) / RAND_MAX);
        a1Vec[i].z = FXdblToReal((rand() * 10.0) / RAND_MAX);
        a2Vec[i].x = FXdblToReal((rand() * 10.0) / RAND_MAX);
        a2Vec[i].y = FXdblToReal((rand() * 10.0) / RAND_MAX);
        a2Vec[i].z = FXdblToReal((rand() * 10.0) / RAND_MAX);
        }
}

#endif  // TEST3D

#ifdef  TEST4D

void fillVectors(void)
{
    int     i;

    for (i = 0; i < MAXITER; i++) {
        a1Vec[i].x = FXdblToReal((rand() * 10.0) / RAND_MAX);
        a1Vec[i].y = FXdblToReal((rand() * 10.0) / RAND_MAX);
        a1Vec[i].z = FXdblToReal((rand() * 10.0) / RAND_MAX);
        a1Vec[i].w = FXdblToReal((rand() * 10.0) / RAND_MAX);
        a2Vec[i].x = FXdblToReal((rand() * 10.0) / RAND_MAX);
        a2Vec[i].y = FXdblToReal((rand() * 10.0) / RAND_MAX);
        a2Vec[i].z = FXdblToReal((rand() * 10.0) / RAND_MAX);
        a2Vec[i].w = FXdblToReal((rand() * 10.0) / RAND_MAX);
        }
}

#endif  // TEST4D

void time_operations(void)
{
    int     i;

    fillVectors();

    // Standard arithmetic operators
    time_binary(+,Vec);
    time_binary(-,Vec);
    time_binary(*,Vec);
    time_binary2(*,Vec,scale);
    time_binary2(*,Vec,iscale);
    time_binary2(/,Vec,scale);
    time_binary2(/,Vec,iscale);

    // Faster arithmetic operators
    time_unary2(+=,Vec);
    time_unary2(-=,Vec);
    time_unary2(*=,Vec);
    time_unary3(*=,Vec,scale);
    time_unary3(*=,Vec,iscale);
    time_unary3(/=,Vec,scale);
    time_unary3(/=,Vec,iscale);

    // Negation
    time_unary(-,Vec);
    time_member(negate,Vec);

    // Miscellaneous
#ifndef TEST4D
    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a1Real[i] = a1Vec[i] % a2Vec[i];
    LZTimerOff();
    cout << "Time for " << MAXITER << " dot products:\t" << LZTimerCount() << endl;

#ifdef  TEST3D
    time_binary(^,Vec);
#endif

    time_unary(~,Vec);
    time_member(normalise,Vec);
    time_member(length,Vec);

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a1Vec[i].perp(a2Vec[i]);
    LZTimerOff();
    cout << "Time for " << MAXITER << " perp()'s:\t\t" << LZTimerCount() << endl;

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        if (a1Vec[i].isZero())
            test++;
    LZTimerOff();
    cout << "Time for " << MAXITER << " isZero()'s:\t" << LZTimerCount() << endl;

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        if (a1Vec[i] == a2Vec[i])
            test++;
    LZTimerOff();
    cout << "Time for " << MAXITER << " =='s:\t\t" << LZTimerCount() << endl;
#endif

#ifdef  TEST4D
    time_member(homogenise,Vec);
#endif

}

void main(void)
{
    ZTimerInit();
    test_operations();
    srand(100);
    time_operations();
    cout << endl;
}
