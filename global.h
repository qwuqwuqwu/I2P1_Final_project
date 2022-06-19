#ifndef GLOBAL_H_INCLUDED
#define GLOBAL_H_INCLUDED
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_video.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#define GAME_TERMINATE ( -1 )
#define NUMOF_TRANSFORM_IMG ( 3 )
#define HP          ( 6 )
#define LIFE        ( 0 )


// note that you can't assign initial value here!
extern const float FPS;
extern const int WIDTH;
extern const int HEIGHT;
extern float g_Gravity;
extern float g_Tick;
extern bool key_state[ ALLEGRO_KEY_MAX ];
extern bool judge_next_window;
extern ALLEGRO_EVENT_QUEUE *event_queue;
extern ALLEGRO_TIMER *fps;
// the type of special attack
enum ESpecialAtk {
    ESA_NORMAL = 0,
    ESA_SWORD = 1,
    ESA_BOMB = 2,
    ESA_FIRE = 3,
    ESA_NUM = 4,
};

// the state of character
enum ECharacterState {
    ECS_STOP = 0,
    ECS_MOVE = 1,
    ECS_ATK = 2,
    ECS_INHALE = 3,
    ECS_SLIDE = 4,
    ECS_TRANSFORMING = 5,
	ECS_INJURED = 6,
};

// the orientation
enum EOrientationDirection {
    EOD_E = 0,
    EOD_S = 1,
    EOD_W = 2,
    EOD_N = 3,
    EOC_NUM = 4,
};

// the state of monster
enum EMonsterState {
    EMS_ALIVE = 0,
    EMS_DIE = 1,
    EMS_ATK = 2,
};

enum EMonsterType {
    EMT_NORMAL = 0,
    EMS_FRIE = 1,
    EMS_BOMB = 2,
};

enum EBlockType {
    EBT_GRASS = 1,
    EBT_CLOUD = 2,
    EBT_LONGCLOUD = 3,
    EBT_BRIDGE = 4,
    EBT_LONGBRIDGE = 5,
    EBT_STONE1 = 6,
    EBT_STONE2 = 7,
    EBT_WALL = 8,
};

enum EBombState {
    EBS_IDLE = 1,
    EBS_FLY = 2,
    EBS_EXPLODE = 3,
};

enum EFoodState {
    EFS_IDLE = 1,
    EFS_EATEN = 2,
};

typedef struct _Character {
	bool dir; // left: false, right: true
    int x, y; // the position of image
    int width, height; // the width and height of image
    int nMoveWidth;
    int nMoveHeight;
    int nMoveY;
    ECharacterState state; // the state of character
    int nSubState;
	int hp;
	int life;
    ESpecialAtk NowSpecialAtk;
    ESpecialAtk NextSpecailAtk;

    int nBombIdx;

	// cursor and time
    int nMoveCursor; 	    // counting the time of animation
    int nMoveTime; 		    // indicate how long the animation
    int nAtkCursor; 		// attack cursor
    int nAtkTime; 			// attack duration
    int nSlideCursor;		// slide cursor
    int nSlideTime;			// slide duration
    int nInhaleCursor; 		// inhale cursor
    int nInhaleTime; 		// inhale duration
	int nTransformCursor;	// transform cursor
    int nTransformTime;		// transform duration
    int nInjuredCursor;     // injured cursor
    int nInjuredTime;       // injured time

	// moving variable
    int x0;
    int nDeltaX;
    int y0;
    float vy;
    int nJumpCount;
    float FallingTick;

    int nSlideRate;
    int nAtkRate;
    float CamPos;

    // bit map
    ALLEGRO_BITMAP *img_inhale[ 2 ];

    ALLEGRO_BITMAP *img_atk[ 2 ];
    ALLEGRO_BITMAP *img_move[ 4 ];
    ALLEGRO_BITMAP *img_slide[ 2 ];
    ALLEGRO_BITMAP *img_transform[ 3 ];
    ALLEGRO_BITMAP *img_atkWord;
    ALLEGRO_BITMAP *img_fire[ 2 ];

    // bit map store
    ALLEGRO_BITMAP *img_store_SpecialAtk[ 2 * ESA_NUM ];
    ALLEGRO_BITMAP *img_store_move[ 4 * ESA_NUM ];
    ALLEGRO_BITMAP *img_store_slide[ 2 * ESA_NUM ];
    ALLEGRO_BITMAP *img_store_transform[ NUMOF_TRANSFORM_IMG * ESA_NUM ];
    ALLEGRO_BITMAP *img_store_AtkWord[ ESA_NUM ];
    ALLEGRO_BITMAP *img_store_HP[ HP + 1 ];

    ALLEGRO_SAMPLE_INSTANCE *atk_Sound;
} Character;

typedef struct _Mon {
    int x, y; // the position of image
    int lm, rm; // left most and right most of monster position
    int width, height; // the width and height of image
    bool dir; // left: false, right: true
    int state; // the state of character
    int type;
    int nAtkCursor; 		// attack cursor
    int nAtkTime; 			// attack duration
    int nAtkanime; 		// attack cursor
    int nAtkanime_time;
    ALLEGRO_BITMAP *img_move[ 2 ];
    ALLEGRO_BITMAP *img_atk[ 2 ];
    ALLEGRO_BITMAP *img_fire[ 2 ];
    int anime; // counting the time of animation
    int anime_time; // indicate how long the animation
    int nSubState;
    int hp;
    int nBombIdx;

    int y0;
    float vy;
    float FallingTick;
} Mon;

typedef struct _Bomb {
    int x, y; // the position of image
    int width, height; // the width and height of image
    bool dir; // left: false, right: true
    int nSubState;
    EBombState state; // the state of character
    ALLEGRO_BITMAP *img;
    ALLEGRO_BITMAP *img_explode[ 3 ];

    int nExplodeCursor;
    int nExplodeTime;

    int y0;
    int x0;
    float vy;
    float vx;
    float FallingTick;
} Bom;

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
extern ALLEGRO_SAMPLE_INSTANCE *g_pMenuSampleInstance;

#endif
