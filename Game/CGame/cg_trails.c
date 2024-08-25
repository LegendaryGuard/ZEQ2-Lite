#include "cg_local.h"

#define TRAIL_SEGMENTS		100

typedef struct {
	vec3_t		segments[TRAIL_SEGMENTS];
	int			numSegments;
} trail_t;

static trail_t cg_trails[MAX_GENTITIES];

/*
===============
CG_InitTrails

Initializes the array of trails for all centities.
Should be called from CG_Init in cg_main.c
===============
*/
void CG_InitTrails( void ) {
	memset( &cg_trails, 0, sizeof(cg_trails) );
}

/*
===============
CG_ResetTrail

Reset entity's trail.
Should be called whenever an entity that has to use a trail, wasn't in the PVS the previous frame.
entityNum: Valid entity number
origin:    Point from where the trail should start.
           (This should be equal to the entity's current position.)
=====================
*/
void CG_ResetTrail( int entityNum, vec3_t origin ) {
	int i;

	for ( i = 0; i < TRAIL_SEGMENTS; i++ ) {
		VectorCopy( origin, cg_trails[entityNum].segments[i] );
	}
	cg_trails[entityNum].numSegments = 0;
}

/*
=====================
CG_Trail

Adds trail segments
=====================
*/
void CG_Trail( int entityNum, vec3_t origin, qboolean remove, int maxNumSegments, float width, qhandle_t hShader, vec3_t color ) {
	int i;
	polyVert_t verts[4];

	if ( !hShader ) {
		return;
	}

	if ( entityNum < 0 || entityNum >= MAX_GENTITIES ) {
		return;
	}

	if ( remove ) { // removes every segment
		cg_trails[entityNum].numSegments--;
	} else {
		if ( cg_trails[entityNum].numSegments < TRAIL_SEGMENTS ) {
			cg_trails[entityNum].numSegments++;
		}
	}

	// shift points down the buffer
	for ( i = cg_trails[entityNum].numSegments - 1; i > 0; i-- ) {
		VectorCopy( cg_trails[entityNum].segments[i - 1], cg_trails[entityNum].segments[i] );
	}

	// add the current position at the start
	VectorCopy( origin, cg_trails[entityNum].segments[0] );

	for ( i = 0; i < cg_trails[entityNum].numSegments - 1; i++ ) {
		vec3_t start, end, forward, right;
		vec3_t viewAxis[3];

		if ( maxNumSegments > TRAIL_SEGMENTS ) {
			maxNumSegments = TRAIL_SEGMENTS;
		}

		if ( i > maxNumSegments ) {
			return;
		}

		VectorCopy( cg_trails[entityNum].segments[i], start );
		VectorCopy( cg_trails[entityNum].segments[i + 1], end );

		VectorSubtract( end, start, forward );
		VectorNormalize( forward );

		VectorSubtract( cg.refdef.vieworg, start, viewAxis[0] );
		CrossProduct( viewAxis[0], forward, right );
		VectorNormalize( right );

		VectorMA( end, width, right, verts[0].xyz );
		verts[0].st[0] = 0;
		verts[0].st[1] = 1;
		verts[0].modulate[0] = color[0] * 255;
		verts[0].modulate[1] = color[1] * 255;
		verts[0].modulate[2] = color[2] * 255;
		verts[0].modulate[3] = 255;

		VectorMA( end, -width, right, verts[1].xyz );
		verts[1].st[0] = 1;
		verts[1].st[1] = 0;
		verts[1].modulate[0] = color[0] * 255;
		verts[1].modulate[1] = color[1] * 255;
		verts[1].modulate[2] = color[2] * 255;
		verts[1].modulate[3] = 255;

		VectorMA( start, -width, right, verts[2].xyz );
		verts[2].st[0] = 1;
		verts[2].st[1] = 0;
		verts[2].modulate[0] = color[0] * 255;
		verts[2].modulate[1] = color[1] * 255;
		verts[2].modulate[2] = color[2] * 255;
		verts[2].modulate[3] = 255;

		VectorMA( start, width, right, verts[3].xyz );
		verts[3].st[0] = 0;
		verts[3].st[1] = 1;
		verts[3].modulate[0] = color[0] * 255;
		verts[3].modulate[1] = color[1] * 255;
		verts[3].modulate[2] = color[2] * 255;
		verts[3].modulate[3] = 255;

		trap_R_AddPolyToScene( hShader, 4, verts );
	}
}
