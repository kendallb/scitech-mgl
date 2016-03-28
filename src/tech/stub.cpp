/****************************************************************************
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
* Description:  A C++ program stub containing sample code to interface to
*               the options parsing class.
*
****************************************************************************/

#include "tcl/options.hpp"          // TCOptions parsing class
#include <process.h>
#include <string.h>
#include <ctype.h>

#ifdef  __MSDOS__
#include <dir.h>
#endif

//---------------------------------------------------------------------------
// The following class is an example of how to derive a useful options
// parsing class. The one presented here simple tests every possible
// option type. Naturally this would normally be in a header file and a
// separate implementation file.
//---------------------------------------------------------------------------

class MyOpt : public TCOptions {
protected:

            // Protected pure virtual to parse a single parameter
    virtual int doParam(char* param,int num);

public:

    // Most of the data stored in this class should be within easy access
    // to the rest of the program, so it is defined as public rather than
    // protected with member functions for access. It also allows us to
    // create a static array with the address of all the option arguments.

    int     Oint;
    uint    Ohex;
    uint    Ooct;
    uint    Ouint;
    long    Olong;
    ulong   Olhex;
    ulong   Oloct;
    ulong   Oulong;
    float   Ofloat;
    double  Odouble;
    long double Oldouble;
    char    *Ostring;
    ibool   Oswitch;

    char    *param1,*param2;

            // Constructor - builds option array table
            MyOpt();
    };

// Instantiate the options class

MyOpt   options;                    // TCOptions used by this program

// Create a static intialised array of describing how to parse the
// options.

static  TCOption    optarr[] =
    {{'d',OPT_INTEGER,  &options.Oint,"A signed integer argument"},
     {'e',OPT_HEX,      &options.Ohex,"An unsigned hex argument"},
     {'o',OPT_OCTAL,    &options.Ooct,"An unsigned octal argument"},
     {'u',OPT_UNSIGNED, &options.Ouint,"An unsigned integer argument"},
     {'D',OPT_LINTEGER, &options.Olong,"A signed long integer argument"},
     {'E',OPT_LHEX,     &options.Olhex,"An unsigned long hex argument"},
     {'O',OPT_LOCTAL,   &options.Oloct,"An unsigned long octal argument"},
     {'U',OPT_LUNSIGNED,&options.Oulong,"An unsigned long integer argument"},
     {'f',OPT_FLOAT,    &options.Ofloat,"A single precision number"},
     {'F',OPT_DOUBLE,   &options.Odouble,"A double precision number"},
     {'L',OPT_LDOUBLE,  &options.Oldouble,"A long double precision number"},
     {'s',OPT_STRING,   &options.Ostring,"A string argument"},
     {'!',OPT_SWITCH,   &options.Oswitch,"A switch argument"}};

MyOpt::MyOpt()
/****************************************************************************
*
* Function:     MyOpt::MyOpt
*
* Description:  Constructor for the options parsing class. We need to
*               set up the array for parsing the command line and fill
*               in the default value for all the program options.
*
****************************************************************************/
{
    optarr = ::optarr;
    numopt = sizeof(::optarr) / sizeof(TCOption);

    // Fill in default options

    Oint = Ohex = Ooct = Ouint = 0;
    Olong = Olhex = Oloct = Oulong = 0;
    Ofloat = 0.0;
    Odouble = 0.0;
    Oldouble = 0.0;
    Ostring = NULL;
    Oswitch = false;
}

int MyOpt::doParam(char *param,int num)
/****************************************************************************
*
* Function:     MyOpt::doParam
* Parameters:   param   - Parameter string to use
*               num     - Current parameter number
* Returns:      OPT_ALLDONE on success. If an error is encountered,
*               return something other than OPT_ALLDONE and this will
*               be passed back as the return argument from parse().
*
* Description:  Internal routine to parse each parameter on the command
*               line as it is expected.
*
****************************************************************************/
{
    // We simply save the value of the parameter string here. This is all
    // that needs to be done. We can then open files etc using these
    // parameter's after parse() has completed it's work, or we could
    // open the files as each parameter is encountered.

    switch (num) {
        case 1:
            param1 = param;
            break;
        case 2:
            param2 = param;
            break;
        default:
            return OPT_HELP;        // Display help for too many parameters
        }
    return OPT_ALLDONE;
}

/*------------------------- Global variables ------------------------------*/


char    *version = "1.1b";          // Version string (eg: 5.20b)
char    nameofus[MAXFILE];          // Name of program (no path)
char    pathtous[MAXDIR];           // Pathname to our program.
char    *progname;                  // Descriptive name of program

/*-------------------------- Implementation -------------------------------*/

void init(char *argv0,char *prognam)
/****************************************************************************
*                                                                           *
* Function:     init                                                        *
* Parameters:   char    *argv0      - The argv[0] array entry.              *
*               char    *prognam    - Descriptive name of program.          *
*                                                                           *
* Description:  Init takes the pathname to our program as a parameter       *
*               (found in argv[0]) and use this to set up three global      *
*               variables:                                                  *
*                                                                           *
*               pathtous    - Contains the pathname to our program          *
*               nameofus    - Contains the name of the program (without the *
*                             .EXE extension)                               *
*                                                                           *
*               We also set up the global variable progname to point to     *
*               the static string passed to init for all to use.            *
*                                                                           *
****************************************************************************/
{
    char    *p,i;

    // Obtain the path to our program from pathname - note that we only
    // do this for MS DOS machines. Under UNIX this is not available
    // since argv[0] holds the name of the program without the path
    // attached. We set pathtous to an empty string under UNIX, and
    // nameofus to the value of argv[0]. Perhaps we should interogate this
    // from the UNIX host also.

#ifndef __UNIX__
    if ((p = strrchr(argv0,'\\')) == NULL) {
        pathtous[0] = '\0';
        p = argv0;
        }
    else {
        i = *++p;
        *p = '\0';
        strcpy(pathtous,argv0);
        *p = i;
        }

    /* Obtain the name of our program from pathname */

    i = 0;
    while (*p != '.')
        nameofus[i++] = *p++;
    nameofus[i] = '\0';
#else
    strcpy(nameofus,argv0);
    pathtous[0] = '\0';
#endif
    progname = prognam;
}

void banner(void)
/****************************************************************************
*
* Function:     banner
*
* Description:  Prints the program's banner to the standard output
*               Under Borland C++, we insert the compilation date into
*               the banner using the __DATE__ macro. This does not
*               seem to be available under some UNIX systems, so for UNIX
*               we do not insert the date into the banner.
*
****************************************************************************/
{
    cout << progname << "  Version " << version
     MS( << " - " << __DATE__ )
         << "  Copyright (C) 1996 SciTech Software" << endl << endl;
}

void help(void)
/****************************************************************************
*
* Function:     help
*
* Description:  Help provides usage information about our program if the
*               options do make any sense or the help switch was specified
*               on the command line.
*
****************************************************************************/
{
    banner();
    cout << "Usage: " << nameofus << " [options]" << endl << endl
         << "TCOptions are:" << endl << options;
    exit(1);
}

void main(int argc,char **argv)
{
    init(argv[0],"Stub");           // Initialise a few globals

    switch (options.parse(argc,argv)) {
        case OPT_INVALID:
            cout << "Invalid option" << endl;
            exit(1);
            break;
        case OPT_HELP:
            help();
        }

    banner();

    cout << "Integer: " << options.Oint << endl
         << "Hex: " << hex << options.Ohex << endl
         << "Oct: " << oct << options.Ooct << endl
         << "Uint: " << dec << options.Ouint << endl
         << "Long:  " << options.Olong << endl;

    cout << "Lhex: " << hex << options.Olhex << endl
         << "Loct: " << oct << options.Oloct << endl
         << "Ulong: " << dec << options.Oulong << endl
         << "Float: " << options.Ofloat << endl;

    cout << "Double: " << options.Odouble << endl
         << "Ldouble: " << options.Oldouble << endl
         << "String: " << (options.Ostring ? options.Ostring : "NONE") << endl
         << "Switch: " << (options.Oswitch ? "ON" : "OFF") << endl;

    cout << "Param1: " << (options.param1 ? options.param1 : "NONE") << endl
         << "Param2: " << (options.param2 ? options.param2 : "NONE") << endl;

    cout << "Name of us: " << nameofus << endl;
    cout << "Path to us: " << pathtous << endl;
}
