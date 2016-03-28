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
* Environment:  Any
*
* Description:  Header file declaring all the PM imports structure for the
*               current version of the PM library. Included in all code
*               that needs to pass the PM imports to BPD files.
*
****************************************************************************/

PM_imports  _VARAPI _PM_imports = {
    sizeof(PM_imports),
    PM_getModeType,
    PM_getBIOSPointer,
    PM_getA0000Pointer,
    PM_mapPhysicalAddr,
    PM_mallocShared,
    NULL,
    PM_freeShared,
    NULL,
    PM_mapRealPointer,
    PM_allocRealSeg,
    PM_freeRealSeg,
    PM_allocLockedMem,
    PM_freeLockedMem,
    PM_callRealMode,
    PM_int86,
    PM_int86x,
    NULL,
    NULL,
    PM_getVESABuf,
    PM_getOSType,
    PM_fatalError,
    NULL,
    NULL,
    NULL,
    PM_getCurrentPath,
    NULL,
    PM_getSNAPPath,
    PM_getSNAPConfigPath,
    PM_getUniqueID,
    PM_getMachineName,
    NULL,
    NULL,
    NULL,
    PM_openConsole,
    PM_getConsoleStateSize,
    PM_saveConsoleState,
    PM_restoreConsoleState,
    PM_closeConsole,
    PM_setOSCursorLocation,
    PM_setOSScreenWidth,
    PM_enableWriteCombine,
    PM_backslash,
    PM_lockDataPages,
    PM_unlockDataPages,
    PM_lockCodePages,
    PM_unlockCodePages,
    PM_setRealTimeClockHandler,
    PM_setRealTimeClockFrequency,
    PM_restoreRealTimeClockHandler,
    NULL,
    PM_getBootDrive,
    PM_freePhysicalAddr,
    PM_inpb,
    PM_inpw,
    PM_inpd,
    PM_outpb,
    PM_outpw,
    PM_outpd,
    NULL,
    PM_setSuspendAppCallback,
    PM_haveBIOSAccess,
    PM_kbhit,
    PM_getch,
    PM_findBPD,
    PM_getPhysicalAddr,
    PM_sleep,
    PM_getCOMPort,
    PM_getLPTPort,
    PM_loadLibrary,
    PM_getProcAddress,
    PM_freeLibrary,
    PCI_enumerate,
    PCI_accessReg,
    PCI_setHardwareIRQ,
    PCI_generateSpecialCyle,
    NULL,
    PCI_getBIOSEntry,
    CPU_getProcessorType,
    CPU_haveMMX,
    CPU_have3DNow,
    CPU_haveSSE,
    CPU_haveRDTSC,
    CPU_getProcessorSpeed,
    ZTimerInit,
    LZTimerOn,
    LZTimerLap,
    LZTimerOff,
    LZTimerCount,
    LZTimerOnExt,
    LZTimerLapExt,
    LZTimerOffExt,
    LZTimerCountExt,
    ULZTimerOn,
    ULZTimerLap,
    ULZTimerOff,
    ULZTimerCount,
    ULZReadTime,
    ULZElapsedTime,
    ULZTimerResolution,
    PM_findFirstFile,
    PM_findNextFile,
    PM_findClose,
    PM_makepath,
    PM_splitpath,
    NULL,
    PM_getdcwd,
    PM_setFileAttr,
    PM_mkdir,
    PM_rmdir,
    PM_getFileAttr,
    PM_getFileTime,
    PM_setFileTime,
    CPU_getProcessorName,
    PM_getVGAStateSize,
    PM_saveVGAState,
    PM_restoreVGAState,
    NULL,
    NULL,
    PM_blockUntilTimeout,
    _PM_add64,
    _PM_sub64,
    _PM_mul64,
    _PM_div64,
    _PM_shr64,
    _PM_sar64,
    _PM_shl64,
    _PM_neg64,
    NULL,
    PCI_readRegBlock,
    PCI_writeRegBlock,
    PM_flushTLB,
    PM_useLocalMalloc,
    PM_malloc,
    PM_calloc,
    PM_realloc,
    PM_free,
    PM_getPhysicalAddrRange,
    PM_allocPage,
    PM_freePage,
    PM_agpInit,
    PM_agpExit,
    PM_agpReservePhysical,
    PM_agpReleasePhysical,
    PM_agpCommitPhysical,
    PM_agpFreePhysical,
    PCI_getNumDevices,
    PM_setLocalBPDPath,
#ifdef __WINDOWS32__
    PM_loadDirectDraw,
    PM_unloadDirectDraw,
    PM_getDirectDrawWindow,
    PM_doSuspendApp,
#else
    NULL,
    NULL,
    NULL,
    NULL,
#endif
    PM_setMaxThreadPriority,
    PM_restoreThreadPriority,
    PM_getOSName,
    _CHK_defaultFail,
    PM_isSDDActive,
    PM_runningInAWindow,
    PM_stopRealTimeClock,
    PM_restartRealTimeClock,
    PM_saveFPUState,
    PM_restoreFPUState,
    CPU_setDefaultProcessorSpeed,
    PM_getOSResolution,
    PM_memProtect,
#if defined(__MSDOS32__) || defined(__NT_DRIVER__)
    PM_setIRQHandler,
    PM_restoreIRQHandler,
    PM_DMACEnable,
    PM_DMACDisable,
    PM_DMACProgram,
    PM_DMACPosition,
#else
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
#endif
    PM_lockSNAPAccess,
    PM_unlockSNAPAccess,
#if defined(__NT_DRIVER__)
    PM_registerHeartBeatCallback,
    PM_unregisterHeartBeatCallback,
#else
    NULL,
    NULL,
#endif
    PM_registerBIOSEmulator
    };

