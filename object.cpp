#include "object.h"

#define DEBUGPRINT_SCENE            ( false )

#define MAX_COUNTOF_FOUNDGROUND     ( 40 )
#define MAX_COUNTOF_GROUND          ( 600 )

// the state of character
enum {NORMAL = 0, HIGHLIGHT};

Pos g_Ground[ MAX_COUNTOF_GROUND ];
int g_nGroundCount = 0;

ALLEGRO_BITMAP *Ground[ MAX_COUNTOF_GROUND ] = { NULL };
ALLEGRO_BITMAP *Ground2[ MAX_COUNTOF_GROUND ] = { NULL };

int FindAndDrawClosestGroundY( void ){
    // find closest lower ground
    int nFoundGroundIdx[ MAX_COUNTOF_FOUNDGROUND ] = { 0 };
    int nFoundGroundCount = 0;
    FindGround( nFoundGroundIdx, &nFoundGroundCount, e_pchara->x + e_pchara->width / 2 );

    int nClosestGroundIdx = -1;
    int nGroundY = -1;
    nClosestGroundIdx = FindClosestGround( nFoundGroundIdx, nFoundGroundCount, e_pchara->x + e_pchara->width / 2, e_pchara->y + e_pchara->height );
    if( nClosestGroundIdx != -1 ) {
        nGroundY = g_Ground[ nClosestGroundIdx ].y;
        // draw the closest lower ground
        al_draw_bitmap( Ground2[ nClosestGroundIdx ], g_Ground[ nClosestGroundIdx ].x, g_Ground[ nClosestGroundIdx ].y, 0 );
    }

    if( DEBUGPRINT_SCENE ) {
        printf( "chara x = %d, y = %d\n", e_pchara->x, e_pchara->y );
        for( int i = 0; i < nFoundGroundCount; i++ ) {
            printf( "*************************************Ground %d: x = %d, y = %d, width = %d, height = %d\n",
                    i, g_Ground[ nFoundGroundIdx[ i ] ].x, g_Ground[ nFoundGroundIdx[ i ] ].y,
                    g_Ground[ nFoundGroundIdx[ i ] ].nWidth, g_Ground[ nFoundGroundIdx[ i ] ].nHeight );
        }
    }
    return nGroundY;
}

int FindClosestGround( int *pGroundIdx, int nGroundCount, const int x, const int nToe ) {
    int nMinY = INT_MAX;
    int nClosestGroundIdx = -1;
    for( int i = 0; i < nGroundCount; i++ ) {
        if( ( g_Ground[ pGroundIdx[ i ] ].x <= x ) && ( x <= ( g_Ground[ pGroundIdx[ i ] ].x + g_Ground[ pGroundIdx[ i ] ].nWidth ) ) ) {
            if( g_Ground[ pGroundIdx[ i ] ].y >= nToe && g_Ground[ pGroundIdx[ i ] ].y < nMinY ) {
                nMinY = g_Ground[ pGroundIdx[ i ] ].y;
                nClosestGroundIdx = pGroundIdx[ i ];
            }
        }
    }
    return nClosestGroundIdx;
}

bool IsInGroundRange( int nGroundIdx, const int x ) {
    if( ( g_Ground[ nGroundIdx ].x <= x ) && ( x <= ( g_Ground[ nGroundIdx ].x + g_Ground[ nGroundIdx ].nWidth ) ) ) {
        return true;
    }
    return false;
}

void FindGround( int *pnFoundGroundIdx, int *pnFoundCount, int x ) {
    int j = 0;
    for( int i = 0; i < g_nGroundCount; i++ ) {
        if( IsInGroundRange( i, x ) == true ) {
            pnFoundGroundIdx[ j++ ] = i;
            *pnFoundCount = j;
        }
    }
}

int CompareGround( const void *plhs, const void *prhs ) {
    Pos *pLHS = ( Pos * )plhs;
    Pos *pRHS = ( Pos * )prhs;
    if( pLHS->x < pRHS->x ) {
        return -1;
    }
    else if( pLHS->x > pRHS->x ) {
        return 1;
    }
    // pLHS->x == pRHS->x
    else {
        if( pLHS->y < pRHS->y ) {
            return -1;
        }
        else if( pLHS->y > pRHS->y ) {
            return 1;
        }
        else {
            return 0;
        }
    }
}

void GroundSetup( void )
{
    FILE* fp = fopen( "./res/ground.txt", "r" );
    int nType = 0;
    int nPosX = 0;
    int nPosY = 0;
    while( fscanf( fp, "%d%d%d", &nType, &nPosX, &nPosY ) != EOF && g_nGroundCount <= MAX_COUNTOF_GROUND ) {
        g_Ground[ g_nGroundCount ].type = NORMAL;
        g_Ground[ g_nGroundCount ].type = nType;
        g_Ground[ g_nGroundCount ].x = nPosX;
        g_Ground[ g_nGroundCount ].y = nPosY;
        g_nGroundCount++;
    }

    // sort ground
    qsort( g_Ground, g_nGroundCount, sizeof( Pos ), CompareGround );

    for( int i = 0; i < g_nGroundCount; i++ ) {
        if( g_Ground[ i ].type == 1 ) {
            Ground[ i ] = al_load_bitmap("./image/ground100_10.png");
            Ground2[ i ] = al_load_bitmap("./image/ground_red_100_10.png");
        }
        else if( g_Ground[ i ].type == 2 ) {
            Ground[ i ] = al_load_bitmap("./image/ground50_10.png");
            Ground2[ i ] = al_load_bitmap("./image/ground_red_50_10.png");
        }
        else if( g_Ground[ i ].type == 3 ) {
            Ground[ i ] = al_load_bitmap("./image/bridge.png");
            Ground2[ i ] = al_load_bitmap("./image/ground_red_50_10.png");
        }

        g_Ground[ i ].nWidth = al_get_bitmap_width( Ground[ i ] );
        g_Ground[ i ].nHeight = al_get_bitmap_height( Ground[ i ] );
    }

    // release the file handler
    fclose( fp );
}

void object_init( void ){
    GroundSetup();
}

void object_draw( void ){
    // plot ground
    for( int i = 0; i < g_nGroundCount; i++ ) {
       al_draw_bitmap( Ground[ i ], g_Ground[ i ].x, g_Ground[ i ].y, 0 );
    }
}

void object_destroy( void ){
    // ground
    for( int i = 0; i < g_nGroundCount; i++ ) {
        al_destroy_bitmap( Ground[ i ] );
        al_destroy_bitmap( Ground2[ i ] );
    }
}

