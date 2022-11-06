#  ____     ___ |    / _____ _____
# |  __    |    |___/    |     |
# |___| ___|    |    \ __|__   |     gsKit Open Source Project.
# ----------------------------------------------------------------------
# Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
# Licenced under Academic Free License version 2.0
# Review gsKit README & LICENSE files for further details.
#
# examples/fontm/Makefile - Makefile for "fontm" example.
#

EE_BIN  = ./out/main.elf
EE_OBJS = ./src/main.o ./src/PS2Pad.o ../../../globals/src/TexturedCube.o ./vu1/draw_3D.o
EE_ISO = ./out/main.iso
EE_DVP = dvp-as

include ./Makefile.global
include ./Makefile.pref

iso:
	mkisofs -o $(EE_ISO) ./out

run:
	pcsx2 $(EE_BIN)

%.o: %.vsm
	$(EE_DVP) $< -o $@

rebuild:
	make clean
	make all
	make iso
	make run
