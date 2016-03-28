#############################################################################
#
#               Copyright (C) 1991-2004 SciTech Software, Inc.
#                            All rights reserved.
#
# Descripton:   Generic makefile for the GLUT libraries.
#
#############################################################################

# DOS extender dependant flags
.IMPORT .IGNORE: DEBUG_OPENGL

CFLAGS          += $(DX_CFLAGS)
ASFLAGS         += $(DX_ASFLAGS)

OBJECTS         = g_9x15$O g_bitmap$O g_bwidth$O g_cindex$O g_cursor$O  \
                  g_dials$O g_event$O g_ext$O g_fullsc$O g_get$O        \
                  g_hel10$O g_hel12$O g_hel18$O g_init$O g_input$O      \
                  g_menu$O g_modifr$O g_mroman$O g_overly$O g_roman$O   \
                  g_shapes$O g_space$O g_stroke$O g_swidth$O g_tablet$O \
                  g_teapot$O g_tr10$O g_tr24$O g_util$O g_win$O         \
                  cmdline$O

.IF $(DEBUG_OPENGL)
CFLAGS          += -DNO_GL_MACROS
.ENDIF

.IF $(BUILD_DLL)
CFLAGS          += -DNO_GL_MACROS
DLLFILE         = mglglut$D
LIBFILE         = mglglut$L
EXELIBS         = mglfxi$L
.ELSE
.IF $(STKCALL)
LIBFILE         = $(LP)sglut$L
.ELSE
LIBFILE         = $(LP)glut$L
.ENDIF
.IF $(USE_WIN32)
OBJECTS         += winmain$O
.ENDIF
.ENDIF
LIBCLEAN        = *.lib *.a

# Define the list of object files to create dependency information for

DEPEND_OBJ      = $(OBJECTS)

.INCLUDE: "$(SCITECH)/makedefs/common.mk"

# Target to build all DOS 32-bit libraries

.PHONY mkdos32:
    @build wc10-d32 install -u OPT=1 NO_PCH=1
    @build wc11-d32 install -u OPT=1 NO_PCH=1
    @build bc45-d32 install -u OPT=1
    @build bc50-d32 install -u OPT=1
    @build vc40-tnt install -u OPT=1
    @build vc50-tnt install -u OPT=1
    @build dj20-d32 install -u OPT=1

# Target to build all DOS libraries

.PHONY mkdos: mkdos32 cleanexe

# Target to build Windows 32-bit DLL's

.PHONY mkdll32:
    @build bc50-w32 install -u BUILD_DLL=1 OPT=1

# Target to build all win 32-bit libraries

.PHONY mkwin32:
    @build wc10-w32 install -u OPT=1 NO_PCH=1
    @build wc11-w32 install -u OPT=1 NO_PCH=1
    @build bc45-w32 install -u OPT=1
    @build bc50-w32 install -u OPT=1
    @build vc40-w32 install -u OPT=1
    @build vc50-w32 install -u OPT=1

# Target to build all Windows libraries

.PHONY mkwin: mkwin32 mkdll32 cleanexe

# Target to build all libraries

.PHONY mkall: mkdos32 mkwin32 mkdll32 cleanexe
