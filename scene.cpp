#include "scene.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define COUNOF_BACKGROUND           ( 4 )

ALLEGRO_FONT *font = NULL;
ALLEGRO_BITMAP *g_background[ COUNOF_BACKGROUND ] = { NULL };

int g_nBackgroundWidth = 0;

// function of menu
void menu_init(){
    font = al_load_ttf_font("./font/pirulen.ttf",12,0);
}
void menu_process(ALLEGRO_EVENT event){
    if( event.type == ALLEGRO_EVENT_KEY_UP )
        if( event.keyboard.keycode == ALLEGRO_KEY_ENTER )
            judge_next_window = true;
}
void menu_draw(){
    al_clear_to_color(al_map_rgb(100,100,100));
    al_draw_text(font, al_map_rgb(255,255,255), WIDTH/2, HEIGHT/2+120 , ALLEGRO_ALIGN_CENTRE, "Press 'Enter' to start");
    al_draw_rectangle(WIDTH/2-150, 310, WIDTH/2+150, 350, al_map_rgb(255, 255, 255), 0);
}
void menu_destroy(){
    al_destroy_font(font);
}

// function of game_scene
void game_scene_init(){
    for( int i = 0; i < COUNOF_BACKGROUND; i++ ) {
        g_background[ i ] = al_load_bitmap("./image/test3.png");
    }
    g_nBackgroundWidth = al_get_bitmap_width( g_background[ 0 ] );

    character_init( COUNOF_BACKGROUND * g_nBackgroundWidth );
    character_init2();
    object_init();
}

void game_scene_draw( void ){
    // plot background
    for( int i = 0; i < COUNOF_BACKGROUND; i++ ) {
       al_draw_bitmap( g_background[ i ], i * g_nBackgroundWidth, 0, 0 );
    }

    object_draw();

    // draw character
    character_draw();
    character_draw2();
}

/*
0         255|256          511|          |          |
  v  ++++++                       +++
++++++                    +++
                v      ++
         ++++++++++++++
 */

void game_scene_destroy(){
    // background
    for( int i = 0; i < COUNOF_BACKGROUND; i++ ) {
        al_destroy_bitmap( g_background[ i ] );
    }

    object_destroy();

    character_destory();
    character_destory2();
}
