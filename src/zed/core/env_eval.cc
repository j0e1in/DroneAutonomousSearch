/**
 * 	Evaluate obstacles' position and size according	to their distance.
 */

#include <iostream>
#include <cmath>
#include "env_eval.h"

using namespace std;

void eval_validity()
{
	/**
	 * ATTENTION:
	 * 		This function may be buggy. (which may cause crash)
	 * 		Check the conditions in actual fly to debug.
	 *
	 * 		This function currently assumes if an area is invalid,
	 * 		drone won't get close to it. Not yet implement optical
	 * 		flow or drone movement to measure if it's closing to an
	 * 		invalid area.
	 *
	 * TODO:
	 * 		Maybe change 3-level validity to 5-level or more
	 * 		distance level for more accurate block measurement,
	 * 		which also provide distances allowed to move.
	 * 		However it would be more complex and may be unable
	 * 		to such measurements. (zed error should be ~30cm)
	 */

	int i, j;
	int dist;

	// Loop over each block to check its validity
	for (i = 0; i < blks_h; ++i){
		for (j = 0; j < blks_w; ++j){
			dist = dist_grid[i][j].dist;

			if (dist < maxUnsafeDistance){
				// case: invalid
				validity_grid[i][j].valid = -1;
			}
			else if (dist >= minSafeDistance){
				// case: valid
				if (validity_grid[i][j].valid < 0
				    && validity_grid[i][j].prev_dist < minCalibrationDist){
					/**
					 * If previous status is not valid and now suddenly
					 * turn to valid, it means it's getting closer to
					 * a obstacle and the distance is smaller than minimal
					 * distance that zed can analyze.
					 *
					 * In this case, still mark it as -1 to make drone avoid it.
					 *
					 * This shouldn't happen in normal condition, however, because
					 * there is minimal safe distance the drone is allowed to keep.
					 * This only happens if external forces apply to it or some
					 * functionality errors, such as wind or unstable flight.
					 *
					 *
					 */

					// if (isClosing()) {
					// 	validity_grid[i][j].valid = -1;
					// } else {
					// 	validity_grid[i][j].valid = 1;
					// }
					validity_grid[i][j].valid = 1;

				} else {
					validity_grid[i][j].valid = 1;
				}
			}
			else{
				// case: neither
				if (validity_grid[i][j].valid < 0
				    && validity_grid[i][j].prev_dist < minCalibrationDist){
					/**
					 * This case may also cause the condition decribed above
					 * (get closer to a obstacle even if its status is not valid),
					 * but also can be caused by leaving obstacle.
					 * Hence this case is more complicated, which should be examined by
					 * the actual move a drone did.
					 */

					/**

						TODO:
						- Get drone move feedback
						- Use the feedback to decide validity

					**/

					// if (isClosing()) {
					// 	validity_grid[i][j].valid = -1;
					// } else {
					// 	validity_grid[i][j].valid = 0;
					// }
					validity_grid[i][j].valid = 0;

				} else {
					validity_grid[i][j].valid = 0;
				}
			}

			validity_grid[i][j].prev_dist = dist;
		}
	}
}


void find_neighbor_with_same_level(int i, int j, int cur_area_no, float *avg_dist, int *num_blks_area)
{
	/**
	 * Examine each nieghbor block whether its validity is same as current block.
	 */

	if (i < 0 || j < 0 || i >= blks_h || j >= blks_w)
		return;

	// upper left
	if ((i-1 >= 0 && j-1 >= 0)
	    && valid_classfied.area_grid[i-1][j-1].examined == false
	    && validity_grid[i][j].valid == validity_grid[i-1][j-1].valid)
	{
		valid_classfied.area_grid[i-1][j-1].area_no = cur_area_no;
		valid_classfied.area_grid[i-1][j-1].examined = true;
		*avg_dist += (float)validity_grid[i-1][j-1].prev_dist;
		*num_blks_area += 1;
		find_neighbor_with_same_level(i-1, j-1, cur_area_no, avg_dist, num_blks_area);
	}

	// upper middle
	if ((i-1 >= 0)
	    && valid_classfied.area_grid[i-1][j].examined == false
	    && validity_grid[i][j].valid == validity_grid[i-1][j].valid)
	{
		valid_classfied.area_grid[i-1][j].area_no = cur_area_no;
		valid_classfied.area_grid[i-1][j].examined = true;
		*avg_dist += (float)validity_grid[i-1][j].prev_dist;
		*num_blks_area += 1;
		find_neighbor_with_same_level(i-1, j, cur_area_no, avg_dist, num_blks_area);
	}

	// upper right
	if ((i-1 >= 0 && j+1 < blks_w)
	    && valid_classfied.area_grid[i-1][j+1].examined == false
	    && validity_grid[i][j].valid == validity_grid[i-1][j+1].valid)
	{
		valid_classfied.area_grid[i-1][j+1].area_no = cur_area_no;
		valid_classfied.area_grid[i-1][j+1].examined = true;
		*avg_dist += (float)validity_grid[i-1][j+1].prev_dist;
		*num_blks_area += 1;
		find_neighbor_with_same_level(i-1, j+1, cur_area_no, avg_dist, num_blks_area);
	}

	// middle left
	if ((j-1 >= 0)
	    && valid_classfied.area_grid[i][j-1].examined == false
	    && validity_grid[i][j].valid == validity_grid[i][j-1].valid)
	{
		valid_classfied.area_grid[i][j-1].area_no = cur_area_no;
		valid_classfied.area_grid[i][j-1].examined = true;
		*avg_dist += (float)validity_grid[i][j-1].prev_dist;
		*num_blks_area += 1;
		find_neighbor_with_same_level(i, j-1, cur_area_no, avg_dist, num_blks_area);
	}

	// middle middle (skip)

	// middle right
	if ((j+1 < blks_w)
	    && valid_classfied.area_grid[i][j+1].examined == false
	    && validity_grid[i][j].valid == validity_grid[i][j+1].valid)
	{
		valid_classfied.area_grid[i][j+1].area_no = cur_area_no;
		valid_classfied.area_grid[i][j+1].examined = true;
		*avg_dist += (float)validity_grid[i][j+1].prev_dist;
		*num_blks_area += 1;
		find_neighbor_with_same_level(i, j+1, cur_area_no, avg_dist, num_blks_area);
	}

	// lower left
	if ((i+1 < blks_h && j-1 >= 0)
	    && valid_classfied.area_grid[i+1][j-1].examined == false
	    && validity_grid[i][j].valid == validity_grid[i+1][j-1].valid)
	{
		valid_classfied.area_grid[i+1][j-1].area_no = cur_area_no;
		valid_classfied.area_grid[i+1][j-1].examined = true;
		*avg_dist += (float)validity_grid[i+1][j-1].prev_dist;
		*num_blks_area += 1;
		find_neighbor_with_same_level(i+1, j-1, cur_area_no, avg_dist, num_blks_area);
	}

	// lower middle
	if ((i+1 < blks_h)
	    && valid_classfied.area_grid[i+1][j].examined == false
	    && validity_grid[i][j].valid == validity_grid[i+1][j].valid)
	{
		valid_classfied.area_grid[i+1][j].area_no = cur_area_no;
		valid_classfied.area_grid[i+1][j].examined = true;
		*avg_dist += (float)validity_grid[i+1][j].prev_dist;
		*num_blks_area += 1;
		find_neighbor_with_same_level(i+1, j, cur_area_no, avg_dist, num_blks_area);
	}

	// lower right
	if ((i+1 < blks_h && j+1 < blks_w)
	    && valid_classfied.area_grid[i+1][j+1].examined == false
	    && validity_grid[i][j].valid == validity_grid[i+1][j+1].valid)
	{
		valid_classfied.area_grid[i+1][j+1].area_no = cur_area_no;
		valid_classfied.area_grid[i+1][j+1].examined = true;
		*avg_dist += (float)validity_grid[i+1][j+1].prev_dist;
		*num_blks_area += 1;
		find_neighbor_with_same_level(i+1, j+1, cur_area_no, avg_dist, num_blks_area);
	}
}

void classify_validity_area()
{
	/**
	 * This function repeats the following job:
	 * 		Examines a block and check its neighbor blocks' validity,
	 * 		classifying an area of blocks with same validity as a group,
	 * 		and then calculate the actual size of an area by the average distance of it.
	 *    Also Calculates average distance for each area,
	 *    and choose the minimal as reference distance.
	 *    Until all blocks are classified.
	 */

	int i, j;
	int cur_area_no = 1;
	int num_blks_area;
	float avg_dist;
	int min_dist_validity = 0;

	valid_classfied.num_area = 0;
	valid_classfied.min_dist_area_no = -1;
	valid_classfied.min_avg_dist = (float)Inf;
	valid_classfied.min_dist_avail = true;
	/* Important: remeber to check whether `valid_classfied.min_dist_avail`
	   is `true` before using `valid_classfied.min_avg_dist` in other places. */

	for (i = 0; i < blks_h; ++i){
		for (j = 0; j < blks_w; ++j){
			valid_classfied.area_grid[i][j].area_no = 0;
			valid_classfied.area_grid[i][j].examined = false;
		}
	}

	for (i = 0; i < blks_h; ++i){
		for (j = 0; j < blks_w; ++j){
			if (valid_classfied.area_grid[i][j].examined == false)
			{
				avg_dist = 0.;
				num_blks_area = 0;

				find_neighbor_with_same_level(i, j, cur_area_no, &avg_dist, &num_blks_area);

				avg_dist /= (float)num_blks_area;
				if (avg_dist >= minCalibrationDist
				    && num_blks_area >= (int)((float)blks_h * (float)blks_w * 0.05)
				    && avg_dist < valid_classfied.min_avg_dist)
				{
					valid_classfied.min_avg_dist = avg_dist;
					valid_classfied.min_dist_area_no = cur_area_no;
					min_dist_validity = validity_grid[i][j].valid;
				}

				cur_area_no++;
			}
		}
	}

	// Set min dist availability to false to prevent others from using it
	if (valid_classfied.min_avg_dist == (float)Inf){
		valid_classfied.min_dist_avail = false;
		cerr << "WARN: min_avg_dist is Inf\n";
	}
	else if (min_dist_validity > 0) {
		/*all blocks in image are valid to move*/
		valid_classfied.min_dist_avail = false;
	}
}


bool isTopLeftBlk(int i, int j)
{
	/**
	 * Check if is top-left block of a valid area.
	 */
	if (i-1 >= 0 && j-1 >= 0)
	{
		if (validity_grid[i-1][j-1].valid <= 0
		    && validity_grid[i-1][j].valid <= 0
		    && validity_grid[i][j-1].valid <= 0)
		{
			return true;
		}
	}else if (i-1 >= 0){
		if (validity_grid[i-1][j].valid <= 0)
		{
			return true;
		}
	}else if (j-1 >= 0){
		if (validity_grid[i][j-1].valid <= 0)
		{
			return true;
		}
	}
	return false;
}


bool isWidthtooShort(grid_index_t l, grid_index_t r)
{
	int l_x = validity_grid[l.h][l.w].x;
	int r_x = validity_grid[r.h][r.w].x;
	int width = r_x - l_x;

	width = convert_pxl_to_len(width, 'w');
	if (width < DroneSafeHeight)
		return true;
	else
		return false;
}

bool isHieghttooShort(grid_index_t top, grid_index_t btm)
{
	int top_y = validity_grid[top.h][top.w].y;
	int btm_y = validity_grid[btm.h][btm.w].y;
	int hieght = btm_y - top_y;

	hieght = convert_pxl_to_len(hieght, 'h');
	if (hieght < DroneSafeHeight)
		return true;
	else
		return false;
}

void search_right_border(grid_index_t* blk, int hlimit=-1)
{
	int k = blk->w;

	while (k < blks_w){
		if (hlimit >= 0 && k+1 > hlimit)
			break;
		else if (k+1 < blks_w && validity_grid[blk->h][k+1].valid > 0)
			k = k+1;
		else
			break;
	}

	if (k > blk->w) blk->w = k;
}

void search_btm_border(grid_index_t* blk, int vlimit=-1)
{
	int k = blk->h;

	while (k < blks_h){
		if (vlimit >= 0 && k+1 > vlimit)
			break;
		if (k+1 < blks_h && validity_grid[k+1][blk->w].valid > 0)
			k = k+1;
		else
			break;
	}

	if (k > blk->h) blk->h = k;
}

void searc_top_border(grid_index_t* blk, int vlimit=-1)
{
	int k = blk->h;

	if (blk->w+1 >= blks_w) {
		cerr << "ERROR: searching top border out of index, fix this bug\n";
		return;
	}
	// check the right neighbor block whether it's invalid
	while (k > 0){
		if (vlimit >= 0 && k-1 < vlimit)
			break;
		if (k-1 >= 0 && validity_grid[k-1][blk->w+1].valid <= 0)
			k = k-1;
		else if (k-1 >= 0 && validity_grid[k-1][blk->w+1].valid > 0){
			k = k-1;
			break;
		}
		else
			break;
	}
	if (k < blk->h) blk->h = k;
}

bool is_whole_area_valid(area_t *area)
{
	// clog << "(" << area->top_l.h << ", " << area->btm_r.h
	     // << ", " << area->top_l.w << ", " << area->top_r.w << ")\n";
	for (int i = area->top_l.h; i <= area->btm_r.h; ++i){
		for (int j = area->top_l.w; j <= area->top_r.w; ++j){
			if (validity_grid[i][j].valid <= 0){
				return false;
			}
		}
	}
	return true;
}

void extract_valid_area()
{
	/**
	 * Use `valid_classfied.min_avg_dist` as reference,
	 * calculate each valid area's size in real world
	 * for path planning.
	 */

	valid_areas.num_area = 0;

	if (!valid_classfied.min_dist_avail)
	{
		if (valid_classfied.min_avg_dist == (float)Inf){
			// no where to move, can only rotate.
		}else{
			// can move to anywhere in sight.
			area_t whole_area;
			whole_area.top_l.h = 0;
			whole_area.top_l.w = 0;
			whole_area.btm_l.h = blks_h;
			whole_area.btm_l.w = 0;
			whole_area.top_r.h = 0;
			whole_area.top_r.w = blks_w;
			whole_area.btm_r.h = blks_h;
			whole_area.btm_r.w = blks_w;

			valid_areas.areas[valid_areas.num_area++] = whole_area;
		}
	}
	else
	{
		grid_index_t top_l_blk = {0, 0};
		grid_index_t top_r_blk = {0, 0};
		grid_index_t btm_l_blk = {0, 0};
		grid_index_t btm_r_blk = {0, 0};

		for (int i = 0; i < blks_h; ++i){
			for (int j = 0; j < blks_w; ++j){
				bool is_tooshort = false;

				/**
				 * If a block's valid then start searching for max size
				 * of a rectangle that allows drone to go through.
				 */
				if (validity_grid[i][j].valid > 0
				    && isTopLeftBlk(i, j))
					// Only start searching if a block is on top-left corner
					// of an valid area.
				{
					top_l_blk.h = i;
					top_l_blk.w = j;
					top_r_blk = top_l_blk;

					// Search top-right border first
					search_right_border(&top_r_blk);

					if (isWidthtooShort(top_l_blk, top_r_blk)){
						continue;
					}
					else
					{
						btm_r_blk = top_r_blk;

						// Then search bottom right border
						search_btm_border(&btm_r_blk);

						if (isHieghttooShort(top_r_blk, btm_r_blk)){
							continue;
						}
						else
						{
							btm_l_blk = top_l_blk;

							// Then search bottom left border
							search_btm_border(&btm_l_blk, btm_r_blk.h);

							if (isHieghttooShort(top_l_blk, btm_l_blk)){
								continue;
							}
							else
							{
								// If hieght of btm-left is shorter than btm-right,
								// then adjust btm-right hieght.
								if (btm_l_blk.h < btm_r_blk.h)
									btm_r_blk.h = btm_l_blk.h;

								// Finally check if no block is invalid is in the area
								// by searching from btm-left to btm-right.
								// If result is not equal to btm-right then there's block
								// in the middle.
								// *It will adjust btm_l_blk if it finds blocks'
								// y-axis is smaller than itself.

								grid_index_t tmp_blk;

								do {
									tmp_blk = btm_l_blk;

									search_right_border(&tmp_blk, btm_r_blk.w);

									if (isHieghttooShort(top_l_blk, tmp_blk)){
										is_tooshort = true;
										break;
									}

									if (grid_index_cmp(tmp_blk, btm_r_blk))
										// right search matches btm_r_blk
										break;

									// If search right border not matches btm_r_blk
									// then search top border from current position.
									searc_top_border(&tmp_blk, top_l_blk.h);

									if (isHieghttooShort(top_l_blk, tmp_blk)){
										is_tooshort = true;
										break;
									}
									else if (tmp_blk.h < btm_l_blk.h){
										btm_l_blk.h = tmp_blk.h;
										btm_r_blk.h = tmp_blk.h;
									}
									// usleep(500000);
								} while(!grid_index_cmp(tmp_blk, btm_r_blk));
								if (is_tooshort) continue;

								area_t tmp_area;
								tmp_area.top_l = top_l_blk;
								tmp_area.btm_l = btm_l_blk;
								tmp_area.top_r = top_r_blk;
								tmp_area.btm_r = btm_r_blk;

								// check if the area contains no invalid block
								if (is_whole_area_valid(&tmp_area)){
									if (valid_areas.num_area >= MaxValidAreas) {
										cerr << "WARN: Number of areas exceeds maximum\n";
										return;
									}
									valid_areas.areas[valid_areas.num_area++] = tmp_area;
								}
							}
						}
					}
				} // end if is top-left blk (start searching)
			} // end for(blks_w)
		} // end for(blks_h)
	}
}


int find_invalid_min_dist()
{
	int min_dist = Inf;

	for (int i = 0; i < blks_h; ++i){
		for (int j = 0; j < blks_w; ++j){
			if (validity_grid[i][j].valid < 0
			    && validity_grid[i][j].prev_dist < min_dist){
				min_dist = validity_grid[i][j].prev_dist;
			}
		}
	}
	return min_dist;
}

void extract_danger_area()
{
	/**
	 *	Extract areas with validity = -1 letting drone
	 *	to avoid.
	 *
	 *  Note: Current version assumes drone can only move
	 *  			foreward and backward.
	 *  			Hence only need to extract min dist of invalid
	 *  			block and make drone move back to avoid.
	 */

	danger_areas.num_area = 0;
	danger_areas.min_dist = find_invalid_min_dist();
	// clog << "min_dist=" << danger_areas.min_dist << endl;
	if (danger_areas.min_dist < maxUnsafeDistance)
	{
		danger_areas.num_area = 1;
	}
}


void set_target_pos(int _x_, int _y_, int _z_)
{
	target_pos._x = _x_;
	target_pos._y = _y_;
	target_pos._z = _z_;
}

void set_target_pos(coord3d_t pos){
	target_pos = pos;
}

int choose_valid_area(grid_index_t *central_points, int num)
{
	double min_shift = (double)Inf, shift;
	int no = -1;
	coord_t pxl_pos;
	for (int i = 0; i < num; ++i)
	{
		intend_pos._x = 50; // sample intend_pos
		intend_pos._y = 50;
		intend_pos._z = -200;

		// find closest valid central point from inteded position
		pxl_pos = convert_pos_to_pxl(intend_pos);
		shift = pow(grid[central_points[i].h][central_points[i].w].x
		            - w/2
		            - pxl_pos.x, 2);
		shift += pow(grid[central_points[i].h][central_points[i].w].y
		            - h/2
		            - pxl_pos.y, 2);
		if (shift < min_shift)
		{
			min_shift = shift;
			no = i;
		}
	}
	// TODO: handle choosed valid area has opsite direction from intended pos
	return no;
}

void find_target_pos()
{

	if (danger_areas.num_area > 0){

		// Set to move backward to avoid collision
		set_target_pos(0, 0, maxUnsafeDistance - danger_areas.min_dist);
		// clog << "maxUnsafeDistance=" << maxUnsafeDistance << endl;
		// clog << "danger_areas.min_dist=" << danger_areas.min_dist << endl;

	}else if(valid_areas.num_area > 0){
		area_t tmp_area;
		grid_index_t central_points[MaxValidAreas];

		int i;
		for (i = 0; i < valid_areas.num_area; ++i)
		{
			tmp_area = valid_areas.areas[i];

			// Calculate central point of an area
			central_points[i].w = tmp_area.top_l.w
											+ (tmp_area.top_r.w - tmp_area.top_l.w)/2;
			central_points[i].h = tmp_area.top_l.h
			 								+ (tmp_area.btm_l.h - tmp_area.top_l.h)/2;
		}
		int no = choose_valid_area(central_points, i);
		int dist = find_area_min_dist(valid_areas.areas[no]);

		// target_pos._z =
		// 		intend_pos._z
		// 		|| min dist in valid area - minSafeDistance
		// 		|| 0

		// clog << "dist=" << dist << endl;
		// clog << "intend_pos._z=" << intend_pos._z << endl;
		if (dist > -intend_pos._z) // negative means foreward
		{
			dist = intend_pos._z;
		}else{
			// dist should > minSafeDistance
			// since it's min dist from valid area
			dist -= minSafeDistance;
			dist = -dist;
		}
		// clog << "dist=" << dist << endl;

		coord_t pxl;
		pxl.x = grid[central_points[no].h][central_points[no].w].x - w/2;
		pxl.y = grid[central_points[no].h][central_points[no].w].y - h/2;
		set_target_pos(convert_pxl_to_pos(pxl, dist));

	}else{

		// clog << "--no danger_areas or valid_areas\n";
		set_target_pos(0,0,0); // nowhere to move, can only turn left or right

	}


	// clog << "target_pos=("
	// 		 << target_pos._x << ", "
	// 		 << target_pos._y << ", "
	// 		 << target_pos._z << ")\n";
}


void analyze_depth_map()
{
	eval_validity();
	classify_validity_area();
	extract_danger_area();
	// clog << "danger_areas.num_area=" << danger_areas.num_area << endl;

	// If has danger detected, skip extracting valid areas
	if (danger_areas.num_area == 0){
		extract_valid_area();
		// clog << "valid_areas.num_area=" << valid_areas.num_area << endl;
	}
	find_target_pos();
}


