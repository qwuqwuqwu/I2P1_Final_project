#include "object.h"

#define DEBUGPRINT_SCENE            ( false )

#define MAX_COUNTOF_FOUNDGROUND     ( 40 )
#define MAX_COUNTOF_GROUND          ( 600 )

// the state of object
enum {NORMAL = 0, HIGHLIGHT};

Pos g_Ground[ MAX_COUNTOF_GROUND ];
int g_nGroundCount = 0;

ALLEGRO_BITMAP *Ground[ MAX_COUNTOF_GROUND ] = { NULL };

bool CheckOverlap( const Position *plhs, const Position *prhs ) {
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

int CompSide( const void * lhs, const void *rhs )
{
    int *LHS = ( int * )lhs;
    int *RHS = ( int * )rhs;
    if( *LHS < *RHS ) {
        return -1;
    }
    else if( *LHS > *RHS ) {
        return 1;
    }
    else {
        return 0;
    }
}

bool AdvCheckOverlap( const Position *plhs, const Position *prhs, EOrientationDirection *pOrientation, int *pClampValue ) {
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

    // vertical
    int nNS[ 4 ] = { plhs->n, plhs->s, prhs->n, prhs->s };
    qsort( nNS, 4, sizeof( int ), CompSide );

    // horizontal
    int nWE[ 4 ] = { plhs->w, plhs->e, prhs->w, prhs->e };
    qsort( nWE, 4, sizeof( int ), CompSide );

    // clamp we
    if( ( nNS[ 2 ] - nNS[ 1 ] ) > ( nWE[ 2 ] - nWE[ 1 ] ) ) {
        // clamp e
        if( plhs->e >= prhs->w && plhs->e < prhs->e ) {
            *pOrientation =  EOD_E;
            *pClampValue = prhs->w - 2;
        }
        // clamp w
        else {
            *pOrientation =  EOD_W;
            *pClampValue = prhs->e + 2;
        }
    }
    // clamp ns
    else {
        // clamp s
        if( plhs->s >= prhs->n && plhs->s < prhs->s ) {
            *pOrientation =  EOD_S;
            *pClampValue = prhs->n - 2;
        }
        // clamp n
        else {
            *pOrientation =  EOD_N;
            *pClampValue = prhs->s + 2;
        }
    }


    return true;
}

bool CheckBlocker( Position *pPos, const bool bDir )
{
    // assume not inside the blocker
    bool bClamped = false;
    int nWidth = pPos->e - pPos->w;
    int nHeight = pPos->s - pPos->n;

    for( int i = 0; i < g_nGroundCount; i++ ) {
        Position Ground;
        Ground.w = g_Ground[ i ].x;
        Ground.e = g_Ground[ i ].x + g_Ground[ i ].nWidth;
        Ground.n = g_Ground[ i ].y;
        Ground.s = g_Ground[ i ].y + g_Ground[ i ].nHeight;

        EOrientationDirection nOrientation = EOD_E;
        int nClampValue = 0;

        bool bOverlap = AdvCheckOverlap( pPos, &Ground, &nOrientation, &nClampValue );

        if( bOverlap == true ) {
            bClamped = true;
            printf( "******** clamped dir = %d, value = %d\n", nOrientation, nClampValue );
            switch( nOrientation ) {
            case EOD_E:
                pPos->e = nClampValue;
                pPos->w = pPos->e - nWidth;
                break;

            case EOD_S:
                pPos->s = nClampValue;
                pPos->n = pPos->s - nHeight;
                break;

            case EOD_W:
                pPos->w = nClampValue;
                pPos->e = pPos->w + nWidth;
                break;

            case EOD_N:
                pPos->n = nClampValue;
                pPos->s = pPos->n + nHeight;
                break;

            default:
                assert( false );
                break;
            }
        }
    }
    return bClamped;
}

int FindAndDrawClosestGroundY( const int w, const int e, const int s ){
    // find closest lower ground
    int nFoundGroundIdx[ MAX_COUNTOF_FOUNDGROUND ] = { 0 };
    int nFoundGroundCount = 0;
    FindGround( nFoundGroundIdx, &nFoundGroundCount, w, e );

    int nClosestGroundIdx = -1;
    int nGroundY = -1;
    nClosestGroundIdx = FindClosestGround( nFoundGroundIdx, nFoundGroundCount, s );
    if( nClosestGroundIdx != -1 ) {
        nGroundY = g_Ground[ nClosestGroundIdx ].y;
    }

//    if( DEBUGPRINT_SCENE ) {
//        printf( "obj x = %d, y = %d\n", e_pchara->x, e_pchara->y );
//        for( int i = 0; i < nFoundGroundCount; i++ ) {
//            printf( "*************************************Ground %d: x = %d, y = %d, width = %d, height = %d\n",
//                    i, g_Ground[ nFoundGroundIdx[ i ] ].x, g_Ground[ nFoundGroundIdx[ i ] ].y,
//                    g_Ground[ nFoundGroundIdx[ i ] ].nWidth, g_Ground[ nFoundGroundIdx[ i ] ].nHeight );
//        }
//    }
    return nGroundY - 1;
}

int FindClosestGround( int *pGroundIdx, int nGroundCount, const int s ) {
    int nMinY = INT_MAX;
    int nClosestGroundIdx = -1;

    for( int i = 0; i < nGroundCount; i++ ) {
        if( g_Ground[ pGroundIdx[ i ] ].y >= s && g_Ground[ pGroundIdx[ i ] ].y < nMinY ) {
            nMinY = g_Ground[ pGroundIdx[ i ] ].y;
            nClosestGroundIdx = pGroundIdx[ i ];
        }
    }
    return nClosestGroundIdx;
}

bool IsInGroundRange( int nGroundIdx, const int w, const int e ) {
    int gw = g_Ground[ nGroundIdx ].x;
    int ge = g_Ground[ nGroundIdx ].x + g_Ground[ nGroundIdx ].nWidth;
    if( ( w < gw && e < gw ) || ( w > ge && e > ge ) ) {
        return false;
    }
    return true;
}

void FindGround( int *pnFoundGroundIdx, int *pnFoundCount, int w, int e ) {
    int j = 0;
    for( int i = 0; i < g_nGroundCount; i++ ) {
        if( IsInGroundRange( i, w, e ) == true ) {
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
            Ground[ i ] = al_load_bitmap("./image/grass.png");
            assert( Ground[ i ] != NULL );
        }
        else if( g_Ground[ i ].type == 2 ) {
            Ground[ i ] = al_load_bitmap("./image/cloud.png");
            assert( Ground[ i ] != NULL );
        }
        else if( g_Ground[ i ].type == 3 ) {
            Ground[ i ] = al_load_bitmap("./image/cloud_long.png");
            assert( Ground[ i ] != NULL );
        }
        else if( g_Ground[ i ].type == 4 ) {
            Ground[ i ] = al_load_bitmap("./image/bridge.png");
            assert( Ground[ i ] != NULL );
        }
        else if( g_Ground[ i ].type == 5 ) {
            Ground[ i ] = al_load_bitmap("./image/bridge_five.png");
            assert( Ground[ i ] != NULL );
        }
        else if( g_Ground[ i ].type == 6 ) {
            Ground[ i ] = al_load_bitmap("./image/stone1.png");
            assert( Ground[ i ] != NULL );
        }
        else if( g_Ground[ i ].type == 7 ) {
            Ground[ i ] = al_load_bitmap("./image/stone2.png");
            assert( Ground[ i ] != NULL );
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
    }
}

