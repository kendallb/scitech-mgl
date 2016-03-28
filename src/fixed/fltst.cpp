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
* Description:  Test program for the floating point routines. Times the
*               high performance floating point routines in comparison to
*               standard floating point routines.
*
****************************************************************************/

#include <iostream.h>
#include <iomanip.h>
#include <stdlib.h>
#include "fx/float.h"
#include "ztimer.h"

#define MAXITER 2000

real    f1,f2,f3,f4;

int     ints[MAXITER+1];
real    a1Float[MAXITER+1];     // Array of floating point numbers
real    a2Float[MAXITER+1];     // Array of floating point numbers
real    a3Float[MAXITER+1];

void SinCosTanTest(void)
{
    for (real angle = 0; angle < 360; angle += 10) {
        f1 = FXsin(angle);
        f2 = FXcos(angle);
        f3 = FXtan(angle);

        cout << "FXsin(" << angle << ") = " << f1
             << ",FXcos(" << angle << ") = " << f2
             << ",FXtan(" << angle << ") = " << f3 << endl;
        }
}

void SqrtTest(void)
{
    double  f1;

    f1 = 1.0;
    cout << "sqrt() test:\n";
    while (f1 != 0.0) {
        cout << "enter value: ";
        cin >> f1;
        cout << "sqrt(" << f1 << ") = " << sqrt(f1);
        cout << ", FXsqrt(" << f1 << ") = " << FXsqrt(f1) << endl;
        }
}

void Log10Test(void)
{
    double  f1;

    f1 = 1.0;
    cout << "log10() test:\n";
    while (1) {
        cout << "enter value: ";
        cin >> f1;
        if (f1 == 0.0)
            break;
        cout << "log10(" << f1 << ") = " << log10(f1);
        cout << ", FXlog10(" << f1 << ") = " << FXlog10(f1) << endl;
        }
}

void LogTest(void)
{
    double  f1;

    f1 = 1.0;
    cout << "log() test:\n";
    while (1) {
        cout << "enter value: ";
        cin >> f1;
        if (f1 == 0.0)
            break;
        cout << "log(" << f1 << ") = " << log(f1);
        cout << ", FXlog(" << f1 << ") = " << FXlog(f1) << endl;
        }
}

// sin timing test

void TimeSin(void)
{
    int     i;

    // Fill the array's with random numbers of the same value
    for (i = 0; i < MAXITER; i++)
        a1Float[i] = (rand() * 360.0) / RAND_MAX;

    // Time how long it takes to convert all floats to integers
    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        ints[i] = a1Float[i];
    LZTimerOff();
    cout << MAXITER << " (int)'s: \t\t" << LZTimerCount() << " us\n";

    // Time how long it takes to fast convert all floats to integers
    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        ints[i] = FXrealToInt(a1Float[i]);
    LZTimerOff();
    cout << MAXITER << " FXrealToInt's: \t" << LZTimerCount() << " us\n";

    // Time how long it takes to fast round all floats to integers
    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        ints[i] = FXrndToInt(a1Float[i]);
    LZTimerOff();
    cout << MAXITER << " FXrndToInt's: \t" << LZTimerCount() << " us\n";

    // Time how long it takes to do MAXITER floating point sines
    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = sin(a1Float[i] * (M_PI / 180));
    LZTimerOff();
    cout << MAXITER << " sin's: \t\t" << LZTimerCount() << " us\n";

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = FXsin(a1Float[i]);
    LZTimerOff();
    cout << MAXITER << " FXsin's: \t\t" << LZTimerCount() << " us\n";
}

// Cos timing test

void TimeCos(void)
{
    int     i;

    // Fill the array's with random numbers of the same value
    for (i = 0; i < MAXITER; i++)
        a1Float[i] = (rand() * 360.0) / RAND_MAX;

    // Time how long it takes to do MAXITER floating point cosines.
    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = cos(a1Float[i] * (M_PI / 180));
    LZTimerOff();
    cout << MAXITER << " cos's: \t\t" << LZTimerCount() << " us\n";

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = FXcos(a1Float[i]);
    LZTimerOff();
    cout << MAXITER << " FXcos's: \t\t" << LZTimerCount() << " us\n";
}

// SinCos timing test

void TimeSinCos(void)
{
    int     i;

    // Fill the array's with random numbers of the same value
    for (i = 0; i < MAXITER; i++)
        a1Float[i] = (rand() * 360.0) / RAND_MAX;

    // Time how long it takes to do MAXITER floating point sines
    LZTimerOn();
    for (i = 0; i < MAXITER; i++) {
        a2Float[i] = sin(a1Float[i] * (M_PI / 180));
        a3Float[i] = cos(a1Float[i] * (M_PI / 180));
        }
    LZTimerOff();
    cout << MAXITER << " sincos's: \t\t" << LZTimerCount() << " us\n";

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        FXsincos(a1Float[i],&a2Float[i],&a3Float[i]);
    LZTimerOff();
    cout << MAXITER << " FXsincos's: \t" << LZTimerCount() << " us\n";
}

// Tan timing test

void TimeTan(void)
{
    int     i;

    // Fill the array's with random numbers of the same value
    for (i = 0; i < MAXITER; i++)
        a1Float[i] = (rand() * 360.0) / RAND_MAX;

    // Time how long it takes to do MAXITER floating point cosines.

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = tan(a1Float[i] * (M_PI / 180));
    LZTimerOff();
    cout << MAXITER << " tan's: \t\t" << LZTimerCount() << " us\n";

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = FXtan(a1Float[i]);
    LZTimerOff();
    cout << MAXITER << " FXtan's: \t\t" << LZTimerCount() << " us\n";
}

void TimeSqrt(void)
{
    int     i;

    // Fill the array with random numbers of the same value
    for (i = 0; i < MAXITER; i++)
        a1Float[i] = (rand() * 10.0) / RAND_MAX;

    // Time how long it takes to sqrt() all the numbers (standard)
    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = sqrt(a1Float[i]);
    LZTimerOff();
    cout << MAXITER << " sqrt()'s:\t\t" << LZTimerCount() << " us\n";

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = FXsqrt(a1Float[i]);
    LZTimerOff();
    cout << MAXITER << " FXsqrt()'s:\t" << LZTimerCount() << " us\n";
}

void TimeLog(void)
{
    int     i;

    // Fill the array's with random numbers of the same value
    for (i = 0; i < MAXITER; i++)
        a1Float[i] = fabs((rand() * 10.0) / RAND_MAX);

    // Time how long it takes to log() all MAXITER floats
    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = log(a1Float[i]);
    LZTimerOff();
    cout << MAXITER << " log's: \t\t" << LZTimerCount() << " us\n";

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = FXlog(a1Float[i]);
    LZTimerOff();
    cout << MAXITER << " FXlog's: \t\t" << LZTimerCount() << " us\n";
}

void TimeLog10(void)
{
    int     i;

    // Fill the array's with random numbers of the same value
    for (i = 0; i < MAXITER; i++) {
        a1Float[i] = fabs((rand() * 10.0) / RAND_MAX);
        a1Float[i] = FXdblToReal(a1Float[i]);
        }

    // Time how long it takes to log10() all MAXITER floats
    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = log10(a1Float[i]);
    LZTimerOff();
    cout << MAXITER << " log10's: \t\t" << LZTimerCount() << " us\n";

    LZTimerOn();
    for (i = 0; i < MAXITER; i++)
        a2Float[i] = FXlog10(a1Float[i]);
    LZTimerOff();
    cout << MAXITER << " FXlog10's: \t" << LZTimerCount() << " us\n";
}

void main(void)
{
    ZTimerInit();

    SinCosTanTest();
    SqrtTest();
    Log10Test();
    LogTest();

    srand(100);
    TimeSin();
    TimeCos();
    TimeSinCos();
    TimeTan();
    TimeSqrt();
    TimeLog10();
    TimeLog();
    cout << endl;
}
