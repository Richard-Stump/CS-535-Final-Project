# _____     ___ ____     ___ ____
#  ____|   |    ____|   |        | |____|
# |     ___|   |____ ___|    ____| |    \    PS2DEV Open Source Project.
#-----------------------------------------------------------------------
# Copyright 2001-2004, ps2dev - http://www.ps2dev.org
# Licenced under Academic Free License version 2.0
# Review ps2sdk README & LICENSE files for further details.

# External libraries and headers for the program. 
EE_OBJS = ./src/glmTexturedCube.o ./src/PS2Pad.o ./src/TexturedCube.o
EE_LIBS = -ldebug -lc -ldraw -lgraph -lmath3d -lpacket2 -ldma -lpacket
EE_DVP = dvp-as

EE_INCS = ./include/
EE_LIB = libcommon.a

all: $(EE_LIB)
	@echo "Done building common library"

clean:
	rm -f $(EE_LIB) $(EE_OBJS)
	rm -rf out

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
