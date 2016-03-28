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
* Environment:  Any
*
* Description:  Simple script driven self-extracting installer creation
*               utility.
*
****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include "scitech.h"  // just to sort out stricmp()
#include "unzip.h"

FILE    *cfgfile, *installfile;
char    *INSTALLER_APPLICATION;
char    *INSTALLER_VERSION;
char    *INSTALLER_TITLE;
char    *INSTALLER_PATH;
char    *INSTALLER_FOLDER;
char    *INSTALLER_PROGRAM;
char    *INSTALLER_SHADOW;
char    *INSTALLER_OBJECT;
char    *INSTALLER_SETS;
char    *INSTALLER_SYSVAR;
char    *INSTALLER_SYSLINE;
char    *INSTALLER_PACKAGES[20];
int     INSTALLER_BITMAP_WIDTH;
int     INSTALLER_BITMAP_HEIGHT;
int     INSTALLER_PACKAGE_COUNT = 0;

int     packagefiles[20];
char    *packagefilename[20];

void append_file(char *filename)
{
    FILE    *appendfile;
    int     amnt;
    char    buffer[512];

    if ((appendfile = fopen(filename, "rb")) == NULL) {
        printf("Error opening %s for reading!\n", filename);
        exit(3);
        }
    while (!feof(appendfile)) {
        amnt = fread(buffer, 1, 512, appendfile);
        fwrite(buffer, 1, amnt, installfile);
        }
    fclose(appendfile);
}

void getline(FILE *f, char *entry, char *entrydata)
{
    char    in[4096];
    int     z;

    memset(in, 0, 4096);
    fgets(in, 4095, f);

    if (in[strlen(in)-1] == '\n')
        in[strlen(in)-1] = 0;

    if (in[0] != '#') {
        for (z = 0; z < strlen(in); z++) {
            if(in[z] == '=') {
                in[z] = 0;
                strcpy(entry, in);
                strcpy(entrydata, &in[z+1]);
                return;
                }
            }
        }
    strcpy(entry, "");
    strcpy(entrydata, "");
}

int main(int argc, char *argv[])
{
    char            entry[8096];
    char            entrydata[8096];
    int             quietMode = 0;
    int             z;
    unzFile         zf;
    unz_global_info zinfo;

    if (argc != 3 && argc != 4) {
        printf("Packinst version 3.1\n");
        printf("Usage: packinst [-q] <filename.cfg> <installer.exe>\n");
        exit(1);
        }
    if (strcmp(argv[1],"-q") == 0) {
        argv++;
        quietMode = 1;
        }
    if ((cfgfile = fopen(argv[1], "r")) == NULL) {
        printf("Error opening config file \"%s\"!\n", argv[1]);
        exit(2);
        }
    if ((installfile = fopen(argv[2], "wb")) == NULL) {
        printf("Error opening %s for writing!\n", argv[2]);
        exit(3);
        }
    if (!quietMode)
        printf("Creating installer...\n");
    append_file("install.exe");
    fwrite("SXINST-HEADER", 1, 13, installfile);
    while (!feof(cfgfile)) {
        getline(cfgfile, entry, entrydata);
        if (stricmp(entry, "INSTALLER_APPLICATION") == 0)
            INSTALLER_APPLICATION = (char *)strdup(entrydata);
        if (stricmp(entry, "INSTALLER_VERSION") == 0)
            INSTALLER_VERSION = (char *)strdup(entrydata);
        if (stricmp(entry, "INSTALLER_TITLE") == 0)
            INSTALLER_TITLE = (char *)strdup(entrydata);
        if (stricmp(entry, "INSTALLER_PATH") == 0)
            INSTALLER_PATH = (char *)strdup(entrydata);
        if (stricmp(entry, "INSTALLER_FOLDER") == 0)
            INSTALLER_FOLDER = (char *)strdup(entrydata);
        if (stricmp(entry, "INSTALLER_PROGRAM") == 0)
            INSTALLER_PROGRAM = (char *)strdup(entrydata);
        if (stricmp(entry, "INSTALLER_SHADOW") == 0)
            INSTALLER_SHADOW = (char *)strdup(entrydata);
        if (stricmp(entry, "INSTALLER_OBJECT") == 0)
            INSTALLER_OBJECT = (char *)strdup(entrydata);
        if (stricmp(entry, "INSTALLER_SETS") == 0)
            INSTALLER_SETS = (char *)strdup(entrydata);
        if (stricmp(entry, "INSTALLER_SYSVAR") == 0)
            INSTALLER_SYSVAR = (char *)strdup(entrydata);
        if (stricmp(entry, "INSTALLER_SYSLINE") == 0)
            INSTALLER_SYSLINE = (char *)strdup(entrydata);
        if (stricmp(entry, "INSTALLER_PACKAGE") == 0) {
            for (z = 0; z < strlen(entrydata); z++) {
                if (entrydata[z] == ',') {
                    entrydata[z] = 0;
                    packagefilename[INSTALLER_PACKAGE_COUNT] = (char *)strdup(entrydata);
                    INSTALLER_PACKAGES[INSTALLER_PACKAGE_COUNT] = (char *)strdup(&entrydata[z+1]);
                    }
                }
                zf = unzOpen(packagefilename[INSTALLER_PACKAGE_COUNT]);
                if (zf == NULL) {
                    printf("Error opening package file \"%s\"!\n", packagefilename[INSTALLER_PACKAGE_COUNT]);
                    exit(4);
                    }
                unzGetGlobalInfo(zf, &zinfo);
                unzClose(zf);

                packagefiles[INSTALLER_PACKAGE_COUNT] = zinfo.number_entry;
                INSTALLER_PACKAGE_COUNT++;
            }
        if (stricmp(entry, "INSTALLER_BITMAP_WIDTH") == 0)
            INSTALLER_BITMAP_WIDTH = atoi(entrydata);
        if (stricmp(entry, "INSTALLER_BITMAP_HEIGHT") == 0)
            INSTALLER_BITMAP_HEIGHT = atoi(entrydata);
        }
    fclose(cfgfile);
    fprintf(installfile, "%s-%s-%s-%s-%s-%s-%s-%s-%s-%s-%s-%d-%d-%d",
            INSTALLER_APPLICATION, INSTALLER_VERSION, INSTALLER_TITLE,
            INSTALLER_PATH, INSTALLER_FOLDER, INSTALLER_PROGRAM,
            INSTALLER_SHADOW, INSTALLER_OBJECT, INSTALLER_SETS, INSTALLER_SYSVAR,
            INSTALLER_SYSLINE, INSTALLER_BITMAP_WIDTH,
            INSTALLER_BITMAP_HEIGHT, INSTALLER_PACKAGE_COUNT);
    for (z = 0; z < INSTALLER_PACKAGE_COUNT; z++)
        fprintf(installfile, "-%s", INSTALLER_PACKAGES[z]);
    fwrite("~", 1, 1, installfile);
    for (z = 0; z < INSTALLER_PACKAGE_COUNT; z++) {
        struct stat file_stat;

        stat(packagefilename[z], &file_stat);
        fprintf(installfile, "SXINST-ZIP%d%d-%d~", z, packagefiles[z], (int)file_stat.st_size);
        append_file(packagefilename[z]);
        if (!quietMode)
            printf("Package: %s - %d files in %s (%d bytes).\n", INSTALLER_PACKAGES[z], packagefiles[z], packagefilename[z], (int)file_stat.st_size);
        }
    fclose(installfile);
    fclose(cfgfile);
    return 0;
}

