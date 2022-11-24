# _____     ___ ____     ___ ____
#  ____|   |    ____|   |        | |____|
# |     ___|   |____ ___|    ____| |    \    PS2DEV Open Source Project.
#-----------------------------------------------------------------------
# Copyright 2001-2004, ps2dev - http://www.ps2dev.org
# Licenced under Academic Free License version 2.0
# Review ps2sdk README & LICENSE files for further details.

# External libraries and headers for the program. The libcommon.a file does get
# prefixed with -l since it is not in a library search path.
EE_LIBS = ../../Common/libcommon.a -L$(GSKIT)/lib -lgskit -lpad -lpadx -ldmakit -ldraw -lgraph -lmath3d -lpacket2 -ldma -lpacket -lstdc++
EE_INCS = ../../Common/include -I$(GSKIT)/include
EE_CXXFLAGS = -fpermissive
# Options related to this specific program
EE_BIN = ./out/LAB3.elf
EE_OBJS = ./src/Main.o ./src/Ps2.o ./src/Application.o ./src/GameObject.o ./src/Mesh.o
EE_DVP = dvp-as
#EE_VCL = vcl

all: $(EE_BIN)
	$(EE_STRIP) --strip-all $(EE_BIN)
	cp resources/*.obj out/
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
	rm -rf out

run: $(EE_BIN)
	ps2client execee host:$(EE_BIN)

reset:
	ps2client reset

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
