#include <assert.h>
#include "charater.h"

#define MAX_COUNTOF_CONTINUOUS_JUMP ( 2 )
#define JUMP_VY ( -90 )
#define MONSTER_NUMBERS ( 4 )

ALLEGRO_SAMPLE *sample = NULL;

float g_CameraPosition = 0.0;
ALLEGRO_TRANSFORM camera;
int g_nTerrainWidth = 0;

float g_Gravity = 9.8;
float g_Tick = 0.2;

int g_nImgMoveWidth[ ESA_NUM ];
int g_nImgMoveHeight[ ESA_NUM ];
int g_nImgTransformWidth;
int g_nImgTransformHeight;
int g_nImgInhaleWidth[ 2 * ESA_NUM ];
int g_nImgInhaleHeight[ 2 * ESA_NUM ];

ECharacterState g_LastState;
int g_nLastSubState;

bool g_bCDing = false;
int g_nCDCursor = 0;
int g_nCDTime = 50;

void CameraUpdate( float *CamPosition, int x, int y, int width, int height )
{
//    printf( "CameraUpdate\n" );
    *CamPosition = -( WIDTH / 2 ) + ( x + width / 2 );

    if( *CamPosition < 0 ) {
        *CamPosition = 0.0;
    }
    else if( *CamPosition >= ( g_nTerrainWidth - WIDTH ) ) {
        *CamPosition = ( float )( g_nTerrainWidth - WIDTH);
    }
//    printf( "CameraUpdate\n" );
}

void character_init( const int nTerrainWidth ){
//    printf( "character_init\n" );

    // load character images
    for(int i = 1 ; i <= 4; i++){
        char temp[50];
        sprintf( temp, "./image/char_move%d.png", i );
        e_pchara->img_move[i-1] = al_load_bitmap(temp);
        assert( e_pchara->img_move[i-1] != NULL );
    }
    // inhale
    for(int i = 1 ; i <= 2; i++){
        char temp[ 50 ];
        sprintf( temp, "./image/char_inhale2%d.png", i );
        e_pchara->img_inhale[ i - 1 ] = al_load_bitmap( temp );
        assert( e_pchara->img_inhale[ i - 1 ] != NULL );
        g_nImgInhaleWidth[ i - 1 ] = al_get_bitmap_width( e_pchara->img_inhale[ i - 1 ] );
        g_nImgInhaleHeight[ i - 1 ] = al_get_bitmap_height( e_pchara->img_inhale[ i - 1 ] );
    }

    for( int i = 0; i < NUMOF_TRANSFORM_IMG; i++ ) {
        char temp[ 50 ];
        sprintf( temp, "./image/char_to_sword%d.png", i + 1 );
        e_pchara->img_transform[ i ] = al_load_bitmap( temp );
        assert( e_pchara->img_transform[ i ] != NULL );
    }
    // load effective sound
    sample = al_load_sample("./sound/inhale.wav");
    e_pchara->atk_Sound  = al_create_sample_instance(sample);
    al_set_sample_instance_playmode(e_pchara->atk_Sound, ALLEGRO_PLAYMODE_ONCE);
    al_attach_sample_instance_to_mixer(e_pchara->atk_Sound, al_get_default_mixer());

    // initial the geometric information of character
    e_pchara->width = al_get_bitmap_width(e_pchara->img_move[0]);
    e_pchara->height = al_get_bitmap_height(e_pchara->img_move[0]);
    e_pchara->x = WIDTH/2-100; // todo: no magical number here!
    e_pchara->y = HEIGHT/2;
    e_pchara->dir = false;
    e_pchara->hp = 3;

    // initial the animation component
    e_pchara->state = ECS_STOP;
    e_pchara->nSubState = 0;
    e_pchara->anime = 0;
    e_pchara->anime_time = 20;
    e_pchara->nAtkCursor = 0;
    e_pchara->nAtkTime = 15;
    e_pchara->nInhaleCursor = 0;
    e_pchara->nInhaleTime = 10;

    e_pchara->x0 = e_pchara->x;
    // gravity
    e_pchara->y0 = e_pchara->y;
    e_pchara->vy = 0.0;
    e_pchara->FallingTick = 0.0;
    e_pchara->nJumpCount = 0;

    // terrain width
    g_nTerrainWidth = nTerrainWidth;

    // transform status
    e_pchara->nTransformCursor = 0;
    e_pchara->nTransformTime = 30;
    e_pchara->NowSpecialAtk = ESA_NORMAL;
    e_pchara->NextSpecailAtk = ESA_NORMAL;

//    printf( "test\n" );
    for( int i = 0; i < ESA_NUM; i++ ){
//        printf( "i = %d\n", i );
        switch( i ) {
        case ESA_NORMAL:
            for(int j = 0; j < 4; j++){
                char temp[ 50 ];
                sprintf( temp, "./image/char_move%d.png", j + 1 );
                e_pchara->img_SpecialAtk[ 4 * i + j ] = al_load_bitmap( temp );
                assert( e_pchara->img_SpecialAtk[ 4 * i + j ] != NULL );
            }
            break;

        case ESA_SWORD:
            for(int j = 0; j < 4; j++){
                char temp[ 50 ];
                sprintf( temp, "./image/char_sword_move%d.png", j + 1 );
                e_pchara->img_SpecialAtk[ 4 * i + j ] = al_load_bitmap( temp );
                assert( e_pchara->img_SpecialAtk[ 4 * i + j ] != NULL );
            }
            break;

        case ESA_BOMB:
            for(int j = 0; j < 4; j++){
                char temp[ 50 ];
                sprintf( temp, "./image/char_sword_move%d.png", j + 1 );
                e_pchara->img_SpecialAtk[ 4 * i + j ] = al_load_bitmap( temp );
                assert( e_pchara->img_SpecialAtk[ 4 * i + j ] != NULL );
            }
            break;

        case ESA_FIRE:
            for(int j = 0; j < 4; j++){
                char temp[ 50 ];
                sprintf( temp, "./image/char_sword_move%d.png", j + 1 );
                e_pchara->img_SpecialAtk[ 4 * i + j ] = al_load_bitmap( temp );
                assert( e_pchara->img_SpecialAtk[ 4 * i + j ] != NULL );
            }
            break;

        default:
            assert( false );
            break;
        }
//        printf( "i = %d\n", i );

        g_nImgMoveWidth[ i ] = al_get_bitmap_width( e_pchara->img_SpecialAtk[ 4 * i ] );
        g_nImgMoveHeight[ i ] = al_get_bitmap_height( e_pchara->img_SpecialAtk[ 4 * i ] );
    }
//    printf( "Character init success\n" );
//    printf( "character_init\n" );
}

void charater_process(ALLEGRO_EVENT event){
//    printf( "charater_process\n" );
    // process the animation
    if( event.type == ALLEGRO_EVENT_TIMER ){
        if( event.timer.source == fps ){

            if( g_bCDing == true ) {
                g_nCDCursor = ( g_nCDCursor + 1 ) % g_nCDTime;
                if( g_nCDCursor == 0 ) {
                    g_bCDing = false;
                }
            }

            if( e_pchara->state == ECS_MOVE ) {
                e_pchara->anime++;
                e_pchara->anime %= e_pchara->anime_time;

                if( e_pchara->anime == 0 ) {
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
        }
    // process the keyboard event
    }
    else if( event.type == ALLEGRO_EVENT_KEY_DOWN ) {
        key_state[event.keyboard.keycode] = true;
        e_pchara->anime = 0;
    }
    else if( event.type == ALLEGRO_EVENT_KEY_UP ) {
        key_state[event.keyboard.keycode] = false;
    }

//    printf( "charater_process\n" );
}

void charater_update(){
//    printf( "charater_update\n" );
    // use the idea of finite state machine to deal with different state
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
            e_pchara->x -= 3;
        }
        else if( key_state[ALLEGRO_KEY_D] ) {
            e_pchara->dir = true;
            e_pchara->x += 3;
        }
        e_pchara->state = ECS_MOVE;

    }else if( key_state[ALLEGRO_KEY_A] ){
        e_pchara->dir = false;
        e_pchara->x -= 3;
        e_pchara->state = ECS_MOVE;
    }else if( key_state[ALLEGRO_KEY_S] ){
        if(key_state[ALLEGRO_KEY_SPACE])
        {
            if( e_pchara->state != ECS_ATK) {
                //e_pchara->dir?e_pchara->x += 100: e_pchara->x-= 100;
                e_pchara->x0 = e_pchara->x;
                e_pchara->state = ECS_ATK;
            }
        }
        else
        {
            e_pchara->y += 10;
            e_pchara->state = ECS_MOVE;
        }
    }else if( key_state[ALLEGRO_KEY_D] ){
        e_pchara->dir = true;
        e_pchara->x += 3;
        e_pchara->state = ECS_MOVE;

    }
    // inhale
    else if( key_state[ ALLEGRO_KEY_SPACE ] ) {   //吸的技能
        if( g_bCDing == false && e_pchara->state != ECS_INHALE && e_pchara->state != ECS_TRANSFORMING ) {
            e_pchara->nInhaleCursor = 0;
            e_pchara->state = ECS_INHALE;
            e_pchara->x0 = e_pchara->x;
        }
    }
    else if( e_pchara->anime == e_pchara->anime_time - 1 ) {
        e_pchara->anime = 0;
        e_pchara->state = ECS_STOP;
    }else if ( e_pchara->anime == 0 ){
        e_pchara->state = ECS_STOP;
    }
//    printf( "charater_update\n" );
}

void character_gravity( int nGroundY ) {
//    printf( "character_gravity\n" );
    if( e_pchara->state == ECS_MOVE || e_pchara->state == ECS_STOP ) {
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
//    printf( "character_gravity\n" );
}

void character_BoundryCheck( void ) {
//    printf( "character_BoundryCheck\n" );
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

bool character_checkOverlap( const Position *plhs, const Position *prhs ) {
//    printf( "character_checkOverlap\n" );
    if( ( plhs->w < prhs->w && plhs->e < prhs->w ) ||
        ( plhs->w > prhs->e && plhs->e > prhs->e ) ) {
//            printf( "character_checkOverlap\n" );
            return false;
    }

    if( ( plhs->n < prhs->n && plhs->s < prhs->n ) ||
        ( plhs->n > prhs->s && plhs->s > prhs->s ) ) {
//            printf( "character_checkOverlap\n" );
            return false;
    }

//    printf( "character_checkOverlap\n" );
    return true;
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

    for( int i = 0; i < MONSTER_NUMBERS; i++ ) {
        if( e_monster[ i ].state == EMS_ALIVE ) {
            // given monster position
            Position MonsterPos;
            MonsterPos.e = e_monster[ i ].x + e_monster[ i ].width;
            MonsterPos.w = e_monster[ i ].x;
            MonsterPos.n = e_monster[ i ].y;
            MonsterPos.s = e_monster[ i ].y + e_monster[ i ].height;

            if( character_checkOverlap( &CharacterPos, &MonsterPos ) == true ) {
                printf( "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n" );
                e_pchara->state = ECS_TRANSFORMING;
                e_pchara->nTransformCursor = 0;
                e_pchara->NextSpecailAtk = ESA_SWORD;

                e_pchara->nInhaleCursor = 0;

                e_monster[ i ].state = EMS_DIE;
                e_monster[ i ].hp = 0;
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

    // update substate
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
            // update image
            for( int i = 0; i < 4; i++ ) {
                e_pchara->img_move[ i ] = e_pchara->img_SpecialAtk[ 4 * e_pchara->NowSpecialAtk + i ];
            }
        }

        if( e_pchara->anime < e_pchara->anime_time / 4 ) {
            e_pchara->nSubState = 0;
        }
        else if( e_pchara->anime_time / 4 <= e_pchara->anime && e_pchara->anime < e_pchara->anime_time / 2) {
            e_pchara->nSubState = 1;
        }
        else if( e_pchara->anime_time / 2 <= e_pchara->anime && e_pchara->anime < e_pchara->anime_time * 3 / 4 ) {
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
        else{
            e_pchara->nSubState = 2;
        }

        //assert( false );
        break;

    default:
        break;
    }

    // update xy
    switch( NewState ) {
    case ECS_STOP:
        // change state
        if( g_LastState != NewState ) {
            int nHeight = al_get_bitmap_height( e_pchara->img_move[ 0 ] );
            e_pchara->y -= nHeight - e_pchara->height;
            e_pchara->height = nHeight;
        }
        printf( "New height is %d\n", e_pchara->height );
        break;
    case ECS_MOVE:
        // change state
        if( g_LastState != NewState ) {
            int nHeight = al_get_bitmap_height( e_pchara->img_move[ 0 ] );
            e_pchara->y -= nHeight - e_pchara->height;
            e_pchara->height = nHeight;
        }
        // change substate
        if( g_LastState == NewState && g_nLastSubState != e_pchara->nSubState ) {
            int nHeight = al_get_bitmap_height( e_pchara->img_move[ e_pchara->nSubState ] );
            e_pchara->y -= nHeight - e_pchara->height;
            e_pchara->height = nHeight;
        }
        break;

    case ECS_INHALE:
        // change state
        if( g_LastState != NewState ) {
            int nHeight = al_get_bitmap_height( e_pchara->img_inhale[ 0 ] );
            e_pchara->y -= nHeight - e_pchara->height;
            e_pchara->height = nHeight;
        }

        // change substate
        if( g_LastState == NewState && g_nLastSubState != e_pchara->nSubState ) {
            int nHeight = al_get_bitmap_height( e_pchara->img_inhale[ e_pchara->nSubState ] );
            e_pchara->y -= nHeight - e_pchara->height;
            e_pchara->height = nHeight;
        }
//        printf( "New height is %d\n", e_pchara->height );
        break;

    case ECS_TRANSFORMING:
        // change state
        if( g_LastState != NewState ) {
            int nHeight = al_get_bitmap_height( e_pchara->img_transform[ 0 ] );
            e_pchara->y -= nHeight - e_pchara->height;
            e_pchara->height = nHeight;
        }
        // change substate
        if( g_LastState == NewState && g_nLastSubState != e_pchara->nSubState ) {
            int nHeight = al_get_bitmap_height( e_pchara->img_transform[ e_pchara->nSubState ] );
            e_pchara->y -= nHeight - e_pchara->height;
            e_pchara->height = nHeight;
        }
//        printf( "New height is %d\n", e_pchara->height );

    default:
        //assert( false );
        break;
    }


    g_LastState = NewState;
    g_nLastSubState = e_pchara->nSubState;
    printf( "State = %d, SubState = %d\n", g_LastState, g_nLastSubState );
    printf( "Character at x = %d, y = %d\n", e_pchara->x, e_pchara->y );
//    printf( "character_StateChangeImage\n" );
}

void character_attack( void )
{
//    printf( "character_attack\n" );
    if( g_bCDing == true ) {
//        printf( "character_attack\n" );
        return;
    }

    int nDeltaX = 0;
    switch( e_pchara->state ) {
    case ECS_ATK:
        nDeltaX = ( e_pchara->dir ? 1 : -1 ) * e_pchara->nAtkCursor * 5;
        e_pchara->x = e_pchara->x0 + nDeltaX;
        break;

    case ECS_INHALE:
        nDeltaX = ( e_pchara->dir ? 1 : -1 ) * e_pchara->nInhaleCursor * 10;
        e_pchara->x = e_pchara->x0 + nDeltaX;
        break;

    case ECS_STOP:
    default:
        // do nothing
        break;
    }
//    printf( "character_attack\n" );
}

void character_draw(){
//    printf( "character_draw\n" );
    CameraUpdate( &g_CameraPosition, e_pchara->x, e_pchara->y, e_pchara->width, e_pchara->height );
    al_identity_transform( &camera );
    al_translate_transform( &camera, -g_CameraPosition, 0 );
    al_use_transform( &camera );

    int nGroundY = FindAndDrawClosestGroundY( e_pchara->x + e_pchara->width / 2, e_pchara->y + e_pchara->height );

    character_gravity( nGroundY );

    character_BoundryCheck();

    // check transform
    character_checkTransform();

    character_attack();

    // check monster alive?
    charater_update2();

    character_StateChangeImage();

    printf( "character_draw\n" );

    // with the state, draw corresponding image
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
            if( e_pchara->nSubState == 1 ) {
                al_play_sample_instance(e_pchara->atk_Sound);
            }
        }
        else {
            al_draw_bitmap( e_pchara->img_inhale[ e_pchara->nSubState ], e_pchara->x, e_pchara->y, 0 );
            if( e_pchara->nSubState == 1 ) {
                al_play_sample_instance(e_pchara->atk_Sound);
            }
        }
        break;

    case ECS_TRANSFORMING:
        if( e_pchara->dir ) {
            al_draw_bitmap( e_pchara->img_transform[ e_pchara->nSubState ], e_pchara->x, e_pchara->y, ALLEGRO_FLIP_HORIZONTAL );
        }
        else {
            al_draw_bitmap( e_pchara->img_transform[ e_pchara->nSubState ], e_pchara->x, e_pchara->y, 0 );
        }
        break;

    default:
        assert( false );
        break;
    } // end of switch
    printf( "character_draw\n" );
}
void character_destory(){
//    printf( "character_destory\n" );
   // al_destroy_bitmap(e_pchara->img_atk[0]);
   // al_destroy_bitmap(e_pchara->img_atk[1]);
    al_destroy_sample_instance(e_pchara->atk_Sound);

    // load character images
    for( int i = 0; i < 4; i++ ) {
        al_destroy_bitmap( e_pchara->img_move[ i ] );
    }
    // inhale
    for( int i = 0; i < 2; i++ ) {
        al_destroy_bitmap( e_pchara->img_inhale[ i ] );
    }

    for( int i = 0; i < 2; i++ ) {
        al_destroy_bitmap( e_pchara->img_transform[ i ] );
    }
//    printf( "character_destory\n" );
}

//==============================================================================================

void character_init2(){
//    printf( "character_init2\n" );

    FILE* fp = fopen( "./res/monster.txt", "r" );
    int nPosX = 0;
    int nPosY = 0;
    int nset= 1;
    while( fscanf( fp, "%d%d", &nPosX, &nPosY ) != EOF && nset < MONSTER_NUMBERS ) {
        e_monster[ nset ].x = nPosX;
        e_monster[ nset ].y = nPosY;
        nset++;
    }
    fclose( fp ); //記得關檔

    for( int n = 0; n < MONSTER_NUMBERS; n++ ) {
        char temp[ 50 ];
        sprintf( temp, "./image/char_move1.png");
        e_monster[ n ].img_move[ 0 ] = al_load_bitmap( temp );
        assert( e_monster[ n ].img_move[ 0 ] != NULL );

        e_monster[ n ].width = al_get_bitmap_width( e_monster[ n ].img_move[ 0 ] );
        e_monster[ n ].height = al_get_bitmap_height( e_monster[ n ].img_move[ 0 ] );
        /*e_monster[ n ].x = WIDTH/2;
        e_monster[ n ].y = HEIGHT/2+40;*/
        e_monster[ n ].dir = true;
        e_monster[ n ].hp = 2;
        e_monster[ n ].state = EMS_ALIVE;

        e_monster[ n ].anime = 0;
        e_monster[ n ].anime_time = 30;

        // gravity
        e_monster[ n ].y0 = e_monster[ n ].y;
        e_monster[ n ].vy = 0.0;
        e_monster[ n ].FallingTick = 0.0;
    }
//    printf( "character_init2\n" );
}

void charater_process2(ALLEGRO_EVENT event){
//    printf( "charater_process2\n" );
    // process the animation                //1.處理動畫
    if( event.type == ALLEGRO_EVENT_TIMER ){ //根據fps+anime
        if( event.timer.source == fps ){
            for( int n = 0; n < MONSTER_NUMBERS; n++ ){
                e_monster[ n ].anime++;
                e_monster[ n ].anime %= e_monster[ n ].anime_time; //讓我們知道現在跑道time的哪一步
            }
        }
    }
//    printf( "charater_process2\n" );
}

void character_destory2()
{
//    printf( "character_destory2\n" );
    for( int n = 0; n < MONSTER_NUMBERS; n++ ) {
        if( e_monster[ n ].state == EMS_ALIVE ) {
            al_destroy_bitmap( e_monster[ n ].img_move[ 0 ] );
        }
    }
//    printf( "character_destory2\n" );
}

void charater_update2()
{
//    printf( "charater_update2\n" );
    for( int n = 0; n < MONSTER_NUMBERS; n++ ) {
        if( e_monster[ n ].state == EMS_ALIVE )
        {
            if( ( e_pchara->x - 60 <= e_monster[ n ].x ) && ( e_monster[ n ].x <= e_pchara->x + 60 ) &&
                ( e_pchara->y - 60 <= e_monster[ n ].y ) && ( e_monster[ n ].y <= e_pchara->y + 60 ) ) {
                e_monster[ n ].hp--;
            }

            if( e_monster[ n ].hp <= 0 ) {
                //如果死掉
                al_destroy_bitmap( e_monster[ n ].img_move[ 0 ]);
                e_monster[ n ].state = EMS_DIE;
            }
        }
    }
//    printf( "charater_update2\n" );
}

void monster_gravity( int nMonsterIdx, int nGroundY ) {
//    printf( "monster_gravity\n" );
    if( nGroundY == -1 ) {
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

void character_draw2(){
//    printf( "character_draw2\n" );
    for( int n = 0; n < MONSTER_NUMBERS; n++ ) {
    // with the state, draw corresponding image
        if( e_monster[ n ].state == EMS_ALIVE ) {

            int nGroundY = FindAndDrawClosestGroundY( e_monster[ n ].x + e_monster[ n ].width / 2, e_monster[ n ].y + e_monster[ n ].height );

            monster_gravity( n, nGroundY );
        //如果活著
            if( e_monster[ n ].dir ) {
                if( e_monster[ n ].x >= WIDTH / 2 + 50 ) {
                    e_monster[ n ].dir = false;
                    al_draw_bitmap( e_monster[ n ].img_move[ 0 ], e_monster[ n ].x, e_monster[ n ].y, 0 );
                }
                al_draw_bitmap( e_monster[ n ].img_move[ 0 ], e_monster[ n ].x, e_monster[ n ].y, ALLEGRO_FLIP_HORIZONTAL );
                e_monster[ n ].x += 1;
            }
            else {
                if( e_monster[ n ].x <= WIDTH / 2 ) {
                    e_monster[ n ].dir = true;
                    al_draw_bitmap( e_monster[ n ].img_move[ 0 ], e_monster[ n ].x, e_monster[ n ].y, ALLEGRO_FLIP_HORIZONTAL );
                }
                al_draw_bitmap( e_monster[ n ].img_move[ 0 ], e_monster[ n ].x, e_monster[ n ].y, 0 );
                e_monster[ n ].x -= 1;
            }
        }
    }
//    printf( "character_draw2\n" );
}



