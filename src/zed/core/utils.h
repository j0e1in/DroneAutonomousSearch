#ifndef UTILS_H
#define UTILS_H

#include "glob_var.h"

void update_grids(int blks);
bool coord_cmp(coord_t l, coord_t r);
bool grid_index_cmp(grid_index_t l, grid_index_t r);
int convert_pxl_to_len(const int pxl, const char dim);
coord_t convert_pos_to_pxl(const coord3d_t pos);
coord3d_t convert_pxl_to_pos(const coord_t pxl, int dist);
int find_area_min_dist(area_t area);
void skip();

#endif