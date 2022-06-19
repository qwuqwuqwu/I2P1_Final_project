#include <assert.h>
#include "charater.h"

#define MAX_COUNTOF_CONTINUOUS_JUMP ( 2 )
#define JUMP_VY ( -90 )
#define MAX_COUNTOF_MONSTER ( 100 )

#define SLIDE_RATE  ( 3 )
#define ATK_RATE    ( 3 )

//note

ALLEGRO_SAMPLE *sample_inhale = NULL;
ALLEGRO_SAMPLE *sample_transform = NULL;

ALLEGRO_SAMPLE_INSTANCE *BossSampleInstance = NULL;
ALLEGRO_SAMPLE *BossSample = NULL;

bool g_bMute = false;
//float e_pchara->CamPos = 0.0;
ALLEGRO_TRANSFORM camera;
int g_nTerrainWidth = 0;

ECharacterState g_LastState;
int g_nLastSubState;

EMonsterState g_MonLastState[ MAX_COUNTOF_MONSTER ];
int g_nMonLastSubState[ MAX_COUNTOF_MONSTER ];

bool g_bCDing = false;
int g_nCDCursor = 0;
int g_nCDTime = 20;
int camera_move = 1;

int g_nWordHeight = 0;
bool g_bImmortal = false;
bool g_bForceImmortal = false;
int g_nImortalCursor = 0;
int g_nImortalTime = 10 * FPS; // 10 sec

int g_nMonsterCount = 0;

void CameraUpdate( float *CamPosition, int x, int y, int width, int height, int nMoveWidth )
{
//    printf( "CameraUpdate\n" );
    if( e_pchara->dir == false && ( e_pchara->state == ECS_ATK || e_pchara->state == ECS_SLIDE ) ) {
        *CamPosition = -( WIDTH / 2 ) + ( x + width - nMoveWidth / 2 );
    }
    else {
        *CamPosition = -( WIDTH / 2 ) + ( x + nMoveWidth / 2 );
    }


    if( *CamPosition < 0 ) {
        *CamPosition = 0.0;
    }
    else if( *CamPosition >= ( g_nTerrainWidth - 9 * WIDTH / 8 ) ) {
        *CamPosition = ( float )( g_nTerrainWidth - WIDTH);
        camera_move = 0;
        al_stop_sample_instance( g_pMenuSampleInstance );
        al_destroy_sample_instance( g_pMenuSampleInstance );

        BossSample = al_load_sample( "./sound/boss.mp3" );
        BossSampleInstance = al_create_sample_instance( BossSample );
        al_set_sample_instance_playmode( BossSampleInstance, ALLEGRO_PLAYMODE_LOOP );
        al_attach_sample_instance_to_mixer( BossSampleInstance, al_get_default_mixer() );
        al_set_sample_instance_gain( BossSampleInstance, 0.7 );
        al_play_sample_instance( BossSampleInstance );
    }
//    printf( "CameraUpdate\n" );
}

void character_init( const int nTerrainWidth, const int nLife ){
//    printf( "character_init\n" );

    // new e_pchara
    e_pchara = ( Character * )malloc( sizeof( Character ) );
    g_bImmortal = false;
    g_bForceImmortal = false;
    g_LastState = ECS_STOP;
    g_nLastSubState = 0;
    g_nCDCursor = 0;
    camera_move = 1;
    g_nWordHeight = 0;
    g_nImortalCursor = 0;
    g_nImortalTime = 10 * FPS; // 10 sec

    // load character images
    // inhale
    for( int i = 0; i < 2; i++ ) {
        char temp[ 50 ];
        sprintf( temp, "./image/char_inhale%d.png", i + 1 );
        e_pchara->img_inhale[ i ] = al_load_bitmap( temp );
        assert( e_pchara->img_inhale[ i ] != NULL );
    }

    // hp
    for(int i = 0; i <= HP; i++){
        char temp[ 50 ];
        sprintf( temp, "./image/hp%d.png", i );
        e_pchara->img_store_HP[ i ] = al_load_bitmap( temp );
        assert( e_pchara->img_store_HP[ i ] != NULL );
    }

    //life
    for(int i = 0; i < 1; i++){
        char temp[ 50 ];
        sprintf( temp, "./image/icon_life.png");
        e_pchara->img_store_LIFE[ i+1] = al_load_bitmap( temp );
        assert( e_pchara->img_store_LIFE[ i+1 ] != NULL );
    }

    for( int i = 0; i < ESA_NUM; i++ ){
        //printf( "i = %d\n", i );
        switch( i ) {
        case ESA_NORMAL:
            // special attack
            for(int j = 0; j < 2; j++){
                char temp[ 50 ];
                sprintf( temp, "./image/char_slide%d.png", j + 1 );
                e_pchara->img_store_SpecialAtk[ 2 * i + j ] = al_load_bitmap( temp );
                assert( e_pchara->img_store_SpecialAtk[ 2 * i + j ] != NULL );
                e_pchara->img_atk[ j ] = e_pchara->img_store_SpecialAtk[ 2 * i + j ];
            }

            // move
            for(int j = 0; j < 4; j++){
                char temp[ 50 ];
                sprintf( temp, "./image/char_move%d.png", j + 1 );
                e_pchara->img_store_move[ 4 * i + j ] = al_load_bitmap( temp );
                assert( e_pchara->img_store_move[ 4 * i + j ] != NULL );
                e_pchara->img_move[ j ] = e_pchara->img_store_move[ 4 * i + j ];
            }

            // slide
            for(int j = 0; j < 2; j++){
                char temp[ 50 ];
                sprintf( temp, "./image/char_slide%d.png", j + 1 );
                e_pchara->img_store_slide[ 2 * i + j ] = al_load_bitmap( temp );
                assert( e_pchara->img_store_slide[ 2 * i + j ] != NULL );
                e_pchara->img_slide[ j ] = e_pchara->img_store_slide[ 2 * i + j ];
            }

            // transform
            e_pchara->img_store_AtkWord[ i ] = al_load_bitmap( "./image/sign_normal.png" );
            e_pchara->img_atkWord = e_pchara->img_store_AtkWord[ i ];
            assert( e_pchara->img_store_AtkWord[ i ] != NULL );
            g_nWordHeight = al_get_bitmap_height( e_pchara->img_store_AtkWord[ i ] );
            // do nothing
            break;

        case ESA_SWORD:
            // special attack
            for(int j = 0; j < 2; j++){
                char temp[ 50 ];
                sprintf( temp, "./image/char_slash%d.png", j + 1 );
                e_pchara->img_store_SpecialAtk[ 2 * i + j ] = al_load_bitmap( temp );
                assert( e_pchara->img_store_SpecialAtk[ 2 * i + j ] != NULL );
            }

            // move
            for(int j = 0; j < 4; j++){
                char temp[ 50 ];
                sprintf( temp, "./image/char_sword_move%d.png", j + 1 );
                e_pchara->img_store_move[ 4 * i + j ] = al_load_bitmap( temp );
                assert( e_pchara->img_store_move[ 4 * i + j ] != NULL );
            }

            // slide
            for(int j = 0; j < 2; j++){
                char temp[ 50 ];
                sprintf( temp, "./image/char_sword_slide%d.png", j + 1 );
                e_pchara->img_store_slide[ 2 * i + j ] = al_load_bitmap( temp );
                assert( e_pchara->img_store_slide[ 2 * i + j ] != NULL );
            }

            // transform
            for(int j = 0; j < 3; j++){
                char temp[ 50 ];
                sprintf( temp, "./image/char_to_sword%d.png", j + 1 );
                e_pchara->img_store_transform[ 3 * i + j ] = al_load_bitmap( temp );
                assert( e_pchara->img_store_transform[ 3 * i + j ] != NULL );
            }

            e_pchara->img_store_AtkWord[ i ] = al_load_bitmap( "./image/sign_sword.png" );
            assert( e_pchara->img_store_AtkWord[ i ] != NULL );
            break;

        case ESA_BOMB:
            // special attack
            for(int j = 0; j < 2; j++){
                char temp[ 50 ];
                sprintf( temp, "./image/char_throw%d.png", j + 1 );
                e_pchara->img_store_SpecialAtk[ 2 * i + j ] = al_load_bitmap( temp );
                assert( e_pchara->img_store_SpecialAtk[ 2 * i + j ] != NULL );
            }

            // move
            for(int j = 0; j < 4; j++){
                char temp[ 50 ];
                sprintf( temp, "./image/char_bomb_move%d.png", j + 1 );
                e_pchara->img_store_move[ 4 * i + j ] = al_load_bitmap( temp );
                assert( e_pchara->img_store_move[ 4 * i + j ] != NULL );
            }

            // slide
            for(int j = 0; j < 2; j++){
                char temp[ 50 ];
                sprintf( temp, "./image/char_bomb_slide%d.png", j + 1 );
                e_pchara->img_store_slide[ 2 * i + j ] = al_load_bitmap( temp );
                assert( e_pchara->img_store_slide[ 2 * i + j ] != NULL );
            }

            // transform
            for(int j = 0; j < 3; j++){
                char temp[ 50 ];
                sprintf( temp, "./image/char_to_bomb%d.png", j + 1 );
                e_pchara->img_store_transform[ 3 * i + j ] = al_load_bitmap( temp );
                assert( e_pchara->img_store_transform[ 3 * i + j ] != NULL );
            }

            e_pchara->img_store_AtkWord[ i ] = al_load_bitmap( "./image/sign_bomb.png" );
            assert( e_pchara->img_store_AtkWord[ i ] != NULL );
            break;

        case ESA_FIRE:
            // special attack
            for(int j = 0; j < 2; j++){
                char temp[ 50 ];
                sprintf( temp, "./image/char_firespit%d.png", j + 1 );
                e_pchara->img_store_SpecialAtk[ 2 * i + j ] = al_load_bitmap( temp );
                assert( e_pchara->img_store_SpecialAtk[ 2 * i + j ] != NULL );

                sprintf( temp, "./image/fire%d.png", j + 1 );
                e_pchara->img_fire[ j ] = al_load_bitmap( temp );
                assert( e_pchara->img_fire[ j ] != NULL );
            }

            // move
            for(int j = 0; j < 4; j++){
                char temp[ 50 ];
                sprintf( temp, "./image/char_fire_move%d.png", j + 1 );
                e_pchara->img_store_move[ 4 * i + j ] = al_load_bitmap( temp );
                assert( e_pchara->img_store_move[ 4 * i + j ] != NULL );
            }

            // slide
            for(int j = 0; j < 2; j++){
                char temp[ 50 ];
                sprintf( temp, "./image/char_fire_slide%d.png", j + 1 );
                e_pchara->img_store_slide[ 2 * i + j ] = al_load_bitmap( temp );
                assert( e_pchara->img_store_slide[ 2 * i + j ] != NULL );
            }

            // transform
            for(int j = 0; j < 3; j++){
                char temp[ 50 ];
                sprintf( temp, "./image/char_to_fire%d.png", j + 1 );
                e_pchara->img_store_transform[ 3 * i + j ] = al_load_bitmap( temp );
                assert( e_pchara->img_store_transform[ 3 * i + j ] != NULL );
            }

            e_pchara->img_store_AtkWord[ i ] = al_load_bitmap( "./image/sign_fire.png" );
            assert( e_pchara->img_store_AtkWord[ i ] != NULL );
            break;

        default:
            assert( false );
            break;
        }
        //printf( "i = %d\n", i );

//        g_nImgMoveWidth[ i ] = al_get_bitmap_width( e_pchara->img_SpecialAtk[ 4 * i ] );
//        g_nImgMoveHeight[ i ] = al_get_bitmap_height( e_pchara->img_SpecialAtk[ 4 * i ] );
    }

    // load effective sound
    sample_inhale = al_load_sample( "./sound/inhale.wav" );
    e_pchara->sound_inhale = al_create_sample_instance( sample_inhale );
    al_set_sample_instance_playmode( e_pchara->sound_inhale, ALLEGRO_PLAYMODE_ONCE );
    al_attach_sample_instance_to_mixer( e_pchara->sound_inhale, al_get_default_mixer() );

    sample_transform = al_load_sample( "./sound/get_ability.wav" );
    e_pchara->sound_transform = al_create_sample_instance( sample_transform );
    al_set_sample_instance_playmode( e_pchara->sound_transform, ALLEGRO_PLAYMODE_ONCE );
    al_attach_sample_instance_to_mixer( e_pchara->sound_transform, al_get_default_mixer() );

    ALLEGRO_SAMPLE *pTemp = al_load_sample( "./sound/slash.wav" );
    e_pchara->sound_slash = al_create_sample_instance( pTemp );
    al_set_sample_instance_playmode( e_pchara->sound_slash, ALLEGRO_PLAYMODE_ONCE );
    al_attach_sample_instance_to_mixer( e_pchara->sound_slash, al_get_default_mixer() );

    pTemp = al_load_sample( "./sound/fireball.wav" );
    e_pchara->sound_fire = al_create_sample_instance( pTemp );
    al_set_sample_instance_playmode( e_pchara->sound_fire, ALLEGRO_PLAYMODE_ONCE );
    al_attach_sample_instance_to_mixer( e_pchara->sound_fire, al_get_default_mixer() );

    // initial the geometric information of character
    e_pchara->width = al_get_bitmap_width(e_pchara->img_move[0]);
    e_pchara->nMoveWidth = e_pchara->width;
    e_pchara->height = al_get_bitmap_height(e_pchara->img_move[0]);
    e_pchara->nMoveHeight = e_pchara->height;
    e_pchara->x = WIDTH/2-100; // todo: no magical number here!
    e_pchara->y = HEIGHT/2;
    e_pchara->nMoveY = e_pchara->y;
    e_pchara->dir = false;
    e_pchara->hp = HP;
    e_pchara->life = nLife;

    e_pchara->nBombIdx = -1;

    // initial the animation component
    e_pchara->state = ECS_STOP;
    e_pchara->nSubState = 0;
    e_pchara->nMoveCursor = 0;
    e_pchara->nMoveTime = 20;
    e_pchara->nAtkCursor = 0;
    e_pchara->nAtkTime = 20;
    e_pchara->nSlideCursor = 0;
    e_pchara->nSlideTime = 20;
    e_pchara->nInhaleCursor = 0;
    e_pchara->nInhaleTime = 10;
    e_pchara->nInjuredCursor = 0;
    e_pchara->nInjuredTime = 120;

    e_pchara->x0 = e_pchara->x;
    e_pchara->nDeltaX = 0;
    // gravity
    e_pchara->y0 = e_pchara->y;
    e_pchara->vy = 0.0;
    e_pchara->FallingTick = 0.0;
    e_pchara->nJumpCount = 0;
    e_pchara->nSlideRate = SLIDE_RATE;
    e_pchara->nAtkRate = ATK_RATE;

    // terrain width
    g_nTerrainWidth = nTerrainWidth;

    // transform status
    e_pchara->nTransformCursor = 0;
    e_pchara->nTransformTime = 30;
    e_pchara->NowSpecialAtk = ESA_NORMAL;
    e_pchara->NextSpecailAtk = ESA_NORMAL;

//    printf( "test\n" );

//    printf( "Character init success\n" );
//    printf( "character_init\n" );
}

void charater_process( ALLEGRO_EVENT event )
{
//    printf( "charater_process\n" );
    // process the animation
    if( event.type == ALLEGRO_EVENT_TIMER ) {
        if( event.timer.source == fps ) {

            if( g_bCDing == true ) {
                g_nCDCursor = ( g_nCDCursor + 1 ) % g_nCDTime;
                if( g_nCDCursor == 0 ) {
                    g_bCDing = false;
                }
            }

            if( g_bImmortal == true ) {
                g_nImortalCursor = ( g_nImortalCursor + 1 ) % g_nImortalTime;
                if( g_bForceImmortal == false && g_nImortalCursor == 0 ) {
                    g_bImmortal = false;
                }
            }

            if( e_pchara->state == ECS_MOVE ) {
                e_pchara->nMoveCursor++;
                e_pchara->nMoveCursor %= e_pchara->nMoveTime;

                if( e_pchara->nMoveCursor == 0 ) {
                    e_pchara->state = ECS_STOP;
                }
            }

            if( e_pchara->state == ECS_ATK ) {
                e_pchara->nAtkCursor++;
                e_pchara->nAtkCursor %= e_pchara->nAtkTime;

                if( e_pchara->nAtkCursor == 0 ) {
                    g_bCDing = true;
                    e_pchara->state = ECS_STOP;
                }
            }

            if( e_pchara->state == ECS_SLIDE ) {
                e_pchara->nSlideCursor++;
                e_pchara->nSlideCursor %= e_pchara->nSlideTime;

                if( e_pchara->nSlideCursor == 0 ) {
                    g_bCDing = true;
                    e_pchara->state = ECS_STOP;
                }
            }

            if( e_pchara->state == ECS_INHALE ) {
                e_pchara->nInhaleCursor++;
                e_pchara->nInhaleCursor %= e_pchara->nInhaleTime;

                if( e_pchara->nInhaleCursor == 0 ) {
                    g_bCDing = true;
                    e_pchara->state = ECS_STOP;
                }
            }

            if( e_pchara->state == ECS_TRANSFORMING ) {
                e_pchara->nTransformCursor++;
                e_pchara->nTransformCursor %= e_pchara->nTransformTime;

                if( e_pchara->nTransformCursor == 0 ) {
                    e_pchara->state = ECS_STOP;
                    e_pchara->NowSpecialAtk = e_pchara->NextSpecailAtk;
                    e_pchara->NextSpecailAtk = ESA_NORMAL;
                }
            }
            if( e_pchara->state == ECS_INJURED ) {
                e_pchara->nInjuredCursor++;
                e_pchara->nInjuredCursor %= e_pchara->nInjuredTime;
//                printf( "e_pchara->nInjuredCursor = %d\n", e_pchara->nInjuredCursor );

                if( e_pchara->nInjuredCursor == 0 ) {
                    e_pchara->state = ECS_STOP;
                }
            }
        }
    // process the keyboard event
    }
    else if( event.type == ALLEGRO_EVENT_KEY_DOWN ) {
        key_state[ event.keyboard.keycode ] = true;
        //e_pchara->nMoveCursor = 0;
    }
    else if( event.type == ALLEGRO_EVENT_KEY_UP ) {
        key_state[event.keyboard.keycode] = false;
    }

    Object_process( event );
//    printf( "charater_process\n" );
}

void charater_update( void )
{
//    printf( "charater_update\n" );
    // use the idea of finite state machine to deal with different state

    if( e_pchara->state == ECS_INHALE ) {
        return;
    }
    if( key_state[ALLEGRO_KEY_W] ) {
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
            e_pchara->x -= 5;
        }
        else if( key_state[ALLEGRO_KEY_D] ) {
            e_pchara->dir = true;
            e_pchara->x += 5;
        }
        e_pchara->state = ECS_MOVE;

    }else if( key_state[ALLEGRO_KEY_A] ){
        e_pchara->dir = false;
        e_pchara->x -= 5;
        e_pchara->state = ECS_MOVE;
    }
    else if( key_state[ALLEGRO_KEY_D] ){
        e_pchara->dir = true;
        e_pchara->x += 5;
        e_pchara->state = ECS_MOVE;

    }
    // attack v
    else if( key_state[ ALLEGRO_KEY_J ] ) {
        if( g_bCDing == false && e_pchara->state != ECS_ATK ) {
            e_pchara->x0 = e_pchara->x;
            e_pchara->state = ECS_ATK;
            e_pchara->nAtkRate = ATK_RATE;
        }
    }
    // slide c
    else if( key_state[ ALLEGRO_KEY_K ] ) {
        if( g_bCDing == false && e_pchara->state != ECS_SLIDE ) {
            e_pchara->x0 = e_pchara->x;
            e_pchara->state = ECS_SLIDE;
            e_pchara->nSlideRate = SLIDE_RATE;
        }
    }
    // inhale
    else if( key_state[ ALLEGRO_KEY_SPACE ] ) {   //吸的技能
        if( g_bCDing == false && e_pchara->state != ECS_INHALE && e_pchara->state != ECS_TRANSFORMING ) {
            e_pchara->nInhaleCursor = 0;
            e_pchara->state = ECS_INHALE;
            e_pchara->x0 = e_pchara->x;
        }
    }
    // mute
    else if( key_state[ ALLEGRO_KEY_M ] ) {
        if( g_bMute == false ) {
            al_set_sample_instance_gain( g_pMenuSampleInstance, 0 );
            g_bMute = true;
        }
        else {
            al_set_sample_instance_gain( g_pMenuSampleInstance, 1 );
            g_bMute = false;
        }
    }
    // force immortal
    else if( key_state[ ALLEGRO_KEY_I ] ) {
        if( g_bForceImmortal == false ) {
            g_bImmortal = true;
            g_bForceImmortal = true;
            g_nImortalCursor = 0;
        }
        else {
            g_bImmortal = false;
            g_bForceImmortal = false;
            g_nImortalCursor = 0;
        }

    }
//    printf( "charater_update\n" );
}

void character_gravity( int nGroundY ) {
//    printf( "character_gravity\n" );
    if( e_pchara->state == ECS_MOVE || e_pchara->state == ECS_STOP || e_pchara->state == ECS_ATK ||
        e_pchara->state == ECS_SLIDE || e_pchara->state == ECS_INJURED ) {
        if( nGroundY == -2 ) {
            nGroundY = HEIGHT;
        }

        // ticktock
        e_pchara->FallingTick += g_Tick;

        // velocity;
        e_pchara->vy += g_Gravity * g_Tick;
        // position
        e_pchara->y = e_pchara->y0
            + ( int )(e_pchara->vy * e_pchara->FallingTick + ( 0.5 * g_Gravity * ( e_pchara->FallingTick * e_pchara->FallingTick ) ) );


        if( ( e_pchara->y + ( e_pchara->height + e_pchara->nMoveHeight ) / 2 ) >= nGroundY ) {

            if( nGroundY == HEIGHT ) {
                if( g_bImmortal == false ) {
                    e_pchara->state = ECS_INJURED;
                    e_pchara->hp--;
                    g_bImmortal = true;
                    g_nImortalCursor = 0;
                }
            }

            e_pchara->y = nGroundY - ( e_pchara->height + e_pchara->nMoveHeight ) / 2;
            e_pchara->y0 = e_pchara->y;
            e_pchara->vy = 0.0;
            e_pchara->FallingTick = 0;
            e_pchara->nJumpCount = 0;
        }
        else {
            // do nothing
        }
    }
//    printf( "character_gravity\n" );
}

void character_BoundryCheck( void ) {
//    printf( "character_BoundryCheck\n" );

    if( e_pchara->state == ECS_ATK || e_pchara->state == ECS_SLIDE ) {
        return;
    }
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
//    printf( "character_BoundryCheck\n" );
}

void character_CheckBlocker( void )
{
    if( e_pchara->state == ECS_ATK || e_pchara->state == ECS_SLIDE ) {
        return;
    }

    Position pos;
    if( e_pchara->dir == false ) {
        pos.e = e_pchara->x + e_pchara->width;
        pos.w = pos.e - e_pchara->nMoveWidth;
    }
    else {
        pos.w = e_pchara->x;
        pos.e = e_pchara->x + e_pchara->nMoveWidth;
    }

    pos.n = e_pchara->y + e_pchara->height / 2 - e_pchara->nMoveHeight / 2;
    pos.s = e_pchara->y + e_pchara->height / 2 + e_pchara->nMoveHeight / 2;

    bool bClamped = CheckBlocker( &pos, e_pchara->dir );

    // update x, y
    if( bClamped == true ) {
        if( e_pchara->y < pos.n ) {
            e_pchara->vy = 0;
        }
        e_pchara->x = pos.w;
        e_pchara->y = pos.n;
        e_pchara->nSlideRate = 0;
        e_pchara->nAtkRate = 0;
    }

}

void character_eat( void )
{
    if( e_pchara->state != ECS_INHALE ) {
        return;
    }

    Position CharacterPos;
    CharacterPos.w = e_pchara->x;
    CharacterPos.e = e_pchara->x + e_pchara->width;
    CharacterPos.n = e_pchara->y;
    CharacterPos.s = e_pchara->y + e_pchara->height;

    int nFoodCount = 0;
    int nCandyCount = 0;
    Eat( &CharacterPos, &nFoodCount, &nCandyCount );
    if( nFoodCount != 0 ) {
        // should return to full blood
        e_pchara->hp = HP;
        printf( "Full blood!\n" );
    }

    if( nCandyCount != 0 ) {
        // immortal status
        g_bImmortal = true;
        g_nImortalCursor = 0;
    }
}

void character_checkTransform( void ) {
//    printf( "character_checkTransform\n" );
    if( e_pchara->state == ECS_TRANSFORMING ) {
//        printf( "character_checkTransform\n" );
        return;
    }

    if( e_pchara->state != ECS_INHALE ) {
        return;
    }

    Position CharacterPos;
    CharacterPos.w = e_pchara->x;
    CharacterPos.e = e_pchara->x + e_pchara->width;
    CharacterPos.n = e_pchara->y;
    CharacterPos.s = e_pchara->y + e_pchara->height;

    for( int i = 0; i < g_nMonsterCount; i++ ) {
        if( e_monster[ i ].state == EMS_ALIVE ) {
            // given monster position
            Position MonsterPos;
            MonsterPos.e = e_monster[ i ].x + e_monster[ i ].width;
            MonsterPos.w = e_monster[ i ].x;
            MonsterPos.n = e_monster[ i ].y;
            MonsterPos.s = e_monster[ i ].y + e_monster[ i ].height;

            if( CheckOverlap( &CharacterPos, &MonsterPos ) == true ) {
                e_monster[ i ].state = EMS_DIE;
                e_monster[ i ].hp = 0;
                e_pchara->nInhaleCursor = 0;

                if( ( ESpecialAtk )e_monster[ i ].type == ESA_NORMAL ) {
                    e_pchara->state = ECS_STOP;
                }
                else {
                    e_pchara->state = ECS_TRANSFORMING;
                    e_pchara->nTransformCursor = 0;
                    e_pchara->NextSpecailAtk = ( ESpecialAtk )e_monster[ i ].type;
                    e_pchara->img_atkWord = e_pchara->img_store_AtkWord[ e_pchara->NextSpecailAtk ];
                }
                printf( "Next Special Attack is %d\n", e_pchara->NextSpecailAtk );
                break;
            }
        }
    }

//    printf( "character_checkTransform\n" );
}

void character_StateChangeImage( void )
{
//    printf( "character_StateChangeImage\n" );
    ECharacterState NewState = e_pchara->state;

    // update image
//    printf( "update image\n" );
    if( NewState != g_LastState ) {
        // special attack
        for(int j = 0; j < 2; j++){
            e_pchara->img_atk[ j ] = e_pchara->img_store_SpecialAtk[ 2 * e_pchara->NowSpecialAtk + j ];
        }

        // move
        for(int j = 0; j < 4; j++){
            e_pchara->img_move[ j ] = e_pchara->img_store_move[ 4 * e_pchara->NowSpecialAtk + j ];
        }
        e_pchara->nMoveWidth = al_get_bitmap_width( e_pchara->img_move[ 0 ] );

        // slide
        for(int j = 0; j < 2; j++){
            e_pchara->img_slide[ j ] = e_pchara->img_store_slide[ 2 * e_pchara->NowSpecialAtk + j ];
        }

        // transform
        for(int j = 0; j < 3; j++){
            e_pchara->img_transform[ j ] = e_pchara->img_store_transform[ 3 * e_pchara->NextSpecailAtk + j ];
        }
    }

    // update substate
//    printf( "update substate\n" );
    switch( NewState ) {
    case ECS_STOP:
        if( NewState != g_LastState ) {
            e_pchara->nSubState = 0;
        }
        // change special atack image
        break;

    case ECS_MOVE:
        if( NewState != g_LastState ) {
            e_pchara->nSubState = 0;
        }

        if( e_pchara->nMoveCursor < e_pchara->nMoveTime / 4 ) {
            e_pchara->nSubState = 0;
        }
        else if( e_pchara->nMoveCursor < e_pchara->nMoveTime / 2 ) {
            e_pchara->nSubState = 1;
        }
        else if( e_pchara->nMoveCursor < e_pchara->nMoveTime * 3 / 4 ) {
            e_pchara->nSubState = 2;
        }
        else{
            e_pchara->nSubState = 3;
        }
        break;

    case ECS_INHALE:
        if( NewState != g_LastState ) {
            e_pchara->nSubState = 0;
        }

        if( e_pchara->nInhaleCursor < e_pchara->nInhaleTime / 2 ) {
            e_pchara->nSubState = 0;
        }
        else{
            e_pchara->nSubState = 1;
        }
        break;

    case ECS_TRANSFORMING:
        if( NewState != g_LastState ) {
            e_pchara->nSubState = 0;
        }
        if( e_pchara->nTransformCursor < e_pchara->nTransformTime / 3 ) {
            e_pchara->nSubState = 0;
        }
        else if( e_pchara->nTransformCursor < e_pchara->nTransformTime * 2 / 3 ) {
            e_pchara->nSubState = 1;
        }
        else {
            e_pchara->nSubState = 2;
        }
        break;

    case ECS_ATK:
        if( NewState != g_LastState ) {
            e_pchara->nSubState = 0;
        }
        if( e_pchara->nAtkCursor < e_pchara->nAtkTime / 2 ) {
            e_pchara->nSubState = 0;
        }
        else {
            e_pchara->nSubState = 1;
        }
        break;

    case ECS_SLIDE:
        if( NewState != g_LastState ) {
            e_pchara->nSubState = 0;
        }
        if( e_pchara->nSlideCursor < e_pchara->nSlideTime / 2 ) {
            e_pchara->nSubState = 0;
        }
        else {
            e_pchara->nSubState = 1;
        }
        break;

    case ECS_INJURED:
        if( NewState != g_LastState ) {
            e_pchara->nSubState = 0;
        }
        break;

    default:
        assert( false );
        break;
    }

    // update xy
//    printf( "update xy\n" );
    switch( NewState ) {
    case ECS_STOP:
        // change state
        if( g_LastState != NewState ) {
            e_pchara->nMoveHeight = e_pchara->height;

            int nHeight = al_get_bitmap_height( e_pchara->img_move[ 0 ] );
            e_pchara->y -= ( nHeight - e_pchara->height ) / 2;
            e_pchara->height = nHeight;
            e_pchara->nMoveY = e_pchara->y;

            if( e_pchara->dir == false ) {
                int nWidth = al_get_bitmap_width( e_pchara->img_move[ 0 ] );
                e_pchara->x -= nWidth - e_pchara->width;
                e_pchara->width = nWidth;
                e_pchara->nMoveWidth = e_pchara->width;
            }
            else {
                int nWidth = al_get_bitmap_width( e_pchara->img_move[ 0 ] );
                e_pchara->width = nWidth;
                e_pchara->nMoveWidth = e_pchara->width;
            }
        }
        break;

    case ECS_MOVE:
        // change state
        if( g_LastState != NewState ) {
            e_pchara->nMoveHeight = e_pchara->height;
            int nHeight = al_get_bitmap_height( e_pchara->img_move[ 0 ] );
            e_pchara->y -= ( nHeight - e_pchara->height ) / 2;
            e_pchara->height = nHeight;
            e_pchara->nMoveY = e_pchara->y;

            if( e_pchara->dir == false ) {
                int nWidth = al_get_bitmap_width( e_pchara->img_move[ 0 ] );
                e_pchara->x -= nWidth - e_pchara->width;
                e_pchara->width = nWidth;
                e_pchara->nMoveWidth = e_pchara->width;
            }
            else {
                int nWidth = al_get_bitmap_width( e_pchara->img_move[ 0 ] );
                e_pchara->width = nWidth;
                e_pchara->nMoveWidth = e_pchara->width;
            }
        }
        // change substate
        if( g_LastState == NewState && g_nLastSubState != e_pchara->nSubState ) {
//            printf( "height is %d\n", e_pchara->height );
            e_pchara->nMoveHeight = e_pchara->height;
            int nHeight = al_get_bitmap_height( e_pchara->img_move[ e_pchara->nSubState ] );
            e_pchara->y -= ( nHeight - e_pchara->height ) / 2;
            e_pchara->height = nHeight;
            e_pchara->nMoveY = e_pchara->y;
//            printf( "New height is %d\n", e_pchara->height );

            if( e_pchara->dir == false ) {
                int nWidth = al_get_bitmap_width( e_pchara->img_move[ e_pchara->nSubState ] );
                e_pchara->x -= nWidth - e_pchara->width;
                e_pchara->width = nWidth;
                e_pchara->nMoveWidth = e_pchara->width;
            }
            else {
                int nWidth = al_get_bitmap_width( e_pchara->img_move[ e_pchara->nSubState ] );
                e_pchara->width = nWidth;
                e_pchara->nMoveWidth = e_pchara->width;
            }
        }
        break;

    case ECS_INHALE:
        // change state
        if( g_LastState != NewState ) {
            int nHeight = al_get_bitmap_height( e_pchara->img_inhale[ 0 ] );
            e_pchara->y -= ( nHeight - e_pchara->height ) / 2;
            e_pchara->height = nHeight;
        }

        // change substate
        if( g_LastState == NewState && g_nLastSubState != e_pchara->nSubState ) {
            int nHeight = al_get_bitmap_height( e_pchara->img_inhale[ e_pchara->nSubState ] );
            e_pchara->y -= ( nHeight - e_pchara->height ) / 2;
            e_pchara->height = nHeight;
        }
//        printf( "New height is %d\n", e_pchara->height );
        break;

    case ECS_TRANSFORMING:
        // change state
        if( g_LastState != NewState ) {
            int nHeight = al_get_bitmap_height( e_pchara->img_transform[ 0 ] );
            e_pchara->y -= ( nHeight - e_pchara->height ) / 2;
            e_pchara->height = nHeight;
        }
        // change substate
        if( g_LastState == NewState && g_nLastSubState != e_pchara->nSubState ) {
            int nHeight = al_get_bitmap_height( e_pchara->img_transform[ e_pchara->nSubState ] );
            e_pchara->y -= ( nHeight - e_pchara->height ) / 2;
            e_pchara->height = nHeight;
        }
        break;

    case ECS_ATK:
        // change state
        if( g_LastState != NewState ) {
            int nHeight = al_get_bitmap_height( e_pchara->img_atk[ 0 ] );
            e_pchara->y -= ( nHeight - e_pchara->height ) / 2;
            e_pchara->height = nHeight;

            if( e_pchara->dir == false ) {
                int nWidth = al_get_bitmap_width( e_pchara->img_atk[ 0 ] );
                e_pchara->x0 -= nWidth - e_pchara->width;
                e_pchara->width = nWidth;
            }
            else {
                int nWidth = al_get_bitmap_width( e_pchara->img_atk[ 0 ] );
                e_pchara->width = nWidth;
            }
        }

        // change substate
        if( g_LastState == NewState && g_nLastSubState != e_pchara->nSubState ) {
            int nHeight = al_get_bitmap_height( e_pchara->img_atk[ e_pchara->nSubState ] );
            e_pchara->y -= ( nHeight - e_pchara->height ) / 2;
            e_pchara->height = nHeight;

            if( e_pchara->dir == false ) {
                int nWidth = al_get_bitmap_width( e_pchara->img_atk[ e_pchara->nSubState ] );
                e_pchara->x0 -= nWidth - e_pchara->width;
                e_pchara->width = nWidth;
            }
            else {
                int nWidth = al_get_bitmap_width( e_pchara->img_atk[ e_pchara->nSubState ] );
                e_pchara->width = nWidth;
            }

            if( e_pchara->nSubState == 1 && e_pchara->NowSpecialAtk == ESA_BOMB ) {
                // first to to register bomb
                if( e_pchara->nBombIdx == -1 ) {
                    e_pchara->nBombIdx = RegisterBomb();
                    assert( e_pchara->nBombIdx != -1 );
                }
                BombThrow( e_pchara->nBombIdx, e_pchara->x, e_pchara->y, e_pchara->dir );
            }
        }

        e_pchara->x = e_pchara->x0 + e_pchara->nDeltaX;
        break;

    case ECS_SLIDE:
        // change state
        if( g_LastState != NewState ) {
            int nHeight = al_get_bitmap_height( e_pchara->img_slide[ 0 ] );
            e_pchara->y -= ( nHeight - e_pchara->height );
            e_pchara->height = nHeight;

            if( e_pchara->dir == false ) {
                int nWidth = al_get_bitmap_width( e_pchara->img_slide[ 0 ] );
                e_pchara->x0 -= nWidth - e_pchara->width;
                e_pchara->width = nWidth;
            }
        }

        // change substate
        if( g_LastState == NewState && g_nLastSubState != e_pchara->nSubState ) {
            int nHeight = al_get_bitmap_height( e_pchara->img_slide[ e_pchara->nSubState ] );
            e_pchara->y -= ( nHeight - e_pchara->height );
            e_pchara->height = nHeight;

            if( e_pchara->dir == false ) {
                int nWidth = al_get_bitmap_width( e_pchara->img_slide[ 1 ] );
                e_pchara->x0 -= nWidth - e_pchara->width;
                e_pchara->width = nWidth;
            }
        }

        e_pchara->x = e_pchara->x0 + e_pchara->nDeltaX;
        break;

    case ECS_INJURED:
        // change state
        if( g_LastState != NewState ) {
            e_pchara->nMoveHeight = e_pchara->height;
            int nHeight = al_get_bitmap_height( e_pchara->img_move[ 0 ] );
            e_pchara->y -= ( nHeight - e_pchara->height ) / 2;
            e_pchara->height = nHeight;
            e_pchara->nMoveY = e_pchara->y;

            if( e_pchara->dir == false ) {
                int nWidth = al_get_bitmap_width( e_pchara->img_move[ 0 ] );
                e_pchara->x -= nWidth - e_pchara->width;
                e_pchara->width = nWidth;
                e_pchara->nMoveWidth = e_pchara->width;
            }
            else {
                int nWidth = al_get_bitmap_width( e_pchara->img_move[ 0 ] );
                e_pchara->width = nWidth;
                e_pchara->nMoveWidth = e_pchara->width;
            }
        }
        break;

    default:
        assert( false );
        break;
    }


    g_LastState = NewState;
    g_nLastSubState = e_pchara->nSubState;
//    printf( "%d %d | ", g_LastState, g_nLastSubState );
//    printf( "Character at x = %d, y = %d\n", e_pchara->x, e_pchara->y );
//    printf( "character_StateChangeImage\n" );
}

void character_attackMonster( void )
{
    // plot fire
    if( e_pchara->NowSpecialAtk == ESA_FIRE && e_pchara->state == ECS_ATK ) {
        if( e_pchara->dir == true ) {
            al_draw_bitmap( e_pchara->img_fire[ e_pchara->nSubState ],
                            e_pchara->x + e_pchara->width,
                            e_pchara->y + ( e_pchara->height- al_get_bitmap_height( e_pchara->img_fire[ e_pchara->nSubState ] ) ) / 2,
                            ALLEGRO_FLIP_HORIZONTAL );
        }
        else {
            al_draw_bitmap( e_pchara->img_fire[ e_pchara->nSubState ],
                            e_pchara->x - al_get_bitmap_width( e_pchara->img_fire[ e_pchara->nSubState ] ),
                            e_pchara->y + ( e_pchara->height- al_get_bitmap_height( e_pchara->img_fire[ e_pchara->nSubState ] ) ) / 2,
                            0 );
        }
    }

    // check monster
    if( e_pchara->state == ECS_ATK || e_pchara->state == ECS_SLIDE ) {
        for( int i = 0; i < g_nMonsterCount; i++ ) {
            if( e_monster[ i ].state != EMS_DIE ) {
                Position CharacterPos, MonsterPos;
                if( e_pchara->state == ECS_SLIDE ) {
                    CharacterPos.w = e_pchara->x;
                    CharacterPos.e = e_pchara->x + e_pchara->width;
                    CharacterPos.n = e_pchara->y;
                    CharacterPos.s = e_pchara->y + e_pchara->height;
                }
                // atk
                else {
                    if( e_pchara->NowSpecialAtk == ESA_FIRE ) {
                        if( e_pchara->dir == true ) {
                            CharacterPos.w = e_pchara->x + e_pchara->width;
                            CharacterPos.e = e_pchara->x + e_pchara->width
                                             + al_get_bitmap_width( e_pchara->img_fire[ e_pchara->nSubState ] );

                            CharacterPos.n = e_pchara->y
                                            + ( e_pchara->height- al_get_bitmap_height( e_pchara->img_fire[ e_pchara->nSubState ] ) ) / 2;

                            CharacterPos.s = e_pchara->y
                                            + ( e_pchara->height+ al_get_bitmap_height( e_pchara->img_fire[ e_pchara->nSubState ] ) ) / 2;
                        }
                        else {
                            CharacterPos.e = e_pchara->x;
                            CharacterPos.w = e_pchara->x
                                             - al_get_bitmap_width( e_pchara->img_fire[ e_pchara->nSubState ] );

                            CharacterPos.n = e_pchara->y
                                            + ( e_pchara->height- al_get_bitmap_height( e_pchara->img_fire[ e_pchara->nSubState ] ) ) / 2;

                            CharacterPos.s = e_pchara->y
                                            + ( e_pchara->height+ al_get_bitmap_height( e_pchara->img_fire[ e_pchara->nSubState ] ) ) / 2;
                        }
                    }
                    // normal, sword
                    else {
                        CharacterPos.w = e_pchara->x;
                        CharacterPos.e = e_pchara->x + e_pchara->width;
                        CharacterPos.n = e_pchara->y;
                        CharacterPos.s = e_pchara->y + e_pchara->height;
                    }
                }

                MonsterPos.e = e_monster[ i ].x + e_monster[ i ].width - 8;
                MonsterPos.w = e_monster[ i ].x + 8;
                MonsterPos.n = e_monster[ i ].y + 8;
                MonsterPos.s = e_monster[ i ].y + e_monster[ i ].height - 8;

                if( CheckOverlap( &CharacterPos, &MonsterPos ) == true ) {
                    e_monster[ i ].hp -= ( ( int )e_pchara->NowSpecialAtk + 1 );
                }
            }
        }
    }

    // check bomb
    if( e_pchara->nBombIdx != -1 ) {
        Position CharacterPos, MonsterPos;

        bool bEffectiveBomb = GetBombPosition( e_pchara->nBombIdx, &CharacterPos );
        if( bEffectiveBomb == false ) {
            return;
        }

        CharacterPos.e -= 30; // shrink boarder
        CharacterPos.s -= 30;
        CharacterPos.w += 30;
        CharacterPos.n += 30;

        for( int i = 0; i < g_nMonsterCount; i++ ) {
            if( e_monster[ i ].state == EMS_DIE ) {
                continue;
            }
            MonsterPos.e = e_monster[ i ].x + e_monster[ i ].width - 16;
            MonsterPos.w = e_monster[ i ].x + 16;
            MonsterPos.n = e_monster[ i ].y + 16;
            MonsterPos.s = e_monster[ i ].y + e_monster[ i ].height - 16;

            if( CheckOverlap( &CharacterPos, &MonsterPos ) == true ) {
                e_monster[ i ].hp -= 3;

                // explode bomb
                if( e_pchara->NowSpecialAtk == ESA_BOMB ) {
                    printf( "Trigger Explode\n" );
                    ExplodeBomb( e_pchara->nBombIdx );
                }
            }
        }
        printf( "%d %d %d %d\n", CharacterPos.e, CharacterPos.s, CharacterPos.w, CharacterPos.n );
    }
}

void character_attack_move( void )
{
//    printf( "character_attack\n" );
    if( g_bCDing == true ) {
//        printf( "character_attack\n" );
        return;
    }

    int nDeltaX = 0;
    switch( e_pchara->state ) {
    case ECS_ATK:
        e_pchara->nDeltaX = ( e_pchara->dir ? 1 : -1 ) * e_pchara->nAtkCursor * e_pchara->nAtkRate;
//        e_pchara->x = e_pchara->x0 + nDeltaX;
        break;

    case ECS_INHALE:
        nDeltaX = ( e_pchara->dir ? 1 : -1 ) * e_pchara->nInhaleCursor * 10;
        e_pchara->x = e_pchara->x0 + nDeltaX;
        break;

    case ECS_SLIDE:
        e_pchara->nDeltaX = ( e_pchara->dir ? 1 : -1 ) * e_pchara->nSlideCursor * e_pchara->nSlideRate;
        break;

    case ECS_STOP:
    case ECS_INJURED:

    default:
        // do nothing
        break;
    }
//    printf( "character_attack\n" );
}

void character_drawhp( void )
{
    if(e_pchara->hp >=6){al_draw_bitmap(e_pchara->img_store_HP[ 6 ],e_pchara->CamPos+280, HEIGHT - g_nWordHeight-20, 0);}
	else if(e_pchara->hp ==5){al_draw_bitmap(e_pchara->img_store_HP[ 5 ],e_pchara->CamPos+280, HEIGHT - g_nWordHeight-20, 0);}
	else if(e_pchara->hp ==4){al_draw_bitmap(e_pchara->img_store_HP[ 4 ],e_pchara->CamPos+280, HEIGHT - g_nWordHeight-20, 0);}
    else if(e_pchara->hp ==3){al_draw_bitmap(e_pchara->img_store_HP[ 3 ],e_pchara->CamPos+280, HEIGHT - g_nWordHeight-20, 0);}
    else if(e_pchara->hp ==2){al_draw_bitmap(e_pchara->img_store_HP[ 2 ],e_pchara->CamPos+280, HEIGHT - g_nWordHeight-20, 0);}
    else if(e_pchara->hp ==1){al_draw_bitmap(e_pchara->img_store_HP[ 1 ],e_pchara->CamPos+280, HEIGHT - g_nWordHeight-20, 0);}
    else {al_draw_bitmap(e_pchara->img_store_HP[ 0 ],e_pchara->CamPos+280, HEIGHT - g_nWordHeight-20, 0);}

    if(e_pchara->life >=3){
    al_draw_bitmap(e_pchara->img_store_LIFE[ 1 ],e_pchara->CamPos+160, HEIGHT - g_nWordHeight-422, 0);
    al_draw_bitmap(e_pchara->img_store_LIFE[ 1 ],e_pchara->CamPos+95, HEIGHT - g_nWordHeight-422, 0);
    al_draw_bitmap(e_pchara->img_store_LIFE[ 1 ],e_pchara->CamPos+30, HEIGHT - g_nWordHeight-422, 0);
    }
	else if(e_pchara->life ==2){
    al_draw_bitmap(e_pchara->img_store_LIFE[ 1 ],e_pchara->CamPos+95, HEIGHT - g_nWordHeight-422, 0);
    al_draw_bitmap(e_pchara->img_store_LIFE[ 1 ],e_pchara->CamPos+30, HEIGHT - g_nWordHeight-422, 0);
    }
	else if(e_pchara->life ==1){al_draw_bitmap(e_pchara->img_store_LIFE[ 1 ],e_pchara->CamPos+30, HEIGHT - g_nWordHeight-422, 0);}
}

void character_draw()
{
//    printf( "character_draw\n" );
//    printf( "e_pchara->substate = %d, e_pchara->y = %d, e_pchara->s = %d, e_pchara->height = %d, e_pchara->nMoveHeight = %d\n",
//           e_pchara->nSubState, e_pchara->y, e_pchara->y + ( e_pchara->height + e_pchara->nMoveHeight ) / 2, e_pchara->height, e_pchara->nMoveHeight );

    int nTemp = 0;
    if( e_pchara->height > e_pchara->nMoveHeight ) {
        nTemp = e_pchara->y + ( e_pchara->height + e_pchara->nMoveHeight ) / 2;
    }
    else {
        nTemp = e_pchara->y + e_pchara->height;
    }
    int nGroundY = FindAndDrawClosestGroundY( e_pchara->x, e_pchara->x + e_pchara->width, nTemp );

    character_gravity( nGroundY );

    // check transform
    character_eat();
    character_checkTransform();

    character_attack_move();

    // check monster alive?
//    charater_update2();

    character_StateChangeImage();


    character_BoundryCheck();
    character_CheckBlocker();
    character_attackMonster();

	if( camera_move == 1 ) {
    	CameraUpdate( &e_pchara->CamPos, e_pchara->x, e_pchara->y, e_pchara->width, e_pchara->height, e_pchara->nMoveWidth );
    	al_identity_transform( &camera );
    	al_translate_transform( &camera, -e_pchara->CamPos, 0 );
    	al_use_transform( &camera );
	}

    character_drawhp();
    //    printf( "character_draw\n" );

    // with the state, draw corresponding image

    if( g_bImmortal == true ) {
        if( g_nImortalCursor % 5 > 1 ) {
            charactor_show();
        }
    }
    else {
        charactor_show();
    }


    al_draw_bitmap( e_pchara->img_atkWord, e_pchara->CamPos, HEIGHT - g_nWordHeight, 0 );

//    printf( "character_draw\n" );
}

void charactor_show( void )
{
    switch( e_pchara->state ) {
    case ECS_STOP:
        if( e_pchara->dir ) {
            al_draw_bitmap( e_pchara->img_move[ e_pchara->nSubState ], e_pchara->x, e_pchara->y, ALLEGRO_FLIP_HORIZONTAL);
        }
        else {
            al_draw_bitmap( e_pchara->img_move[ e_pchara->nSubState ], e_pchara->x, e_pchara->y, 0);
        }
        break;

    case ECS_MOVE:
        if( e_pchara->dir ) {
            al_draw_bitmap( e_pchara->img_move[ e_pchara->nSubState ], e_pchara->x, e_pchara->y, ALLEGRO_FLIP_HORIZONTAL );
        }
        else {
            al_draw_bitmap( e_pchara->img_move[ e_pchara->nSubState ], e_pchara->x, e_pchara->y, 0 );
        }
        break;

    case ECS_INHALE:
        if( e_pchara->dir ) {
            al_draw_bitmap( e_pchara->img_inhale[ e_pchara->nSubState ], e_pchara->x, e_pchara->y, ALLEGRO_FLIP_HORIZONTAL );
        }
        else {
            al_draw_bitmap( e_pchara->img_inhale[ e_pchara->nSubState ], e_pchara->x, e_pchara->y, 0 );

        }
        if( e_pchara->nSubState == 0 ) {
            al_play_sample_instance( e_pchara->sound_inhale );
        }
        break;

    case ECS_TRANSFORMING:
        if( e_pchara->dir ) {
            al_draw_bitmap( e_pchara->img_transform[ e_pchara->nSubState ], e_pchara->x, e_pchara->y, ALLEGRO_FLIP_HORIZONTAL );
        }
        else {
            al_draw_bitmap( e_pchara->img_transform[ e_pchara->nSubState ], e_pchara->x, e_pchara->y, 0 );
        }

        if( e_pchara->nSubState == 0 ) {
            al_play_sample_instance( e_pchara->sound_transform );
        }
        break;

    case ECS_ATK:
        // plot fire
        if( e_pchara->NowSpecialAtk == ESA_FIRE ) {
            if( e_pchara->dir == true ) {
                al_draw_bitmap( e_pchara->img_fire[ e_pchara->nSubState ],
                                e_pchara->x + e_pchara->width,
                                e_pchara->y + ( e_pchara->height- al_get_bitmap_height( e_pchara->img_fire[ e_pchara->nSubState ] ) ) / 2,
                                ALLEGRO_FLIP_HORIZONTAL );
            }
            else {
                al_draw_bitmap( e_pchara->img_fire[ e_pchara->nSubState ],
                                e_pchara->x - al_get_bitmap_width( e_pchara->img_fire[ e_pchara->nSubState ] ),
                                e_pchara->y + ( e_pchara->height- al_get_bitmap_height( e_pchara->img_fire[ e_pchara->nSubState ] ) ) / 2,
                                0 );
            }
        }
        // plot character
        if( e_pchara->dir ) {
            al_draw_bitmap( e_pchara->img_atk[ e_pchara->nSubState ], e_pchara->x, e_pchara->y, ALLEGRO_FLIP_HORIZONTAL );
        }
        else {
            al_draw_bitmap( e_pchara->img_atk[ e_pchara->nSubState ], e_pchara->x, e_pchara->y, 0 );
        }

        if( e_pchara->nSubState == 0 ) {
            if( e_pchara->NowSpecialAtk == ESA_SWORD ) {
                al_play_sample_instance( e_pchara->sound_slash );
            }
            else if( e_pchara->NowSpecialAtk == ESA_FIRE ) {
                al_play_sample_instance( e_pchara->sound_fire );
            }
        }
        break;

    case ECS_SLIDE:
        if( e_pchara->dir ) {
            al_draw_bitmap( e_pchara->img_slide[ e_pchara->nSubState ], e_pchara->x, e_pchara->y, ALLEGRO_FLIP_HORIZONTAL );
        }
        else {
            al_draw_bitmap( e_pchara->img_slide[ e_pchara->nSubState ], e_pchara->x, e_pchara->y, 0 );
        }
        break;

    case ECS_INJURED:
        //printf("%d\n",e_pchara->anime);
        if( e_pchara->nInjuredCursor < 5 ) {
            if( e_pchara->dir ) {
                al_draw_bitmap( e_pchara->img_move[ e_pchara->nSubState ], e_pchara->x, e_pchara->y, ALLEGRO_FLIP_HORIZONTAL );
            }
            else {
                al_draw_bitmap( e_pchara->img_move[ e_pchara->nSubState ], e_pchara->x, e_pchara->y, 0);
            }
        }
        break;



    default:
        //assert( false );
        break;
    } // end of switch

}

void character_destroy( void )
{
//    printf( "character_destory\n" );
    al_destroy_sample_instance( e_pchara->sound_inhale );
    al_destroy_sample_instance( e_pchara->sound_transform );
    al_destroy_sample_instance( e_pchara->sound_slash );
    al_destroy_sample_instance( e_pchara->sound_fire );

    al_identity_transform( &camera );
    al_translate_transform( &camera, 0, 0 );
    al_use_transform( &camera );

    // bit map
    for( int i = 0; i < 2; i++ ) {
        al_destroy_bitmap( e_pchara->img_inhale[ i ] );
    }
    for( int i = 0; i < 2; i++ ) {
        al_destroy_bitmap( e_pchara->img_fire[ i ] );
    }
    // bit map store
    for( int i = 0; i < ESA_NUM; i++ ) {
        for( int j = 0; j < 2; j++ ) {
            al_destroy_bitmap( e_pchara->img_store_SpecialAtk[ 2 * i + j ] );
        }
        for( int j = 0; j < 4; j++ ) {
            al_destroy_bitmap( e_pchara->img_store_move[ 4 * i + j ] );
        }
        for( int j = 0; j < 2; j++ ) {
            al_destroy_bitmap( e_pchara->img_store_slide[ 2 * i + j ] );
        }

        if( i == 0 ) {
            // do nothing
        }
        else {
            for( int j = 0; j < NUMOF_TRANSFORM_IMG; j++ ) {
                al_destroy_bitmap( e_pchara->img_store_transform[ NUMOF_TRANSFORM_IMG * i + j ] );
            }
        }

        al_destroy_bitmap( e_pchara->img_store_AtkWord[ i ] );
    }

    for( int i = 0; i < HP + 1; i++ ) {
        al_destroy_bitmap( e_pchara->img_store_HP[ i ] );
    }

    free( e_pchara );
    e_pchara = NULL;
    printf( "character destroy success!\n" );
//    printf( "character_destory\n" );
}

//==============================================================================================
//ESA_NORMAL = 0,
//ESA_SWORD = 1,
//ESA_BOMB = 2,
//ESA_FIRE = 3,

void monster_init( void )
{
//    printf( "character_init2\n" );
    FILE* fp = fopen( "./res/monster.txt", "r" );
    int nPosX = 0;
    int nPosY = 0;
    int nLM = 0;
    int nRM = 0;
    int nType = 0;
    g_nMonsterCount = 0;
    for( int i = 0; i < MAX_COUNTOF_MONSTER; i++ ) {
        g_MonLastState[ i ] = EMS_ALIVE;
        g_nMonLastSubState[ i ] = 0;
    }

    while( fscanf( fp, "%d%d%d%d%d", &nPosX, &nPosY, &nLM, &nRM, &nType ) != EOF ) {
        e_monster[ g_nMonsterCount ].x = nPosX;
        e_monster[ g_nMonsterCount ].y = nPosY;
        e_monster[ g_nMonsterCount ].lm = nLM;
        e_monster[ g_nMonsterCount ].rm = nRM;
        e_monster[ g_nMonsterCount ].type = nType;
        g_nMonsterCount++;

        if( g_nMonsterCount == MAX_COUNTOF_MONSTER ) {
            break;
        }
    }
    fclose( fp ); //記得關檔

    for( int n = 0; n < g_nMonsterCount; n++ ) {
        if( e_monster[ n ].type == ESA_NORMAL ) {
            for( int i = 0; i < 2; i++ ) {
                char temp[ 50 ];
                sprintf( temp, "./image/enemy_move%d.png",i+1);
                e_monster[ n ].img_move[ i ] = al_load_bitmap( temp );
                assert( e_monster[ n ].img_move[ i ] != NULL );
                e_monster[ n ].width = al_get_bitmap_width( e_monster[ n ].img_move[ 0 ] );
                e_monster[ n ].height = al_get_bitmap_height( e_monster[ n ].img_move[ 0 ] );
                e_monster[ n ].img_atk[ i ] = al_load_bitmap( temp );
                assert( e_monster[ n ].img_atk[ i ] != NULL );
            }
        }
        else if( e_monster[ n ].type == ESA_SWORD ) {
            for( int i = 0; i < 2; i++ ) {
                char temp[ 50 ];
                char tmpatk[ 50 ];
                sprintf( temp, "./image/enemy_sword_move%d.png", i + 1 );
                e_monster[ n ].img_move[ i ] = al_load_bitmap( temp );
                assert( e_monster[ n ].img_move[ i ] != NULL );
                e_monster[ n ].width = al_get_bitmap_width( e_monster[ n ].img_move[ 0 ] );
                e_monster[ n ].height = al_get_bitmap_height( e_monster[ n ].img_move[ 0 ] );

                sprintf( tmpatk, "./image/enemy_sword_slash%d.png", i + 1 );
                e_monster[ n ].img_atk[ i ] = al_load_bitmap( tmpatk );
                assert( e_monster[ n ].img_atk[ i ] != NULL );
            }
        }
        else if( e_monster[ n ].type == ESA_BOMB ) {
            for( int i = 0; i < 2; i++ ) {
                char temp[ 50 ];
                char tmpatk[ 50 ];
                sprintf( temp, "./image/enemy_bomb_move%d.png", i + 1 );
                e_monster[ n ].img_move[ i ] = al_load_bitmap( temp );
                assert( e_monster[ n ].img_move[ i ] != NULL );
                e_monster[ n ].width = al_get_bitmap_width( e_monster[ n ].img_move[ 0 ] );
                e_monster[ n ].height = al_get_bitmap_height( e_monster[ n ].img_move[ 0 ] );

                sprintf( tmpatk, "./image/enemy_bomb_throw%d.png", i + 1 );
                e_monster[ n ].img_atk[ i ] = al_load_bitmap( tmpatk );
                assert( e_monster[ n ].img_atk[ i ] != NULL );
            }
        }
        else if( e_monster[ n ].type == ESA_FIRE ) {
            for( int i = 0; i < 2; i++ ) {
                char temp[ 50 ];
                char tmpatk[ 50 ];
                sprintf( temp, "./image/enemy_fire_move%d.png", i + 1 );
                e_monster[ n ].img_move[ i ] = al_load_bitmap( temp );
                assert( e_monster[ n ].img_move[ i ] != NULL );
                e_monster[ n ].width = al_get_bitmap_width( e_monster[ n ].img_move[ 0 ] );
                e_monster[ n ].height = al_get_bitmap_height( e_monster[ n ].img_move[ 0 ] );

                sprintf( tmpatk, "./image/enemy_fire_spit%d.png", i + 1 );
                e_monster[ n ].img_atk[ i ] = al_load_bitmap( tmpatk );
                assert( e_monster[ n ].img_atk[ i ] != NULL );

                // fire
                sprintf( temp, "./image/fire%d.png", i + 1 );
                e_monster[ n ].img_fire[ i ] = al_load_bitmap( temp );
                assert( e_monster[ n ].img_fire[ i ] != NULL );
            }
        }

        e_monster[ n ].dir = true;
        e_monster[ n ].hp = 1; // TODO different type should have different hp
        e_monster[ n ].state = EMS_ALIVE;
        e_monster[ n ].nSubState = 0;
        e_monster[ n ].nBombIdx = -1;

        e_monster[ n ].nMoveCursor = n % 30; // different attack phase
        e_monster[ n ].nMoveTime = 30;

        e_monster[ n ].nAtkCursor = 0; 		// attack cursor
        e_monster[ n ].nAtkTime = 30;

        e_monster[ n ].nSwitchCursor = 0;
        e_monster[ n ].nSwitchTime = 300;

        // gravity
        e_monster[ n ].y0 = e_monster[ n ].y;
        e_monster[ n ].vy = 0.0;
        e_monster[ n ].FallingTick = 0.0;
    }
//    printf( "character_init2\n" );
}

void monster_process( ALLEGRO_EVENT event )
{
//    printf( "charater_process2\n" );
    // process the animation                //1.處理動畫
    if( event.type == ALLEGRO_EVENT_TIMER ) { //根據fps+anime
        if( event.timer.source == fps ) {
            for( int n = 0; n < g_nMonsterCount; n++ ) {

                if( e_monster[ n ].state == EMS_ALIVE ) {
                    // move cursor
                    e_monster[ n ].nMoveCursor++;
                    e_monster[ n ].nMoveCursor %= e_monster[ n ].nMoveTime; //讓我們知道現在跑道time的哪一步

                    // switch cursor
                    e_monster[ n ].nSwitchCursor++;  //用以判斷說要進行攻擊了的週期
                    e_monster[ n ].nSwitchCursor %= e_monster[ n ].nSwitchTime;

                    if( e_monster[ n ].nSwitchCursor == 0 ) {
                        e_monster[ n ].state = EMS_ATK;
                        e_monster[ n ].nAtkCursor = 0;
                    }
                }

                if( e_monster[ n ].state == EMS_ATK ) {
                    e_monster[ n ].nAtkCursor++;
                    e_monster[ n ].nAtkCursor %= e_monster[ n ].nAtkTime;

                    if( e_monster[ n ].nAtkCursor == 0 ) {
                        e_monster[ n ].state = EMS_ALIVE;
                        e_monster[ n ].nSwitchCursor = 0;
                        e_monster[ n ].nMoveCursor = 0;
                    }
                }
            }
        }
    }
//    printf( "charater_process2\n" );
}

void monster_update( void )
{
    for( int i = 0; i < g_nMonsterCount; i++ ) {
        if( e_monster[ i ].state == EMS_ALIVE || e_monster[ i ].state == EMS_ATK ) {
            Position CharacterPosNoAtk,MonsterPosNoAtk;
            CharacterPosNoAtk.w = e_pchara->x;
            CharacterPosNoAtk.e = e_pchara->x + e_pchara->width;
            CharacterPosNoAtk.n = e_pchara->y;
            CharacterPosNoAtk.s = e_pchara->y + e_pchara->height;
            MonsterPosNoAtk.e = e_monster[ i ].x + e_monster[ i ].width - 8;
            MonsterPosNoAtk.w = e_monster[ i ].x + 8;
            MonsterPosNoAtk.n = e_monster[ i ].y + 8;
            MonsterPosNoAtk.s = e_monster[ i ].y + e_monster[ i ].height - 8;

            if( CheckOverlap( &CharacterPosNoAtk, &MonsterPosNoAtk ) == true ) {
                if( g_bImmortal == true || e_pchara->state == ECS_ATK || e_pchara->state == ECS_INHALE || e_pchara->state == ECS_SLIDE ) {
                    // do nothing
                }
                else {
                    printf( "%d ", e_pchara->state );
                    g_bImmortal = true;
                    g_nImortalCursor = 0;
                    e_pchara->state = ECS_INJURED;
                    e_pchara->nInjuredCursor = 0;
                    e_pchara->hp--;
                    printf( "yo\n" );
                }
            }

            if( e_monster[ i ].hp <= 0 ) {
                //如果死掉
                al_destroy_bitmap( e_monster[ i ].img_move[ 0 ] );
                e_monster[ i ].state = EMS_DIE;

            }
        }

    }
//    printf( "charater_update2\n" );
}

void monsrt_attackCharacter( void ){
        for( int i = 0; i < g_nMonsterCount; i++ ) {
            if( e_monster[ i ].state == EMS_ATK ) {
                Position CharacterPos, MonsterPos;

                    if( e_monster[ i ].type == ESA_FIRE ) { // atk-fire
                        if( e_monster[ i ].dir == true ) {
                            MonsterPos.w = e_monster[ i ].x + e_monster[ i ].width;
                            MonsterPos.e = e_monster[ i ].x + e_monster[ i ].width
                                             + al_get_bitmap_width( e_monster[ i ].img_fire[ e_monster[ i ].nSubState ] );

                            MonsterPos.n = e_monster[ i ].y
                                            + ( e_monster[ i ].height- al_get_bitmap_height( e_monster[ i ].img_fire[ e_monster[ i ].nSubState ] ) ) / 2;

                            MonsterPos.s = e_monster[ i ].y
                                            + ( e_monster[ i ].height+ al_get_bitmap_height( e_monster[ i ].img_fire[ e_monster[ i ].nSubState ] ) ) / 2;
                        }
                        else {
                            MonsterPos.e = e_monster[ i ].x;
                            MonsterPos.w = e_monster[ i ].x
                                             - al_get_bitmap_width( e_monster[ i ].img_fire[ e_monster[ i ].nSubState ] );

                            MonsterPos.n = e_monster[ i ].y
                                            + ( e_monster[ i ].height- al_get_bitmap_height( e_monster[ i ].img_fire[ e_monster[ i ].nSubState ] ) ) / 2;

                            MonsterPos.s = e_monster[ i ].y
                                            + ( e_monster[ i ].height+ al_get_bitmap_height( e_monster[ i ].img_fire[ e_monster[ i ].nSubState ] ) ) / 2;
                        }
                    }
                    // normal / sword
                    else {
                        MonsterPos.w = e_monster[ i ].x;
                        MonsterPos.e = e_monster[ i ].x + e_monster[ i ].width;
                        MonsterPos.n = e_monster[ i ].y;
                        MonsterPos.s = e_monster[ i ].y + e_monster[ i ].height;
                    }


                CharacterPos.e = e_pchara->x + e_pchara->width - 8;
                CharacterPos.w = e_pchara->x + 8;
                CharacterPos.n = e_pchara->y + 8;
                CharacterPos.s = e_pchara->y + e_pchara->height - 8;

                if( CheckOverlap( &CharacterPos, &MonsterPos ) == true ) {
                     if( g_bImmortal == true || e_pchara->state == ECS_ATK || e_pchara->state == ECS_INHALE || e_pchara->state == ECS_SLIDE ) {
                    // do nothing
                    }
                    else {
                        printf( "%d ", e_pchara->state );
                        g_bImmortal = true;
                        g_nImortalCursor = 0;
                        e_pchara->state = ECS_INJURED;
                        e_pchara->nInjuredCursor = 0;
                        e_pchara->hp--;
                        printf( "yo\n" );
                    }
                }
        }
    }
}







void monster_gravity( int nMonsterIdx, int nGroundY ) {
//    printf( "monster_gravity\n" );
    if( nGroundY == -2 ) {
        nGroundY = HEIGHT;
    }

    // ticktock
    e_monster[ nMonsterIdx ].FallingTick += g_Tick;

    // velocity;
    e_monster[ nMonsterIdx ].vy += g_Gravity * g_Tick;
    // position
    e_monster[ nMonsterIdx ].y =
        e_monster[ nMonsterIdx ].y0
        + ( int )( e_monster[ nMonsterIdx ].vy * e_monster[ nMonsterIdx ].FallingTick + ( 0.5 * g_Gravity * ( e_monster[ nMonsterIdx ].FallingTick * e_monster[ nMonsterIdx ].FallingTick ) ) );


    if( ( e_monster[ nMonsterIdx ].y + e_monster[ nMonsterIdx ].height ) >= nGroundY ) {
        e_monster[ nMonsterIdx ].y = nGroundY - e_monster[ nMonsterIdx ].height;
        e_monster[ nMonsterIdx ].y0 = e_monster[ nMonsterIdx ].y;
        e_monster[ nMonsterIdx ].vy = 0.0;
        e_monster[ nMonsterIdx ].FallingTick = 0;
    }
    else {
        // do nothing
    }
//    printf( "monster_gravity\n" );
}

void monster_CheckBlocker( const int nMonsterIdx )
{
    if( e_monster[ nMonsterIdx ].state == EMS_DIE ) {
        return;
    }

    Position pos;
    if( e_monster[ nMonsterIdx ].dir == false ) {
        pos.e = e_monster[ nMonsterIdx ].x + e_monster[ nMonsterIdx ].width;
        pos.w = pos.e - e_monster[ nMonsterIdx ].width;
    }
    else {
        pos.w = e_monster[ nMonsterIdx ].x;
        pos.e = e_monster[ nMonsterIdx ].x + e_monster[ nMonsterIdx ].width;
    }

    pos.n = e_monster[ nMonsterIdx ].y;
    pos.s = e_monster[ nMonsterIdx ].y + e_monster[ nMonsterIdx ].height;

    bool bClamped = CheckBlocker( &pos, e_monster[ nMonsterIdx ].dir );

    // update x, y
    if( bClamped == true ) {
        e_monster[ nMonsterIdx ].x = pos.w;
        e_monster[ nMonsterIdx ].y = pos.n;
    }
}

void monster_StateChangeImage( const int i )
{
    //    printf( "monster_StateChangeImage\n" );
    EMonsterState NewState = ( EMonsterState )e_monster[ i ].state;

    // update substate
//    printf( "update substate\n" );
    switch( NewState ) {
    case EMS_ALIVE:
        if( NewState != g_MonLastState[ i ] ) {
            e_monster[ i ].nSubState = 0;
        }

        if( e_monster[ i ].nMoveCursor < e_monster[ i ].nMoveTime / 2 ) {
            e_monster[ i ].nSubState = 0;
        }
        else {
            e_monster[ i ].nSubState = 1;
        }
        break;

    case EMS_ATK:
        if( NewState != g_MonLastState[ i ] ) {
            e_monster[ i ].nSubState = 0;
        }

        if( e_monster[ i ].nAtkCursor < e_monster[ i ].nAtkTime / 2 ) {
            e_monster[ i ].nSubState = 0;
        }
        else {
            e_monster[ i ].nSubState = 1;
        }
        break;

    case EMS_DIE:
        // do nothing
        break;

    default:
        assert( false );
        break;
    }

    // update xy
//    printf( "update xy\n" );
    switch( NewState ) {
    case EMS_ALIVE:
        // change state
        if( g_MonLastState[ i ] != NewState ||
            ( g_MonLastState[ i ] == NewState && ( g_nMonLastSubState[ i ] != e_monster[ i ].nSubState ) ) ) {
            if( e_monster[ i ].dir == false ) {
                int nWidth = al_get_bitmap_width( e_monster[ i ].img_move[ e_monster[ i ].nSubState ] );
                e_monster[ i ].x -= nWidth - e_monster[ i ].width;
                e_monster[ i ].width = nWidth;
            }
            else {
                int nWidth = al_get_bitmap_width( e_monster[ i ].img_move[ 0 ] );
                e_monster[ i ].width = nWidth;
            }
        }
        break;

    case EMS_ATK:
        // change state
        if( g_MonLastState[ i ] != NewState ||
            ( g_MonLastState[ i ] == NewState && ( g_nMonLastSubState[ i ] != e_monster[ i ].nSubState ) ) ) {
            if( e_monster[ i ].dir == false ) {
                int nWidth = al_get_bitmap_width( e_monster[ i ].img_atk[ e_monster[ i ].nSubState ] );
                e_monster[ i ].x -= nWidth - e_monster[ i ].width;
                e_monster[ i ].width = nWidth;
            }
            else {
                int nWidth = al_get_bitmap_width( e_monster[ i ].img_atk[ 0 ] );
                e_monster[ i ].width = nWidth;
            }
        }
        break;

    case EMS_DIE:
        // do nothing;
        break;

    default:
        assert( false );
        break;
    }


    g_MonLastState[ i ] = NewState;
    g_nMonLastSubState[ i ] = e_pchara->nSubState;
//    printf( "%d %d | ", g_LastState, g_nLastSubState );
//    printf( "Character at x = %d, y = %d\n", e_pchara->x, e_pchara->y );
//    printf( "character_StateChangeImage\n" );
}

void monster_draw( void )
{
    //printf( "character_draw2\n" );
    for( int i = 0; i < g_nMonsterCount; i++ ) {
        monster_StateChangeImage( i );
    }

    for( int n = 0; n < g_nMonsterCount; n++ ) {
    // with the state, draw corresponding image
        if( e_monster[ n ].state == EMS_ALIVE || e_monster[ n ].state == EMS_ATK ) {

            int nGroundY = FindAndDrawClosestGroundY( e_monster[ n ].x, e_monster[ n ].x + e_monster[ n ].width, e_monster[ n ].y + e_monster[ n ].height );

            monster_gravity( n, nGroundY );
            monster_CheckBlocker( n );


            monsrt_attackCharacter();

            if( e_monster[ n ].state == EMS_ALIVE ) {
//                if( e_monster[ n ].anime < e_monster[ n ].anime_time / 2 ) {
                    int nSubState = e_monster[ n ].nSubState;
                    if( e_monster[ n ].dir == true ) { //如果面右
                        if( e_monster[ n ].x >= e_monster[ n ].rm ) {
                            e_monster[ n ].dir = false;
                            al_draw_bitmap( e_monster[ n ].img_move[ nSubState ], e_monster[ n ].x, e_monster[ n ].y, 0 );
                        }
                        al_draw_bitmap( e_monster[ n ].img_move[ nSubState ], e_monster[ n ].x, e_monster[ n ].y, ALLEGRO_FLIP_HORIZONTAL );
                        e_monster[ n ].x += 1;
                    }
                    else {//如果面左
                        if( e_monster[ n ].x <= e_monster[ n ].lm ) {
                            e_monster[ n ].dir = true;
                            al_draw_bitmap( e_monster[ n ].img_move[ nSubState ], e_monster[ n ].x, e_monster[ n ].y, ALLEGRO_FLIP_HORIZONTAL );
                        }
                        al_draw_bitmap( e_monster[ n ].img_move[ nSubState ], e_monster[ n ].x, e_monster[ n ].y, 0 );
                        e_monster[ n ].x -= 1;
                    }
            }
            else if( e_monster[ n ].state == EMS_ATK ) {
//                printf("nice");
                int nSubState = e_monster[ n ].nSubState;

                // throw bomb
                if( e_monster[ n ].nSubState == 1 && e_monster[ n ].type == ESA_BOMB ) {
                    // first to to register bomb
                    if( e_monster[ n ].nBombIdx == -1 ) {
                        e_monster[ n ].nBombIdx = RegisterBomb();
                        assert( e_monster[ n ].nBombIdx != -1 );
                    }
                    BombThrow( e_monster[ n ].nBombIdx, e_monster[ n ].x, e_monster[ n ].y, e_monster[ n ].dir );
                }

                // plot fire
                if( e_monster[ n ].type == ESA_FIRE ) {
                    if( e_monster[ n ].dir == true ) {
                        al_draw_bitmap( e_monster[ n ].img_fire[ e_monster[ n ].nSubState ],
                                        e_monster[ n ].x + e_monster[ n ].width,
                                        e_monster[ n ].y + ( e_monster[ n ].height- al_get_bitmap_height( e_monster[ n ].img_fire[ e_monster[ n ].nSubState ] ) ) / 2,
                                        ALLEGRO_FLIP_HORIZONTAL );
                    }
                    else {
                        al_draw_bitmap( e_monster[ n ].img_fire[ e_monster[ n ].nSubState ],
                                        e_monster[ n ].x - al_get_bitmap_width( e_monster[ n ].img_fire[ e_monster[ n ].nSubState ] ),
                                        e_monster[ n ].y + ( e_monster[ n ].height- al_get_bitmap_height( e_monster[ n ].img_fire[ e_monster[ n ].nSubState ] ) ) / 2,
                                        0 );
                    }
                }

                if( e_monster[ n ].dir == true ) {
                    al_draw_bitmap( e_monster[ n ].img_atk[ nSubState ],
                                    e_monster[ n ].x,
                                    e_monster[ n ].y,
                                    ALLEGRO_FLIP_HORIZONTAL );
                }
                else {
                    al_draw_bitmap( e_monster[ n ].img_atk[ nSubState ],
                                e_monster[ n ].x,
                                e_monster[ n ].y, 0);
                }
            }
        }
    }
//    printf( "character_draw2\n" );
}

void monster_destroy( void )
{
//    printf( "character_destory2\n" );
    // bit map
    for( int i = 0; i < g_nMonsterCount; i++ ) {
        for( int j = 0; j < 2; j++ ) {
            al_destroy_bitmap( e_monster[ i ].img_move[ j ] );
        }
        for( int j = 0; j < 2; j++ ) {
            al_destroy_bitmap( e_monster[ i ].img_atk[ j ] );
        }
        if( e_monster[ i ].type == ESA_FIRE ) {
            for( int j = 0; j < 2; j++ ) {
                al_destroy_bitmap( e_monster[ i ].img_fire[ j ] );
            }
        }
    }
    printf( "monster destroy success!\n" );
//    printf( "character_destory2\n" );
}

bool isCharacterAlive( void )
{
    if( e_pchara == NULL ) {
        return false;
    }
    return e_pchara->hp > 0;
}
