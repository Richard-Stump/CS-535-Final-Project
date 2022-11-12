# _____     ___ ____     ___ ____
#  ____|   |    ____|   |        | |____|
# |     ___|   |____ ___|    ____| |    \    PS2DEV Open Source Project.
#-----------------------------------------------------------------------
# Copyright 2001-2004, ps2dev - http://www.ps2dev.org
# Licenced under Academic Free License version 2.0
# Review ps2sdk README & LICENSE files for further details.

# External libraries and headers for the program. The libcommon.a file does get
# prefixed with -l since it is not in a library search path.
EE_LIBS = -ldraw -lgraph -lmath3d -lpacket2 -ldma ../Common/libcommon.a
EE_INCS = ../Common/include

EE_BIN = ./out/STARTUP.ELF
EE_OBJS = ./src/Main.o
EE_DVP = dvp-as

all: $(EE_BIN)
	mkdir -p out
	@echo "Done building startup program"

%.o: %.vsm
	$(EE_DVP) $< -o $@

vu1:
	openvcl ./vu1/vu1_code.vlc > ./vu1/vu1_code.vsm

clean:
	rm -f $(EE_BIN) $(EE_OBJS)
	rm -rf out

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
