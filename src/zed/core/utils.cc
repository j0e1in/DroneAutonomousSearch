#include <iostream>
// #include <unistd.h> // only on linux
#include "utils.h"

// Glut and OpenGL extension (GLEW) for shaders
#include "GL/glew.h"
#include "GL/glut.h"

using namespace std;


void update_grids(const int blks)
{
	/**
	 * Change number of blocks in grids at runtime and initialize them.
	 */

	int i, j;
	int h_wnd = glutGet(GLUT_WINDOW_HEIGHT);
	int w_wnd = glutGet(GLUT_WINDOW_WIDTH);
	w_wnd /= 2; // only right half is displaying depth map

	blks_w = blks;
	blks_h = int((float)blks_w * aspect_ratio + 0.5);
	// clog << "blks_w=" << blks_w << " -- blks_h=" << blks_h << endl;

	float full_blk_h = (float)h / (float)blks_h; // width of each blk
	float full_blk_w = (float)w / (float)blks_w; // height of each blk
	float half_blk_h = full_blk_h / 2.f;
	float half_blk_w = full_blk_w / 2.f;

	//Free up old resources
	if (grid != NULL)
		free(grid);

	if (r_grid != NULL)
		free(r_grid);

	if (valid_classfied.area_grid != NULL)
		free(valid_classfied.area_grid);

	if (valid_areas.areas != NULL)
		free(valid_areas.areas);

	if (danger_areas.areas != NULL)
		free(danger_areas.areas);


	// alloc a matrix for storing block position information for image
	grid = (blk_grid_t**) malloc(sizeof(blk_grid_t*) * blks_h);
	for (i = 0; i < blks_h; ++i)
		grid[i] = (blk_grid_t*) malloc(sizeof(blk_grid_t) * blks_w);

	// alloc a matrix for storing block position information for display
	r_grid = (coord_t**) malloc(sizeof(coord_t*) * blks_h);
	for (i = 0; i < blks_h; ++i)
		r_grid[i] = (coord_t*) malloc(sizeof(coord_t) * blks_w);

	// alloc a matrix for storing area block classification information
	valid_classfied.area_grid = (coord_area_t**) malloc(sizeof(coord_area_t*) * blks_h);
	for (i = 0; i < blks_h; ++i)
		valid_classfied.area_grid[i] = (coord_area_t*) malloc(sizeof(coord_area_t) * blks_w);


	// init grids
	for (i = 0; i < blks_h; ++i){
		for (j = 0; j < blks_w; ++j){

			grid[i][j].x = (int)((float)(j+1)*full_blk_w - half_blk_w); // center of each blk in pxl
			grid[i][j].y = (int)((float)(i+1)*full_blk_h - half_blk_h);
			grid[i][j].dist = 0; // dist measured of each blk
			grid[i][j].prev_dist = 0;
			grid[i][j].valid = 0; // validity of each blk

			// center of each blk on draw window
			r_grid[i][j].x = (int)((float)grid[i][j].x / (float)w * (float)w_wnd) + w_wnd;
			r_grid[i][j].y = (int)((float)grid[i][j].y / (float)h * (float)h_wnd);

			valid_classfied.num_area = 0;
			valid_classfied.area_grid[i][j].x = grid[i][j].x;
			valid_classfied.area_grid[i][j].y = grid[i][j].y;
			valid_classfied.area_grid[i][j].examined = false;
			valid_classfied.area_grid[i][j].area_no = 0;
		}
	}

	valid_areas.areas = (area_t*) malloc(sizeof(area_t)*MaxValidAreas);
	invalid_areas.areas = (area_t*) malloc(sizeof(area_t)*1);
	danger_areas.areas = (area_t*) malloc(sizeof(area_t)*1);

	return;
}


bool coord_cmp(const coord_t l, const coord_t r)
{
	if (l.x == r.x && l.y == r.y)
		return true;
	else
		return false;
}


bool grid_index_cmp(const grid_index_t l, const grid_index_t r)
{
	if (l.h == r.h && l.w == r.w)
		return true;
	else
		return false;
}


int cvt_pxl_to_len(const int pxl, const char dim)
{
	if (valid_classfied.min_dist_avail)
	{
		float cvt_base_w = ScaleRatio_w/(float)w;
		float cvt_base_h = ScaleRatio_h/(float)h;
		int len = 0;

		if (dim == 'w')
			len = (int)((float)pxl * valid_classfied.min_avg_dist * cvt_base_w);
		if (dim == 'h')
			len = (int)((float)pxl * valid_classfied.min_avg_dist * cvt_base_h);

		return len;
	} else {
		cerr << "ERR: cvt_pxl_to_len => valid_classfied.min_dist_avail=false, cannot convert pxl to dim\n";
		return -1;
	}
}

coord_t cvt_pos_to_pxl(const coord3d_t pos)
{
	/**
	 * convert a position of real world (relative) to a pixel point on image.
	 * convert only x & y.
	 */
	float cvt_base_w = ScaleRatio_w/(float)w;
	float cvt_base_h = ScaleRatio_h/(float)h;
	coord_t pxl;

	if (pos._z > 0){
		cerr << "WARN: pos._z is positive, intended pos is backward.\n";
	}

	pxl.x = (int)((float)pos._x/(float)abs(pos._z)/cvt_base_w);
	pxl.y = (int)((float)pos._y/(float)abs(pos._z)/cvt_base_h);

	if (pxl.x < -(h+1)/2 || pxl.x > (h+1)/2){
		cerr << "WARN: pxl.x exceeds border\n";
	}
	if (pxl.y < -(w+1)/2 || pxl.y > (w+1)/2){
		cerr << "WARN: pxl.y exceeds border\n";
	}
	return pxl;
}

coord3d_t cvt_pxl_to_pos(const coord_t pxl, const int dist)
{
	/**
	 * convert a pixel point on image to a position of real world (relative).
	 * convert only x & y.
	 */
	float cvt_base_w = ScaleRatio_w/(float)w;
	float cvt_base_h = ScaleRatio_h/(float)h;
	coord3d_t pos;

	pos._x = (int)((float)pxl.x * (float)dist * cvt_base_w);
	pos._y = (int)((float)pxl.y * (float)dist * cvt_base_h);
	pos._z = dist;
	return pos;
}

int find_area_min_dist(const area_t area)
{
	int min_dist = Inf;
	for (int i = area.topl.h; i < area.btmr.h; ++i)
	{
		for (int j = area.topl.w; j < area.btmr.w; ++j)
		{
			if (grid[i][j].prev_dist < min_dist)
			{
				min_dist = grid[i][j].prev_dist;
			}
		}
	}
	return min_dist;
}

void skip(){ return; }

int getNum_valid_areas(){
	return valid_areas.num_area;
}

bool has_target_obj_found(){
	// TODO
	return false;
}

// void setIntendPos(const char axis, const int val){
// 	if (axis == 'x')
// 		intend_pos._x = val;
// 	if (axis == 'y')
// 		intend_pos._y = val;
// 	if (axis == 'z')
// 		intend_pos._z = val;
// }

void resetIntendPos(){
	intend_pos._x = 0;
	intend_pos._y = 0;
	intend_pos._z = 0;
}

// Convert specified area in pixel to a corresponding area in blocks
area_t cvt_pxl_to_area(const area_t area_pxl){
	area_t area;
	area.topl.w = (area_pxl.topl.w + w/blks_w/2) / (w/blks_w);
	area.btmr.w = (area_pxl.btmr.w + w/blks_w/2) / (w/blks_w);
	area.topl.h = (area_pxl.topl.h + h/blks_h/2) / (h/blks_h);
	area.btmr.h = (area_pxl.btmr.h + h/blks_h/2) / (h/blks_h);

	if (area.topl.w >= area.btmr.w || area.topl.h >= area.btmr.h){
		cerr << "WARN: cvt_pxl_to_area starting blk is >= to ending blk";
	}

	return area;
}
