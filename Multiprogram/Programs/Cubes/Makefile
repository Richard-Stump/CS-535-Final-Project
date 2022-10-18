# _____     ___ ____     ___ ____
#  ____|   |    ____|   |        | |____|
# |     ___|   |____ ___|    ____| |    \    PS2DEV Open Source Project.
#-----------------------------------------------------------------------
# Copyright 2001-2004, ps2dev - http://www.ps2dev.org
# Licenced under Academic Free License version 2.0
# Review ps2sdk README & LICENSE files for further details.

EE_BIN = ./out/milestoneA.elf
EE_OBJS = ./vu1/draw_3D.o ./src/main.o
EE_LIBS = -ldraw -lgraph -lmath3d -lpacket2 -ldma
EE_DVP = dvp-as
ISO_NAME = ./out/milestoneA.iso
#EE_VCL = vcl

all: ./images/purdue_logo.c $(EE_BIN)
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

# zbyszek.c:
# 	./bin2c zbyszek.raw zbyszek.c zbyszek

purdue_logo.c:
	./tools/bin2c purdue_logo.raw purdue_logo.c purdue_logo

# purdue.c:
# 	./bin2c purdue.raw purdue.c purdue

# small_purdue_logo.c:
# 	./bin2c small_purdue_logo.raw small_purdue_logo.c small_purdue_logo

clean:
	rm -f $(EE_BIN) $(EE_OBJS) $(ISO_NAME)

run: $(EE_BIN)
	ps2client execee host:$(EE_BIN)

reset:
	ps2client reset

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
