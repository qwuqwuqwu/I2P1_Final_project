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

// note that you can't assign initial value here!
extern const float FPS;
extern const int WIDTH;
extern const int HEIGHT;
extern bool key_state[ALLEGRO_KEY_MAX];
extern bool judge_next_window;
extern ALLEGRO_EVENT_QUEUE *event_queue;
extern ALLEGRO_TIMER *fps;

enum ETransformStatus {
    ETS_NORMAL = 0,
    ETS_TRANSFORMING = 1,
    ETS_TRANSFORMED = 2,
    ETS_NUM = 3
};

enum ESpecialAtk {
    ESA_NORMAL = 0,
    ESA_SWORD = 1,
    ESA_BOMB = 2,
    ESA_FIRE = 3,
    ESA_NUM = 4
};

typedef struct character
{
    int x, y; // the position of image
    int width, height; // the width and height of image
    bool dir; // left: false, right: true
    int state; // the state of character
    ALLEGRO_BITMAP *img_move[2];
    ALLEGRO_BITMAP *img_atk[2];
    ALLEGRO_SAMPLE_INSTANCE *atk_Sound;
    int anime; // counting the time of animation
    int anime_time; // indicate how long the animation
    int y0;
    float vy;
    int nJumpCount;
    float FallingTick;
    int hp;
    ETransformStatus TransformStatus;
    int nTransformCursor;
    int nTransformTime;
    ALLEGRO_BITMAP *img_transform[ 2 ];

    ESpecialAtk NowSpecialAtk;
    ESpecialAtk NextSpecailAtk;
    ALLEGRO_BITMAP *img_SpecialAtk[ 2 * ESA_NUM ];
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

extern Character *e_pchara;
extern Mon *monster;
#endif
