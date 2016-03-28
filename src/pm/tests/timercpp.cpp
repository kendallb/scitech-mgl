/****************************************************************************
*
*                   SciTech OS Portability Manager Library
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
* Environment:  Any
*
* Description:  Test program for the Zen Timer Library C++ interface.
*
****************************************************************************/

#include <iostream.h>
#include "pmapi.h"
#include "ztimer.h"

/*-------------------------- Implementation -------------------------------*/

int     i,j,k;                              /* NON register variables! */

void dummy() {}

int main(void)
{
    LZTimer     ltimer;
    ULZTimer    ultimer;

    ZTimerInit();

    /* Test the long period Zen Timer (we don't check for overflow coz
     * it would take tooooo long!)
     */

    cout << endl;
    ultimer.restart();
    ltimer.start();
    for (j = 0; j < 10; j++)
        for (i = 0; i < 20000; i++)
            dummy();
    ltimer.stop();
    ultimer.stop();
    cout << "LCount:  " << ltimer.count() << endl;
    cout << "Time:    " << ltimer << " secs\n";
    cout << "ULCount: " << ultimer.count() << endl;
    cout << "ULTime:  " << ultimer << " secs\n";

    cout << endl << "Timing ... \n";
    ultimer.restart();
    ltimer.restart();
    for (j = 0; j < 200; j++)
        for (i = 0; i < 20000; i++)
            dummy();
    ltimer.stop();
    ultimer.stop();
    cout << "LCount:  " << ltimer.count() << endl;
    cout << "Time:    " << ltimer << " secs\n";
    cout << "ULCount: " << ultimer.count() << endl;
    cout << "ULTime:  " << ultimer << " secs\n";

    /* Test the lap function of the long period Zen Timer */

    cout << endl << "Timing ... \n";
    ultimer.restart();
    ltimer.restart();
    for (j = 0; j < 20; j++) {
        for (k = 0; k < 10; k++)
            for (i = 0; i < 20000; i++)
                dummy();
        cout << "lap: " << ltimer.lap() << endl;
        }
    ltimer.stop();
    ultimer.stop();
    cout << "LCount:  " << ltimer.count() << endl;
    cout << "Time:    " << ltimer << " secs\n";
    cout << "ULCount: " << ultimer.count() << endl;
    cout << "ULTime:  " << ultimer << " secs\n";

#ifdef  LONG_TEST
    /* Test the ultra long period Zen Timer */

    ultimer.start();
    delay(DELAY_SECS * 1000);
    ultimer.stop();
    cout << "Delay of " << DELAY_SECS << " secs took " << ultimer.count()
         << " 1/10ths of a second\n";
    cout << "Time: " << ultimer << " secs\n";
#endif
    return 0;
}
