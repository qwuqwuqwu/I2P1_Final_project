#include "charater.h"

#define MAX_COUNTOF_CONTINUOUS_JUMP ( 2 )
#define JUMP_VY ( -90 )

// the state of character
enum {STOP = 0, MOVE, ATK};

ALLEGRO_SAMPLE *sample = NULL;

float g_CameraPosition = 0.0;
ALLEGRO_TRANSFORM camera;
int g_nTerrainWidth = 0;

Character *pCharacter;

float g_Gravity = 9.8;
float g_Tick = 0.2;


void CameraUpdate( float *CamPosition, int x, int y, int width, int height )
{
    *CamPosition = -( WIDTH / 2 ) + ( x + width / 2 );

    if( *CamPosition < 0 ) {
        *CamPosition = 0.0;
    }
    else if( *CamPosition >= ( g_nTerrainWidth - WIDTH ) ) {
        *CamPosition = ( float )( g_nTerrainWidth - WIDTH);
    }
}

void character_init( const int nTerrainWidth ){
    // load character images
    for(int i = 1 ; i <= 2 ; i++){
        char temp[50];
        sprintf( temp, "./image/char_move_small%d.png", i );
        e_pchara->img_move[i-1] = al_load_bitmap(temp);
    }
    for(int i = 1 ; i <= 2 ; i++){
        char temp[50];
        sprintf( temp, "./image/char_atk_small%d.png", i );
        e_pchara->img_atk[i-1] = al_load_bitmap(temp);
    }
    for(int i = 1 ; i <= 2 ; i++){
        char temp[50];
        sprintf( temp, "./image/char_transform_small%d.png", i );
        e_pchara->img_transform[i-1] = al_load_bitmap(temp);
    }
    // load effective sound
    sample = al_load_sample("./sound/atk_sound.wav");
    e_pchara->atk_Sound  = al_create_sample_instance(sample);
    al_set_sample_instance_playmode(e_pchara->atk_Sound, ALLEGRO_PLAYMODE_ONCE);
    al_attach_sample_instance_to_mixer(e_pchara->atk_Sound, al_get_default_mixer());

    // initial the geometric information of character
    e_pchara->width = al_get_bitmap_width(e_pchara->img_move[0]);
    e_pchara->height = al_get_bitmap_height(e_pchara->img_move[0]);
    e_pchara->x = WIDTH/2-100;
    e_pchara->y = HEIGHT/2;
    e_pchara->dir = false;
    e_pchara->hp = 3;

    // initial the animation component
    e_pchara->state = STOP;
    e_pchara->anime = 0;
    e_pchara->anime_time = 30;

    // gravity
    e_pchara->y0 = e_pchara->y;
    e_pchara->vy = 0.0;
    e_pchara->FallingTick = 0.0;
    e_pchara->nJumpCount = 0;

    // terrain width
    g_nTerrainWidth = nTerrainWidth;

    // transform status
    e_pchara->TransformStatus = ETS_NORMAL;
    e_pchara->nTransformCursor = 0;
    e_pchara->nTransformTime = 30;
    e_pchara->NowSpecialAtk = ESA_NORMAL;
    e_pchara->NextSpecailAtk = ESA_NORMAL;
    for( int i = 0; i < ESA_NUM; i++ ){
        switch( i ) {
        case ESA_NORMAL:
            e_pchara->img_SpecialAtk[ 2 * ESA_NORMAL ] = al_load_bitmap( "./image/char_move_small1.png" );
            e_pchara->img_SpecialAtk[ 2 * ESA_NORMAL + 1 ] = al_load_bitmap( "./image/char_move_small2.png" );
            break;

        case ESA_SWORD:
            e_pchara->img_SpecialAtk[ 2 * ESA_SWORD ] = al_load_bitmap( "./image/char_move_sword1.png" );
            e_pchara->img_SpecialAtk[ 2 * ESA_SWORD + 1 ] = al_load_bitmap( "./image/char_move_sword2.png" );
            break;

        case ESA_BOMB:
            e_pchara->img_SpecialAtk[ 2 * ESA_BOMB ] = al_load_bitmap( "./image/char_move_bomb1.png" );
            e_pchara->img_SpecialAtk[ 2 * ESA_BOMB + 1 ] = al_load_bitmap( "./image/char_move_bomb2.png" );
            break;

        case ESA_FIRE:
            e_pchara->img_SpecialAtk[ 2 * ESA_FIRE ] = al_load_bitmap( "./image/char_move_fire1.png" );
            e_pchara->img_SpecialAtk[ 2 * ESA_FIRE + 1 ] = al_load_bitmap( "./image/char_move_fire2.png" );
            break;

        default:
            assert( false );
            break;
        }
    }
}

void charater_process(ALLEGRO_EVENT event){
    // process the animation
    if( event.type == ALLEGRO_EVENT_TIMER ){
        if( event.timer.source == fps ){
            e_pchara->anime++;
            e_pchara->anime %= e_pchara->anime_time;

            if( e_pchara->TransformStatus == ETS_TRANSFORMING ) {
                e_pchara->nTransformCursor++;
                e_pchara->nTransformCursor %= e_pchara->nTransformTime;

                if( e_pchara->nTransformCursor == 0 ) {
                    e_pchara->TransformStatus = ETS_TRANSFORMED;
                    e_pchara->NowSpecialAtk = e_pchara->NextSpecailAtk;
                    e_pchara->NextSpecailAtk = ESA_NORMAL;
                }
            }
        }
    // process the keyboard event
    }else if( event.type == ALLEGRO_EVENT_KEY_DOWN ){
        key_state[event.keyboard.keycode] = true;
        e_pchara->anime = 0;
    }else if( event.type == ALLEGRO_EVENT_KEY_UP ){
        key_state[event.keyboard.keycode] = false;
    }
}

void charater_update(){
    // use the idea of finite state machine to deal with different state
    if( key_state[ALLEGRO_KEY_W] ){
        if( e_pchara->nJumpCount < MAX_COUNTOF_CONTINUOUS_JUMP ) {
            // 1. prevent second jump before certain time of the first jump
            // 2. only two consecutive jump is allowed
            if( e_pchara->nJumpCount >= 1 ) {
                if( e_pchara->vy > JUMP_VY * 0.6 ) {
                    e_pchara->vy = JUMP_VY; // jump upward velocity
                    e_pchara->nJumpCount++;
                    e_pchara->y0 = e_pchara->y;
                    e_pchara->FallingTick = 0;
                }
            }
            else {
                e_pchara->vy = JUMP_VY; // jump upward velocity
                e_pchara->nJumpCount++;
            }
        }

        if( key_state[ALLEGRO_KEY_A] ) {
            e_pchara->dir = false;
            e_pchara->x -= 3;
        }
        else if( key_state[ALLEGRO_KEY_D] ) {
            e_pchara->dir = true;
            e_pchara->x += 3;
        }
        e_pchara->state = MOVE;

    }else if( key_state[ALLEGRO_KEY_A] ){
        e_pchara->dir = false;
        e_pchara->x -= 3;
        e_pchara->state = MOVE;
    }else if( key_state[ALLEGRO_KEY_S] ){
        if(key_state[ALLEGRO_KEY_SPACE])
        {
            e_pchara->dir?e_pchara->x += 10: e_pchara->x-= 10;
            e_pchara->state = ATK;
        }
        else
        {
            e_pchara->y += 10;
            e_pchara->state = MOVE;
        }
    }else if( key_state[ALLEGRO_KEY_D] ){
        e_pchara->dir = true;
        e_pchara->x += 3;
        e_pchara->state = MOVE;

    /*}else if( key_state[ALLEGRO_KEY_SPACE] ){   //吸的技能
        e_pchara->state = ATK;*/


    }else if( e_pchara->anime == e_pchara->anime_time-1 ){
        e_pchara->anime = 0;
        e_pchara->state = STOP;
    }else if ( e_pchara->anime == 0 ){
        e_pchara->state = STOP;
    }

}

void character_gravity( int nGroundY ) {
    if( nGroundY == -1 ) {
        nGroundY = HEIGHT;
    }

    // ticktock
    e_pchara->FallingTick += g_Tick;

    // velocity;
    e_pchara->vy += g_Gravity * g_Tick;
    // position
    e_pchara->y = e_pchara->y0
        + ( int )(e_pchara->vy * e_pchara->FallingTick + ( 0.5 * g_Gravity * ( e_pchara->FallingTick * e_pchara->FallingTick ) ) );


    if( ( e_pchara->y + e_pchara->height ) >= nGroundY ) {
        e_pchara->y = nGroundY - e_pchara->height;
        e_pchara->y0 = e_pchara->y;
        e_pchara->vy = 0.0;
        e_pchara->FallingTick = 0;
        e_pchara->nJumpCount = 0;
    }
    else {
        // do nothing
    }
}

void character_BoundryCheck( void ) {
    // check x
    if( e_pchara->x < 0 ) {
        e_pchara->x = 0;
    }
    else if( e_pchara->x + e_pchara->width > g_nTerrainWidth ) {
        e_pchara->x = g_nTerrainWidth - e_pchara->width;
    }

    // check y
    // sky is unlimited
    if( e_pchara->y + e_pchara->height > HEIGHT ) {
        e_pchara->y = HEIGHT - e_pchara->height;
    }
}

typedef struct _Position {
    int w;
    int e;
    int n;
    int s;
} Position;

bool character_checkOverlap( const Position *plhs, const Position *prhs ) {
    if( ( plhs->w < prhs->w && plhs->w < prhs->e ) ||
        ( plhs->e > prhs->e && plhs->e > prhs->w ) ) {
            return false;
    }

    if( ( plhs->n < prhs->n && plhs->n < prhs->s ) ||
        ( plhs->s > prhs->s && plhs->s > prhs->n ) ) {
            return false;
    }

    return true;
}

void character_checkTransform( void ) {
    if( e_pchara->TransformStatus == ETS_TRANSFORMING ) {
        return;
    }
    // given monster position
    Position MonsterPos;
    MonsterPos.e = 100;
    MonsterPos.w = 300;
    MonsterPos.n = 100;
    MonsterPos.s = 400;

    Position CharacterPos;
    CharacterPos.w = e_pchara->x;
    CharacterPos.e = e_pchara->x + e_pchara->width;
    CharacterPos.n = e_pchara->y;
    CharacterPos.s = e_pchara->y + e_pchara->height;

    if( character_checkOverlap( &CharacterPos, &MonsterPos ) == true ) {
        e_pchara->state = STOP;
        e_pchara->TransformStatus = ETS_TRANSFORMING;
        e_pchara->nTransformCursor = 0;
        e_pchara->NextSpecailAtk = ESA_SWORD;
    }
}

void character_draw(){
    CameraUpdate( &g_CameraPosition, e_pchara->x, e_pchara->y, e_pchara->width, e_pchara->height );
    al_identity_transform( &camera );
    al_translate_transform( &camera, -g_CameraPosition, 0 );
    al_use_transform( &camera );

    int nGroundY = FindAndDrawClosestGroundY();

    character_gravity( nGroundY );

    character_BoundryCheck();

    // check transform
    character_checkTransform();


    // check monster alive?
    charater_update2();

    if( e_pchara->TransformStatus == ETS_TRANSFORMING ) {
        if( e_pchara->dir ){
            if( e_pchara->anime < e_pchara->nTransformTime / 2 ){
                al_draw_bitmap(e_pchara->img_transform[ 0 ], e_pchara->x, e_pchara->y, ALLEGRO_FLIP_HORIZONTAL );
            }else{
                al_draw_bitmap(e_pchara->img_transform[ 1 ], e_pchara->x, e_pchara->y, ALLEGRO_FLIP_HORIZONTAL );
            }
        }else{
            if( e_pchara->anime < e_pchara->nTransformTime / 2 ){
                al_draw_bitmap(e_pchara->img_transform[ 0 ], e_pchara->x, e_pchara->y, 0 );
            }else{
                al_draw_bitmap(e_pchara->img_transform[ 1 ], e_pchara->x, e_pchara->y, 0 );
            }
        }
    }
    else {
        ALLEGRO_BITMAP *img_move[ 2 ] = { e_pchara->img_move[ 0 ], e_pchara->img_move[ 1 ] };
        ALLEGRO_BITMAP *img_atk[ 2 ] = { e_pchara->img_atk[ 0 ], e_pchara->img_atk[ 1 ] };
        if( e_pchara->TransformStatus == ETS_TRANSFORMED ) {
            img_move[ 0 ] = e_pchara->img_SpecialAtk[ 2 * e_pchara->NowSpecialAtk ];
            img_move[ 1 ] = e_pchara->img_SpecialAtk[ 2 * e_pchara->NowSpecialAtk + 1 ];
        }

        // with the state, draw corresponding image
        if( e_pchara->state == STOP ){
            if( e_pchara->dir )
                al_draw_bitmap( img_move[ 0 ], e_pchara->x, e_pchara->y, ALLEGRO_FLIP_HORIZONTAL);
            else
                al_draw_bitmap( img_move[ 0 ], e_pchara->x, e_pchara->y, 0);
        }else if( e_pchara->state == MOVE ){
            if( e_pchara->dir ){
                if( e_pchara->anime < e_pchara->anime_time/2 ){
                    al_draw_bitmap( img_move[ 0 ], e_pchara->x, e_pchara->y, ALLEGRO_FLIP_HORIZONTAL);
                }else{
                    al_draw_bitmap( img_move[ 1 ], e_pchara->x, e_pchara->y, ALLEGRO_FLIP_HORIZONTAL);
                }
            }else{
                if( e_pchara->anime < e_pchara->anime_time/2 ){
                    al_draw_bitmap( img_move[ 0 ], e_pchara->x, e_pchara->y, 0);
                }else{
                    al_draw_bitmap( img_move[ 1 ], e_pchara->x, e_pchara->y, 0);
                }
            }
        }else if( e_pchara->state == ATK ){
            if( e_pchara->dir ){
                if( e_pchara->anime < e_pchara->anime_time/2 ){
                    al_draw_bitmap( img_atk[ 0 ], e_pchara->x, e_pchara->y, ALLEGRO_FLIP_HORIZONTAL);
                    al_draw_bitmap( img_atk[ 0 ], e_pchara->x+50, e_pchara->y, ALLEGRO_FLIP_HORIZONTAL);
                }else{
                    al_draw_bitmap( img_atk[ 1 ], e_pchara->x, e_pchara->y, ALLEGRO_FLIP_HORIZONTAL);
                    al_play_sample_instance(e_pchara->atk_Sound);
                }
            }else{
                if( e_pchara->anime < e_pchara->anime_time/2 ){
                    al_draw_bitmap( img_atk[ 0 ], e_pchara->x, e_pchara->y, 0);
                    al_draw_bitmap( img_atk[ 0 ], e_pchara->x+50, e_pchara->y, 0);
                }else{
                    al_draw_bitmap( img_atk[ 1 ], e_pchara->x, e_pchara->y, 0);
                    al_play_sample_instance(e_pchara->atk_Sound);
                }
            }
        }
    }
}
void character_destory(){
   // al_destroy_bitmap(e_pchara->img_atk[0]);
   // al_destroy_bitmap(e_pchara->img_atk[1]);
    al_destroy_bitmap(e_pchara->img_move[0]);
    al_destroy_bitmap(e_pchara->img_move[1]);
    al_destroy_sample_instance(e_pchara->atk_Sound);
}

//==============================================================================================

enum {ALIVE };

void character_init2(){
    char temp[50];
    sprintf( temp, "./image/char_move1.png");
    monster->img_move[0] = al_load_bitmap(temp);

    monster->width = al_get_bitmap_width(monster->img_move[0]);
    monster->height = al_get_bitmap_height(monster->img_move[0]);
    monster->x = WIDTH/2;
    monster->y = HEIGHT/2+40;
    monster->dir = true;
    monster->hp = 2;
    monster->amidie = 1;

    // initial the animation component
    monster->state = ALIVE;
    monster->anime = 0;
    monster->anime_time = 30;
}

void charater_process2(ALLEGRO_EVENT event){
    // process the animation                //1.處理動畫
    if( event.type == ALLEGRO_EVENT_TIMER ){ //根據fps+anime
        if( event.timer.source == fps ){
            monster->anime++;
            monster->anime %= monster->anime_time; //讓我們知道現在跑道time的哪一步
        }

    }

}

void character_destory2(){
    al_destroy_bitmap(monster->img_move[0]);
}

void charater_update2(){

     if( ( e_pchara->x - 60 <= monster->x ) && ( monster->x <= e_pchara->x + 60 ) &&
         ( e_pchara->y - 60 <= monster->y ) && ( monster->y <= e_pchara->y + 60 ) ) {
        monster->hp--;
     }

     if( monster->hp <= 0 && monster->amidie == 1 ) {
        //如果死掉
        character_destory2();
        monster->amidie = 0;
     }
}

void character_draw2(){

    // with the state, draw corresponding image
    if( monster->state == ALIVE ) {
        //如果活著
        if( monster->dir )
        {
            if(monster->x >= WIDTH/2+50){monster->dir=false;al_draw_bitmap(monster->img_move[0], monster->x, monster->y, 0);}
            al_draw_bitmap(monster->img_move[0], monster->x, monster->y, ALLEGRO_FLIP_HORIZONTAL);
            monster->x += 1;
        }
        else
        {
        //{
            if(monster->x <= WIDTH/2){monster->dir=true;al_draw_bitmap(monster->img_move[0], monster->x, monster->y, ALLEGRO_FLIP_HORIZONTAL);}
            al_draw_bitmap(monster->img_move[0], monster->x, monster->y, 0);
            monster->x -= 1;

        //}
        }
    }
}



