#include "GameWindow.h"

bool g_bDraw = false;
int g_nWindow = 1;

const char *title = "Final Project Team-4";

// ALLEGRO Variables
ALLEGRO_SAMPLE_INSTANCE *g_pMenuSampleInstance = NULL, *DeathSampleInstance = NULL, *OverSampleInstance = NULL;
ALLEGRO_DISPLAY *g_pDisplay = NULL;
ALLEGRO_SAMPLE *g_pMenuSample = NULL, *DeathSample = NULL, *OverSample = NULL;
ALLEGRO_VIDEO *g_pVideo = NULL;
ALLEGRO_EVENT_QUEUE *g_pVideoQueue = NULL;
ALLEGRO_BITMAP *img_over;

// TODO
// 1. release g_pMenuSample?
// 2. release g_pMenuSampleInstance?

int Game_establish( void )
{
    int nMsg = 0;

    game_init();
    game_begin();

    while( nMsg != GAME_TERMINATE ) {
        nMsg = game_run();
        if( nMsg == GAME_TERMINATE ) {
			printf( "Game Over\n" );
		}
    }

    game_destroy();
    return 0;
}

void game_init( void )
{
    printf( "Game Initializing...\n" );

	// init allegro
    al_init();
    // init audio
    al_install_audio();
    al_init_acodec_addon();
    // Create display
    g_pDisplay = al_create_display( WIDTH, HEIGHT );
    // create event queue
    event_queue = al_create_event_queue();
    // Initialize Allegro settings
    al_set_window_position( g_pDisplay, 640, 280 );
    al_set_window_title( g_pDisplay, title );
    al_init_primitives_addon();
    al_init_font_addon(); // initialize the font addon
    al_init_ttf_addon(); // initialize the ttf (True Type Font) addon
    al_init_image_addon(); // initialize the image addon
    al_init_acodec_addon(); // initialize acodec addon
    al_install_keyboard(); // install keyboard event
    al_install_mouse();    // install mouse event
    al_install_audio();    // install audio event

    // create timer
    fps  = al_create_timer( 1.0 / FPS );

    // initialize the icon on the display
    ALLEGRO_BITMAP *pIcon = al_load_bitmap( "./image/icon.jpg" );
    assert( pIcon != NULL );
    al_set_display_icon( g_pDisplay, pIcon );

	// new e_pchara
    e_pchara = ( Character * )malloc( sizeof( Character ) );
}

void init_video( void )
{
    al_init_video_addon();

    //al_set_new_display_flags(ALLEGRO_RESIZABLE);
    //al_set_new_display_option(ALLEGRO_VSYNC, 1, ALLEGRO_SUGGEST);
    //screen = al_create_display( 640 , 480 );

    // linear interpolation for scaling images
    al_set_new_bitmap_flags( ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR );
    const char *pfilename = "intro.ogv";

    printf( "reading video.....\n" );
    g_pVideo = al_open_video( pfilename );
    if( g_pVideo ) {
        printf( "read video succeed\n" );
	}
    else {
        printf( "read video fail!!!!\n" );
		assert( false );
	}
    g_pVideoQueue = al_create_event_queue();

    // register video event
    ALLEGRO_EVENT_SOURCE *pTemp = al_get_video_event_source( g_pVideo );
    al_register_event_source( g_pVideoQueue, pTemp );
    al_register_event_source( g_pVideoQueue, al_get_display_event_source( g_pDisplay ) );
    al_register_event_source( g_pVideoQueue, al_get_keyboard_event_source() );
    al_register_event_source( g_pVideoQueue, al_get_timer_event_source( fps ) );
}

void video_display( void )
{
    ALLEGRO_BITMAP *pframe = al_get_video_frame( g_pVideo );
    // check if we get the frame successfully
    // Note the this code is necessary
    if( pframe == NULL ) {
        return;
	}

    // Display the frame.
    // rescale the frame into the size of screen
    al_draw_scaled_bitmap(	pframe,
							// the rescale position of the frame
							107, 0,
							// the width and height you want to rescale
							al_get_bitmap_width( pframe )-214,
							al_get_bitmap_height( pframe ),
							// the position of result image
							0, 0,
							// the width and height of result image
							al_get_display_width( g_pDisplay ),
							al_get_display_height( g_pDisplay ), 0 );
    al_flip_display();
}

void video_begin( void )
{
    al_reserve_samples( 1 );
    al_start_video( g_pVideo, al_get_default_mixer() );
    al_start_timer( fps );
}

void game_begin( void )
{
	printf( "Intro video...\n" );
    // intro video
    init_video();
    video_begin();

    while( true ){
		ALLEGRO_EVENT event;
        al_wait_for_event( g_pVideoQueue, &event );
        if( event.type == ALLEGRO_EVENT_TIMER ) {
            video_display();
        }
		else if( event.type == ALLEGRO_EVENT_DISPLAY_CLOSE ) {
            al_close_video( g_pVideo );
            break;
        }
		else if( event.type == ALLEGRO_EVENT_VIDEO_FINISHED ) {
            al_close_video( g_pVideo );
            break;
        }
        else if( event.type == ALLEGRO_EVENT_KEY_UP ) {
            if( event.keyboard.keycode == ALLEGRO_KEY_SPACE ) {
                al_close_video( g_pVideo );
                break;
            }
        }
    }
    al_stop_timer( fps );
    al_destroy_event_queue( g_pVideoQueue );

    // register event of game g_nWindow
    al_register_event_source( event_queue, al_get_display_event_source( g_pDisplay ) );
    al_register_event_source( event_queue, al_get_keyboard_event_source() );
    al_register_event_source( event_queue, al_get_mouse_event_source() );
    al_register_event_source( event_queue, al_get_timer_event_source( fps ) ) ;
    al_start_timer( fps );

    // Load sound
    g_pMenuSample = al_load_sample( "./sound/overworld.mp3" );
    al_reserve_samples( 20 );
    g_pMenuSampleInstance = al_create_sample_instance( g_pMenuSample );
    // Loop the song until the display closes
    al_set_sample_instance_playmode( g_pMenuSampleInstance, ALLEGRO_PLAYMODE_LOOP );
    al_restore_default_mixer();
    al_attach_sample_instance_to_mixer( g_pMenuSampleInstance, al_get_default_mixer() );
    // set the volume of instance
    al_set_sample_instance_gain( g_pMenuSampleInstance, 0.8 );
    al_play_sample_instance( g_pMenuSampleInstance );

    DeathSample = al_load_sample( "./sound/death.wav" );
    DeathSampleInstance = al_create_sample_instance( DeathSample );
    al_set_sample_instance_playmode( DeathSampleInstance, ALLEGRO_PLAYMODE_ONCE );
    al_attach_sample_instance_to_mixer( DeathSampleInstance, al_get_default_mixer() );

    OverSample = al_load_sample( "./sound/game_over.mp3" );
    OverSampleInstance = al_create_sample_instance( OverSample );
    al_set_sample_instance_playmode( OverSampleInstance, ALLEGRO_PLAYMODE_ONCE );
    al_attach_sample_instance_to_mixer( OverSampleInstance, al_get_default_mixer() );
    //al_start_timer(fps);

    // initialize the menu before entering the loop
    menu_init();
}

void game_update( void )
{
    if( judge_next_window == true ) {
        if( g_nWindow == 1 ) {
            // not back menu anymore, therefore destroy it
            menu_destroy();
            // initialize next scene
            game_scene_init();
            judge_next_window = false;
            g_nWindow = 2;
        }
    }
    if( g_nWindow == 2 && e_pchara->hp >= 1 ){
        charater_update();
        charater_update2();
    }
    else if( g_nWindow == 2 && e_pchara->hp < 1 ) {
        al_stop_sample_instance( g_pMenuSampleInstance );
        al_set_sample_instance_gain( DeathSampleInstance, 1 );
        al_play_sample_instance( DeathSampleInstance );
        al_rest(4);

        if( e_pchara->life >= 1){

            //to be fetched

        }
        else{
            g_nWindow = 3;
        }
    }
    else if( g_nWindow == 3 ){
        al_set_sample_instance_gain( OverSampleInstance, 1 );
        al_play_sample_instance( OverSampleInstance );
        img_over = al_load_bitmap("./image/over.png");
        al_draw_bitmap(img_over, e_pchara->CamPos, 0, 0);
        al_flip_display();
        assert( img_over != NULL );
        al_rest(4);
        game_destroy();
    }
}

int process_event( void )
{
    // Request the event
    ALLEGRO_EVENT event;
    al_wait_for_event( event_queue, &event );

    // process the event of other component
    if( g_nWindow == 1 ) {
        menu_process( event );
    }
	else if( g_nWindow == 2 ) {
        charater_process( event );
        charater_process2( event );
    }


    // Shutdown our program
    if( event.type == ALLEGRO_EVENT_DISPLAY_CLOSE ) {
        return GAME_TERMINATE;
	}
    else if( event.type == ALLEGRO_EVENT_TIMER ) {
        if( event.timer.source == fps ) {
            g_bDraw = true;
		}
	}
    if( g_bDraw ) {
		game_update();
	}
    return 0;
}

void game_draw( void )
{
    if( g_nWindow == 1 ) {
        menu_draw();
    }
	else if( g_nWindow == 2 ) {
        game_scene_draw();
    }
    al_flip_display();
}

int game_run( void )
{
    int nError = 0;
    if( g_bDraw ) {
        game_draw();
        g_bDraw = false;
    }

    if( al_is_event_queue_empty( event_queue ) == false ) {
		nError = process_event();
    }

    return nError;
}

void game_destroy( void )
{
    // Make sure you destroy all things
    free( e_pchara );

    al_destroy_event_queue( event_queue );
    al_destroy_display( g_pDisplay );
    game_scene_destroy();
}




