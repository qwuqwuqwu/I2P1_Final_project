#include "global.h"
#include "object.h"

void character_init( const int nTerrainWidth, const int nLife );
void charater_process( ALLEGRO_EVENT event );
void charater_update();
void character_predraw( int nGroundY );
void charactor_show( void );
void character_draw();
void character_destroy();

void monster_init( void );
void monster_process( ALLEGRO_EVENT event );
void monster_update( void );
void monster_draw( void );
void monster_destroy( void );

bool isCharacterAlive( void );
