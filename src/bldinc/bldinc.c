/****************************************************************************
*
*                        SciTech MGL Graphics Library
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
* Description:  Simple program to increment the MGL build number, and to
*               mark the changes in the Perforce source code repository.
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mgl\mglbld.h"
#include "mgl\mglver.h"

/*---------------------------- Global Variables ---------------------------*/

const char  *szP4P = "p4p";
const char  *szBuildFile = "c:\\scitech\\include\\mgl\\mglbld.h";
const char  *outname = "_bldinc_.xxx";

const char  *szPublicLabel =
"Label: %s\n"
"\n"
"Owner: KendallB\n"
"\n"
"Description:\n"
"   Label for build of SciTech MGL Graphics Library.\n"
"\n"
"Options:   %s\n"
"\n"
"View:\n"
"   //depot/main/...\n";

const char  *szPublicSubmit =
"Change:    new\n"
"\n"
"Client:    BUILDSERVER\n"
"\n"
"User:  KendallB\n"
"\n"
"Status:    new\n"
"\n"
"Description:\n"
"   *BUILD %s - CHECKIN OF INCREMENTED BUILD NUMBER*\n"
"\n"
"Files:\n"
"   //depot/main/include/mgl/mglbld.h   # add\n";

/*----------------------------- Implementation ----------------------------*/

int createLabel(const char *p4cmd,const char *label,const char *labelTemplate)
{
    FILE    *f;
    char    command[80];

    /* First create a label for this build in Perforce */
    printf("Creating Perforce Label...\n");
    if ((f = fopen(outname,"w")) == NULL)
        return -1;
    fprintf(f, labelTemplate, label, "unlocked");
    fclose(f);
    sprintf(command, "%s label -i < %s", p4cmd, outname);
    if (system(command) != 0)
        return -1;

    /* Sync files in repository to the label */
    printf("Syncing Perforce Label...\n");
    sprintf(command, "%s labelsync -l %s", p4cmd, label);
    if (system(command) != 0)
        return -1;

    /* Lock the label to prevent future updates */
    printf("Locking Perforce Label...\n");
    if ((f = fopen(outname,"w")) == NULL)
        return -1;
    fprintf(f, labelTemplate, label, "locked");
    fclose(f);
    sprintf(command, "%s label -i < %s", p4cmd, outname);
    if (system(command) != 0)
        return -1;
    return 0;
}

int main(void)
{
    FILE    *f;
    char    command[80],label[80];
    int     buildNo;

    /* Find the integer build number */
    buildNo = atoi(BUILD_NUMBER);

    /* Create a label for the build number in both private and public servers */
    sprintf(label, "mgl_build_%s", BUILD_NUMBER);
    if (createLabel(szP4P,label,szPublicLabel) == -1)
        return -1;

#ifndef BETA
    /* Create a label for the release build if not beta */
    sprintf(label, "mgl%s.%s", MGL_RELEASE_MAJOR_STR, MGL_RELEASE_MINOR_STR);
    if (createLabel(szP4P,label,szPublicLabel) == -1)
        return -1;
#endif

    /* Next open the build file number in Perforce and increment it */
    printf("Updating Build Number...\n");
    sprintf(command, "%s open %s", szP4P, szBuildFile);
    if (system(command) != 0)
        return -1;
    if ((f = fopen(szBuildFile,"w")) == NULL)
        return -1;
    fprintf(f, "#define BUILD_NUMBER \"%d\"", buildNo+1);
    fclose(f);

    /* Submit the new build number to the version manager */
    if ((f = fopen(outname,"w")) == NULL)
        return -1;
    fprintf(f, szPublicSubmit, BUILD_NUMBER);
    fclose(f);
    sprintf(command, "%s submit -i < %s", szP4P, outname);
    if (system(command) != 0)
        return -1;

    unlink(outname);
    return 0;
}
