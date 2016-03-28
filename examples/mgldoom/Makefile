#############################################################################
#
#               Copyright (C) 1991-2004 SciTech Software, Inc.
#                            All rights reserved.
#
# Descripton:   MGL version of DOOM, the classic arcade game
#
#
#############################################################################

.IMPORT .IGNORE: USE_OS232 USE_DOS USE_WIN32 USE_RTTARGET ISV_LICENSE
.IMPORT .IGNORE: USE_WATCOM USE_WASM

# Link with standard MGL libraries

.INCLUDE: "../mgl.mak"

# Set objects to be included in the make process
DOSOBJS  = i_netd$O d_maind$O
OS2OBJS  = i_sound$O i_net$O d_main$O
LNXOBJS  = i_soundd$O i_netu$O d_main$O
QNXOBJS  = i_soundd$O i_netd$O d_main$O
OBJS1    = doomdef$O doomstat$O dstrings$O tables$O f_finale$O f_wipe$O d_net$O
OBJS2    = d_items$O g_game$O m_menu$O m_misc$O m_argv$O m_bbox$O m_fixed$O m_swap$O
OBJS3    = m_cheat$O m_random$O am_map$O p_ceilng$O p_doors$O p_enemy$O p_floor$O p_inter$O
OBJS4    = p_lights$O p_map$O p_maputl$O p_plats$O p_pspr$O p_setup$O p_sight$O p_spec$O
OBJS5    = p_switch$O p_mobj$O p_telept$O p_tick$O p_saveg$O p_user$O r_bsp$O r_data$O
OBJS6    = r_draw$O r_main$O r_plane$O r_segs$O r_sky$O r_things$O w_wad$O wi_stuff$O
OBJS7    = v_video$O st_lib$O st_stuff$O hu_stuff$O hu_lib$O s_sound$O z_zone$O info$O
OBJS8    = sounds$O i_system$O i_video$O i_main$O
OBJECTS  = $(OBJS1) $(OBJS2) $(OBJS3) $(OBJS4) $(OBJS5) $(OBJS6) $(OBJS7) $(OBJS8)

TOTAL_OBJS = $(OBJECTS)

EXELIBS         = $(BASELIBS)


.IF $(USE_OS232)

# Building OS/2 version (icc/gcc/Watcom)

TOTAL_OBJS += $(OS2OBJS)
# to compile under emx you need the mm4emx11.zip from hobbes
# and the so32dll/tcp32dll libs compiled under emx.
.IF $(EMX_LIBBASE)
CFLAGS      = -DNORMALUNIX -DOS2 -D__OS2__ -DTCPV40HDRS -Zomf -O3
EXELIBS     = -lso32dll -ltcp32dll -lmgl -lconsole -los2me -lsocket
.ELIF $(WC_LIBBASE)
CFLAGS      = -zq -bt=os2 -s -DNORMALUNIX -DOS2 -j -zp1 -bm -ei -5r -otexan
EXELIBS    += MMPM2.LIB
TOTAL_OBJS += fpfunc$O
.ELSE
CFLAGS      = -Q+ -Ss -DNORMALUNIX -DOS2 -J- -Sp1 -Su -Gm+ -O+ -G5 -Gf+ -Gi+
EXELIBS    += MMPM2.LIB TCP32DLL.LIB SO32DLL.LIB
TOTAL_OBJS += fpfunc$O
.ENDIF

.ELIF $(USE_LINUX)

CFLAGS     += -DNORMALUNIX -D__UNIX__ -DLINUX
TOTAL_OBJS += $(LNXOBJS)

.ELIF $(USE_WIN32)

.IF $(USE_WATCOM)

# Building Win32 console version (wcc386 assumed)

TOTAL_OBJS += $(DOSOBJS) i_soundd$O fpfunc$O
CFLAGS      = -zq -bt=nt -s -DNORMALUNIX -DDOS -j -zp1 -ei -5r -otexan

.ELSE

# Building Win32 console version (VC assumed)

TOTAL_OBJS += $(DOSOBJS) i_soundd$O
CFLAGS     += /Zp1
CFLAGS     += -DNORMALUNIX -DDOS

.ENDIF

.ELIF $(USE_RTTARGET)

# Building DOS/Win32-compatible version, including RTTarget32 (VC assumed)

TOTAL_OBJS += $(DOSOBJS) i_soundd$O
CFLAGS     += /Zp1
CFLAGS     += -DNORMALUNIX -DDOS

.ELIF $(USE_LINUX)

# Building Linux version (GCC assumed)

TOTAL_OBJS += $(LNXOBJS)
CFLAGS     += -DNORMALUNIX -D__UNIX__

.ELIF $(USE_QNXNTO)

# Building QNX 6 version (GCC assumed)

TOTAL_OBJS += $(QNXOBJS)
CFLAGS     += -DNORMALUNIX -D__UNIX__ -DQNX

.ELSE

# Building DOS version (wcc386 assumed)

TOTAL_OBJS += $(DOSOBJS) i_sounds$O fpfunc$O
CFLAGS      = -zq -bt=dos -s -DNORMALUNIX -DDOS -j -zp1 -ei -6r -otexan

.ENDIF

# ISV license required for MGL demo version with SNAP graphics
.IF $(ISV_LICENSE)
CFLAGS     += -DISV_LICENSE
.ENDIF

doom$E: $(TOTAL_OBJS)

#----------------------------------------------------------------------------
# Define the list of object files to create dependency information for
#----------------------------------------------------------------------------

DEPEND_OBJ      = $(OBJECTS) $(DOSOBJS)

# Define the list of object files to create dependency information for

.INCLUDE: "$(SCITECH)/makedefs/common.mk"
