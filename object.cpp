#include "object.h"

#define DEBUGPRINT_SCENE            ( false )

#define MAX_COUNTOF_FOUNDGROUND     ( 40 )
#define MAX_COUNTOF_GROUND          ( 600 )
#define MAX_COUNTOF_FOOD            ( 100 )
#define MAX_COUNTOF_CANDY           ( 100 )
#define MAX_COUNTOF_BOMB            ( 100 )
#define MAX_COUNTOF_BOSSBOMB_ROW    ( 10 )
#define MAX_COUNTOF_BOSSBOMB_COL    ( 10 )

#define BOMB_GRAVITY_RATE           ( 0.5 )

// the state of object
enum {NORMAL = 0, HIGHLIGHT};

Pos g_Ground[ MAX_COUNTOF_GROUND ];
Bom g_Bomb[ MAX_COUNTOF_BOMB ];
Pos g_Food[ MAX_COUNTOF_FOOD ];
Pos g_Candy[ MAX_COUNTOF_CANDY ];

Bom g_BossBomb[ MAX_COUNTOF_BOSSBOMB_ROW ][ MAX_COUNTOF_BOSSBOMB_COL ];

int g_nGroundCount = 0;
int g_nFoodCount = 0;
int g_nCandyCount = 0;
int g_nBombCount = 0;
int g_nBossBombColCount[ MAX_COUNTOF_BOSSBOMB_ROW ];
int g_nBossBombRow = 0;

bool g_bStartFalling = false;
int g_nFallingCursor = 0;
int g_nFallingTime = 120;
int g_nFallingRow = 0;

ALLEGRO_BITMAP *Ground[ MAX_COUNTOF_GROUND ] = { NULL };
ALLEGRO_BITMAP *g_BombMap[ MAX_COUNTOF_BOMB ] = { NULL };
ALLEGRO_BITMAP *g_ExplodeMap[ 3 ] = { NULL };
ALLEGRO_BITMAP *g_BossBombMap = NULL;

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
 //       printf( "obj s = %d\n", s );
 //       printf( "*************************************Ground %d: x = %d, y = %d, width = %d, height = %d\n",
 //              nClosestGroundIdx,
 //              g_Ground[ nClosestGroundIdx ].x, g_Ground[ nClosestGroundIdx ].y,
 //               g_Ground[ nClosestGroundIdx ].nWidth, g_Ground[ nClosestGroundIdx ].nHeight );
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
    g_nGroundCount = 0;
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

    // release the file handler
    fclose( fp );

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
  //      printf( "i, height = %d, %d\n", i, g_Ground[ i ].nHeight );
    }
}

void BombSetup( void )
{
    g_nBombCount = 0;
    for( int i = 0; i < 3; i++ ) {
        char temp[ 50 ];
        sprintf( temp, "./image/explode%d.png", i + 1 );
        g_ExplodeMap[ i ] = al_load_bitmap( temp );

        assert( g_ExplodeMap[ i ] != NULL );
    }

    for( int i = 0; i < MAX_COUNTOF_BOMB; i++ ) {
        g_BombMap[ i ] = al_load_bitmap( "./image/bomb.png" );
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

        g_Bomb[ i ].sound_sample = al_load_sample( "./sound/explode.wav" );
        g_Bomb[ i ].sound_explode = al_create_sample_instance( g_Bomb[ i ].sound_sample );
        al_set_sample_instance_playmode( g_Bomb[ i ].sound_explode, ALLEGRO_PLAYMODE_ONCE );
        al_set_sample_instance_gain( g_Bomb[ i ].sound_explode, 0.4 );
        al_attach_sample_instance_to_mixer( g_Bomb[ i ].sound_explode, al_get_default_mixer() );
    }
}

void FoodSetup( void )
{
    printf( "FoodSetup\n" );
    g_nFoodCount = 0;
    FILE* fp = fopen( "./res/food.txt", "r" );
    int nPosX = 0;
    int nPosY = 0;
    while( fscanf( fp, "%d%d", &nPosX, &nPosY ) != EOF && g_nFoodCount <= MAX_COUNTOF_FOOD ) {
        g_Food[ g_nFoodCount ].state = EFS_IDLE;
        g_Food[ g_nFoodCount ].x = nPosX;
        g_Food[ g_nFoodCount ].y = nPosY;
        g_nFoodCount++;
    }

    fclose( fp );

    for( int i = 0; i < g_nFoodCount; i++ ) {
        g_FoodMap[ i ] = al_load_bitmap( "./image/food.png" );
        assert( g_FoodMap[ i ] != NULL );

        g_Food[ i ].nWidth = al_get_bitmap_width( g_FoodMap[ i ] );
        g_Food[ i ].nHeight = al_get_bitmap_height( g_FoodMap[ i ] );
    }
}

void CandySetup( void )
{
    printf( "CandySetup\n" );
    g_nCandyCount = 0;
    FILE* fp = fopen( "./res/candy.txt", "r" );
    int nPosX = 0;
    int nPosY = 0;
    while( fscanf( fp, "%d%d", &nPosX, &nPosY ) != EOF && g_nCandyCount <= MAX_COUNTOF_CANDY ) {
        g_Candy[ g_nCandyCount ].state = EFS_IDLE;
        g_Candy[ g_nCandyCount ].x = nPosX;
        g_Candy[ g_nCandyCount ].y = nPosY;
        g_nCandyCount++;
    }
    fclose( fp );

    for( int i = 0; i < g_nCandyCount; i++ ) {
        g_CandyMap[ i ] = al_load_bitmap( "./image/candy.png" );
        assert( g_CandyMap[ i ] != NULL );

        g_Candy[ i ].nWidth = al_get_bitmap_width( g_CandyMap[ i ] );
        g_Candy[ i ].nHeight = al_get_bitmap_height( g_CandyMap[ i ] );
    }
}

void BossBombSetup( void )
{
    printf( "BossBombSetup\n" );
    FILE* fp = fopen( "./res/bossbomb.txt", "r" );
    int nPosX = 0;
    int nCol = 0;
    g_nBossBombRow = 0;
    while( fscanf( fp, "%d", &nCol ) != EOF ) {
        for( int i = 0; i < nCol; i++ ) {
            fscanf( fp, "%d", &nPosX );
            g_BossBomb[ g_nBossBombRow ][ i ].x = 20483 + 56 * nPosX;
//            printf( "x = %d\n", g_BossBomb[ g_nBossBombRow ][ i ].x );
            g_BossBomb[ g_nBossBombRow ][ i ].img = al_load_bitmap( "./image/bomb.png" );
            for( int j = 0; j < 3; j++ ) {
                g_BossBomb[ g_nBossBombRow ][ i ].img_explode[ j ] = g_ExplodeMap[ j ];
            }
            g_BossBomb[ g_nBossBombRow ][ i ].nSubState = 0;
            g_BossBomb[ g_nBossBombRow ][ i ].vx = 0;
            g_BossBomb[ g_nBossBombRow ][ i ].y = -80;
            g_BossBomb[ g_nBossBombRow ][ i ].y0 = 0;
            g_BossBomb[ g_nBossBombRow ][ i ].dir = true;
            g_BossBomb[ g_nBossBombRow ][ i ].FallingTick = 0;
            g_BossBomb[ g_nBossBombRow ][ i ].state = EBS_IDLE;
            g_BossBomb[ g_nBossBombRow ][ i ].vy = 0.0;
            g_BossBomb[ g_nBossBombRow ][ i ].width = al_get_bitmap_width( g_BossBomb[ g_nBossBombRow ][ i ].img );
            g_BossBomb[ g_nBossBombRow ][ i ].height = al_get_bitmap_height( g_BossBomb[ g_nBossBombRow ][ i ].img );
            g_BossBomb[ g_nBossBombRow ][ i ].nExplodeCursor = 0;
            g_BossBomb[ g_nBossBombRow ][ i ].nExplodeTime = 30;

            g_BossBomb[ g_nBossBombRow ][ i ].sound_sample = al_load_sample( "./sound/explode.wav" );
            g_BossBomb[ g_nBossBombRow ][ i ].sound_explode = al_create_sample_instance( g_BossBomb[ g_nBossBombRow ][ i ].sound_sample );
            al_set_sample_instance_playmode( g_BossBomb[ g_nBossBombRow ][ i ].sound_explode, ALLEGRO_PLAYMODE_ONCE );
            al_set_sample_instance_gain( g_BossBomb[ g_nBossBombRow ][ i ].sound_explode, 0.4 );
            al_attach_sample_instance_to_mixer( g_BossBomb[ g_nBossBombRow ][ i ].sound_explode, al_get_default_mixer() );
        }
        g_nBossBombColCount[ g_nBossBombRow ] = nCol;
        g_nBossBombRow++;
    }

    fclose( fp );

    g_bStartFalling = false;
    g_nFallingCursor = 0;
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

    // bomb is effective only when it is flying
    // after bomb explode, no harm will be cause
    if( g_Bomb[ idx ].state == EBS_FLY ) {
        pBombPos->e = g_Bomb[ idx ].x + g_Bomb[ idx ].width;
        pBombPos->s = g_Bomb[ idx ].y + g_Bomb[ idx ].height;
        pBombPos->w = g_Bomb[ idx ].x;
        pBombPos->n = g_Bomb[ idx ].y;
        return true;
    }
    return false;
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

void BombUpdate( void )
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
            float localTick = BOMB_GRAVITY_RATE * g_Tick;

            // ticktock
            g_Bomb[ i ].FallingTick += localTick;

            // velocity;
            g_Bomb[ i ].vy += g_Gravity * localTick;
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
    BossBombSetup();
}

void BossBomb_Trigger( void )
{
    g_bStartFalling = true;
}

void BossBomb_update( void )
{
    // update certain row
    if( g_bStartFalling == false ) {
        return;
    }
    g_nFallingCursor = ( g_nFallingCursor+ 1 ) % g_nFallingTime;
//    printf( "g_nFallingCursor = %d\n", g_nFallingCursor );
    if( g_nFallingCursor == 0 ) {
        printf( "Trigger Row %d\n", g_nFallingRow );
        for( int i = 0; i < g_nBossBombColCount[ g_nFallingRow ]; i++ ) {
            g_BossBomb[ g_nFallingRow ][ i ].state = EBS_FLY;
            g_BossBomb[ g_nFallingRow ][ i ].y = 0;
        }
        g_nFallingRow = ( g_nFallingRow + 1 ) % g_nBossBombRow;
    }

    for( int i = 0; i < g_nBossBombRow; i++ ) {
        for( int j = 0; j < g_nBossBombColCount[ i ]; j++ ) {
            if( g_BossBomb[ i ][ j ].state == EBS_FLY ) {
                // find ground first
                int nGroundY = FindAndDrawClosestGroundY( g_BossBomb[ i ][ j ].x, g_BossBomb[ i ][ j ].x + g_BossBomb[ i ][ j ].width,
                                                          g_BossBomb[ i ][ j ].y + g_BossBomb[ i ][ j ].height );
                if( nGroundY == -2 ) {
                    nGroundY = HEIGHT;
                }

                float LocalTick = BOMB_GRAVITY_RATE * g_Tick;

                // ticktock
                g_BossBomb[ i ][ j ].FallingTick += LocalTick;

                // velocity;
                g_BossBomb[ i ][ j ].vy += g_Gravity * BOMB_GRAVITY_RATE * LocalTick;
                // position
                g_BossBomb[ i ][ j ].y = g_BossBomb[ i ][ j ].y0
                                + ( int )( g_BossBomb[ i ][ j ].vy * g_BossBomb[ i ][ j ].FallingTick
                                          + ( 0.5 * g_Gravity * ( g_BossBomb[ i ][ j ].FallingTick * g_BossBomb[ i ][ j ].FallingTick ) ) );
//                g_BossBomb[ i ][ j ].x = g_BossBomb[ i ][ j ].x0 + g_BossBomb[ i ][ j ].vx * g_BossBomb[ i ][ j ].FallingTick;

//                printf( "bomb x = %d\n", g_BossBomb[ i ][ j ].x );
                if( ( g_BossBomb[ i ][ j ].y + g_BossBomb[ i ][ j ].height ) >= nGroundY ) {
                    g_BossBomb[ i ][ j ].y = nGroundY - ( g_BossBomb[ i ][ j ].height );
                    g_BossBomb[ i ][ j ].y0 = g_BossBomb[ i ][ j ].height;
                    g_BossBomb[ i ][ j ].vy = 0.0;
                    g_BossBomb[ i ][ j ].FallingTick = 0;
                    g_BossBomb[ i ][ j ].state = EBS_EXPLODE;
                }
                else {
                    // do nothing
                }
            }
            else if( g_BossBomb[ i ][ j ].state == EBS_EXPLODE ) {
                if( g_BossBomb[ i ][ j ].nExplodeCursor < g_BossBomb[ i ][ j ].nExplodeTime / 3 ) {
                    g_BossBomb[ i ][ j ].nSubState = 0;
                }
                else if( g_BossBomb[ i ][ j ].nExplodeCursor < g_BossBomb[ i ][ j ].nExplodeTime * 2 / 3 ) {
                    g_BossBomb[ i ][ j ].nSubState = 1;
                }
                else {
                    g_BossBomb[ i ][ j ].nSubState = 2;
                }
            }
        }
    }
}

void BossBomb_draw( void )
{
    if( g_bStartFalling == false ) {
        return;
    }
    for( int j = 0; j < g_nBossBombRow; j++ ) {
        for( int i = 0; i < g_nBossBombColCount[ j ]; i++ ) {
            if( g_BossBomb[ j ][ i ].state == EBS_FLY ) {
//                printf( "bomb %d %d\n", g_BossBomb[ j ][ i ].x, g_BossBomb[ j ][ i ].y );
                if( g_BossBomb[ j ][ i ].dir == false ) {
                    al_draw_bitmap( g_BossBomb[ j ][ i ].img, g_BossBomb[ j ][ i ].x, g_BossBomb[ j ][ i ].y, 0 );
                }
                else {
                    al_draw_bitmap( g_BossBomb[ j ][ i ].img, g_BossBomb[ j ][ i ].x, g_BossBomb[ j ][ i ].y, ALLEGRO_FLIP_HORIZONTAL );
                }
            }
            else if( g_BossBomb[ j ][ i ].state == EBS_EXPLODE ) {
//                printf( "bomb %d %d\n", g_BossBomb[ j ][ i ].x, g_BossBomb[ j ][ i ].y );
                if( g_BossBomb[ j ][ i ].dir == false ) {
                    al_draw_bitmap( g_BossBomb[ j ][ i ].img_explode[ g_BossBomb[ j ][ i ].nSubState ], g_BossBomb[ j ][ i ].x, g_BossBomb[ j ][ i ].y, 0 );
                }
                else {
                    al_draw_bitmap( g_BossBomb[ j ][ i ].img_explode[ g_BossBomb[ j ][ i ].nSubState ], g_BossBomb[ j ][ i ].x, g_BossBomb[ j ][ i ].y, ALLEGRO_FLIP_HORIZONTAL );
                }
                if( e_pchara->nSubState == 0 ) {
                    al_play_sample_instance( g_BossBomb[ j ][ i ].sound_explode );
                }
            }
        }
    }

}

void object_draw( void )
{
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
            if( e_pchara->nSubState == 0 ) {
                al_play_sample_instance( g_Bomb[ i ].sound_explode );
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

    // draw boss bomb
    BossBomb_update();
    BossBomb_draw();
}

void object_destroy( void )
{
    printf( "1\n" );
    al_stop_samples();
    // ground
    for( int i = 0; i < g_nGroundCount; i++ ) {
        al_destroy_bitmap( Ground[ i ] );
    }
    printf( "2\n" );
    // bomb
    for( int i = 0; i < MAX_COUNTOF_BOMB; i++ ) {
        al_destroy_bitmap( g_BombMap[ i ] );printf( "2.1\n" );
        al_stop_sample_instance( g_Bomb[ i ].sound_explode );
//        al_destroy_sample( g_Bomb[ i ].sound_sample );printf( "2.2\n" );
        al_destroy_sample_instance( g_Bomb[ i ].sound_explode );printf( "2.3\n" );
    }
    printf( "3\n" );
    for( int i = 0; i < 3; i++ ) {
        al_destroy_bitmap( g_ExplodeMap[ i ] );
    }
    printf( "4\n" );
    // food
    for( int i = 0; i < g_nFoodCount; i++ ) {
        al_destroy_bitmap( g_FoodMap[ i ] );
    }
    printf( "5\n" );
    // candy
    for( int i = 0; i < g_nCandyCount; i++ ) {
        al_destroy_bitmap( g_CandyMap[ i ] );
    }
    printf( "yo\n" );

    // boss bomb
    for( int i = 0; i < g_nBossBombRow; i++ ) {
        for( int j = 0; j < g_nBossBombColCount[ i ]; j++ ) {
            al_destroy_bitmap( g_BossBomb[ i ][ j ].img );
            al_stop_sample_instance( g_BossBomb[ i ][ j ].sound_explode );
//            al_destroy_sample( g_BossBomb[ i ][ j ].sound_sample );
            al_destroy_sample_instance( g_BossBomb[ i ][ j ].sound_explode );
        }
    }

    printf( "object destroy success!\n" );
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
//                        printf( "Bomb %d idle\n", i );
                    }
                }
            }

            for( int i = 0; i < g_nBossBombRow; i++ ) {
                for( int j = 0; j < g_nBossBombColCount[ i ]; j++ ) {
                    if( g_BossBomb[ i ][ j ].state == EBS_EXPLODE ) {
                        g_BossBomb[ i ][ j ].nExplodeCursor++;
                        g_BossBomb[ i ][ j ].nExplodeCursor %= g_BossBomb[ i ][ j ].nExplodeTime;

                        if( g_BossBomb[ i ][ j ].nExplodeCursor == 0 ) {
                            g_BossBomb[ i ][ j ].state = EBS_IDLE;
    //                        printf( "Bomb %d idle\n", i );
                        }
                    }
                }
            }
        }
    // process the keyboard event
    }
}

int CheckBossBomb( const Position CharPos )
{
//    printf( "check1\n" );
    if( g_bStartFalling == false ) {
        return 0;
    }
    int nHarm = 0;
//    printf( "check2\n" );

    // check bomb
    for( int i = 0; i < g_nBossBombRow; i++ ) {
        for( int j = 0; j < g_nBossBombColCount[ i ]; j++ ) {
            if( g_BossBomb[ i ][ j ].state == EBS_FLY ) {
                Position MonsterPos;

                MonsterPos.e = g_BossBomb[ i ][ j ].x + g_BossBomb[ i ][ j ].width - 8; // shrink boarder
                MonsterPos.s = g_BossBomb[ i ][ j ].y + g_BossBomb[ i ][ j ].height - 8;
                MonsterPos.w = g_BossBomb[ i ][ j ].x + 8;
                MonsterPos.n = g_BossBomb[ i ][ j ].y + 8;

                if( CheckOverlap( &CharPos, &MonsterPos ) == true ) {
                    g_BossBomb[ i ][ j ].state = EBS_EXPLODE;
                    nHarm += 1;
                }
            }
        }
    }
    return nHarm;
}
