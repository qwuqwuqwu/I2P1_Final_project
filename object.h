#include "global.h"
int FindAndDrawClosestGroundY( const int w, const int e, const int s );
int FindClosestGround( int *pGroundIdx, int nGroundCount, const int s );
bool IsInGroundRange( int nGroundIdx, const int w, const int e );
void FindGround( int *pnFoundGroundIdx, int *pnFoundCount, int w, int e );
int CompareGround( const void *plhs, const void *prhs );
void object_init( void );
void object_draw( void );
void object_destroy( void );
