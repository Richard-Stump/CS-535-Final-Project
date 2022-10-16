# _____     ___ ____     ___ ____
#  ____|   |    ____|   |        | |____|
# |     ___|   |____ ___|    ____| |    \    PS2DEV Open Source Project.
#-----------------------------------------------------------------------
# Copyright 2001-2004, ps2dev - http://www.ps2dev.org
# Licenced under Academic Free License version 2.0
# Review ps2sdk README & LICENSE files for further details.

EE_BIN = ./out/project.elf
EE_OBJS = ./vu1/vu1_code.o ./src/project.o ./src/fplib.o
EE_LIBS = -ldebug -lc -ldraw -lgraph -lmath3d -lpacket2 -ldma -lpacket
EE_DVP = dvp-as
ISO_NAME = ./out/project.iso

all: $(EE_BIN)
	mkisofs -o $(ISO_NAME) ./out
	pcsx2 ./out/project.iso

%.o: %.vsm
	$(EE_DVP) $< -o $@

iso:
	mkisofs -o ./out/project.iso ./out

vu1:
	openvcl ./vu1/vu1_code.vlc > ./vu1/vu1_code.vsm

clean:
	rm -f $(EE_BIN) $(EE_OBJS) $(ISO_NAME)

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
