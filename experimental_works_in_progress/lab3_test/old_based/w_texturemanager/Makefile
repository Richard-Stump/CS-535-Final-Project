## from https://github.com/williamblair/PS2Pad, modified by Rey Gonzalez and Richard Stump

# ---------------------------------------------------------

#ifndef SOURCE_PATH
#SOURCE_PATH = /home/bj/Documents/console_dev/ps2/glampert/glampert-ps2dev-tests-dfef4fc8fdeb/source
#endif


# NAMETAG = main
ELF_NT = ./out/main.elf
ISO_NT = ./out/main.iso

ifndef INSTALL_PATH
INSTALL_PATH = /Applications/pcsx2.app/Contents/Resources/pcsx2
endif

ifndef RUN_CMD
RUN_CMD = open -a pcsx2
endif

ifndef EE_BIN
EE_BIN = $(ELF_NT)
endif


# ---------------------------------------------------------

EE_OBJS =                                           \
	./src/main.o \
	./src/oldCamera.o \
	./src/Shape.o \
	./src/TexturedShape.o \
	./src/TexturedCubeMesh.o \
	./src/TextureManager.o \
	../../../../globals/src/PS2Pad.o \
	../../../../globals/vu1/draw_3D.o
# ./src/font.o \
# ./src/texture.o

# ---------------------------------------------------------

EE_LIBS =     \
	-lc       \
	-ldma     \
	-lgraph   \
	-ldraw    \
	-lpatches \
	-lpad \
	-ldebug \
	-lmath3d \
	-lpacket2 \
	-lpacket
# -lfont

# ---------------------------------------------------------

GLOBAL_DEFINES =                      \
	-I./                              \
	-Dnullptr=NULL                    \
	-DLOG_PRINTF_ADD_MESSAGE_PREFIX=1 \
	-DUSE_CUSTOM_ASSERT=1

# ---------------------------------------------------------

EE_CXXFLAGS +=                \
	-std=c++11                  \
	-O3                       \
	-funroll-loops            \
	-fexpensive-optimizations \
	-fno-exceptions           \
	-fno-rtti                 \
	-Wall                     \
	-Wformat=2                \
	-Wmissing-braces          \
	-Wparentheses             \
	-Wpointer-arith           \
	-Wreturn-type             \
	-Wsequence-point          \
	-Wswitch                  \
	-Wuninitialized           \
	-Wunknown-pragmas         \
	-Wwrite-strings           \
	$(GLOBAL_DEFINES)

# ---------------------------------------------------------

ISO_NAME = $(ISO_NT)

all: $(EE_BIN)
	mkisofs -o $(ISO_NT) ./out
	
# ee-strip --strip-all $(EE_BIN)

# ---------------------------------------------------------

clean:
	rm -f ./out/*.elf ./out/*.iso ./src/*.o
# rm -f ./out/*.elf ./src/*.o *.a
# ---------------------------------------------------------

run: $(EE_BIN)
	cp $(EE_BIN) $(INSTALL_PATH)/$(EE_BIN)
	$(RUN_CMD)

# ---------------------------------------------------------

rebuild: clean all

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal_cpp

# ---------------------------------------------------------

