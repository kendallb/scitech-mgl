/****************************************************************************
*
*                   SciTech SNAP Graphics Graphics Architecture
*
*               Copyright (C) 1991-2002 SciTech Software, Inc.
*                            All rights reserved.
*
*  ======================================================================
*  |REMOVAL OR MODIFICATION OF THIS HEADER IS STRICTLY PROHIBITED BY LAW|
*  |                                                                    |
*  |This copyrighted computer code contains proprietary technology      |
*  |owned by SciTech Software, Inc., located at 505 Wall Street,        |
*  |Chico, CA 95928 USA (http://www.scitechsoft.com).                   |
*  |                                                                    |
*  |The contents of this file are subject to the SciTech SNAP Graphics  |
*  |License; you may *not* use this file or related software except in  |
*  |compliance with the License. You may obtain a copy of the License   |
*  |at http://www.scitechsoft.com/snap-license.txt                      |
*  |                                                                    |
*  |Software distributed under the License is distributed on an         |
*  |"AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or      |
*  |implied. See the License for the specific language governing        |
*  |rights and limitations under the License.                           |
*  |                                                                    |
*  |REMOVAL OR MODIFICATION OF THIS HEADER IS STRICTLY PROHIBITED BY LAW|
*  ======================================================================
*
* Language:     ANSI C
* Environment:  32-bit console
*
* Description:  Implementation of the spawn() functions for Linux systems.
*
****************************************************************************/

#include <sys/types.h>
#include <errno.h>
#include <sys/wait.h>
#include <stddef.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include "spawn.h"

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
PARAMETERS:
mode    - Mode used to spawn the process (P_WAIT, P_NOWAIT)
path    - Path to the program to spawn
argv    - Array of arguments to spawned process, terminated by a NULL

RETURNS:
Result code returned from the spawned process
****************************************************************************/
int spawnvp(
    int mode,
    char *path,
    char *argv[])
{
    pid_t               pid;
    int                 err;
    int                 status;
    struct sigaction    oldAction;
    struct sigaction    newAction;
    int                 status_pipe[2];

    if (pipe(status_pipe) == -1)
        return -1;

    if (fcntl(status_pipe[1], F_SETFD, FD_CLOEXEC)) {
        close(status_pipe[0]);
        close(status_pipe[1]);
        return -1;
        }

    oldAction.sa_handler = SIG_DFL;
    if (mode == P_WAIT) {
        sigaction(SIGCHLD, NULL, &oldAction);
        if (oldAction.sa_handler == SIG_IGN) {
            newAction = oldAction;
            newAction.sa_handler = SIG_DFL;
            sigaction(SIGCHLD, &newAction, NULL);
            }
        }
    err = pid = fork();
    if (pid == 0) {
        /* Inside child process, so exec the binary. Exec
         * should never return, but if it does, it is an
         * error condition so we report it.
         */
        close(status_pipe[0]);
        execvp(path, argv);
        write(status_pipe[1], &errno, sizeof errno);
	    _exit(127);
        }
    close(status_pipe[1]);

    /* Inside the parent, process so wait for process to finish */
    if (err != -1)
        err = read(status_pipe[0], &errno, sizeof errno);
    if (err != -1) {
        if (err > 0) {
            err = errno;
            waitpid(pid, NULL, 0);
            errno = err;
            err = -1;
            }
        else if (mode == P_WAIT) {
            /* If P_WAIT return invoked task's status otherwise P_NOWAIT so
             * return pid and let user do the wait
             */
            do {
                err = waitpid(pid, &status, 0);
                } while (err == -1 && errno == EINTR);
            if (err == pid)
                err = WEXITSTATUS(status);
            }
        }
    if (oldAction.sa_handler == SIG_IGN)
        sigaction(SIGCHLD, &oldAction, NULL);
    close(status_pipe[0]);
    return err;
}

/****************************************************************************
PARAMETERS:
mode    - Mode used to spawn the process (P_WAIT, P_NOWAIT)
path    - Path to the program to spawn
arg     - First argument to the spawned process
...     - Remained of arguments to spawned process, terminated by a NULL

RETURNS:
Result code returned from the spawned process
****************************************************************************/
int spawnlp(
    int mode,
    char *path,
    char *arg,
    ...)
{
    return spawnvp(mode, path, &arg);
}

