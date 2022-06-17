#include "global.h"

// variables for global usage
const float FPS = 60.0;
const int WIDTH = 640;
const int HEIGHT = 480;
bool key_state[ ALLEGRO_KEY_MAX ] = { false };
bool judge_next_window = false;
ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_TIMER *fps = NULL;
Character *e_pchara = NULL;
Mon e_monster[ 100 ];
