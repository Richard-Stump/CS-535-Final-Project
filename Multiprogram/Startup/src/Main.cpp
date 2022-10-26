
// Any of the SDK headers that are not part of GCC's libraries need to be in an
// extern "C" block to prevent C++ name mangling. 
#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include <kernel.h>
#include <stdio.h>
#include <tamtypes.h>
#include <sifrpc.h>
#include <debug.h>
#include <unistd.h>
#include <gif_tags.h>

#include <gs_gp.h>
#include <gs_psm.h>

#include <dma.h>
#include <dma_tags.h>

#include <draw.h>
#include <graph.h>
#include <packet.h>


#include <loadfile.h>
#include <debug.h>

#ifdef __cplusplus
}
#endif//__cplusplus

int main(int argc, char** argv)
{
    // Load up the cubes elf to see if this works.
    LoadExecPS2("cdrom0:\\MILES000.ELF", argc, argv);
}