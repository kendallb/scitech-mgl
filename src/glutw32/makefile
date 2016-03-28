#############################################################################
#
#               Copyright (C) 1991-2004 SciTech Software, Inc.
#                            All rights reserved.
#
# Descripton:   Generic makefile for the Mesa GLUT libraries.
#
#############################################################################

#----------------------------------------------------------------------------
# Define the lists of object files
#----------------------------------------------------------------------------

OBJECTS         = glut_8x13$O glut_9x15$O glut_bitmap$O glut_bwidth$O \
                  glut_cindex$O glut_cmap$O glut_cursor$O glut_dials$O \
                  glut_dstr$O glut_event$O glut_ext$O glut_fbc$O \
                  glut_fullscrn$O glut_gamemode$O glut_get$O glut_hel10$O \
                  glut_hel12$O glut_hel18$O glut_init$O glut_input$O \
                  glut_joy$O glut_key$O glut_keyctrl$O glut_keyup$O \
                  glut_mesa$O glut_modifier$O glut_mroman$O glut_overlay$O \
                  glut_roman$O glut_shapes$O glut_space$O glut_stroke$O \
                  glut_swap$O glut_swidth$O glut_tablet$O glut_teapot$O \
                  glut_tr10$O glut_tr24$O glut_util$O glut_vidresize$O \
                  glut_warp$O glut_win$O glut_winmisc$O win32_glx$O \
                  win32_menu$O win32_util$O win32_winproc$O win32_x11$O

#----------------------------------------------------------------------------
# To build WIN32 GLUT DLL, disable SciTech MGL/GLSwitch library macros
# and link with default DLL import OpenGL libraries
#----------------------------------------------------------------------------

.IF $(BUILD_DLL)
CFLAGS          += -DNO_GL_MACROS -DNO_GLU_MACROS -DNO_WGL_MACROS
EXELIBS         += opengl32$L glu32$L winmm$L
DLLFILE         = $(LP)glut32$D
.ENDIF
CFLAGS          += -D__USE_GLUTW32__

#----------------------------------------------------------------------------
# Names of target libaries
#----------------------------------------------------------------------------

.IF $(STKCALL)
LIBFILE         = $(LP)sglut$L
.ELSE
LIBFILE         = $(LP)glut$L
.ENDIF
LIBCLEAN        = *.lib *.dll

# Define the list of object files to create dependency information for

DEPEND_OBJ      = $(OBJECTS)

.INCLUDE: "$(SCITECH)/makedefs/common.mk"

