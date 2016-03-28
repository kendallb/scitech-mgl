/****************************************************************************
*
*                           SciTech SNAP Graphics
*
*               Copyright (C) 1991-2003 SciTech Software, Inc.
*                            All rights reserved.
*
*  ======================================================================
*  |REMOVAL OR MODIFICATION OF THIS HEADER IS STRICTLY PROHIBITED BY LAW|
*  |                                                                    |
*  |This copyrighted computer code is a proprietary trade secret of     |
*  |SciTech Software, Inc., located at 505 Wall Street, Chico, CA 95928 |
*  |USA (www.scitechsoft.com).  ANY UNAUTHORIZED POSSESSION, USE,       |
*  |VIEWING, COPYING, MODIFICATION OR DISSEMINATION OF THIS CODE IS     |
*  |STRICTLY PROHIBITED BY LAW.  Unless you have current, express       |
*  |written authorization from SciTech to possess or use this code, you |
*  |may be subject to civil and/or criminal penalties.                  |
*  |                                                                    |
*  |If you received this code in error or you would like to report      |
*  |improper use, please immediately contact SciTech Software, Inc. at  |
*  |530-894-8400.                                                       |
*  |                                                                    |
*  |REMOVAL OR MODIFICATION OF THIS HEADER IS STRICTLY PROHIBITED BY LAW|
*  ======================================================================
*
* Language:     ANSI C
* Environment:  Any
*
* Description:  Module to test the C runtime library functions from within
*               a loaded DLL and the native runtime libraries.
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <setjmp.h>
#include <sys/types.h>
#include <clib/sys/cdecl.h>

#define MAXALLOC    2000

jmp_buf jmp_env;

#ifndef __WATCOMC__
/* Pull in FP support */
extern unsigned _fltused_;
#endif

void _CEXPORT TestMalloc(void)
{
    int     i;
    ulong   allocs;
    char    *p,*pa[MAXALLOC];

    printf("\r\n");
    for (allocs = i = 0; i < MAXALLOC; i++) {
        if (i % 1)
            pa[i] = malloc(10*1024);
        else
            pa[i] = calloc(10*1024,1);
        if (pa[i] != 0) {
            p = pa[allocs];
            memset(p, 0, 10*1024); /* touch every byte              */
            *p = 'x';           /* do something, anything with      */
            p[1023] = 'y';      /* the allocated memory             */
            allocs++;
            printf("Allocated %lu bytes\r", 10*(allocs << 10));
            }
        else
            break;
        }
    printf("\n\nAllocated total of %lu bytes\n", 10 * (allocs << 10));
    for (i = allocs-1; i >= 0; i--)
        free(pa[i]);
}

void _CEXPORT TestMath(void)
{
    double  flt,ipart;
    int     i;

#ifndef __WATCOMC__
    _fltused_ = 0;
#endif
    printf("acos(0.5) = %10.8f\n", acos(0.5));
    printf("asin(0.5) = %10.8f\n", asin(0.5));
    printf("atan(0.5) = %10.8f\n", atan(0.5));
    printf("atan2(1,2) = %10.8f\n", atan2(1,2));
    printf("ceil(0.5) = %10.8f\n", ceil(0.5));
    printf("cos(0.5) = %10.8f\n", cos(0.5));
    printf("cosh(0.5) = %10.8f\n", cosh(0.5));
    printf("exp(0.5) = %10.8f\n", exp(0.5));
    printf("fabs(-5.5) = %10.8f\n", fabs(-5.5));
    printf("floor(0.5) = %10.8f\n", floor(0.5));
    printf("fmod(8,3) = %10.8f\n", fmod(8,3));
    flt = frexp(10.5, &i);
    printf("frexp(10.5) = %10.8f, %d\n", flt, i);
    printf("ldexp(1,5) = %10.8f\n", ldexp(1,5));
    printf("log(5) = %10.8f\n", log(5));
    flt = modf(10.5, &ipart);
    printf("modf(10.5) = %10.8f, %10.8f\n", flt, ipart);
    printf("pow(2,5) = %10.8f\n", pow(2,5));
    printf("sin(0.5) = %10.8f\n", sin(0.5));
    printf("sinh(0.5) = %10.8f\n", sinh(0.5));
    printf("sqrt(25) = %10.8f\n", sqrt(25));
    printf("tan(0.5) = %10.8f\n", tan(0.5));
    printf("log10(0.5) = %10.8f\n", log10(0.5));
}

#if 0
void test_rtn(void)
{
    printf("about to longjmp\n");
    longjmp(jmp_env, 1);
}

void _CEXPORT TestLongjmp(void)
{
    int ret_val;

    if ((ret_val = setjmp(jmp_env)) == 0) {
        printf( "after setjmp, ret_val = %d\n", ret_val );
        test_rtn();
        printf( "back from rtn %d\n", ret_val );
        }
    else {
        printf( "back from longjmp, ret_val = %d\n", ret_val );
        }
}
#endif
