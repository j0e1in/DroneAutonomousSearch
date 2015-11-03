#include "glob_var.h"

int w; // image width (in pixel)
int h; // image height (in pixel)
int blks_w; // #blocks in image on horizontal axis
int blks_h; // #blocks in image on vertical axis
int num_blks_w;
float aspect_ratio;

coord_t image_central;
coord_t **grid = NULL;
coord_t **r_grid = NULL;
coord_dist_t **dist_grid = NULL;
coord_valid_t **validity_grid = NULL;
area_struct_t valid_classfied;
areas_t valid_areas;
areas_t danger_areas;
coord3d_t intend_pos;
coord3d_t target_pos;


int getNum_valid_areas(){
	return valid_areas.num_area;
}

bool has_target_obj_found(){
	// remember to add if stats
	return false;
}

void setIntendPos(char axis, int val){
	if (axis == 'x')
		intend_pos._x = val;
	if (axis == 'y')
		intend_pos._y = val;
	if (axis == 'z')
		intend_pos._z = val;
}

void resetIntendPos(){
	intend_pos._x = 0;
	intend_pos._y = 0;
	intend_pos._z = 0;
}
