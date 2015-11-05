#include "glob_var.h"

// log flags
bool log_loopDuration;

int drawDepthInfo = 0;

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
log_t log_opt;