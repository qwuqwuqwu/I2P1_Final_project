#include "global.h"
int FindAndDrawClosestGroundY( int nMid, int nToe );
int FindClosestGround( int *pGroundIdx, int nGroundCount, const int x, const int nToe );
bool IsInGroundRange( int nGroundIdx, const int x );
void FindGround( int *pnFoundGroundIdx, int *pnFoundCount, int x );
int CompareGround( const void *plhs, const void *prhs );
void object_init( void );
void object_draw( void );
void object_destroy( void );
