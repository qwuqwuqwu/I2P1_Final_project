#ifndef GLOBAL_H_INCLUDED
#define GLOBAL_H_INCLUDED
#define GAME_TERMINATE -1
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#define NUMOF_TRANSFORM_IMG ( 3 )

// note that you can't assign initial value here!
extern const float FPS;
extern const int WIDTH;
extern const int HEIGHT;
extern bool key_state[ALLEGRO_KEY_MAX];
extern bool judge_next_window;
extern ALLEGRO_EVENT_QUEUE *event_queue;
extern ALLEGRO_TIMER *fps;


enum ESpecialAtk {
    ESA_NORMAL = 0,
    ESA_SWORD = 1,
    ESA_BOMB = 2,
    ESA_FIRE = 3,
    ESA_NUM = 4
};

// the state of character
enum ECharacterState {
    ECS_STOP = 0,
    ECS_MOVE = 1,
    ECS_ATK = 2,
    ECS_INHALE = 3,
    ECS_TRANSFORMING = 4,
};

// the state of monster
enum EMonsterState {
    EMS_ALIVE = 0,
    EMS_DIE = 1,
};

typedef struct character
{
    int x, y; // the position of image
    int width, height; // the width and height of image
    bool dir; // left: false, right: true
    ECharacterState state; // the state of character
    int nSubState;
    ALLEGRO_BITMAP *img_move[ 4 ];
    ALLEGRO_BITMAP *img_inhale[ 2 ];
    ALLEGRO_BITMAP *img_atk[2];
    ALLEGRO_SAMPLE_INSTANCE *atk_Sound;
    int anime; // counting the time of animation
    int anime_time; // indicate how long the animation

    int nAtkCursor; // counting the time of animation
    int nAtkTime; // indicate how long the animation
    int nInhaleCursor; // counting the time of animation
    int nInhaleTime; // indicate how long the animation
    int x0;
    int y0;
    float vy;
    int nJumpCount;
    float FallingTick;
    int hp;
    int nTransformCursor;
    int nTransformTime;
    ALLEGRO_BITMAP *img_transform[ NUMOF_TRANSFORM_IMG ];

    ESpecialAtk NowSpecialAtk;
    ESpecialAtk NextSpecailAtk;
    ALLEGRO_BITMAP *img_SpecialAtk[ 4 * ESA_NUM ];
} Character;

typedef struct mon
{
    int x, y; // the position of image
    int width, height; // the width and height of image
    bool dir; // left: false, right: true
    int state; // the state of character
    ALLEGRO_BITMAP *img_move[1];
    //ALLEGRO_BITMAP *img_atk[2];
    //ALLEGRO_SAMPLE_INSTANCE *atk_Sound;
    int anime; // counting the time of animation
    int anime_time; // indicate how long the animation
    int hp;
    int amidie;

    int y0;
    float vy;
    float FallingTick;
} Mon;

// structure to represent ground
typedef struct t_pos {
    int state;
    int type;
    int x;
    int y;
    int nWidth;
    int nHeight;
    ALLEGRO_BITMAP *img;
} Pos;

typedef struct _Position {
    int w;
    int e;
    int n;
    int s;
} Position;

extern Character *e_pchara;
extern Mon e_monster[ 100 ];
#endif
