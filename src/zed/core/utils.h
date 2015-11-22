#ifndef UTILS_H
#define UTILS_H

#include "glob_var.h"

void update_grids(const int blks);
bool coord_cmp(const coord_t l, const coord_t r);
bool grid_index_cmp(const grid_index_t l, const grid_index_t r);
int cvt_pxl_to_len(const int pxl, const char dim);
coord_t cvt_pos_to_pxl(const coord3d_t pos);
coord3d_t cvt_pxl_to_pos(const coord_t pxl, const int dist);
int find_area_min_dist(area_t area);
void skip();

int getNum_valid_areas();
bool has_target_obj_found();
// void setIntendPos(const char axis, const int val);
void resetIntendPos();
area_t cvt_pxl_to_area(const area_t area_pxl);

#endif