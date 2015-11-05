#ifndef GLOB_VAR_H
#define GLOB_VAR_H


#include "structs.h"

// struct coord_;
// struct coord_dist_;
// struct coord_valid_;
// struct coord_area_;
// struct area_struct_;
// struct grid_index_;
// struct area_;
// struct valid_areas_;

// typedef coord_ 				coord_t;
// typedef coord_dist_ 	coord_dist_t;
// typedef coord_valid_ 	coord_valid_t;
// typedef coord_area_ 	coord_area_t;
// typedef area_struct_ 	area_struct_t;
// typedef grid_index_ 	grid_index_t;
// typedef area_ 				area_t;
// typedef valid_areas_ 	areas_t;

/** Constants **/

//constants
const int DefaultBlocks = 22; // max: 30
const float DefaultFPS = 30.0;
const int CameraResolution = 720;
// camera resolution:
// 		480 (640*480)
//		720 (1280*720)
//		1080 (1920*1080)
const int SensingMode = 1;
// sensing mode:
//		1 (FULL)
//	  2 (RAW)

// real width in image of every 1 cm from camera, measured in cm
// const float Dist_Width_Ratio = 17.8/12.5;//= 1.424 //11/7.5 = 1.4667 //20.8/14.4 = 1.4444
// real hieght in image of every 1 cm from camera, measured in cm
// const float Dist_Hieght_Ratio = 9.0/7.8;
/* note: these ratios assume no distortion on images */
// const int FocalLength = 144; // cm
// const float ScaleRatio = (float)FocalLength/100.;
const float ScaleRatio_w = (float)7.1/(float)5.;
const float ScaleRatio_h = (float)9.5/(float)12.;
const int Inf = 999999;

// (w)
// 7.1/5
// 14.2/10

// (h)
// 3.3/4
// 6.3/8
// 9.4/12

/**
 * Rules of validity:
 * 		dist > minSafeDistance: validity = 1
 * 		maxUnsafeDistance < dist < minSafeDistance: validity = 0
 * 		dist < maxUnsafeDistance: validity = -1
 */

const int minCalibrationDist = 50; // min distance zed can analyze
const int defaultSafeDistance = minCalibrationDist + 100; // in cm
const int maxUnsafeDistance = defaultSafeDistance - 30;
const int minSafeDistance = defaultSafeDistance;
const int MaxValidAreas = 20;
const int DroneSafeWidth = 80; // minimal width a drone can pass through
const int DroneSafeHeight = 50; // minimal height a drone can pass through


/** Global variables **/

extern int drawDepthInfo;
// depth info to draw:
// 		0 (none)
// 		1 (distance)
// 		2 (area classification)


// log flags
extern bool log_loopDuration;

extern int w;
extern int h;
extern int blks_w;
extern int blks_h;
extern int num_blks_w;
extern float aspect_ratio;

extern coord_t image_central;
extern coord_t **grid;
extern coord_t **r_grid;
extern coord_dist_t **dist_grid;
extern coord_valid_t **validity_grid;
extern area_struct_t valid_classfied;
extern areas_t valid_areas;
extern areas_t danger_areas;
extern coord3d_t intend_pos;
extern coord3d_t target_pos;
extern log_t log_opt;

int getNum_valid_areas();
bool has_target_obj_found();
void setIntendPos(char axis, int val);
void resetIntendPos();


#endif