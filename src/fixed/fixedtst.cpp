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
* Description:  Test program for the fixed point class. Times the fixed
*               point routines in comparison to floating point routines.
*
****************************************************************************/

#include <iostream.h>
#include <iomanip.h>
#include <stdlib.h>
#include <math.h>
#include "fx/fixed.h"
#include "ztimer.h"

#ifndef M_PI
#define M_PI        3.14159265358979323846
#endif

#define MAXITER 2000

inline double round(double d)
{
    return floor(d + 0.5);
}

// Routines to compute sine/cosine's of real numbers given in degrees.

inline float sin(float& r)
{
    return sin(r * (M_PI / 180));
}

inline float cos(float& r)
{
    return cos(r * (M_PI / 180));
}

inline float tan(float& r)
{
    return tan(r * (M_PI / 180));
}

FXFixed     f1,f2,f3,f4;
FXZFixed    zf1,zf2,zf3;

int         a1Int[MAXITER+1];       // Array of integers
FXFixed     a1Fixed[MAXITER+1];     // Array of fixed point numbers
FXFixed     a2Fixed[MAXITER+1];     // Array of fixed point numbers
FXFixed     a3Fixed[MAXITER+1];
float       a1Float[MAXITER+1];     // Array of floating point numbers
float       a2Float[MAXITER+1];     // Array of floating point numbers
float       a3Float[MAXITER+1];

void SinCosTanTest(void)
{
    for (FXFixed angle = 0; angle < REAL(360); angle += REAL(10)) {
        f1 = FXsin(angle);
        f2 = FXcos(angle);
        f3 = FXtan(angle);
//      float rangle = FXrealToFlt(angle);

        cout << "FXsin(" << FXrealToDbl(angle) << ") = " << FXrealToDbl(f1)
             << ",FXcos(" << FXrealToDbl(angle) << ") = " << FXrealToDbl(f2)
             << ",FXtan(" << FXrealToDbl(angle) << ") = " << FXrealToDbl(f3)
             << endl;
//      cout << "  sin(" << FXrealToDbl(angle) << ") = " << sin(rangle)
//           << ",  cos(" << FXrealToDbl(angle) << ") = " << cos(rangle)
//           << ",  tan(" << FXrealToDbl(angle) << ") = " << tan(rangle)
//           << endl;
        }
}

void CeilFloorTest(void)
{
    f1 = FXdblToReal(123.54);

    cout << "f1 = " << FXrealToDbl(f1) << endl;
    cout << "ceil(f1) = " << FXrealToDbl(FXceil(f1)) << endl;
    cout << "floor(f1) = " << FXrealToDbl(FXfloor(f1)) << endl;
    cout << "round(f1) = " << FXrealToDbl(FXround(f1)) << endl << endl;

    f1 = FXdblToReal(-123.54);

    cout << "f1 = " << FXrealToDbl(f1) << endl;
    cout << "ceil(f1) = " << FXrealToDbl(FXceil(f1)) << endl;
    cout << "floor(f1) = " << FXrealToDbl(FXfloor(f1)) << endl;
    cout << "round(f1) = " << FXrealToDbl(FXround(f1)) << endl;
}

void MultDivTest(void)
{
    f1 = FXdblToReal(123.54);
    f3 = FXdblToReal(123.54);
    f2 = FXmul(f1,f3);
    f4 = f1;

    cout << "f1 = " << FXrealToDbl(f1) << endl;
    cout << "f3 = " << FXrealToDbl(f3) << endl;
    cout << "f1 * f1 = " << FXrealToDbl(FXmul(f1,f1)) << endl;
    cout << "f1 * f3 = " << FXrealToDbl(f2) << endl;
    cout << "f2 * 1.04 = " << FXrealToDbl(FXmul(f2,FXdblToReal(1.04))) << endl;
    FXmuleq(f4,f3);
    cout << "f1 *= f3 = " << FXrealToDbl(f4) << endl;
    cout << "muldiv(f1,f3,10) = " << FXrealToDbl(FXmuldiv(f1,f3,REAL(10))) << endl;

    f3 = FXdiv(REAL(17),REAL(8));
    cout << "17 / 8 = " << FXrealToDbl(f3) << endl;

    f3 = FXdiv(REAL(17),REAL(-8));
    cout << "17 / -8 = " << FXrealToDbl(f3) << endl;

    f3 = FXdiv(REAL(-17),REAL(8));
    cout << "-17 / 8 = " << FXrealToDbl(f3) << endl;

    f3 = FXdiv(REAL(-17),REAL(-8));
    cout << "-17 / -8 = " << FXrealToDbl(f3) << endl;

    f3 = FXdiv(f2,f1);
    cout << "f2 / f1 = " << FXrealToDbl(f3) << endl;

    FXdiveq(f2,f1);
    cout << "f2 /= f1 = " << FXrealToDbl(f2) << endl;

    f3 = f1 << 2;
    cout << "f1 << 2 = " << FXrealToDbl(f3) << endl;

    f3 = f1 >> 2;
    cout << "f1 >> 2 = " << FXrealToDbl(f3) << endl;
}

void ZDivTest(void)
{
    zf1 = REALZ(1.2354);
    f1 = FXdblToReal(1.2354);
    f3 = FXdblToReal(6.9178);
    zf2 = FXdivFF(f1,f3);
    zf3 = FXdivZF(zf1,f3);

    cout << "zf1 = " << (zf1 / 268435456.0) << endl;
    cout << "f1 = " << FXrealToDbl(f1) << endl;
    cout << "f3 = " << FXrealToDbl(f3) << endl;
    cout << "f1 / f3        = " << ((zf1 / 268435456.0) / FXrealToDbl(f3)) << endl;
    cout << "FXdivFF(f1,f3) = " << (zf2 / 268435456.0) << endl;
    cout << "FXdivZF(f1,f3) = " << (zf3 / 268435456.0) << endl;
}

void SqrtTest(void)
{
    double  f1;
    FXFixed fx1;

    f1 = 1.0;
    cout << "sqrt() test:\n";
    while (f1 != 0.0) {
        cout << "enter value: ";
        cin >> f1;
        fx1 = FXdblToReal(f1);
        cout << "sqrt(" << f1 << ") = " << sqrt(f1);
        cout << ", fixed sqrt(" << FXrealToDbl(fx1) << ") = "
             << FXrealToDbl(FXsqrt(fx1)) << endl;
        }
}

void PowTest(void)
{
}

void Log10Test(void)
{
    double  f1;
    FXFixed fx1;

    f1 = 1.0;
    cout << "log10() test:\n";
    while (1) {
        cout << "enter value: ";
        cin >> f1;
        if (f1 == 0.0)
            break;
        fx1 = FXdblToReal(f1);
        cout << "log10(" << f1 << ") = " << log10(f1);
        cout << ", fixed log10(" << FXrealToDbl(fx1) << ") = "
             << FXrealToDbl(FXlog10(fx1)) << endl;
        }
}

void LogTest(void)
{
    double  f1;
    FXFixed fx1;

    f1 = 1.0;
    cout << "log() test:\n";
    while (1) {
        cout << "enter value: ";
        cin >> f1;
        if (f1 == 0.0)
            break;
        fx1 = FXdblToReal(f1);
        cout << "log(" << f1 << ") = " << log(f1);
        cout << ", fixed log(" << FXrealToDbl(fx1) << ") = "
             << FXrealToDbl(FXlog(fx1)) << endl;
        }
}

// Multiplication timing test

void TimeMult(void)
{
    int     i;
    float   mFloat = 1.06453;
    FXFixed mFixed = FXdblToReal(mFloat);

    // Fill the array's with random numbers of the same value

    for (i = 0; i < MAXITER; i++) {
        a1Float[i] = (rand() * 10.0) / RAND_MAX;
        a1Fixed[i] = FXdblToReal(a1Float[i]);
        }

    // Time how long it takes to multiply all MAXITER floats

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = a1Float[i] * mFloat;
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = a1Float[i] * mFloat;
    LZTimerOff();
    cout << MAXITER*2 << " floating point *'s: \t" << LZTimerCount() << " us\n";

    // Time how long it takes to multiply all MAXITER fixed's

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Fixed[i] = FXmul(a1Fixed[i],mFixed);
    for (i = 0; i < MAXITER; i++)
        a2Fixed[i] = FXmul(a1Fixed[i],mFixed);
    LZTimerOff();
    cout << MAXITER*2 << " fixed point *'s: \t\t" << LZTimerCount() << " us\n";
}

// Multiplication timing test

void TimeMulteq(void)
{
    int     i;
    float   mFloat = 1.06453;
    FXFixed mFixed = FXdblToReal(mFloat);

    // Fill the array's with random numbers of the same value

    for (i = 0; i < MAXITER; i++) {
        a1Float[i] = (rand() * 10.0) / RAND_MAX;
        a1Fixed[i] = FXdblToReal(a1Float[i]);
        }

    // Time how long it takes to multiply all MAXITER floats

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a1Float[i] *= mFloat;
    for (i = 0; i < MAXITER; i++)
        a1Float[i] *= mFloat;
    LZTimerOff();
    cout << MAXITER*2 << " floating point *='s: \t" << LZTimerCount() << " us\n";

    // Time how long it takes to multiply all MAXITER fixed's

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        FXmuleq(a1Fixed[i],mFixed);
    for (i = 0; i < MAXITER; i++)
        FXmuleq(a1Fixed[i],mFixed);
    LZTimerOff();
    cout << MAXITER*2 << " fixed point *='s: \t\t" << LZTimerCount() << " us\n";
}

// Multiplication/divide timing test

void TimeMulDiv(void)
{
    int     i;
    float   mFloat1 = 1.06453,mFloat2 = 0.6789;
    FXFixed mFixed1 = FXdblToReal(mFloat1);
    FXFixed mFixed2 = FXdblToReal(mFloat2);

    // Fill the array's with random numbers of the same value

    for (i = 0; i < MAXITER; i++) {
        a1Float[i] = (rand() * 10.0) / RAND_MAX;
        a1Fixed[i] = FXdblToReal(a1Float[i]);
        }

    // Time how long it takes to multiply all MAXITER floats

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = a1Float[i] * mFloat1 / mFloat2;
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = a1Float[i] * mFloat1 / mFloat2;
    LZTimerOff();
    cout << MAXITER*2 << " floating point muldiv's: \t" << LZTimerCount() << " us\n";

    // Time how long it takes to multiply all MAXITER fixed's

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Fixed[i] = FXmuldiv(a1Fixed[i],mFixed1,mFixed2);
    for (i = 0; i < MAXITER; i++)
        a2Fixed[i] = FXmuldiv(a1Fixed[i],mFixed1,mFixed2);
    LZTimerOff();
    cout << MAXITER*2 << " fixed point muldiv's: \t" << LZTimerCount() << " us\n";
}

// Square timing tests

void TimeSquare(void)
{
    int     i;

    // Fill the array's with random numbers of the same value

    for (i = 0; i < MAXITER; i++) {
        a1Float[i] = (rand() * 10.0) / RAND_MAX;
        a1Fixed[i] = FXdblToReal(a1Float[i]);
        }

    // Time how long it takes to square all MAXITER floats

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = a1Float[i] * a1Float[i];
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = a1Float[i] * a1Float[i];
    LZTimerOff();
    cout << MAXITER*2 << " floating point square's: \t" << LZTimerCount() << " us\n";

    // Time how long it takes to multiply all MAXITER fixed's

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Fixed[i] = FXsquare(a1Fixed[i]);
    for (i = 0; i < MAXITER; i++)
        a2Fixed[i] = FXsquare(a1Fixed[i]);
    LZTimerOff();
    cout << MAXITER*2 << " fixed point square's: \t" << LZTimerCount() << " us\n";
}

// Shift left timing test (multiply by powers of two)

void TimeShiftLeft(void)
{
    int     i;

    // Fill the array's with random numbers of the same value

    for (i = 0; i < MAXITER; i++) {
        a1Float[i] = (rand() * 10.0) / RAND_MAX;
        a1Fixed[i] = FXdblToReal(a1Float[i]);
        }

    // Time how long it takes to multiply all MAXITER floats by 4

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = a1Float[i] * 4;
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = a1Float[i] * 4;
    LZTimerOff();
    cout << MAXITER*2 << " floating point *4's: \t" << LZTimerCount() << " us\n";

    // Time how long it takes to multiply all MAXITER fixed's by 4

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Fixed[i] = a1Fixed[i] << 2;
    for (i = 0; i < MAXITER; i++)
        a2Fixed[i] = a1Fixed[i] << 2;
    LZTimerOff();
    cout << MAXITER*2 << " fixed point *4's: \t\t" << LZTimerCount() << " us\n";
}

// Division timing test

void TimeDiv(void)
{
    int     i;
    float   mFloat = 10.06453;
    FXFixed mFixed = FXdblToReal(mFloat);

    // Fill the array's with random numbers of the same value

    for (i = 0; i < MAXITER; i++) {
        a1Float[i] = (rand() * 10.0) / RAND_MAX;
        a1Fixed[i] = FXdblToReal(a1Float[i]);
        }

    // Time how long it takes to divide all MAXITER floats

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = a1Float[i] / mFloat;
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = a1Float[i] / mFloat;
    LZTimerOff();
    cout << MAXITER*2 << " floating point /'s: \t" << LZTimerCount() << " us\n";

    // Time how long it takes to divide all MAXITER fixed's

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Fixed[i] = FXdiv(a1Fixed[i],mFixed);
    for (i = 0; i < MAXITER; i++)
        a2Fixed[i] = FXdiv(a1Fixed[i],mFixed);
    LZTimerOff();
    cout << MAXITER*2 << " fixed point /'s: \t\t" << LZTimerCount() << " us\n";
}

// Division timing test

void TimeDiveq(void)
{
    int     i;
    float   mFloat = 1.06453;
    FXFixed mFixed = FXdblToReal(mFloat);

    // Fill the array's with random numbers of the same value

    for (i = 0; i < MAXITER; i++) {
        a1Float[i] = (rand() * 10.0) / RAND_MAX;
        a1Fixed[i] = FXdblToReal(a1Float[i]);
        }

    // Time how long it takes to divide all MAXITER floats

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a1Float[i] /= mFloat;
    for (i = 0; i < MAXITER; i++)
        a1Float[i] /= mFloat;
    LZTimerOff();
    cout << MAXITER*2 << " floating point /='s: \t" << LZTimerCount() << " us\n";

    // Time how long it takes to divide all MAXITER fixed's

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        FXdiveq(a1Fixed[i],mFixed);
    for (i = 0; i < MAXITER; i++)
        FXdiveq(a1Fixed[i],mFixed);
    LZTimerOff();
    cout << MAXITER*2 << " fixed point /='s: \t\t" << LZTimerCount() << " us\n";
}

// 4.28 Division timing test

void TimeZDiv(void)
{
    int     i;
    FXFixed mFixed = FXdblToReal(6.91829);

    // Fill the array's with random numbers of the same value

    for (i = 0; i < MAXITER; i++) {
        a1Float[i] = (rand() * 1.0) / RAND_MAX;
        a1Fixed[i] = FXdblToReal(a1Float[i]);
        a2Fixed[i] = REALZ(a1Float[i]);
        }

    // Time how long it takes to divide all MAXITER fixed's

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Fixed[i] = FXdivFF(a1Fixed[i],mFixed);
    for (i = 0; i < MAXITER; i++)
        a2Fixed[i] = FXdivFF(a1Fixed[i],mFixed);
    LZTimerOff();
    cout << MAXITER*2 << " FXdivFF's: \t\t" << LZTimerCount() << " us\n";

    // Time how long it takes to divide all MAXITER fixed's

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Fixed[i] = FXdivZF(a2Fixed[i],mFixed);
    for (i = 0; i < MAXITER; i++)
        a2Fixed[i] = FXdivZF(a2Fixed[i],mFixed);
    LZTimerOff();
    cout << MAXITER*2 << " FXdivZF's: \t\t" << LZTimerCount() << " us\n";
}

// Inversion timing test

void TimeOneOver(void)
{
    int     i;

    // Fill the array's with random numbers of the same value

    for (i = 0; i < MAXITER; i++) {
        a1Float[i] = (rand() * 10.0) / RAND_MAX;
        a1Fixed[i] = FXdblToReal(a1Float[i]);
        }

    // Time how long it takes to divide all MAXITER floats

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = 1.0 / a1Float[i];
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = 1.0 / a1Float[i];
    LZTimerOff();
    cout << MAXITER*2 << " floating point 1.0/'s: \t" << LZTimerCount() << " us\n";

    // Time how long it takes to divide all MAXITER fixed's

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Fixed[i] = FXoneOver(a1Fixed[i]);
    for (i = 0; i < MAXITER; i++)
        a2Fixed[i] = FXoneOver(a1Fixed[i]);
    LZTimerOff();
    cout << MAXITER*2 << " fixed point 1.0/'s: \t" << LZTimerCount() << " us\n";
}

// Shift right timing test (divide by powers of two)

void TimeShiftRight(void)
{
    int     i;

    // Fill the array's with random numbers of the same value

    for (i = 0; i < MAXITER; i++) {
        a1Float[i] = (rand() * 10.0) / RAND_MAX;
        a1Fixed[i] = FXdblToReal(a1Float[i]);
        }

    // Time how long it takes to divide all MAXITER floats by 4

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = a1Float[i] / 4;
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = a1Float[i] / 4;
    LZTimerOff();
    cout << MAXITER*2 << " floating point /4's: \t" << LZTimerCount() << " us\n";

    // Time how long it takes to divide all MAXITER fixed's by 4

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Fixed[i] = a1Fixed[i] >> 2;
    for (i = 0; i < MAXITER; i++)
        a2Fixed[i] = a1Fixed[i] >> 2;
    LZTimerOff();
    cout << MAXITER*2 << " fixed point /4's: \t\t" << LZTimerCount() << " us\n";
}

// Addition timing test

void TimeAdd(void)
{
    int     i;
    float   mFloat = 1.06453;
    FXFixed mFixed = FXdblToReal(mFloat);

    // Fill the array's with random numbers of the same value

    for (i = 0; i < MAXITER; i++) {
        a1Float[i] = (rand() * 10.0) / RAND_MAX;
        a1Fixed[i] = FXdblToReal(a1Float[i]);
        }

    // Time how long it takes to add all MAXITER floats

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = a1Float[i] + mFloat;
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = a1Float[i] + mFloat;
    LZTimerOff();
    cout << MAXITER*2 << " floating point +'s: \t" << LZTimerCount() << " us\n";

    // Time how long it takes to add all MAXITER fixed's

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Fixed[i] = a1Fixed[i] + mFixed;
    for (i = 0; i < MAXITER; i++)
        a2Fixed[i] = a1Fixed[i] + mFixed;
    LZTimerOff();
    cout << MAXITER*2 << " fixed point +'s: \t\t" << LZTimerCount() << " us\n";
}

// Subtraction timing test

void TimeSub(void)
{
    int     i;
    float   mFloat = 1.06453;
    FXFixed mFixed = FXdblToReal(mFloat);

    // Fill the array's with random numbers of the same value

    for (i = 0; i < MAXITER; i++) {
        a1Float[i] = (rand() * 10.0) / RAND_MAX;
        a1Fixed[i] = FXdblToReal(a1Float[i]);
        }

    // Time how long it takes to subtract all MAXITER floats

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = a1Float[i] - mFloat;
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = a1Float[i] - mFloat;
    LZTimerOff();
    cout << MAXITER*2 << " floating point -'s: \t" << LZTimerCount() << " us\n";

    // Time how long it takes to subtract all MAXITER fixed's

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Fixed[i] = a1Fixed[i] - mFixed;
    for (i = 0; i < MAXITER; i++)
        a2Fixed[i] = a1Fixed[i] - mFixed;
    LZTimerOff();
    cout << MAXITER*2 << " fixed point -'s: \t\t" << LZTimerCount() << " us\n";
}

// Ceiling timing test

void TimeCeil(void)
{
    int     i;

    // Fill the array's with random numbers of the same value

    for (i = 0; i < MAXITER; i++) {
        a1Float[i] = (rand() * 10.0) / RAND_MAX;
        a1Fixed[i] = FXdblToReal(a1Float[i]);
        }

    // Time how long it takes to ceil() all MAXITER floats

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = ceil(a1Float[i]);
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = ceil(a1Float[i]);
    LZTimerOff();
    cout << MAXITER*2 << " floating point ceil's: \t" << LZTimerCount() << " us\n";

    // Time how long it takes to ceil() all MAXITER fixed's

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Fixed[i] = FXceil(a1Fixed[i]);
    for (i = 0; i < MAXITER; i++)
        a2Fixed[i] = FXceil(a1Fixed[i]);
    LZTimerOff();
    cout << MAXITER*2 << " fixed point ceil's: \t" << LZTimerCount() << " us\n";
}

// Floor timing test

void TimeFloor(void)
{
    int     i;

    // Fill the array's with random numbers of the same value

    for (i = 0; i < MAXITER; i++) {
        a1Float[i] = (rand() * 10.0) / RAND_MAX;
        a1Fixed[i] = FXdblToReal(a1Float[i]);
        }

    // Time how long it takes to floor() all MAXITER floats

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = floor(a1Float[i]);
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = floor(a1Float[i]);
    LZTimerOff();
    cout << MAXITER*2 << " floating point floor's: \t" << LZTimerCount() << " us\n";

    // Time how long it takes to floor() all MAXITER fixed's

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Fixed[i] = FXfloor(a1Fixed[i]);
    for (i = 0; i < MAXITER; i++)
        a2Fixed[i] = FXfloor(a1Fixed[i]);
    LZTimerOff();
    cout << MAXITER*2 << " fixed point floor's: \t" << LZTimerCount() << " us\n";
}

// Round timing test

void TimeRound(void)
{
    int     i;

    // Fill the array's with random numbers of the same value

    for (i = 0; i < MAXITER; i++) {
        a1Float[i] = (rand() * 10.0) / RAND_MAX;
        a1Fixed[i] = FXdblToReal(a1Float[i]);
        }

    // Time how long it takes to round() all MAXITER floats

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = round(a1Float[i]);
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = round(a1Float[i]);
    LZTimerOff();
    cout << MAXITER*2 << " floating point round's: \t" << LZTimerCount() << " us\n";

    // Time how long it takes to round() all MAXITER fixed's

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Fixed[i] = FXround(a1Fixed[i]);
    for (i = 0; i < MAXITER; i++)
        a2Fixed[i] = FXround(a1Fixed[i]);
    LZTimerOff();
    cout << MAXITER*2 << " fixed point round's: \t" << LZTimerCount() << " us\n";
}

// sin timing test

void TimeSin(void)
{
    int     i;

    // Fill the array's with random numbers of the same value

    for (i = 0; i < MAXITER; i++) {
        a1Float[i] = (rand() * 360.0) / RAND_MAX;
        a1Fixed[i] = FXdblToReal(a1Float[i]);
        }

    // Time how long it takes to do MAXITER floating point sines

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = sin(a1Float[i]);
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = sin(a1Float[i]);
    LZTimerOff();
    cout << MAXITER*2 << " floating point sin's: \t" << LZTimerCount() << " us\n";

    // Time how long it takes to do MAXITER fixed point sines
    // of fixed point numbers

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Fixed[i] = FXsin(a1Fixed[i]);
    for (i = 0; i < MAXITER; i++)
        a2Fixed[i] = FXsin(a1Fixed[i]);
    LZTimerOff();
    cout << MAXITER*2 << " fixed point sin's: \t" << LZTimerCount() << " us\n";
}

// Cos timing test

void TimeCos(void)
{
    int     i;

    // Fill the array's with random numbers of the same value

    for (i = 0; i < MAXITER; i++) {
        a1Float[i] = (rand() * 360.0) / RAND_MAX;
        a1Fixed[i] = FXdblToReal(a1Float[i]);
        }

    // Time how long it takes to do MAXITER floating point cosines.

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = cos(a1Float[i]);
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = cos(a1Float[i]);
    LZTimerOff();
    cout << MAXITER*2 << " floating point cos's: \t" << LZTimerCount() << " us\n";

    // Time how long it takes to do MAXITER fixed point cosines
    // of fixed point numbers

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Fixed[i] = FXcos(a1Fixed[i]);
    for (i = 0; i < MAXITER; i++)
        a2Fixed[i] = FXcos(a1Fixed[i]);
    LZTimerOff();
    cout << MAXITER*2 << " fixed point cos's: \t" << LZTimerCount() << " us\n";
}

// Tan timing test

void TimeTan(void)
{
    int     i;

    // Fill the array's with random numbers of the same value

    for (i = 0; i < MAXITER; i++) {
        a1Float[i] = (rand() * 360.0) / RAND_MAX;
        a1Fixed[i] = FXdblToReal(a1Float[i]);
        }

    // Time how long it takes to do MAXITER floating point cosines.

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = tan(a1Float[i]);
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = tan(a1Float[i]);
    LZTimerOff();
    cout << MAXITER*2 << " floating point tan's: \t" << LZTimerCount() << " us\n";

    // Time how long it takes to do MAXITER fixed point cosines
    // of fixed point numbers

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Fixed[i] = FXtan(a1Fixed[i]);
    for (i = 0; i < MAXITER; i++)
        a2Fixed[i] = FXtan(a1Fixed[i]);
    LZTimerOff();
    cout << MAXITER*2 << " fixed point tan's: \t" << LZTimerCount() << " us\n";
}

// Relational timing test

void TimeRelational(void)
{
    int     i;

    // Fill the array's with random numbers of the same value

    for (i = 0; i <= MAXITER; i++) {
        a1Float[i] = (rand() * 10.0) / RAND_MAX;
        a1Fixed[i] = FXdblToReal(a1Float[i]);
        }

    // Time how long it takes to do MAXITER floating point comparisons

    LZTimerOn();
    for (i = 0; i < MAXITER; i++) {
        if (a1Float[i] <= a1Float[i+1])
            a2Float[i] = a1Float[i];
        else
            a2Float[i] = a1Float[i+1];
        }
    for (i = 0; i < MAXITER; i++) {
        if (a1Float[i] <= a1Float[i+1])
            a2Float[i] = a1Float[i];
        else
            a2Float[i] = a1Float[i+1];
        }
    LZTimerOff();
    cout << MAXITER*2 << " floating point <='s: \t" << LZTimerCount() << " us\n";

    // Time how long it takes to do MAXITER fixed point comparisons

    LZTimerOn();
    for (i = 0; i < MAXITER; i++) {
        if (a1Fixed[i] <= a1Fixed[i+1])
            a2Fixed[i] = a1Fixed[i];
        else
            a2Fixed[i] = a1Fixed[i+1];
        }
    for (i = 0; i < MAXITER; i++) {
        if (a1Fixed[i] <= a1Fixed[i+1])
            a2Fixed[i] = a1Fixed[i];
        else
            a2Fixed[i] = a1Fixed[i+1];
        }
    LZTimerOff();
    cout << MAXITER*2 << " fixed point <='s: \t\t" << LZTimerCount() << " us\n";
}

void TimeSqrt(void)
{
    int     i;

    // Fill the array with random numbers of the same value

    for (i = 0; i < MAXITER; i++) {
        a1Float[i] = (rand() * 10.0) / RAND_MAX;
        a1Fixed[i] = FXdblToReal(a1Float[i]);
        }

    // Time how long it takes to sqrt() all the numbers (standard)

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = sqrt(a1Float[i]);
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = sqrt(a1Float[i]);
    LZTimerOff();
    cout << MAXITER*2 << " floating point sqrt()'s:\t" << LZTimerCount() << " us\n";

    // Time how long it takes to sqrt() all the fixed point numbers

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Fixed[i] = FXsqrt(a1Fixed[i]);
    for (i = 0; i < MAXITER; i++)
        a2Fixed[i] = FXsqrt(a1Fixed[i]);
    LZTimerOff();
    cout << MAXITER*2 << " fixed point sqrt()'s:\t" << LZTimerCount() << " us\n";

#ifdef  CHECK_ERROR
    // Work out the percentage error encountered

    float error = 0.0;
    for (i = 0; i < MAXITER; i++)
        error += fabs(a2Float[i] - FXrealToDbl(a2Fixed[i])) / a2Float[i];

    cout.precision(4);
    cout << "percentage error: " << (error / MAXITER) * 100.0 << " %\n";
#endif
}

void TimePow(void)
{
    int     i;

    // Fill the array's with random numbers of the same value

    for (i = 0; i <= MAXITER; i++) {
        a1Float[i] = (rand() * 10.0) / RAND_MAX;
        a1Fixed[i] = FXdblToReal(a1Float[i]);
        }

    // Time how long it takes to do MAXITER floating point pow's

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = pow(a1Float[i],a1Float[i+1]);
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = pow(a1Float[i],a1Float[i+1]);
    LZTimerOff();
    cout << MAXITER*2 << " floating point pow's: \t" << LZTimerCount() << " us\n";

    // Time how long it takes to do MAXITER fixed point pow's

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Fixed[i] = FXpow(a1Fixed[i],a1Fixed[i+1]);
    for (i = 0; i < MAXITER; i++)
        a2Fixed[i] = FXpow(a1Fixed[i],a1Fixed[i+1]);
    LZTimerOff();
    cout << MAXITER*2 << " fixed point pow's: \t" << LZTimerCount() << " us\n";

#ifdef  CHECK_ERROR
    // Work out the percentage error encountered

    float error = 0.0;
    for (i = 0; i < MAXITER; i++)
        error += fabs(a2Float[i] - FXrealToDbl(a2Fixed[i])) / a2Float[i];

    cout.precision(2);
    cout << "percentage error: " << (error / MAXITER) * 100.0 << " %\n";
#endif
}

void TimeLog(void)
{
    int     i;

    // Fill the array's with random numbers of the same value

    for (i = 0; i < MAXITER; i++) {
        a1Float[i] = fabs((rand() * 10.0) / RAND_MAX);
        a1Fixed[i] = FXdblToReal(a1Float[i]);
        }

    // Time how long it takes to log() all MAXITER floats

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = log(a1Float[i]);
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = log(a1Float[i]);
    LZTimerOff();
    cout << MAXITER*2 << " floating point log's: \t" << LZTimerCount() << " us\n";

    // Time how long it takes to log() all MAXITER fixed's

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Fixed[i] = FXlog(a1Fixed[i]);
    for (i = 0; i < MAXITER; i++)
        a2Fixed[i] = FXlog(a1Fixed[i]);
    LZTimerOff();
    cout << MAXITER*2 << " fixed point log's: \t" << LZTimerCount() << " us\n";

#ifdef  CHECK_ERROR
    // Work out the percentage error encountered

    float error = 0.0;
    for (i = 0; i < MAXITER; i++)
        error += fabs(a2Float[i] - FXrealToDbl(a2Fixed[i])) / a2Float[i];

    cout.precision(4);
    cout << "percentage error: " << (error / MAXITER) * 100.0 << " %\n";
#endif
}

void TimeLog10(void)
{
    int     i;

    // Fill the array's with random numbers of the same value

    for (i = 0; i < MAXITER; i++) {
        a1Float[i] = fabs((rand() * 10.0) / RAND_MAX);
        a1Fixed[i] = FXdblToReal(a1Float[i]);
        }

    // Time how long it takes to log10() all MAXITER floats

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = log10(a1Float[i]);
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = log10(a1Float[i]);
    LZTimerOff();
    cout << MAXITER*2 << " floating point log10's: \t" << LZTimerCount() << " us\n";

    // Time how long it takes to log10() all MAXITER fixed's

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Fixed[i] = FXlog10(a1Fixed[i]);
    for (i = 0; i < MAXITER; i++)
        a2Fixed[i] = FXlog10(a1Fixed[i]);
    LZTimerOff();
    cout << MAXITER*2 << " fixed point log10's: \t" << LZTimerCount() << " us\n";

#ifdef  CHECK_ERROR
    // Work out the percentage error encountered

    float error = 0.0;
    for (i = 0; i < MAXITER; i++)
        error += fabs(a2Float[i] - FXrealToDbl(a2Fixed[i])) / a2Float[i];

    cout.precision(4);
    cout << "percentage error: " << (error / MAXITER) * 100.0 << " %\n";
#endif
}

void main(void)
{
    ZTimerInit();

    MultDivTest();
    ZDivTest();
    CeilFloorTest();
    SinCosTanTest();
    SqrtTest();
    PowTest();
    Log10Test();
    LogTest();

    srand(100);
    TimeMult();
    TimeMulteq();
    TimeMulDiv();
    TimeSquare();
    TimeShiftLeft();
    TimeDiv();
    TimeDiveq();
    TimeZDiv();
    TimeOneOver();
    TimeShiftRight();
    TimeAdd();
    TimeSub();
    TimeCeil();
    TimeFloor();
    TimeRound();
    TimeSin();
    TimeCos();
    TimeTan();
    TimeRelational();
    TimeSqrt();
    TimePow();
    TimeLog10();
    TimeLog();
}
