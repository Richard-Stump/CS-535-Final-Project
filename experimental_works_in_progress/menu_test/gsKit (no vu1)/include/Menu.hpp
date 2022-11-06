#pragma once
#include "glm/glm.hpp"
using Vec2 = glm::vec2;
using Color4 = glm::vec4;

#include <tamtypes.h>

#include "./gs/gsKit.h"
#include "./dma/include/dmaKit.h"
#include "./toolkit/include/gsToolkit.h"

class Menu
{
public:
    GSGLOBAL* context;
    Vec2 top_left_corner;
    Vec2 bottom_right_corner;
    int max_x, mid_x, max_y, mid_y;

    const char* title;
    int title_length, title_offset_x, title_offset_y;
    
    u64 menu_color;
    u64 font_color;
    GSFONTM* font;

    Menu(GSGLOBAL* context, Vec2 top_left, Vec2 size, Color4 _color);
    ~Menu();

    void draw();
    void set_font(Color4 _color);
    void set_title(const char* _title);
    void print_title();
};

u64 int_to_hex(int n);