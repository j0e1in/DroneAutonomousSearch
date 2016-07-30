#include "glob_var.h"

// log flags
bool log_loopDuration;

int w; // image width (in pixel)
int h; // image height (in pixel)
int blks_w; // #blks in the image on horizontal axis
int blks_h; // #blks in the image on vertical axis
int num_blks_w;
float aspect_ratio;
bool isMovingForward;
bool objDetected;
std::vector<std::string> cascade_files;

blk_grid_t **grid = NULL;
coord_t **r_grid = NULL;
area_t sens_area;
area_t sens_area_pxl;
area_t sens_area_pxl_moving;
area_struct_t valid_classfied;
areas_t valid_areas;
areas_t invalid_areas;
areas_t danger_areas;
coord3d_t intend_pos;
coord3d_t target_pos;
log_t log_opt;