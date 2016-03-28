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
* Language:     ANSI C
* Environment:  any
*
* Description:  A simple text file translator to translate text files
*               between UNIX and MS-DOS and to optionally expand all tabs
*               to the correct number of space.
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include "pmapi.h"
#include "cmdline.h"
#if defined(__QNX__)
#include <sys/stat.h>
#elif   defined(__UNIX__)
#include <sys/stat.h>
extern int unlink(const char *name);
#ifndef _MAX_DIR
#define _MAX_DIR        255
#define _MAX_FNAME      255
#define _MAX_PATH       255
#endif
#else
#include <sys/stat.h>
#include <io.h>
#include <process.h>
#if defined(__MSDOS__) || defined(__WINDOWS__)
#include <dos.h>
#endif
#endif

#define MAXLINELENGTH   768         /* Maximum formatted line length    */
#define MAXTABSIZE      40          /* Maximum number of chars per tab  */
#define BUFSIZE         30*1024     /* Buffer size for read and writes  */
#define MAX_FILES       1000        /* 1000 files maximum               */

/*------------------------- Global variables ------------------------------*/

char    *version = "3.1";           /* Version string (eg: 5.20b)       */
char    nameofus[_MAX_FNAME];       /* Name of program (no path)        */
char    pathtous[_MAX_DIR];         /* Pathname to our program.         */
char    *progname;                  /* Descriptive name of program      */

char    *inbuf;                     /* Buffer to hold input data        */
char    *outbuf;                    /* Buffer to hold output data       */
int     inpos;                      /* Current position in input buffer */
int     outpos;                     /* Current position in output buffer*/
int     inlen;                      /* Length of input buffer           */
char    *filenames[MAX_FILES];

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
    /* Obtain the path to our program from pathname - note that we only */
    /* do this for MS DOS machines. Under UNIX this is not available    */
    /* since argv[0] holds the name of the program without the path     */
    /* attached. We set pathtous to an empty string under UNIX, and     */
    /* nameofus to the value of argv[0].                                */

#if defined(__MSDOS__) || defined(__WINDOWS__)
    char    *p;
    int     i;

    p = strrchr(argv0,'\\') + 1;
    i = p-argv0;
    strncpy(pathtous,argv0,i);
    pathtous[i] = '\0';

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
    printf("%s  Version %s - %s  Copyright (C) 1992 Kendall Bennett\n\n"
        ,progname,version,__DATE__);
}

void help(void)
/****************************************************************************
*
* Function:     help
*
* Description:  Help provides usage information about our program if the
*               options do make any sense.
*
****************************************************************************/
{
    banner();
    printf("Usage: %s -udthlc [-f<name>] [files(s)]\n\n",nameofus);
    printf("%s translates text files from one format to another according to various\n",progname);
    printf("options.\n\n");
    printf("Options are:\n");
    printf("  -u       - Convert to UNIX format. CR/LF's are converted to LF's\n");
    printf("  -d       - Convert to DOS format. LF's are converted to CR/LF's\n");
    printf("  -a       - Convert to Mac to DOS format. CR's are converted to CR/LF's\n");
    printf("  -m       - Strip CR's from the file\n");
    printf("  -s       - Filter Ctrl-D's from postscript files\n");
    printf("  -l       - Convert all characters to lowercase\n");
    printf("  -c       - Convert all characters to uppercase (captials)\n");
    printf("  -txx     - Convert TABS to xx spaces. Maximum is %d\n",MAXTABSIZE);
    printf("  -f<name> - Read names of files to format from file <name>\n");
    printf("  -h       - Display help screen (this screen)\n");
    exit(1);
}

int getline(FILE *infile,char **line,int *length,int *nl,int mac_format)
/****************************************************************************
*
* Function:     getline
* Parameters:   infile  - stream to obtain input from.
*               line    - buffer to hold text.
*               length  - variable reference to hold buffer length.
*               nl      - True when newline was encountered.
*
* Description:  Reads a line of text from the input buffer. if the input
*               buffer is exhausted, we read a new chunk from the input
*               file.
*
****************************************************************************/
{
    int         i;

    if (inpos == 0)
        inlen = fread(inbuf,1,BUFSIZE,infile);

    *line = &(inbuf[inpos]);
    for (i = 0,*nl = false; i < inlen - inpos; i++) {
        if ((*line)[i] == '\n') {
            i++;
            *nl = true;
            break;
            }
        if (mac_format && (*line)[i] == '\r') {
            i++;
            *nl = true;
            break;
            }
        }
    inpos += i;
    *length = i;
    if (inpos == inlen)
        inpos = 0;
    return (inpos == 0 && inlen != BUFSIZE);
}

void outline(FILE *outfile,char *line,int length)
/****************************************************************************
*
* Function:     outline
* Parameters:   outfile     - Stream to obtain input from
*               line        - Text to output to file.
*               length      - Buffer length.
*
* Description:  Outputs a line of text to the output buffer. If the buffer
*               is full, it is flushed to the output file.
*
****************************************************************************/
{
    int         i;

    if ((outpos + length) > BUFSIZE) {
        fwrite(outbuf,outpos,1,outfile);
        outpos = 0;
        }

    for (i = 0; i < length; i++)
        outbuf[outpos+i] = line[i];

    outpos += length;
}

ibool format_text(FILE *infile,FILE *outfile,int tab_size,int unix_format,
    int dos_format,int mac_format,int cr_format,int d_format,int all_lower,
    int all_caps)
/****************************************************************************
*
* Function:     format_text
* Parameters:   infile      -   stream to obtain intput from.
*               outfile     -   stream to send output to.
*               tab_size    -   size of tab chars when expanded.
*               unix_format -   convert CR/LF's to LF's
*               dos_format  -   convert LF's to CR/LF's
*               mac_format  -   convert CR's to CR/LF's
*               cr_format   -   strip CR's from file
*               d_format    -   Filter Ctrl-D's from file
* Returns:      True on successful format, false if lines were too long.
*
* Description:  Takes the text from the input file, reformats it and sends
*               it to the output file. The reformatting consists of:
*
*                   -   converting all tabs to a variable number of spaces
*                       so that text is tabbed out to the correct position.
*
*                   -   converting CR/LF combination to single LF's for
*                       sending DOS files to UNIX systems.
*
*                   -   converting LF's to CR/LF combinations for sending
*                       UNIX files to DOS systems.
*
****************************************************************************/
{
    int     i,j;                    /* Loop counters                    */
    char    *in;                    /* Pointer to input line of text    */
    char    out[MAXLINELENGTH+2];   /* Buffer to hold output line       */
    int     length;                 /* Length of buffer                 */
    int     done;                   /* flags when finished              */
    int     linepos;                /* Current position in output line  */
    int     spaces;                 /* Spaces to print for tab          */
    int     newline;

    inpos = outpos = inlen = linepos = 0;
    do {
        done = getline(infile,&in,&length,&newline,mac_format);
        if (length > MAXLINELENGTH)
            return false;
        for (i = 0; i < length; i++) {
            if (all_lower)
                in[i] = tolower(in[i]);
            if (all_caps)
                in[i] = toupper(in[i]);
            switch (in[i]) {
                case '\t':
                    if (tab_size) {
                        spaces = (tab_size - (linepos % tab_size));
                        for (j = 0; j < spaces; j++)
                            out[linepos++] = ' ';
                        }
                    else
                        out[linepos++] = '\t';
                    break;
                case '\r':
                    if (unix_format && (in[i+1] == '\n')) {
                        out[linepos++] = '\n';
                        i++;
                        }
                    else if (mac_format) {
                        out[linepos++] = '\r';
                        out[linepos++] = '\n';
                        }
                    else if (!cr_format)
                        out[linepos++] = in[i];
                    break;
                case '\n':
                    if (dos_format) {
                        out[linepos++] = '\r';
                        out[linepos++] = '\n';
                        }
                    else
                        out[linepos++] = in[i];
                    break;
                case '\04':
                    if (!d_format)
                        out[linepos++] = in[i];
                    break;
                case '\x1A':
                    if (!d_format)
                        out[linepos++] = in[i];
                    break;
                default:
                    out[linepos++] = in[i];
                }
            }
        if (newline || done) {
            outline(outfile,out,linepos);
            linepos = 0;
            }
        } while(!done);
    fwrite(outbuf,outpos,1,outfile);
    return true;
}

int openfile(FILE **in,char *filename,char *mode)
{
    if( (*in = fopen(filename,mode) ) == NULL) {
        return false;   /* Open failed                                  */
        }
    else
        return true;    /* Open was successful                          */
}

void readfilenames(char *name,char *filenames[],int *numfiles)
/****************************************************************************
*
* Function:     readfilenames
* Parameters:   name        - Name of file to read filenames from
*               filenames[] - Array to place filenames in
*               numfiles    - Number of filenames read
*
* Description:  Reads the names of the files to translate from the
*               specified file 'name'.
*
****************************************************************************/
{
    char    buf[_MAX_PATH];
    FILE    *f;

    if (!openfile(&f,name,"r")) {
        printf("Unable to open the file: %s\n",name);
        exit(1);
        }

    while (!feof(f) && (fgets(buf,_MAX_PATH,f) != NULL)) {
        if (buf[strlen(buf) - 1] == '\n')
            buf[strlen(buf) - 1] = '\0';
        filenames[*numfiles] = strdup(buf);
#if defined(__MSDOS__) || defined(__WINDOWS__)
        strlwr(filenames[*numfiles]);
#endif
        (*numfiles)++;
        if (*numfiles == MAX_FILES) {
            printf("Too many input files!\n");
            exit(1);
            }
        }

    fclose(f);
}

static ibool loadfiles(const char *mask,int *numfiles)
/****************************************************************************
*
* Function:     loadfiles
* Parameters:   filename    - Name of files to look for
*               fileList    - Place to store the filenames
* Returns:      True on success, false on memory error
*
* Description:  Loads a list of all the filenames from the current
*               directory into the specified name list. Does not include
*               any subdirectories in the list.
*
****************************************************************************/
{
    PM_findData findData;
    int         done;
    void        *hfile;

    findData.dwSize = sizeof(findData);
    done = (hfile = PM_findFirstFile(mask,&findData)) == PM_FILE_INVALID;
    while (!done) {
        if ((strcmp(findData.name,".") != 0) && (strcmp(findData.name,"..") != 0) && !(findData.attrib & PM_FILE_DIRECTORY)) {
            filenames[*numfiles] = strdup(findData.name);
#if defined(__MSDOS__) || defined(__WINDOWS__)
            strlwr(filenames[*numfiles]);
#endif
            (*numfiles)++;
            if (*numfiles == MAX_FILES) {
                printf("Too many input files!\n");
                exit(1);
                }
            }
        done = !PM_findNextFile(hfile,&findData);
        }
    if (hfile != PM_FILE_INVALID)
        PM_findClose(hfile);
    return true;
}

ibool checkstatus(char *filename)
/****************************************************************************
*
* Function:     checkstatus
* Parameters:   filename    - Name of file to check
* Returns:      True if file is of the required type.
*
* Description:  Checks to see that the file 'filename' is a regular file,
*               with both read and write permissions. We output a diagnostic
*               message if it is not.
*
****************************************************************************/
{
    struct stat st;
    short       mask;

    if (stat(filename,&st) == -1) {
        printf("Unable to open '%s' - skipping\n",filename);
        return false;
        }

    /* Skip over directories without saying anything */

    if (st.st_mode & S_IFDIR)
        return false;

    /* Check to make sure it is a regular file with both read and write
     * access permissions. We have to check this separately, as checking
     * them altogether does not work on some UNIX systems.
     */

    if (!(st.st_mode & S_IFREG)) {
        printf("Not a regular file '%s' - skipping\n",filename);
        return false;
        }

    mask = S_IREAD | S_IWRITE;
    if ((st.st_mode & mask) != mask) {
        printf("No write access on '%s' - skipping\n",filename);
        return false;
        }
    return true;
}

int main(int argc,char *argv[])
{
    int     option;             /* Option returned by getopt            */
    char    *argument;          /* Argument returned by getopt          */
    int     tab_size;           /* Default tab size                     */
    int     dos_format;         /* Convert to DOS format                */
    int     unix_format;        /* Convert to UNIX format               */
    int     mac_format;         /* Convert from Mac to DOS format       */
    int     cr_format;          /* Strip CR's from file                 */
    int     d_format;           /* Filter control D's from file         */
    int     all_lower;          /* Convert to lowercase?                */
    int     all_caps;           /* Convert to uppercase?                */
    FILE    *infile;
    FILE    *outfile;
    char    *outname;
    int     numfiles,i;

    init(argv[0],"Trans");
    tab_size = numfiles = 0;
    d_format = unix_format = dos_format = mac_format = cr_format = false;
    all_lower = all_caps = false;

    /* Parse the command line for format specifier options              */

    do {
        option = getcmdopt(argc,argv,"uUdDaAhHmMsSt:T:f:F:lLcC",&argument);
        if (option >= 0)
            option = tolower(option);
        switch(option) {
            case 'u':
                unix_format = true;
                dos_format = false;
                mac_format = false;
                cr_format = false;
                break;
            case 'd':
                dos_format = true;
                unix_format = false;
                mac_format = false;
                cr_format = false;
                break;
            case 'a':
                dos_format = false;
                unix_format = false;
                mac_format = true;
                cr_format = false;
                break;
            case 's':
                d_format = true;
                break;
            case 'l':
                all_lower = true;
                all_caps = false;
                break;
            case 'c':
                all_lower = false;
                all_caps = true;
                break;
            case 'm':
                cr_format = true;
                dos_format = false;
                unix_format = false;
                break;
            case 'h':
                help();
                break;
            case 't':
                if((tab_size = atoi(argument)) <= 0 || tab_size > MAXTABSIZE) {
                    fputs("Invalid tab size\a\n",stderr);
                    exit(1);
                    }
                break;
            case 'f':
                readfilenames(argument,filenames,&numfiles);
                break;
            case INVALID:
                fputs("Invalid option\a\n",stderr);
                exit(1);
                break;
            }
        } while (option != ALLDONE && option != PARAMETER);

    /* The user must at least inform us of what they intend to be done to
     * the input file!
     */

    if (!tab_size && !unix_format && !dos_format && !mac_format && !cr_format
            && !d_format && !all_lower && !all_caps)
        help();

    /* Ensure that we at least have on file to translate                */

    if (numfiles == 0 && (argc - nextargv) < 1)
        help();

    /* If the user has not provided the file names using the -f option,
     * we must get them from the command line...
     */

    while (nextargv < argc)
        loadfiles(argv[nextargv++],&numfiles);

    /* Allocate room for buffers */

    inbuf = malloc(BUFSIZE);
    outbuf = malloc(BUFSIZE);
    if (inbuf == NULL || outbuf == NULL) {
        printf("Not enough memory.\a\n");
        exit(1);
        }

    banner();

    /* Damned tmpnam does not work on DOS, and under Unix it puts the files
     * in the temporary directory.
     */
/*  outname = tmpnam(NULL);
    if (outname == NULL)*/
        outname = "__tran__.xxx";

    for (i = 0; i < numfiles; i++) {
        /* Obtain information about the file and ensure that it is really
         * an ordinary file, that has both read and write permissions.
         */

        if (!checkstatus(filenames[i]))
            continue;

        if (!openfile(&outfile,outname,"wb")) {
            printf("Cannot create output file .. aborting\n");
            exit(1);
            }
        openfile(&infile,filenames[i],"rb");

        printf("Formatting '%s' ... ",filenames[i]);
        fflush(stdout);

        /* Format the text */

        if (!format_text(infile,outfile,tab_size,unix_format,dos_format,
                mac_format,cr_format,d_format,all_lower,all_caps)) {
            printf("lines too long - skipping.\n");
            fclose(infile);
            fclose(outfile);
            unlink(outname);
            continue;
            }

        fclose(infile);         /* Close any opened files */
        fclose(outfile);
        unlink(filenames[i]);
        rename(outname,filenames[i]);
        printf("done.\n");
        }
    return 0;
}
