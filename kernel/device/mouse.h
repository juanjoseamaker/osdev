#pragma once

#define LEFT_BUTTON 0x01
#define RIGHT_BUTTON 0x02

void init_mouse();
char mouse_buttons();
int mouse_coord_x();
int mouse_coord_y();
