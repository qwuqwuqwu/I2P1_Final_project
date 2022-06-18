#include "global.h"
bool CheckOverlap( const Position *plhs, const Position *prhs );
int CompSide( const void * lhs, const void *rhs );
int CompSide( const void * lhs, const void *rhs );
bool AdvCheckOverlap( const Position *plhs, const Position *prhs, EOrientationDirection *pOrientation, int *pClampValue );
bool CheckBlocker( Position *pPos, const bool bDir );
int FindAndDrawClosestGroundY( const int w, const int e, const int s );
int FindClosestGround( int *pGroundIdx, int nGroundCount, const int s );
bool IsInGroundRange( int nGroundIdx, const int w, const int e );
void FindGround( int *pnFoundGroundIdx, int *pnFoundCount, int w, int e );
int CompareGround( const void *plhs, const void *prhs );
void object_init( void );
void object_draw( void );
void object_destroy( void );

void BombThrow( const int x, const int y, const bool bDir );
void Object_process( ALLEGRO_EVENT event );
