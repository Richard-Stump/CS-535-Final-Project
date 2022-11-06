#include "../include/TextureManager.hpp"

TextureManager::TextureManager(unsigned char* _texture_array)
{
    count = 1;
    texbuffer = (texbuffer_t**)malloc(1);
    texture_array = (unsigned char**)malloc(1);
    texture_array[0] = _texture_array;
    lod = (lod_t*)malloc(1);
    clut = (clutbuffer_t*)malloc(1);
    prim = (prim_t*)malloc(1);

    init();
}

// TextureManager::TextureManager(unsigned int _count) : count(_count)
// {
//     texbuffer = (texbuffer_t*)malloc(count);
// }

TextureManager::~TextureManager()
{
    free(texbuffer);
    free(texture_array);
    free(lod);
    free(clut);
    free(prim);
}

void TextureManager::init()
{
    for(int i = 0; i < count; i++)
    {
        lod[i].calculation = LOD_USE_K;
        lod[i].max_level = 0;
        lod[i].mag_filter = LOD_MAG_NEAREST;
        lod[i].min_filter = LOD_MIN_NEAREST;
        lod[i].l = 0;
        lod[i].k = 0;

        clut[i].storage_mode = CLUT_STORAGE_MODE1;
        clut[i].start = 0;
        clut[i].psm = 0;
        clut[i].load_method = CLUT_NO_LOAD;
        clut[i].address = 0;

        // Define the triangle primitive we want to use.
        prim[i].type = PRIM_TRIANGLE;
        prim[i].shading = PRIM_SHADE_GOURAUD;
        prim[i].mapping = DRAW_ENABLE;
        prim[i].fogging = DRAW_DISABLE;
        prim[i].blending = DRAW_ENABLE;
        prim[i].antialiasing = DRAW_DISABLE;
        prim[i].mapping_type = PRIM_MAP_ST;
        prim[i].colorfix = PRIM_UNFIXED;

        texbuffer[i]->info.width = draw_log2(128);
        texbuffer[i]->info.height = draw_log2(128);
        texbuffer[i]->info.components = TEXTURE_COMPONENTS_RGB;
        texbuffer[i]->info.function = TEXTURE_FUNCTION_DECAL;
    }
}

void TextureManager::send_texture()
{
    if(count == 1)
    {
        packet2_t *packet2 = packet2_create(50, P2_TYPE_NORMAL, P2_MODE_CHAIN, 0);
        packet2_update(packet2, draw_texture_transfer(packet2->next, texture_array[0], 128, 128, GS_PSM_24, texbuffer[0]->address, texbuffer[0]->width));
        packet2_update(packet2, draw_texture_flush(packet2->next));
        dma_channel_send_packet2(packet2, DMA_CHANNEL_GIF, 1);
        dma_wait_fast();
        packet2_free(packet2);
    }
}

// void TextureManager::send_textures(unsigned char** list_of_texture_arrays)
// {
//     texbuffer_t* texbuf = nullptr;
//     for(int i = 0; i < count; i++)
//     {
//         texbuf = texbuffer[i];
//         packet2_t *packet2 = packet2_create(50, P2_TYPE_NORMAL, P2_MODE_CHAIN, 0);
//         packet2_update(packet2, draw_texture_transfer(packet2->next, zbyszek, 128, 128, GS_PSM_24, texbuf->address, texbuf->width));
//         packet2_update(packet2, draw_texture_flush(packet2->next));
//         dma_channel_send_packet2(packet2, DMA_CHANNEL_GIF, 1);
//         dma_wait_fast();
//         packet2_free(packet2);
//     }
// }