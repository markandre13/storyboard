#
# Storyboard 0.1
# Copyright (C) 1998 Mark-André Hopf
#
#-----------------------------------------------------------------------------

PRGFILE	    = storyboard

SRCS        = main.cc view3d.cc glwindow.cc

LIBS 	    = -lXext -lMesaGL -lMesaGLU -lm
LIBDIRS	    = -L/opt/Mesa-3.0/lib
INCDIRS	    = -I/opt/Mesa-3.0/include

CLEAN =

# DEFINES = -D_REENTRANT -D__TOAD_THREADS

include ../toad/conf/MakefilePrg

