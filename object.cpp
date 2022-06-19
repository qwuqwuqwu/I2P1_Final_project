#include "object.h"

#define DEBUGPRINT_SCENE            ( false )

#define MAX_COUNTOF_FOUNDGROUND     ( 40 )
#define MAX_COUNTOF_GROUND          ( 600 )
#define MAX_COUNTOF_FOOD            ( 100 )
#define MAX_COUNTOF_CANDY           ( 100 )
#define MAX_COUNTOF_BOMB            ( 100 )

// the state of object
enum {NORMAL = 0, HIGHLIGHT};

Pos g_Ground[ MAX_COUNTOF_GROUND ];
Bom g_Bomb[ MAX_COUNTOF_BOMB ];
Pos g_Food[ MAX_COUNTOF_FOOD ];
Pos g_Candy[ MAX_COUNTOF_CANDY ];

int g_nGroundCount = 0;
int g_nFoodCount = 0;
int g_nCandyCount = 0;
int g_nBombCount = 0;

ALLEGRO_BITMAP *Ground[ MAX_COUNTOF_GROUND ] = { NULL };
ALLEGRO_BITMAP *g_BombMap[ MAX_COUNTOF_BOMB ] = { NULL };
ALLEGRO_BITMAP *g_ExplodeMap[ 3 ] = { NULL };

ALLEGRO_BITMAP *g_FoodMap[ MAX_COUNTOF_FOOD ] = { NULL };
ALLEGRO_BITMAP *g_CandyMap[ MAX_COUNTOF_CANDY ] = { NULL };

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
        //
        Position Ground;
        Ground.w = g_Ground[ i ].x;
        Ground.e = g_Ground[ i ].x + g_Ground[ i ].nWidth;
        Ground.n = g_Ground[ i ].y;
        Ground.s = g_Ground[ i ].y + g_Ground[ i ].nHeight;

        EOrientationDirection nOrientation = EOD_E;
        int nClampValue = 0;

        bool bOverlap = AdvCheckOverlap( pPos, &Ground, &nOrientation, &nClampValue );

        if( bOverlap == true && ( g_Ground[ i ].type == EBT_GRASS || g_Ground[ i ].type == EBT_STONE1 || g_Ground[ i ].type == EBT_STONE2 || g_Ground[ i ].type == EBT_WALL ) ) {
            bClamped = true;
//            printf( "******** clamped dir = %d, value = %d\n", nOrientation, nClampValue );
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

    if( DEBUGPRINT_SCENE ) {
        printf( "obj s = %d\n", s );
        printf( "*************************************Ground %d: x = %d, y = %d, width = %d, height = %d\n",
               nClosestGroundIdx,
               g_Ground[ nClosestGroundIdx ].x, g_Ground[ nClosestGroundIdx ].y,
                g_Ground[ nClosestGroundIdx ].nWidth, g_Ground[ nClosestGroundIdx ].nHeight );
//        for( int i = 0; i < nFoundGroundCount; i++ ) {
//            printf( "*************************************Ground %d: x = %d, y = %d, width = %d, height = %d\n",
//                    i, g_Ground[ nFoundGroundIdx[ i ] ].x, g_Ground[ nFoundGroundIdx[ i ] ].y,
//                    g_Ground[ nFoundGroundIdx[ i ] ].nWidth, g_Ground[ nFoundGroundIdx[ i ] ].nHeight );
//        }
    }
    return nGroundY - 1;
}

int FindClosestGround( int *pGroundIdx, int nGroundCount, const int s ) {
    int nMinY = INT_MAX;
    int nClosestGroundIdx = -1;

    for( int i = 0; i < nGroundCount; i++ ) {
        if( g_Ground[ pGroundIdx[ i ] ].y > s && g_Ground[ pGroundIdx[ i ] ].y < nMinY ) {
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
        else if( g_Ground[ i ].type == 8 ) {
            Ground[ i ] = al_load_bitmap("./image/wall.png");
            assert( Ground[ i ] != NULL );
        }

        g_Ground[ i ].nWidth = al_get_bitmap_width( Ground[ i ] );
        g_Ground[ i ].nHeight = al_get_bitmap_height( Ground[ i ] );
        printf( "i, height = %d, %d\n", i, g_Ground[ i ].nHeight );
    }

    // release the file handler
    fclose( fp );
}

void BombSetup( void )
{
    for( int i = 0; i < 3; i++ ) {
        char temp[ 50 ];
        sprintf( temp, "./image/explode%d.png", i + 1 );
        g_ExplodeMap[ i ] = al_load_bitmap( temp );

        assert( g_ExplodeMap[ i ] != NULL );
    }

    for( int i = 0; i < MAX_COUNTOF_BOMB; i++ ) {
        g_BombMap[ i ] = al_load_bitmap("./image/bomb.png");
        assert( g_BombMap[ i ] != NULL );

        g_Bomb[ i ].img = g_BombMap[ i ];
        g_Bomb[ i ].nSubState = 0;
        g_Bomb[ i ].x = 0;
        g_Bomb[ i ].y = 0;
        g_Bomb[ i ].y0 = 0;
        g_Bomb[ i ].dir = true;
        g_Bomb[ i ].FallingTick = 0;
        g_Bomb[ i ].state = EBS_IDLE;
        g_Bomb[ i ].vy = 0.0;
        g_Bomb[ i ].width = al_get_bitmap_width( g_BombMap[ i ] );
        g_Bomb[ i ].height = al_get_bitmap_height( g_BombMap[ i ] );
        g_Bomb[ i ].nExplodeCursor = 0;
        g_Bomb[ i ].nExplodeTime = 30;

        for( int j = 0; j < 3; j++ ) {
            g_Bomb[ i ].img_explode[ j ] = g_ExplodeMap[ j ];
        }
    }
}

void FoodSetup( void )
{
    printf( "FoodSetup\n" );
    FILE* fp = fopen( "./res/food.txt", "r" );
    int nPosX = 0;
    int nPosY = 0;
    while( fscanf( fp, "%d%d", &nPosX, &nPosY ) != EOF && g_nFoodCount <= MAX_COUNTOF_FOOD ) {
        g_Food[ g_nFoodCount ].state = EFS_IDLE;
        g_Food[ g_nFoodCount ].x = nPosX;
        g_Food[ g_nFoodCount ].y = nPosY;
        g_nFoodCount++;
    }

    for( int i = 0; i < g_nFoodCount; i++ ) {
        g_FoodMap[ i ] = al_load_bitmap( "./image/food.png" );
        assert( g_FoodMap[ i ] != NULL );

        g_Food[ i ].nWidth = al_get_bitmap_width( g_FoodMap[ i ] );
        g_Food[ i ].nHeight = al_get_bitmap_height( g_FoodMap[ i ] );
    }

    fclose( fp );
}

void CandySetup( void )
{
    printf( "CandySetup\n" );
    FILE* fp = fopen( "./res/candy.txt", "r" );
    int nPosX = 0;
    int nPosY = 0;
    while( fscanf( fp, "%d%d", &nPosX, &nPosY ) != EOF && g_nCandyCount <= MAX_COUNTOF_CANDY ) {
        g_Candy[ g_nCandyCount ].state = EFS_IDLE;
        g_Candy[ g_nCandyCount ].x = nPosX;
        g_Candy[ g_nCandyCount ].y = nPosY;
        g_nCandyCount++;
    }

    for( int i = 0; i < g_nCandyCount; i++ ) {
        g_CandyMap[ i ] = al_load_bitmap( "./image/candy.png" );
        assert( g_CandyMap[ i ] != NULL );

        g_Candy[ i ].nWidth = al_get_bitmap_width( g_CandyMap[ i ] );
        g_Candy[ i ].nHeight = al_get_bitmap_height( g_CandyMap[ i ] );
    }

    fclose( fp );
}

int RegisterBomb( void )
{
    int nTemp = g_nBombCount;
    g_nBombCount++;
    return nTemp;
}

void BombThrow( const int idx, const int x, const int y, const bool bDir )
{
    if( idx < 0 || idx >= g_nBombCount ) {
        assert( false );
        return;
    }

    g_Bomb[ idx ].dir = bDir;
    g_Bomb[ idx ].x = x;
    g_Bomb[ idx ].y = y;
    g_Bomb[ idx ].x0 = x;
    g_Bomb[ idx ].y0 = y;
    g_Bomb[ idx ].vx = bDir ? 30 : -30;
    g_Bomb[ idx ].vy = -100;
    g_Bomb[ idx ].FallingTick = 0;
    g_Bomb[ idx ].nExplodeCursor = 0;
    g_Bomb[ idx ].state = EBS_FLY;
}

bool GetBombPosition( const int idx, Position *pBombPos )
{
    if( idx < 0 || idx >= g_nBombCount ) {
        assert( false );
        return false;
    }

    if( g_Bomb[ idx ].state != EBS_IDLE ) {
        pBombPos->e = g_Bomb[ idx ].x + g_Bomb[ idx ].width;
        pBombPos->s = g_Bomb[ idx ].y + g_Bomb[ idx ].height;
        pBombPos->w = g_Bomb[ idx ].x;
        pBombPos->n = g_Bomb[ idx ].x;
        return true;
    }
}

void ExplodeBomb( const int idx )
{
    if( idx < 0 || idx >= g_nBombCount ) {
        assert( false );
        return;
    }
    // if in EBS_EXPLODE, just let it explode
    // only deal with fly condition
    if( g_Bomb[ idx ].state == EBS_FLY ) {
        g_Bomb[ idx ].state = EBS_EXPLODE;
    }
}

void Eat( const Position *plhs, int *pFoodCount, int *pCandyCount )
{
    int nFoodCount = 0;
    int nCandyCount = 0;

    // eat food
    for( int i = 0; i < g_nFoodCount; i++ ) {
        if( g_Food[ i ].state == EFS_IDLE ) {
            Position FoodPos;
            FoodPos.e = g_Food[ i ].x + g_Food[ i ].nWidth;
            FoodPos.s = g_Food[ i ].y + g_Food[ i ].nHeight;
            FoodPos.w = g_Food[ i ].x;
            FoodPos.n = g_Food[ i ].y;

            bool bOverlaped = CheckOverlap( plhs, &FoodPos );
            if( bOverlaped == true ) {
                g_Food[ i ].state = EFS_EATEN;
                nFoodCount++;
            }
        }
    }

    // eat candy
    for( int i = 0; i < g_nCandyCount; i++ ) {
        if( g_Candy[ i ].state == EFS_IDLE ) {
            Position CandyPos;
            CandyPos.e = g_Candy[ i ].x + g_Candy[ i ].nWidth;
            CandyPos.s = g_Candy[ i ].y + g_Candy[ i ].nHeight;
            CandyPos.w = g_Candy[ i ].x;
            CandyPos.n = g_Candy[ i ].y;

            bool bOverlaped = CheckOverlap( plhs, &CandyPos );
            if( bOverlaped == true ) {
                g_Candy[ i ].state = EFS_EATEN;
                nCandyCount++;
            }
        }
    }

    *pFoodCount = nFoodCount;
    *pCandyCount = nCandyCount;
}

bool BombUpdate( void )
// advanced one tick
{
    for( int i = 0; i < g_nBombCount; i++ ) {
        if( g_Bomb[ i ].state == EBS_FLY ) {
            // find ground first
            int nGroundY = FindAndDrawClosestGroundY( g_Bomb[ i ].x, g_Bomb[ i ].x + g_Bomb[ i ].width,
                                                      g_Bomb[ i ].y + g_Bomb[ i ].height );
            if( nGroundY == -2 ) {
                nGroundY = HEIGHT;
            }

            // ticktock
            g_Bomb[ i ].FallingTick += g_Tick;

            // velocity;
            g_Bomb[ i ].vy += g_Gravity * g_Tick;
            // position
            g_Bomb[ i ].y = g_Bomb[ i ].y0
                            + ( int )( g_Bomb[ i ].vy * g_Bomb[ i ].FallingTick
                                      + ( 0.5 * g_Gravity * ( g_Bomb[ i ].FallingTick * g_Bomb[ i ].FallingTick ) ) );
            g_Bomb[ i ].x = g_Bomb[ i ].x0 + g_Bomb[ i ].vx * g_Bomb[ i ].FallingTick;


            if( ( g_Bomb[ i ].y + g_Bomb[ i ].height ) >= nGroundY ) {
                g_Bomb[ i ].y = nGroundY - ( g_Bomb[ i ].height );
                g_Bomb[ i ].y0 = g_Bomb[ i ].height;
                g_Bomb[ i ].vy = 0.0;
                g_Bomb[ i ].FallingTick = 0;
                g_Bomb[ i ].state = EBS_EXPLODE;
            }
            else {
                // do nothing
            }
        }
        else if( g_Bomb[ i ].state == EBS_EXPLODE ) {
            if( g_Bomb[ i ].nExplodeCursor < g_Bomb[ i ].nExplodeTime / 3 ) {
                g_Bomb[ i ].nSubState = 0;
            }
            else if( g_Bomb[ i ].nExplodeCursor < g_Bomb[ i ].nExplodeTime * 2 / 3 ) {
                g_Bomb[ i ].nSubState = 1;
            }
            else {
                g_Bomb[ i ].nSubState = 2;
            }
        }
    }
}

void object_init( void ){
    GroundSetup();
    BombSetup();
    FoodSetup();
    CandySetup();
}

void object_draw( void ){
    // plot ground
    for( int i = 0; i < g_nGroundCount; i++ ) {
       al_draw_bitmap( Ground[ i ], g_Ground[ i ].x, g_Ground[ i ].y, 0 );
    }

    BombUpdate();
    for( int i = 0; i < g_nBombCount; i++ ) {
//        printf( "bomb %d state = %d, substate = %d\n", i, g_Bomb[ i ].state, g_Bomb[ i ].nSubState );
        if( g_Bomb[ i ].state == EBS_FLY ) {
            if( g_Bomb[ i ].dir == false ) {
                al_draw_bitmap( g_Bomb[ i ].img, g_Bomb[ i ].x, g_Bomb[ i ].y, 0 );
            }
            else {
                al_draw_bitmap( g_Bomb[ i ].img, g_Bomb[ i ].x, g_Bomb[ i ].y, ALLEGRO_FLIP_HORIZONTAL );
            }
        }
        else if( g_Bomb[ i ].state == EBS_EXPLODE ) {
            if( g_Bomb[ i ].dir == false ) {
                al_draw_bitmap( g_Bomb[ i ].img_explode[ g_Bomb[ i ].nSubState ], g_Bomb[ i ].x, g_Bomb[ i ].y, 0 );
            }
            else {
                al_draw_bitmap( g_Bomb[ i ].img_explode[ g_Bomb[ i ].nSubState ], g_Bomb[ i ].x, g_Bomb[ i ].y, ALLEGRO_FLIP_HORIZONTAL );
            }
        }
    }

    // draw food
    for( int i = 0; i < g_nFoodCount; i++ ) {
        if( g_Food[ i ].state == EFS_IDLE ) {
            al_draw_bitmap( g_FoodMap[ i ], g_Food[ i ].x, g_Food[ i ].y, 0 );
        }
    }

    // draw candy
    for( int i = 0; i < g_nCandyCount; i++ ) {
        if( g_Candy[ i ].state == EFS_IDLE ) {
            al_draw_bitmap( g_CandyMap[ i ], g_Candy[ i ].x, g_Candy[ i ].y, 0 );
        }
    }
}

void object_destroy( void ){
    // ground
    for( int i = 0; i < g_nGroundCount; i++ ) {
        al_destroy_bitmap( Ground[ i ] );
    }

    // bomb
    for( int i = 0; i < MAX_COUNTOF_BOMB; i++ ) {
        al_destroy_bitmap( g_BombMap[ i ] );
    }
    for( int i = 0; i < 3; i++ ) {
        al_destroy_bitmap( g_ExplodeMap[ i ] );
    }

    // food
    for( int i = 0; i < g_nFoodCount; i++ ) {
        al_destroy_bitmap( g_FoodMap[ i ] );
    }

    // candy
    for( int i = 0; i < g_nCandyCount; i++ ) {
        al_destroy_bitmap( g_CandyMap[ i ] );
    }
}

void Object_process( ALLEGRO_EVENT event )
{
    if( event.type == ALLEGRO_EVENT_TIMER ) {
        if( event.timer.source == fps ) {
            for( int i = 0; i < g_nBombCount; i++ ) {
                if( g_Bomb[ i ].state == EBS_EXPLODE ) {
                    g_Bomb[ i ].nExplodeCursor++;
                    g_Bomb[ i ].nExplodeCursor %= g_Bomb[ i ].nExplodeTime;

                    if( g_Bomb[ i ].nExplodeCursor == 0 ) {
                        g_Bomb[ i ].state = EBS_IDLE;
                        printf( "Bomb %d idle\n", i );
                    }
                }
            }
        }
    // process the keyboard event
    }
}

