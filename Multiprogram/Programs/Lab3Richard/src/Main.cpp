/*
# _____     ___ ____     ___ ____
#  ____|   |    ____|   |        | |____|
# |     ___|   |____ ___|    ____| |    \    PS2DEV Open Source Project.
#-----------------------------------------------------------------------
# Copyright 2022, ps2dev - http://www.ps2dev.org
# Licenced under Academic Free License version 2.0
# Review ps2sdk README & LICENSE files for further details.
#
*/

#include <kernel.h>
#include <stdint.h>
#include <stdio.h>
#include <ps2sdkapi.h>
#include <dma.h>

#include <packet2.h>

#include <stdint.h>

const float lineData[] = {
    0, 0,
    200, 200
};
const uint32_t numLines = 2;

void sendDataToVu1(void* ee_ptr, void* vu1_ptr, size_t numBytes)
{
    packet2_t* packet = packet2_create_from()
}

int main(int argc, char *argv[])
{    
    dma_channel_initialize(DMA_CHANNEL_VIF1, nullptr, 0);
	dma_channel_fast_waits(DMA_CHANNEL_VIF1);



    return 0;
}