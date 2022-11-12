# _____     ___ ____     ___ ____
#  ____|   |    ____|   |        | |____|
# |     ___|   |____ ___|    ____| |    \    PS2DEV Open Source Project.
#-----------------------------------------------------------------------
# Copyright 2001-2004, ps2dev - http://www.ps2dev.org
# Licenced under Academic Free License version 2.0
# Review ps2sdk README & LICENSE files for further details.

EE_BIN = ./out/milestoneA.elf
EE_OBJS = ./src/Main.o
EE_LIBS = -ldraw -lgraph -lmath3d -lpacket2 -ldma
EE_DVP = dvp-as
ISO_NAME = ./out/milestoneA.iso
#EE_VCL = vcl

all: $(EE_BIN)
#all: ./images/bedrich.c $(EE_BIN)
	$(EE_STRIP) --strip-all $(EE_BIN)
	mkisofs -o $(ISO_NAME) ./out
#	pcsx2 $(ISO_NAME)

# Original VCL tool preferred. 
# It can be runned on WSL, but with some tricky commands: 
# https://github.com/microsoft/wsl/issues/2468#issuecomment-374904520
#%.vsm: %.vcl
#	$(EE_VCL) $< >> $@

%.o: %.vsm
	$(EE_DVP) $< -o $@

clean:
	rm -f $(EE_BIN) $(EE_OBJS) $(ISO_NAME)

run: $(EE_BIN)
	ps2client execee host:$(EE_BIN)

reset:
	ps2client reset

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
