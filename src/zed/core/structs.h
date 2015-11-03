#ifndef STRUCTS_H
#define STRUCTS_H

#include <cstddef>

typedef struct coord_ {
	int x;
	int y;
} coord_t;

typedef struct coord_dist_ {
	int x;		// in pixel
	int y;
	int dist; // in cm
} coord_dist_t;

typedef struct coord3d_ {
	int _x;		// in cm
	int _y;
	int _z;
	/**
	 * x: right
	 * y: up
	 * z: back
	 */
} coord3d_t;

typedef struct coord_valid_ {
	int x;
	int y;
	int prev_dist; // distance in previous frame
	int valid;     // validity of a block
	/**
	 * valid's cases:
	 * 		case =0: not safe to go but no need to move backward (default)
	 * 		case >0: safe to go
	 * 		case <0: need to move backward to prevent crash
	 */
} coord_valid_t;

typedef struct coord_area_ {
	int x;
	int y;
	bool examined;	// has been examined or not
	int area_no;		// the area number it has been classified to
} coord_area_t;

typedef struct area_struct_ {
	int num_area;		// number of areas have been classified in the round
	int min_dist_area_no;
	float min_avg_dist;
	bool min_dist_avail;
	coord_area_t **area_grid = NULL;
} area_struct_t;

typedef struct grid_index_ {
	int h;
	int w;
} grid_index_t;

typedef struct area_ {
	grid_index_t top_l;
	grid_index_t btm_l;
	grid_index_t top_r;
	grid_index_t btm_r;
} area_t;

typedef struct areas_ {
	int min_dist;
	int num_area; // number of areas in current frame
	area_t *areas;
} areas_t;


#endif

