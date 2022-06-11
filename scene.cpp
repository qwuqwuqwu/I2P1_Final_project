#include "scene.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define COUNOF_BACKGROUND           ( 4 )

ALLEGRO_FONT *font = NULL;
ALLEGRO_BITMAP *g_background[ COUNOF_BACKGROUND ] = { NULL };
ALLEGRO_BITMAP *img;

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
    img = al_load_bitmap("intro.jpg"); // ANS: load image
    assert( img != NULL );
    al_draw_bitmap(img, 0, 0, 0); // ANS: draw image

}
void menu_destroy(){
    al_destroy_font(font);
}

// function of game_scene
void game_scene_init(){
    for( int i = 0; i < COUNOF_BACKGROUND; i++ ) {
        g_background[ i ] = al_load_bitmap("./image/background1.png");
        assert( g_background[ i ] != NULL );
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
