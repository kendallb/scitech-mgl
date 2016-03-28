
                  ************************************
                  * MGL Sample Program:   Fox & Bear *
                  ************************************

Supported environments:
    32 bit DOS protected mode
    32 bit Windows

Color depths supported:
    All.

Interface:
    Command line for DOS.
    Font end dialog box for Windows.

Description:
    This sample program is a simple sprite and tile based scrolling game
    using the MGL and the Game Framework. It is based on the original ATI 
    Technologies Fox & Bear benchmark program for hardware acceleration, 
    and has been ported from the original C version to C++. This version 
    of the program supports high speed software sprites using run length 
    encoded transparent bitmaps and hardware accelerated transparent 
    bitmaps (all managed by the Game Framework Sprite Library). If there 
    is enough hardware video memory available, bitmaps are cached in 
    video memory for maximum speed, but when graphics memory is low bitmaps 
    are stored in system memory and rendered via software.

    If you are looking at writing a scrolling based game with the MGL, this
    is a great place to start, although is does contain quite a bit of
    complexity.

Keyboard Interface:
    left    - Move left
    right   - Move right
    down    - Crawl
    up      - Stand up
    space   - Jump
    5       - Stop
    t       - Throw apple (only works in one spot)
    v       - Toggle vertical retrace syncing
    s       - Toggle system buffer rendering
    a       - Toggle acceleration support
    +       - Increase resolution within current color depth
    -       - Decrease resolution within current color depth
    *       - Increase color depth
    /       - Decrease color depth

Sound Support:
    The pre-compiled demo versions of the Fox & Bear demo use the
    DiamondWare Sound ToolKit to provide the sound support. By default
    if you run the DOS FOXBEAR.EXE program by itself you will get no sound
    support. To run with sound support run the provided FBSND.EXE which
    will install the sound support and run the FOXBEAR.EXE program with
    sound support enabled. For the Windows version simply run the
    FOXBEAR.EXE program and it will run with sound support enabled.

    In order to rebuild this demo for sound support, run dmake as follows
    (currently we only support the sharware libs for Watcom C++):

        dmake USE_DWSTK=1

    By default the demos will be built without sound, since you will need
    to get a copy of either the shareware libraries or the retail libraries
    from DiamondWare. Note that if you purchased the MGL on CD-ROM, copies
    of the shareware versions of these libraries are included for your
    enjoyment on your CD.

    Note that the demos were built with the STK for DOS v2.21 and the STK
    for Windows v1.0, and the necessary header files are provided in this
    directory. If you are using a later version of these libraries, you
    should copy the new headers into this directory on top of the old
    ones.

    For more information on the DiamondWare Sound ToolKit, please see the
    file COPYRIGH.SND
