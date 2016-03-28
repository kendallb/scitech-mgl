/****************************************************************************
*
*                   SciTech Multi-platform Graphics Library
*
*  ========================================================================
*
*    The contents of this file are subject to the SciTech MGL Public
*    License Version 1.0 (the "License"); you may not use this file
*    except in compliance with the License. You may obtain a copy of
*    the License at http://www.scitechsoft.com/mgl-license.txt
*
*    Software distributed under the License is distributed on an
*    "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
*    implied. See the License for the specific language governing
*    rights and limitations under the License.
*
*    The Original Code is Copyright (C) 1991-2004 SciTech Software, Inc.
*
*    The Initial Developer of the Original Code is SciTech Software, Inc.
*    All Rights Reserved.
*
*  ========================================================================
*
*                Portions Copyright (c) Mark J. Kilgard, 1994.
*
* Language:     ANSI C
* Environment:  Any
*
* Description:  SciTech MGL minimal GLUT implementation for fullscreen
*               and windowed operation.
*
****************************************************************************/

#include "glutint.h"
#include "cmdline.h"
#include "pmapi.h"

#ifndef _MAX_PATH
#define _MAX_PATH   255
#endif

/*--------------------------- Global Variables ----------------------------*/

char        *__glutProgramName = NULL;
uint        __glutDisplayMode = GLUT_RGB | GLUT_SINGLE | GLUT_DEPTH;
int         __glutInitWidth = 300, __glutInitHeight = 300;
int         __glutInitX = -1, __glutInitY = -1;
int         __glutOpenGLType = MGL_GL_AUTO;
ibool       __glutUseVBE = true;
ibool       __glutUseSNAP = true;
int         __glutXRes = 640;
int         __glutYRes = 480;
int         __glutBitsPerPixel = 16;
char        __glutMGLPath[_MAX_PATH] = "";
GLUTwindow  __glutCurrentWindow;
font_t      *__glutFont8x8 = NULL;
#ifdef  __WINDOWS__
MGL_HINSTANCE __glutHinst = NULL;
MGL_HWND    __glutMainWindow = NULL;
MGL_HWND    __glutMainDialog = NULL;
int         __glutMGLMode = -1;
int         __glutMGLPrevMode = -1;
ibool       __glutDone = false;
ibool       __glutFullscreen = false;
ibool       __glutStaticPalette = true;
int         __glutSelectBits = 16;
const char  __glutWinClassName[] = "MGLGlutWindowClass";
static ibool classRegistered = false;
#endif

/*------------------------- Implementation --------------------------------*/

void __glutInitTime(GLUTtimeval *beginning)
{
    static int beenhere = 0;
    static GLUTtimeval genesis;

    if (!beenhere) {
        GETTIMEOFDAY(&genesis);
        beenhere = 1;
        }
    *beginning = genesis;
}

/****************************************************************************
REMARKS:
This function disables any of the display drivers as necessary to
enable and disable DirectDraw or SciTech SNAP Graphics.
****************************************************************************/
static void disableDrivers(void)
{
    MGL_enableAllDrivers();
    if (!__glutUseVBE)
        MGL_disableDriver(MGL_VBENAME);
    if (!__glutUseSNAP)
        MGL_disableDriver(MGL_SNAPNAME);
}

static void initFatalError(void)
{
    char    buf[80];
    sprintf(buf,"Graphics error: %s\n",MGL_errorMsg(MGL_result()));
    MGL_fatalError(buf);
}

int waitEvent(void)
{
    event_t evt;
    EVT_halt(&evt,EVT_KEYDOWN | EVT_KEYREPEAT | EVT_MOUSEDOWN);
    if (!(evt.what & EVT_MOUSEDOWN))
        return EVT_asciiCode(evt.message);
    return 0;
}

#ifdef __CONSOLE__
#include <ctype.h>

void help(void)
/****************************************************************************
*
* Function:     help
*
* Description:  Provide usage information about the program.
*
****************************************************************************/
{
    int mode,xRes,yRes,bits;

    disableDrivers();
    if (MGL_init(__glutMGLPath,NULL) == 0)
        initFatalError();
    printf("GLUT demo usage: [-novbe -nonuc -h] [<xRes> <yRes> <bitsPerPixel>]\n\n");
    printf("    -novbe  Don't use VESA VBE driver.\n");
    printf("    -nonuc  Don't use SciTech SNAP Graphics driver.\n");
    printf("    -h      Provide this usage information.\n\n");
    printf("Press a key for list of video modes.");
    fflush(stdout);
    if (waitEvent() != ASCII_esc) {
        printf("\n\nAvailable modes are:\n");
        for (mode = 0; MGL_modeResolution(mode,&xRes,&yRes,&bits); mode++) {
            printf("  %4d x %4d x %2d %3d page (%s)\n",
                xRes,yRes,bits,
                MGL_availablePages(mode),
                MGL_modeDriverName(mode));
            if (mode != 0 && ((mode % 20) == 0)) {
                printf("Press a key to continue ... ");
                fflush(stdout);
                if (waitEvent() == ASCII_esc)
                    break;
                printf("\n");
                }
            }
        }
    MGL_exit();
    exit(1);
}

void parseArguments(int argc,char *argv[])
/****************************************************************************
*
* Function:     parseArguments
* Parameters:   argc    - Number of command line arguments
*               argv    - Array of command line arguments
*
* Description:  Parses the command line.
*
****************************************************************************/
{
    int     option;
    char    *argument;

    /* Parse command line options */
    do {
        option = getcmdopt(argc,argv,"N:n:Hh",&argument);
        if (isascii(option))
            option = tolower(option);
        switch (option) {
            case 'n':
                if (stricmp(argument,"ovbe") == 0)
                    __glutUseVBE = false;
                if (stricmp(argument,"onuc") == 0)
                    __glutUseSNAP = false;
                break;
            case ALLDONE:
            case PARAMETER:
                break;
            case 'h':
            case INVALID:
            default:
                help();
            }
        } while (option != ALLDONE && option != PARAMETER);
}
#endif  /* __CONSOLE__ */

void glutInit(int *argcp, char **argv)
{
    char            *str;
    GLUTtimeval     genesis;

    /* determine temporary program name */
    str = strrchr(argv[0], '\\');
    if (str == NULL)
        __glutProgramName = argv[0];
    else
        __glutProgramName = str + 1;
    memset(&__glutCurrentWindow,0,sizeof(__glutCurrentWindow));
    __glutInitTime(&genesis);
    glutReshapeFunc(NULL);
#ifdef __CONSOLE__
    parseArguments(*argcp,argv);
    if (nextargv <= *argcp-3) {
        __glutXRes = atoi(argv[nextargv]);
        __glutYRes = atoi(argv[nextargv+1]);
        __glutBitsPerPixel = atoi(argv[nextargv+2]);
        }
    *argcp -= nextargv;
#else
    (void)argcp;
#endif
}

void glutInitWindowPosition(int x, int y)
{
    __glutInitX = x;
    __glutInitY = y;
}

void glutInitWindowSize(int width, int height)
{
    if (width > 0 && height > 0) {
        __glutInitWidth = width;
        __glutInitHeight = height;
        }
}

void glutInitDisplayMode(uint mask)
{
    __glutDisplayMode = mask;
}

void glutInitMGLOpenGLType(int type)
{
    __glutOpenGLType = type;
}

void glutInitMGLSysPalNoStatic(ibool flag)
{
#ifdef  __WINDOWS__
    __glutStaticPalette = flag;
#else
    (void)flag;
#endif
}

void glutInitMGLMode(int xRes,int yRes,int bitsPerPixel)
{
    __glutXRes = xRes;
    __glutYRes = yRes;
    __glutBitsPerPixel = bitsPerPixel;
}

void glutInitMGLDrivers(ibool useVBE,ibool useSNAP)
{
    __glutUseVBE = useVBE;
    __glutUseSNAP = useSNAP;
}

void glutInitMGLPath(const char *MGLPath)
{
    strcpy(__glutMGLPath,MGLPath);
}

static void __glutInitOpenGL(void)
{
    MGLVisual   visual;

    /* Start OpenGL for the MGL device context */
    memset(&visual,0,sizeof(visual));
    visual.rgb_flag = (__glutDisplayMode & GLUT_INDEX) == 0;
    visual.db_flag = (__glutDisplayMode & GLUT_DOUBLE) != 0;
    visual.stereo_flag = (__glutDisplayMode & GLUT_STEREO) != 0;
    if (__glutDisplayMode & GLUT_DEPTH)
        visual.depth_size = 16;
    if (__glutDisplayMode & GLUT_ALPHA)
        visual.alpha_size = 8;
    if (__glutDisplayMode & GLUT_STENCIL)
        visual.stencil_size = 8;
    if (__glutDisplayMode & GLUT_ACCUM)
        visual.accum_size = 8;
    MGL_glChooseVisual(__glutCurrentWindow.dc,&visual);
    MGL_glSetVisual(__glutCurrentWindow.dc,&visual);
    if (!MGL_glCreateContext(__glutCurrentWindow.dc,MGL_GL_VISUAL))
        MGL_fatalError("Unable to start OpenGL!");
    MGL_glMakeCurrent(__glutCurrentWindow.dc);

    /* Load the 8x8 font into the device context */
    if (!__glutFont8x8)
        __glutFont8x8 = MGL_loadFont("pc8x8.fnt");
    MGL_useFont(__glutFont8x8);
}

#ifdef __CONSOLE__

int glutCreateWindow(char *)
{
    int mode,numPages;

    /* Create the MGL fullscreen device context */
    if (MGL_init(__glutMGLPath,NULL) == 0)
        initFatalError();
    disableDrivers();
    if ((mode = MGL_findMode(__glutXRes,__glutYRes,__glutBitsPerPixel)) == -1)
        initFatalError();
    numPages = MIN(MGL_availablePages(mode),2);
    if ((__glutCurrentWindow.dc = MGL_createDisplayDC(mode,numPages,MGL_DEFAULT_REFRESH)) == NULL)
        initFatalError();
    MGL_makeCurrentDC(__glutCurrentWindow.dc);
    __glutCurrentWindow.waitVRT = true;
    __glutInitOpenGL();
    return true;
}

void __glutExitFullscreen(void)
{
    MGL_glDeleteContext(__glutCurrentWindow.dc);
    MGL_destroyDC(__glutCurrentWindow.dc);
    __glutCurrentWindow.dc = NULL;
    MGL_exit();
    printf("Type '%s -h' for a list of available modes\n", __glutProgramName);
}

#elif defined(__OS2_PM__)

// TODO: Add code to support OS/2 Presentation Manager apps

#elif defined(__WINDOWS__)
#undef  WINGDIAPI
#undef  APIENTRY
#undef  STRICT
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "GL/glutdlg.rh"

/* Prototype needs Windows declarations for MGL GLUT apps to link correctly. */
extern LONG WINAPI  __glutWindowProc(HWND hWnd, uint uMsg, uint wParam, long lParam);

static short modeNums[MAX_MODES];    /* List of modes to select from    */

void CenterWindow(HWND hWndCenter, HWND parent, BOOL repaint)
/****************************************************************************
*
* Function:     CenterWindow
* Parameters:   hWndCenter  - Window to center
*               parent      - Handle for parent window
*               repaint     - True if window should be re-painted
*
* Description:  Centers the specified window within the bounds of the
*               specified parent window. If the parent window is NULL, then
*               we center it using the Desktop window.
*
****************************************************************************/
{
    HWND    hWndParent = (parent ? parent : GetDesktopWindow());
    RECT    RectParent;
    RECT    RectCenter;
    int     CenterX,CenterY,Height,Width;

    GetWindowRect(hWndParent, &RectParent);
    GetWindowRect(hWndCenter, &RectCenter);

    Width = (RectCenter.right - RectCenter.left);
    Height = (RectCenter.bottom - RectCenter.top);
    CenterX = ((RectParent.right - RectParent.left) - Width) / 2;
    CenterY = ((RectParent.bottom - RectParent.top) - Height) / 2;

    if ((CenterX < 0) || (CenterY < 0)) {
        /* The Center Window is smaller than the parent window. */
        if (hWndParent != GetDesktopWindow()) {
            /* If the parent window is not the desktop use the desktop size. */
            CenterX = (GetSystemMetrics(SM_CXSCREEN) - Width) / 2;
            CenterY = (GetSystemMetrics(SM_CYSCREEN) - Height) / 2;
            }
        CenterX = (CenterX < 0) ? 0: CenterX;
        CenterY = (CenterY < 0) ? 0: CenterY;
        }
    else {
        CenterX += RectParent.left;
        CenterY += RectParent.top;
        }

    /* Copy the values into RectCenter */
    RectCenter.left = CenterX;
    RectCenter.right = CenterX + Width;
    RectCenter.top = CenterY;
    RectCenter.bottom = CenterY + Height;

    /* Move the window to the new location */
    MoveWindow(hWndCenter, RectCenter.left, RectCenter.top,
            (RectCenter.right - RectCenter.left),
            (RectCenter.bottom - RectCenter.top), repaint);
}

void RefreshModeList(HWND hwnd)
/****************************************************************************
*
* Function:     RefreshModeList
* Parameters:   hwnd    - Handle to dialog box window
*
* Description:  Refreshes the list of available video modes in the video
*               mode list box given the newly selected pixel depth.
*
****************************************************************************/
{
    char    buf[MAX_STR];
    int     i,modes,x,y,bits,mode,selectIndex;
    HWND    hwndLst = GetDlgItem(hwnd,IDC_MODELIST);
    ibool   haveMode = false;

    /* Find out if we should use both SNAP and VBE or just one */
#if 0
    __glutUseSNAP = IsDlgButtonChecked(hwnd,IDC_USEDDRAW);
    __glutUseVBE = IsDlgButtonChecked(hwnd,IDC_USEWDIRECT);
    if (IsDlgButtonChecked(hwnd,IDC_USEBOTH))
        __glutUseSNAP = __glutUseVBE = true;
    disableDrivers();
#endif
    MGL_glSetOpenGLType(__glutOpenGLType);

    SendMessage(hwndLst,LB_RESETCONTENT,0,0);
    if (IsDlgButtonChecked(hwnd,IDC_WINDOWED)) {
        __glutSelectBits = -1;
        SendMessage(hwndLst,LB_ADDSTRING,0,(LONG)"Default window");
        modeNums[0] = GLUT_MGL_WINDOWED;
        SendMessage(hwndLst,LB_ADDSTRING,0,(LONG)"320x240 window");
        modeNums[1] = GLUT_MGL_WINDOWED_320x240;
        SendMessage(hwndLst,LB_ADDSTRING,0,(LONG)"512x384 window");
        modeNums[2] = GLUT_MGL_WINDOWED_512x384;
        SendMessage(hwndLst,LB_ADDSTRING,0,(LONG)"640x480 window");
        modeNums[3] = GLUT_MGL_WINDOWED_640x480;
        SendMessage(hwndLst,LB_ADDSTRING,0,(LONG)"800x600 window");
        modeNums[4] = GLUT_MGL_WINDOWED_800x600;
        modes = 5;
        haveMode = true;
        if (__glutMGLPrevMode < 0)
            selectIndex = (-__glutMGLPrevMode)-1;
        else
            selectIndex = 0;
        }
    else {
        if (IsDlgButtonChecked(hwnd,IDC_8BIT))          __glutSelectBits = 8;
        else if (IsDlgButtonChecked(hwnd,IDC_15BIT))    __glutSelectBits = 15;
        else if (IsDlgButtonChecked(hwnd,IDC_16BIT))    __glutSelectBits = 16;
        else if (IsDlgButtonChecked(hwnd,IDC_24BIT))    __glutSelectBits = 24;
        else if (IsDlgButtonChecked(hwnd,IDC_32BIT))    __glutSelectBits = 32;

        selectIndex = 0;
        for (i = modes = 0; i < 0xFF; i++) {
            if (!MGL_modeResolution(mode = i,&x,&y,&bits))
                continue;
            if (bits != __glutSelectBits)
                continue;
            sprintf(buf,"%4d x %4d x %2d %2d page (%s)",
                x,y,bits,MGL_availablePages(mode),
                MGL_modeDriverName(mode));
            SendMessage(hwndLst,LB_ADDSTRING,0,(LONG)buf);
            if (mode == __glutMGLPrevMode)
                selectIndex = modes;
            modeNums[modes++] = mode;
            if (modes >= MAX_MODES)
                break;
            haveMode = true;
            }
        }
    SendMessage(hwndLst,LB_SETCURSEL,selectIndex,0);

    if (!haveMode) {
        SendMessage(hwndLst,LB_ADDSTRING,0,(LONG)"No available modes");
        EnableWindow(hwndLst,FALSE);
        }
    else {
        EnableWindow(hwndLst,TRUE);
        }
}

BOOL CALLBACK MainDlgProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
/****************************************************************************
*
* Function:     MainDlgProc
*
* Description:  Dialog procedure for front end dialog box.
*
****************************************************************************/
{
    HWND    hwndLst;

    switch (msg) {
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDC_CANCEL:
                    EndDialog(hwnd,true);
                    break;
                case IDC_MODELIST:
                    if (HIWORD(wParam) != LBN_DBLCLK)
                        break;
                case IDC_OK:
                    hwndLst = GetDlgItem(hwnd,IDC_MODELIST);
                    if (SendMessage(hwndLst,LB_GETCOUNT,0,0)) {
                        __glutMGLMode = modeNums[SendMessage(hwndLst,LB_GETCURSEL,0,0)];
                        __glutMGLPrevMode = __glutMGLMode;
                        EndDialog(hwnd,false);
                        }
                    break;
                case IDC_WINDOWED:
                case IDC_8BIT:
                case IDC_15BIT:
                case IDC_16BIT:
                case IDC_24BIT:
                case IDC_32BIT:
                    CheckDlgButton(hwnd,IDC_WINDOWED,LOWORD(wParam) == IDC_WINDOWED);
                    CheckDlgButton(hwnd,IDC_8BIT,LOWORD(wParam) == IDC_8BIT);
                    CheckDlgButton(hwnd,IDC_15BIT,LOWORD(wParam) == IDC_15BIT);
                    CheckDlgButton(hwnd,IDC_16BIT,LOWORD(wParam) == IDC_16BIT);
                    CheckDlgButton(hwnd,IDC_24BIT,LOWORD(wParam) == IDC_24BIT);
                    CheckDlgButton(hwnd,IDC_32BIT,LOWORD(wParam) == IDC_32BIT);
                    RefreshModeList(hwnd);
                    break;
                case IDC_USEDDRAW:
                case IDC_USEWDIRECT:
                case IDC_USEBOTH:
                    CheckDlgButton(hwnd,IDC_USEDDRAW,LOWORD(wParam) == IDC_USEDDRAW);
                    CheckDlgButton(hwnd,IDC_USEWDIRECT,LOWORD(wParam) == IDC_USEWDIRECT);
                    CheckDlgButton(hwnd,IDC_USEBOTH,LOWORD(wParam) == IDC_USEBOTH);
                    RefreshModeList(hwnd);
                    break;
                case IDC_AUTO:
                case IDC_MICROSOFT:
                case IDC_SGI:
                case IDC_MESA:
                    CheckDlgButton(hwnd,IDC_AUTO,LOWORD(wParam) == IDC_AUTO);
                    CheckDlgButton(hwnd,IDC_MICROSOFT,LOWORD(wParam) == IDC_MICROSOFT);
                    CheckDlgButton(hwnd,IDC_SGI,LOWORD(wParam) == IDC_SGI);
                    CheckDlgButton(hwnd,IDC_MESA,LOWORD(wParam) == IDC_MESA);
                    switch (LOWORD(wParam)) {
                        case IDC_AUTO:
                            __glutOpenGLType = MGL_GL_AUTO;
                            break;
                        case IDC_MICROSOFT:
                            __glutOpenGLType = MGL_GL_DEFAULT;
                            break;
                        case IDC_SGI:
                            __glutOpenGLType = MGL_GL_SNAP;
                            break;
                        case IDC_MESA:
                            __glutOpenGLType = MGL_GL_SNAP;
                            break;
                        }
                    RefreshModeList(hwnd);
                    break;
                }
            break;
        case WM_INITDIALOG:
            CenterWindow(hwnd,NULL,FALSE);
            if (__glutSelectBits == -1)
                CheckDlgButton(hwnd,IDC_WINDOWED,TRUE);
            else if (__glutSelectBits == 8)
                CheckDlgButton(hwnd,IDC_8BIT,TRUE);
            else if (__glutSelectBits == 15)
                CheckDlgButton(hwnd,IDC_15BIT,TRUE);
            else if (__glutSelectBits == 16)
                CheckDlgButton(hwnd,IDC_16BIT,TRUE);
            else if (__glutSelectBits == 24)
                CheckDlgButton(hwnd,IDC_24BIT,TRUE);
            else if (__glutSelectBits == 32)
                CheckDlgButton(hwnd,IDC_32BIT,TRUE);
#if 0
            if (__glutUseSNAP && __glutUseVBE)
                CheckDlgButton(hwnd,IDC_USEBOTH,TRUE);
            else if (__glutUseSNAP)
                CheckDlgButton(hwnd,IDC_USEDDRAW,TRUE);
            else if (__glutUseVBE)
                CheckDlgButton(hwnd,IDC_USEWDIRECT,TRUE);
#endif
            if (__glutOpenGLType == MGL_GL_AUTO)
                CheckDlgButton(hwnd,IDC_AUTO,TRUE);
            else if (__glutOpenGLType == MGL_GL_DEFAULT)
                CheckDlgButton(hwnd,IDC_MICROSOFT,TRUE);
#if 0
            else if (__glutOpenGLType == MGL_GL_SGI)
                CheckDlgButton(hwnd,IDC_SGI,TRUE);
#endif
            else if (__glutOpenGLType == MGL_GL_SNAP)
                CheckDlgButton(hwnd,IDC_MESA,TRUE);
            RefreshModeList(hwnd);
            return TRUE;
        }
    lParam = lParam;
    return FALSE;
}

int glutCreateWindow(char *title)
{
    WNDCLASS    cls;
    int         pages,width,height,dx,dy;

    /* Create a Window class for the fullscreten window in here, since we need
     * to register one that will do all our event handling for us.
     */
    __glutHinst = GetModuleHandle(NULL);
    if (!classRegistered) {
        cls.hCursor         = LoadCursor(NULL,IDC_ARROW);
        cls.hIcon           = LoadIcon((HINSTANCE)__glutHinst,MAKEINTRESOURCE(1));
        cls.lpszMenuName    = NULL;
        cls.lpszClassName   = __glutWinClassName;
        cls.hbrBackground   = (HBRUSH)GetStockObject(BLACK_BRUSH);
        cls.hInstance       = (HINSTANCE)__glutHinst;
        cls.style           = CS_DBLCLKS;
        cls.lpfnWndProc     = (WNDPROC)__glutWindowProc;
        cls.cbWndExtra      = 0;
        cls.cbClsExtra      = 0;
        if (!RegisterClass(&cls))
            MGL_fatalError("Unable to register Window Class!");
        classRegistered = true;
        }

    /* Init MGL for dialog to enumerate display modes */
    if (!MGL_init(__glutMGLPath,NULL))
        initFatalError();
    disableDrivers();
    if (__glutMGLMode == -1) {
        __glutDone = DialogBox((HINSTANCE)__glutHinst,MAKEINTRESOURCE(IDD_MAINDLG),NULL,(DLGPROC)MainDlgProc);
        if (__glutDone)
            exit(1);
        }
    else
        __glutDone = true;
    /* Configure MGL GLUT driver options based on dialog settings */
    MGL_glSetOpenGLType(__glutOpenGLType);
    __glutFullscreen = (__glutMGLMode >= 0);
    if (!__glutFullscreen) {
        switch (__glutMGLMode) {
            case GLUT_MGL_WINDOWED:
                width = __glutInitWidth;
                height = __glutInitHeight;
                break;
            case GLUT_MGL_WINDOWED_320x240:
                width = 320;
                height = 240;
                break;
            case GLUT_MGL_WINDOWED_512x384:
                width = 512;
                height = 384;
                break;
            case GLUT_MGL_WINDOWED_640x480:
                width = 640;
                height = 480;
                break;
            case GLUT_MGL_WINDOWED_800x600:
                width = 800;
                height = 600;
                break;
            }
        /* Account for window decorations like border and caption */
        dx = GetSystemMetrics(SM_CXSIZEFRAME) * 2;
        dy = GetSystemMetrics(SM_CYSIZEFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION);
        }
    else {
        width = __glutInitWidth;
        height = __glutInitHeight;
        dx = dy = 0;
        }

    /* Create the main window and display it */
    __glutCurrentWindow.waitVRT = true;
    __glutMainWindow = CreateWindow(__glutWinClassName,title,WS_OVERLAPPEDWINDOW,
        __glutInitX,__glutInitY,width+dx,height+dy,NULL,NULL,(HINSTANCE)__glutHinst,NULL);
    if (__glutFullscreen) {
        /* Create the MGL fullscreen device context */
        MGL_registerFullScreenWindow(__glutMainWindow);
        pages = MGL_availablePages(__glutMGLMode);
        if (pages > 2)
            pages = 2;
        if ((__glutCurrentWindow.dc = MGL_createDisplayDC(__glutMGLMode,pages,MGL_DEFAULT_REFRESH)) == NULL)
            initFatalError();
        }
    else {
        /* Create the MGL window device context */
        ShowWindow((HWND)__glutMainWindow,SW_SHOW);
        __glutCurrentWindow.dc = MGL_createWindowedDC(__glutMainWindow);
        if (!__glutCurrentWindow.dc)
            initFatalError();
        __glutCurrentWindow.windc = __glutCurrentWindow.dc;

        /* Create an offscreen device context for rendering */
        __glutCurrentWindow.backdc = MGL_createOffscreenDC(__glutCurrentWindow.dc,width,height);
        if (!__glutCurrentWindow.backdc)
            initFatalError();
        __glutCurrentWindow.dc = __glutCurrentWindow.backdc;
        MGL_makeCurrentDC(__glutCurrentWindow.dc);

        /* Force double buffering off for offscreen rendering context */
        __glutDisplayMode &= ~GLUT_DOUBLE;

        /* If we have hardware OpenGL then we need to set the system to static
         * palette mode as the hardware does not do palette re-mapping.
         */
        if (__glutStaticPalette || (__glutCurrentWindow.dc->mi.bitsPerPixel == 8 && MGL_glHaveHWOpenGL())) {
            HDC hdc = GetDC(NULL);
            SetSystemPaletteUse(hdc, SYSPAL_NOSTATIC);
            ReleaseDC(NULL,hdc);
            __glutStaticPalette = true;
            }
        }
    __glutInitOpenGL();
    return true;
}

static void ClearMessageQueue(void)
{
    MSG msg;
    while (PeekMessage(&msg,NULL,0,0,PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        }
}

void __glutExitFullscreen(void)
{
    MGL_glDeleteContext(__glutCurrentWindow.dc);
    MGL_destroyDC(__glutCurrentWindow.dc);
    __glutCurrentWindow.dc = NULL;
    MGL_exit();
    DestroyWindow((HWND)__glutMainWindow);
    ClearMessageQueue();
    __glutMainWindow = NULL;
    __glutMGLMode = -1;
}
#endif
