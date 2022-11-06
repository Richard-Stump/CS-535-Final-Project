#include "../include/Menu.hpp"


u64 int_to_hex(int n)
{
    return 255;
}

Menu::Menu(
            GSGLOBAL* _context, Vec2 top_left,
            Vec2 bottom_right,
            Color4 _color
            )
{
    context = _context;
    top_left_corner = Vec2(top_left);
    bottom_right_corner = Vec2(bottom_right);
    max_x = bottom_right_corner.x - top_left_corner.x;
    max_y = bottom_right_corner.y - top_left_corner.y;
    mid_x = (int)((top_left_corner.x + bottom_right_corner.x) / 2);

    menu_color = GS_SETREG_RGBAQ(
                            (int)_color[0],
                            (int)_color[1],
                            (int)_color[2],
                            (int)_color[3],
                            0
                            );


}

Menu::~Menu()
{

}

void Menu::draw()
{
    gsKit_prim_sprite(
                        context,
                        top_left_corner.x,
                        top_left_corner.y,
                        bottom_right_corner.x,
                        bottom_right_corner.y,
                        3, 
                        menu_color
                    );
}

void Menu::set_font(Color4 _color)
{
    font = gsKit_init_fontm();
	gsKit_fontm_upload(context, font);
    font_color = GS_SETREG_RGBAQ(
                            (int)_color[0],
                            (int)_color[1],
                            (int)_color[2],
                            (int)_color[3],
                            0
                            );
}

void Menu::set_title(const char* _title)
{
    title = _title;
    title_length = strlen(title);
    title_offset_x = (int)((max_x - title_length)/2);
    title_offset_y = (int)((max_y)/16);
}

void Menu::print_title()
{
    gsKit_fontm_print_scaled(context, font, top_left_corner.x + title_offset_x, top_left_corner.y + title_offset_y, 4, 0.55f, font_color, title);
	font->Align = GSKIT_FALIGN_CENTER;
}