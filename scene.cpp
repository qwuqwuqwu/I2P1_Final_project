#include "scene.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define COUNOF_BACKGROUND           ( 33 )

ALLEGRO_FONT *font = NULL;
ALLEGRO_BITMAP *g_background[ COUNOF_BACKGROUND ] = { NULL };
ALLEGRO_BITMAP *img;

int g_nBackgroundWidth = 0;

// function of menu
void menu_init(){
    font = al_load_ttf_font("./font/pirulen.ttf",12,0);
}

void menu_process( ALLEGRO_EVENT event )
{
    if( event.type == ALLEGRO_EVENT_KEY_UP ) {
        if( event.keyboard.keycode == ALLEGRO_KEY_ENTER ) {
            judge_next_window = true;
        }
    }
}

void menu_draw( void )
{
    img = al_load_bitmap( "intro.png" );
    assert( img != NULL );

    al_draw_bitmap( img, 0, 0, 0 );
}

void menu_destroy( void )
{
    al_destroy_font( font );
}

// function of game_scene
void game_scene_init( const int nLife )
{
    for( int i = 0; i < COUNOF_BACKGROUND; i++ ) {
        g_background[ i ] = al_load_bitmap("./image/background3.png");
        assert( g_background[ i ] != NULL );
    }
    g_nBackgroundWidth = al_get_bitmap_width( g_background[ 0 ] );

    character_init( COUNOF_BACKGROUND * g_nBackgroundWidth, nLife );
    monster_init();
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
    monster_draw();
}

/*
0         255|256          511|          |          |
  v  ++++++                       +++
++++++                    +++
                v      ++
         ++++++++++++++
 */

void game_scene_destroy( void )
{
    // background
    for( int i = 0; i < COUNOF_BACKGROUND; i++ ) {
        al_destroy_bitmap( g_background[ i ] );
    }
    printf( "destroy backgroun success\n" );
    object_destroy();
    character_destroy();
    monster_destroy();
    printf( "game scene destroy success!\n" );
}
