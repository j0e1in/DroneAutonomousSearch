/**
 * 	Evaluate obstacles' position and size according	to their distance.
 */

#include <iostream>
#include <cmath>
#include "env_eval.h"

using namespace std;

void eval_validity()
{
	// Loop over each block to check its validity
	for (int i = sens_area.topl.h-1; i < sens_area.btmr.h; ++i){
		for (int j = sens_area.topl.w-1; j < sens_area.btmr.w; ++j){
			if (grid[i][j].dist > 0){
				if (grid[i][j].dist > maxSafeDistance){
					grid[i][j].valid = 1;
				}else if (grid[i][j].dist < minSafeDistance){
					grid[i][j].valid = -1;
				}else{
					// minSafeDistance <= grid[i][j].dist <= maxSafeDistance
					grid[i][j].valid = 0;
				}
			}else{ // dist = NAN
				grid[i][j].valid = 1;
			}
		}
	}
}

void detect_danger_blk()
{
	danger_areas.num_area = 0; // reset num_area
	danger_areas.min_dist = Inf;

	// Detect invalid blocks, if one does exist then set num_area to 1
	for (int i = sens_area.topl.h-1; i < sens_area.btmr.h; ++i){
		for (int j = sens_area.topl.w-1; j < sens_area.btmr.w; ++j){
			if (grid[i][j].valid < 0){
				danger_areas.num_area = 1;
				if (grid[i][j].dist < danger_areas.min_dist)
					danger_areas.min_dist = grid[i][j].dist;
				break;
			}
		}
	}
}

void detect_invalid_blk()
{
	invalid_areas.num_area = 0; // reset num_area
	invalid_areas.min_dist = Inf;

	// Detect invalid blocks, if one does exist then set num_area to 1
	for (int i = sens_area.topl.h-1; i < sens_area.btmr.h; ++i){
		for (int j = sens_area.topl.w-1; j < sens_area.btmr.w; ++j){
			if (grid[i][j].valid == 0){
				invalid_areas.num_area = 1;
				if (grid[i][j].dist < invalid_areas.min_dist)
					invalid_areas.min_dist = grid[i][j].dist;
				break;
			}
		}
	}
}

void analyze_depth_map()
{
	eval_validity();

	detect_danger_blk();

	if (log_opt.danger && danger_areas.num_area > 0){
		clog << "danger_areas.num_area=" << danger_areas.num_area << "\n";
		clog << "danger_areas.min_dist=" << danger_areas.min_dist << "\n";
	}

	// detect invalid blks if there is no danger blk
	if (danger_areas.num_area == 0){
		detect_invalid_blk();

		if (log_opt.invalid && invalid_areas.num_area > 0){
			clog << "invalid_areas.num_area=" << invalid_areas.num_area << "\n";
			clog << "invalid_areas.min_dist=" << invalid_areas.min_dist << "\n";
		}
	}

}


