#ifndef STRUCTS_H
#define STRUCTS_H

#include <cstddef>
#include "opencv2/highgui/highgui.hpp"

typedef struct coord_ {
	int x;
	int y;
} coord_t;

typedef struct blk_grid_ {
	int x;
	int y;
	int dist; // in cm
	int prev_dist; // distance in previous frame
	int valid;     // validity of a block
	/**
	 * valid's cases:
	 * 		case =0: not safe to go but no need to move backward (default)
	 * 		case >0: safe to go
	 * 		case <0: need to move backward to prevent crash
	 */
} blk_grid_t;

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
	int w;
	int h;
} grid_index_t;

typedef struct area_ {
	grid_index_t topl;
	grid_index_t btmr;
} area_t;

typedef struct areas_ {
	int min_dist;	// min dist in the image
	int num_area; // number of areas in current frame
	area_t *areas;
} areas_t;

typedef struct log_ {
	bool loop_time = false;
	bool invalid = false;
	bool danger = false;

	int drawDepthMode = 0;
		// depth mode for drawing:
		// 		0 (none)
		// 		1 (distance)
		// 		2 (area classification)

} log_t;

typedef struct classifiers_ {
	std::vector<std::string> classFilePaths;
	int num_classifiers;
} classifiers_t;


#endif

