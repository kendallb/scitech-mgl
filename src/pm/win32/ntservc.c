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
* Language:     ANSI C
* Environment:  Windows NT, Windows 2K or Windows XP.
*
* Description:  Main module to do the installation of the SDD and GLDirect
*               device driver components under Windows NT/2K/XP.
*
****************************************************************************/

#include "pmapi.h"
#include "win32/oshdr.h"

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
DESCRIPTION:
Installs a Windows NT/2000/XP service.

HEADER:
pmapi.h

PARAMETERS:
szDriverName    - Actual name of the driver to install in the system
szServiceName   - Name of the service to create
szLoadGroup     - Load group for the driver (NULL for normal drivers)
dwServiceType   - Service type to create
dwStartType     - Service start type to create

RETURNS:
ERROR_SUCCESS on success, error code on failure.

REMARKS:
This function does all the work to install the system service into the
system (ie: a Windows NT style device driver). The driver is not however
activated; for that you must use the PM_startService function. This version
also allows you to specify the service start type.

NOTE:   This function is Windows specific! It is quite useful so it
        is documented here.

SEE ALSO:
PM_installService, PM_startService, PM_stopService, PM_removeService
****************************************************************************/
ulong PMAPI PM_installServiceExt(
    const char *szDriverName,
    const char *szServiceName,
    const char *szLoadGroup,
    ulong dwServiceType,
    ulong dwStartType)
{
    SC_HANDLE   scmHandle;
    SC_HANDLE   driverHandle;
    char        szDriverPath[MAX_PATH];
    ulong       status;

    // Obtain a handle to the service control manager requesting all access
    if ((scmHandle = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS)) == NULL)
        return GetLastError();

    // Use NT system directory alias for early loading drivers
    strcpy(szDriverPath, "\\SystemRoot\\System32");
    strcat(szDriverPath, "\\drivers\\");
    strcat(szDriverPath, szDriverName);

    // Create the service with the Service Control Manager.
    driverHandle = CreateService(scmHandle,
                                 szServiceName,
                                 szServiceName,
                                 SERVICE_ALL_ACCESS,
                                 dwServiceType,
                                 dwStartType,
                                 SERVICE_ERROR_IGNORE,
                                 szDriverPath,
                                 szLoadGroup,
                                 NULL,
                                 NULL,
                                 NULL,
                                 NULL);

    // Check to see if the driver could actually be installed.
    if (!driverHandle) {
        status = GetLastError();
        CloseServiceHandle(scmHandle);
        return status;
        }

    // Clean up and exit
    CloseServiceHandle(driverHandle);
    CloseServiceHandle(scmHandle);
    return ERROR_SUCCESS;
}

/****************************************************************************
DESCRIPTION:
Installs a Windows NT/2000/XP service.

HEADER:
pmapi.h

PARAMETERS:
szDriverName    - Actual name of the driver to install in the system
szServiceName   - Name of the service to create
szLoadGroup     - Load group for the driver (NULL for normal drivers)
dwServiceType   - Service type to create

RETURNS:
ERROR_SUCCESS on success, error code on failure.

REMARKS:
This function does all the work to install the system service into the
system (ie: a Windows NT style device driver). The driver is not however
activated; for that you must use the PM_startService function. This version
always creates the service with the SERVICE_BOOT_START start type.

NOTE:   This function is Windows specific! It is quite useful so it
        is documented here.

SEE ALSO:
PM_installServiceExt, PM_startService, PM_stopService, PM_removeService
****************************************************************************/
ulong PMAPI PM_installService(
    const char *szDriverName,
    const char *szServiceName,
    const char *szLoadGroup,
    ulong dwServiceType)
{
    return PM_installServiceExt(szDriverName,szServiceName,szLoadGroup,
        dwServiceType,SERVICE_BOOT_START);
}

/****************************************************************************
DESCRIPTION:
Starts a Windows NT/2000/XP service.

HEADER:
pmapi.h

PARAMETERS:
szServiceName   - Name of the service to start

RETURNS:
ERROR_SUCCESS on success, error code on failure.

REMARKS:
This function is used to start the specified service and make it active.

NOTE:   This function is Windows specific! It is quite useful so it
        is documented here.

SEE ALSO:
PM_installServiceExt, PM_stopService, PM_removeService
****************************************************************************/
ulong PMAPI PM_startService(
    const char *szServiceName)
{
    SC_HANDLE       scmHandle;
    SC_HANDLE       driverHandle;
    SERVICE_STATUS	serviceStatus;
    ulong           status;

    // Obtain a handle to the service control manager requesting all access
    if ((scmHandle = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS)) == NULL)
        return GetLastError();

    // Open the service with the Service Control Manager.
    if ((driverHandle = OpenService(scmHandle,szServiceName,SERVICE_ALL_ACCESS)) == NULL) {
        status = GetLastError();
        CloseServiceHandle(scmHandle);
        return status;
        }

    // Start the service
    if (!StartService(driverHandle,0,NULL)) {
        status = GetLastError();
        CloseServiceHandle(driverHandle);
        CloseServiceHandle(scmHandle);
        return status;
        }

    // Query the service to make sure it is there
    if (!QueryServiceStatus(driverHandle,&serviceStatus)) {	
        status = GetLastError();
        CloseServiceHandle(driverHandle);
        CloseServiceHandle(scmHandle);
        return status;
        }
    CloseServiceHandle(driverHandle);
    CloseServiceHandle(scmHandle);
    return ERROR_SUCCESS;
}

/****************************************************************************
DESCRIPTION:
Queries a Windows NT/2000/XP service.

HEADER:
pmapi.h

PARAMETERS:
szServiceName   - Name of the service to query

RETURNS:
ERROR_SUCCESS on success, error code on failure.

REMARKS:
This function is used to query if the specified service is active.

NOTE:   This function is Windows specific! It is quite useful so it
        is documented here.

SEE ALSO:
PM_installServiceExt, PM_startService, PM_stopService, PM_removeService
****************************************************************************/
ulong PMAPI PM_queryService(
    const char *szServiceName)
{
    SC_HANDLE       scmHandle;
    SC_HANDLE       driverHandle;
    SERVICE_STATUS	serviceStatus;
    ulong           status;

    // Obtain a handle to the service control manager requesting only query access
    if ((scmHandle = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT)) == NULL)
        return GetLastError();

    // Open the service with the Service Control Manager for query access
    if ((driverHandle = OpenService(scmHandle,szServiceName,SERVICE_QUERY_STATUS)) == NULL) {
        status = GetLastError();
        CloseServiceHandle(scmHandle);
        return status;
        }

    // Query the service to make sure it is there
    if (!QueryServiceStatus(driverHandle,&serviceStatus)) {	
        status = GetLastError();
        CloseServiceHandle(driverHandle);
        CloseServiceHandle(scmHandle);
        return status;
        }

    // Finally we would like to know if it is actually running
    if (serviceStatus.dwCurrentState != SERVICE_RUNNING)
        status = ERROR_SERVICE_NOT_ACTIVE;
    else
        status = ERROR_SUCCESS;

    CloseServiceHandle(driverHandle);
    CloseServiceHandle(scmHandle);
    return status;
}

/****************************************************************************
DESCRIPTION:
Stops a Windows NT/2000/XP service.

HEADER:
pmapi.h

PARAMETERS:
szServiceName   - Name of the service to start

RETURNS:
ERROR_SUCCESS on success, error code on failure.

REMARKS:
This function is used to stop the specified service and disable it.

NOTE:   This function is Windows specific! It is quite useful so it
        is documented here.

SEE ALSO:
PM_installServiceExt, PM_startService, PM_removeService
****************************************************************************/
ulong PMAPI PM_stopService(
    const char *szServiceName)
{
    SC_HANDLE       scmHandle;
    SC_HANDLE       driverHandle;
    SERVICE_STATUS	serviceStatus;
    ulong           status;

    // Obtain a handle to the service control manager requesting all access
    if ((scmHandle = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS)) == NULL)
        return GetLastError();

    // Open the service with the Service Control Manager.
    if ((driverHandle = OpenService(scmHandle,szServiceName,SERVICE_ALL_ACCESS)) == NULL) {
        status = GetLastError();
        CloseServiceHandle(scmHandle);
        return status;
        }

    // Stop the service from running
    if (!ControlService(driverHandle, SERVICE_CONTROL_STOP, &serviceStatus)) {
        status = GetLastError();
        CloseServiceHandle(driverHandle);
        CloseServiceHandle(scmHandle);
        return status;
        }
    CloseServiceHandle(driverHandle);
    CloseServiceHandle(scmHandle);
    return ERROR_SUCCESS;
}

/****************************************************************************
DESCRIPTION:
Removes a Windows NT/2000/XP service.

HEADER:
pmapi.h

PARAMETERS:
szServiceName   - Name of the service to start

RETURNS:
ERROR_SUCCESS on success, error code on failure.

REMARKS:
This function is used to remove a service completely from the system.

NOTE:   This function is Windows specific! It is quite useful so it
        is documented here.

SEE ALSO:
PM_installServiceExt, PM_startService, PM_stopService
****************************************************************************/
ulong PMAPI PM_removeService(
    const char *szServiceName)
{
    SC_HANDLE   scmHandle;
    SC_HANDLE   driverHandle;
    ulong       status;

    // Obtain a handle to the service control manager requesting all access
    if ((scmHandle = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS)) == NULL)
        return GetLastError();

    // Open the service with the Service Control Manager.
    if ((driverHandle = OpenService(scmHandle,szServiceName,SERVICE_ALL_ACCESS)) == NULL) {
        status = GetLastError();
        CloseServiceHandle(scmHandle);
        return status;
        }

    // Remove the service
    if (!DeleteService(driverHandle)) {
        status = GetLastError();
        CloseServiceHandle(driverHandle);
        CloseServiceHandle(scmHandle);
        return status;
        }
    CloseServiceHandle(driverHandle);
    CloseServiceHandle(scmHandle);
    return ERROR_SUCCESS;
}

