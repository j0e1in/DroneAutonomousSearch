#include <iostream>
// #include <unistd.h> // only on linux
#include "utils.h"

// Glut and OpenGL extension (GLEW) for shaders
#include "GL/glew.h"
#include "GL/glut.h"

using namespace std;


void update_grids(int blks)
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

	float full_blk_h = (float)h / (float)blks_h;
	float full_blk_w = (float)w / (float)blks_w;
	float half_blk_h = full_blk_h / (float)2.;
	float half_blk_w = full_blk_w / (float)2.;

	image_central.x = h/2;
	image_central.y = w/2;

	//Free up old resources
	if (grid != NULL)
		free(grid);

	if (r_grid != NULL)
		free(r_grid);

	if (dist_grid != NULL)
		free(dist_grid);

	if (validity_grid != NULL)
		free(validity_grid);

	if (valid_classfied.area_grid != NULL)
		free(valid_classfied.area_grid);

	if (valid_areas.areas != NULL)
		free(valid_areas.areas);

	if (danger_areas.areas != NULL)
		free(danger_areas.areas);


	// alloc a matrix for storing block position information for image
	grid = (coord_t**) malloc(sizeof(coord_t*) * blks_h);
	for (i = 0; i < blks_h; ++i)
		grid[i] = (coord_t*) malloc(sizeof(coord_t) * blks_w);

	// alloc a matrix for storing block position information for display
	r_grid = (coord_t**) malloc(sizeof(coord_t*) * blks_h);
	for (i = 0; i < blks_h; ++i)
		r_grid[i] = (coord_t*) malloc(sizeof(coord_t) * blks_w);

	// alloc a matrix for storing distance information of each block
	dist_grid = (coord_dist_t**) malloc(sizeof(coord_dist_t*) * blks_h);
	for (i = 0; i < blks_h; ++i)
		dist_grid[i] = (coord_dist_t*) malloc(sizeof(coord_dist_t) * blks_w);

	// alloc a matrix for storing block validity information of each block
	validity_grid = (coord_valid_t**) malloc(sizeof(coord_valid_t*) * blks_h);
	for (i = 0; i < blks_h; ++i)
		validity_grid[i] = (coord_valid_t*) malloc(sizeof(coord_valid_t) * blks_w);

	// alloc a matrix for storing area block classification information
	valid_classfied.area_grid = (coord_area_t**) malloc(sizeof(coord_area_t*) * blks_h);
	for (i = 0; i < blks_h; ++i)
		valid_classfied.area_grid[i] = (coord_area_t*) malloc(sizeof(coord_area_t) * blks_w);


	// init position matrix same as real image size
	for (i = 0; i < blks_h; ++i){
		for (j = 0; j < blks_w; ++j){
			grid[i][j].x = (int)((float)(j+1)*full_blk_w - half_blk_w);
			grid[i][j].y = (int)((float)(i+1)*full_blk_h - half_blk_h);

			r_grid[i][j].x = (int)((float)grid[i][j].x / (float)w * (float)w_wnd) + w_wnd;
			r_grid[i][j].y = (int)((float)grid[i][j].y / (float)h * (float)h_wnd);

			dist_grid[i][j].x = grid[i][j].x;
			dist_grid[i][j].y = grid[i][j].y;
			dist_grid[i][j].dist = 0;

			validity_grid[i][j].x = grid[i][j].x;
			validity_grid[i][j].y = grid[i][j].y;
			validity_grid[i][j].prev_dist = 0;
			validity_grid[i][j].valid = 0;

			valid_classfied.num_area = 0;
			valid_classfied.area_grid[i][j].x = grid[i][j].x;
			valid_classfied.area_grid[i][j].y = grid[i][j].y;
			valid_classfied.area_grid[i][j].examined = false;
			valid_classfied.area_grid[i][j].area_no = 0;
		}
	}

	valid_areas.areas = (area_t*) malloc(sizeof(area_t)*MaxValidAreas);
	danger_areas.areas = (area_t*) malloc(sizeof(area_t)*MaxValidAreas);

	return;
}


bool coord_cmp(coord_t l, coord_t r)
{
	if (l.x == r.x && l.y == r.y)
		return true;
	else
		return false;
}


bool grid_index_cmp(grid_index_t l, grid_index_t r)
{
	if (l.h == r.h && l.w == r.w)
		return true;
	else
		return false;
}


int convert_pxl_to_len(const int pxl, const char dim)
{
	if (valid_classfied.min_dist_avail)
	{
		float convert_base_w = ScaleRatio_w/(float)w;
		float convert_base_h = ScaleRatio_h/(float)h;
		int len = 0;

		if (dim == 'w')
			len = (int)((float)pxl * valid_classfied.min_avg_dist * convert_base_w);
		if (dim == 'h')
			len = (int)((float)pxl * valid_classfied.min_avg_dist * convert_base_h);

		return len;
	} else {
		cerr << "ERR: convert_pxl_to_len => valid_classfied.min_dist_avail=false, cannot convert pxl to dim\n";
		return -1;
	}
}

coord_t convert_pos_to_pxl(const coord3d_t pos)
{
	/**
	 * Convert a position of real world (relative) to a pixel point on image.
	 * Convert only x & y.
	 */
	float convert_base_w = ScaleRatio_w/(float)w;
	float convert_base_h = ScaleRatio_h/(float)h;
	coord_t pxl;

	if (pos._z > 0){
		cerr << "WARN: pos._z is positive, intended pos is backward.\n";
	}

	pxl.x = (int)((float)pos._x/(float)abs(pos._z)/convert_base_w);
	pxl.y = (int)((float)pos._y/(float)abs(pos._z)/convert_base_h);

	if (pxl.x < -(h+1)/2 || pxl.x > (h+1)/2){
		cerr << "WARN: pxl.x exceeds border\n";
	}
	if (pxl.y < -(w+1)/2 || pxl.y > (w+1)/2){
		cerr << "WARN: pxl.y exceeds border\n";
	}
	return pxl;
}

coord3d_t convert_pxl_to_pos(const coord_t pxl, int dist)
{
	/**
	 * Convert a pixel point on image to a position of real world (relative).
	 * Convert only x & y.
	 */
	float convert_base_w = ScaleRatio_w/(float)w;
	float convert_base_h = ScaleRatio_h/(float)h;
	coord3d_t pos;

	pos._x = (int)((float)pxl.x * (float)dist * convert_base_w);
	pos._y = (int)((float)pxl.y * (float)dist * convert_base_h);
	pos._z = dist;
	return pos;
}

int find_area_min_dist(area_t area)
{
	int min_dist = Inf;
	for (int i = area.top_l.h; i < area.btm_r.h; ++i)
	{
		for (int j = area.top_l.w; j < area.btm_r.w; ++j)
		{
			if (validity_grid[i][j].prev_dist < min_dist)
			{
				min_dist = validity_grid[i][j].prev_dist;
			}
		}
	}
	return min_dist;
}

void skip(){ return; }