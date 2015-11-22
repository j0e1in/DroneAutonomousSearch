var ardrone = require('ar-drone').createClient();
var Navdata = require('./navdata');
var sleep = require('sleep');
var wait = require('wait.for');
var abs = Math.abs;
var sign = Math.sign;

var MaxVelocity = 500;
var GoalThresh = 30;
var CalibThresh = 80; // mm
var CloseRange = GoalThresh + 200;
var velStopThresh = 140;

var MaxAcc = 0.1;
var MaxYaw = 0.1;
var DegThresh = 5;


ardrone.config('general:navdata_demo', 'TRUE');
ardrone.config('CONTROL:control_level', '1');

navdata = new Navdata(ardrone);
setTimeout(function(){

	ardrone.disableEmergency();
	console.log("Battery:", navdata.getvar('battery'));

	ardrone.takeoff();
	ardrone.stop();
	ardrone
	.after(4000, function(){
		console.log("start moving...");
		move({x:80, y:0, z:0}, function(){
			ardrone.stop();
			ardrone.after(5000, function(){
				move({x:-80, y:0, z:0}, function(){
					ardrone.land();
				});
			});
			// var deg_s = navdata.getvar('clockwiseDegrees');
			// turn(180, function(){
			// 	var deg_e = navdata.getvar('clockwiseDegrees');
			// 	console.log("deg_s:", deg_s, "deg_e:", deg_e);
			// 	move({x:80, y:0, z:0}, function(){
			// 		ardrone.land();
			// 	});
			// });
		});
	});

}, 1500);


function speed(velocity, dist, calib_mode){
	/**
	 * Calculate proper speed (-0.6~0.2) via current velocity and distance to target.
	 * 	Move foreward:
	 * 		0.2: 	dist > DistRange.long(mm), 									velocity < VelLimit.long(mm/s)
	 * 		0.1: 	DistRange.mid < dist <= DistRange.long, 		velocity < VelLimit.mid
	 * 		0.05: DistRange.short < dist <= DistRange.mid, 		velocity < VelLimit.short
	 *  	0.02: DistRange.thresh < dist <= DistRange.short, velocity < VelLimit.short
	 *  	0: 		dist <= DistRange.thresh, 									velocity < velThresh
	 *
	 * 	Hold back: ### USE DIFFERENT STANDARD FOR EACH RANGE ###
	 * 		INF-Long
	 *  		-0.3: 1		 <= velocity/vel_limit < 1.25
	 *  		-0.4: 1.25 <= velocity/vel_limit < 1.5
	 *  		-0.5: 1.5	 <= velocity/vel_limit < 1.75
	 *  		-0.6: 1.75 <= velocity/vel_limit < 2
	 *  		-0.7: 2		 <= velocity/vel_limit
	 *    Long-Mid:
	 *  		-0.1: 1		 <= velocity/vel_limit < 1.25
	 *  		-0.2: 1.25 <= velocity/vel_limit < 1.5
	 *  		-0.3: 1.5	 <= velocity/vel_limit < 1.75
	 *  		-0.4: 1.75 <=	velocity/vel_limit
	 *    Mid-Short:
	 *    	-0.05: 1	 <= velocity/vel_limit < 1.25
	 *  		-0.1: 1.25 <= velocity/vel_limit < 1.5
	 *  		-0.2: 1.5	 <= velocity/vel_limit < 1.75
	 *  		-0.3: 1.75 <=	velocity/vel_limit
	 *    Short-Thresh:
	 *    	-0.02: 1		<= velocity/vel_limit < 1.25
	 *  		-0.05: 1.25 <= velocity/vel_limit < 1.5
	 *  		-0.1:  1.5	<= velocity/vel_limit < 1.75
	 *  		-0.15: 1.75 <= velocity/vel_limit
	 *
	 * 	Calibration mode:
	 * 		tolerent dist: 80
	 * 		tolerent vel:  140
	 * 		-0.05: 	1			<= velocity/vel_limit < 1.25
	 * 		-0.1: 	1.25	<= velocity/vel_limit < 1.5
	 * 		-0.15: 	1.5		<= velocity/vel_limit < 1.75
	 * 		-0.2: 	1.75	<= velocity/vel_limit
	 */
	var VelLimit = {long:800, mid:600, short:300, thresh:80, calib:velStopThresh};
	var DistRange = {long:3000, mid:1000, short:500, thresh:GoalThresh, calib:CalibThresh};
	var vel_limit;
	var abs_dist = abs(dist);
	var speed = 0;
	var calib = (typeof calib_mode !== 'undefined' && calib_mode == true) ? true : false;

	// find velocity limit in the dist range
	if (abs_dist > DistRange.long){
		vel_limit = VelLimit.long;
	}else if (abs_dist <= DistRange.long && abs_dist > DistRange.mid){
		vel_limit = VelLimit.mid;
	}else if (abs_dist <= DistRange.mid && abs_dist > DistRange.short){
		vel_limit = VelLimit.short;
	}else if (abs_dist <= DistRange.thresh){
		vel_limit = VelLimit.thresh;
	}
	if (calib) {
		vel_limit = VelLimit.calib;
	}

	if (abs(velocity) >= vel_limit) {
		// handle velocity that exceeds limit

		var velFactor = abs(velocity)/vel_limit;
		// convert velFactor to corresponding level
		var categ = parseInt(Math.round((velFactor-1)/0.25).toFixed(0))+1;

		if (vel_limit === VelLimit.long) {
			if (categ >= 5)	categ = 5; // INF-long's case
		}else if (categ >= 4) {
			categ = 4;	// other cases
		}

		if (vel_limit === VelLimit.long) {
			switch(categ){
				case 1:
					speed = -0.2;
					break;
				case 2:
					speed = -0.3;
					break;
				case 3:
					speed = -0.4;
					break;
				case 4:
					speed = -0.5;
					break;
				case 5:
					speed = -0.6;
					break;
				default: console.log("ERROR: categ out of range.");
			}
		}else if (vel_limit === VelLimit.mid) {
			switch(categ){
				case 1:
					speed = -0.05;
					break;
				case 2:
					speed = -0.08;
					break;
				case 3:
					speed = -0.1;
					break;
				case 4:
					speed = -0.2;
					break;
				default: console.log("ERROR: categ out of range.");
			}
		}else if (vel_limit === VelLimit.short) {
			switch(categ){
				case 1:
					speed = -0.05;
					break;
				case 2:
					speed = -0.05;
					break;
				case 3:
					speed = -0.08;
					break;
				case 4:
					speed = -0.1;
					break;
				default: console.log("ERROR: categ out of range.");
			}
		}else if (vel_limit === VelLimit.thresh) {
			switch(categ){
				case 1:
					speed = -0.02;
					break;
				case 2:
					speed = -0.02;
					break;
				case 3:
					speed = -0.05;
					break;
				case 4:
					speed = -0.08;
					break;
				default: console.log("ERROR: categ out of range.");
			}
		}else if (vel_limit === VelLimit.calib) {
			switch(categ){
				case 1:
					speed = -0.05;
					break;
				case 2:
					speed = -0.1;
					break;
				case 3:
					speed = -0.15;
					break;
				case 4:
					speed = -0.2;
					break;
				default: console.log("ERROR: categ out of range.");
			}
		}else{
			console.log("ERROR: vel_limit does not belong to any distance.");
			// program_pause();
		}

		speed = parseFloat(speed);
		if ((velocity < 0 && dist > 0)
		 || (velocity > 0 && dist < 0)){
			speed = -speed;
		}

	}else{

		// set appropriate velocity for the dist range
		if (abs_dist > DistRange.long) {
			speed = 0.2;
		}else if (abs_dist > DistRange.mid && abs_dist <= DistRange.long){
			speed = 0.1;
		}else if (abs_dist > DistRange.short && abs_dist <= DistRange.mid){
			speed = 0.08;
		}else if (abs_dist > DistRange.thresh && abs_dist <= DistRange.short){
			speed = 0.05;
		}else if (abs_dist <= DistRange.thresh){
			speed = 0;
		}
		if (calib && abs_dist <= DistRange.calib){
			speed = 0.05;
		}
		speed = parseFloat(speed);
		if (dist < 0 && speed != 0)
			speed = -speed;
	}

	return speed.toFixed(2);
}

function move_v2(_goal, callback){
	var dist_left = {x:_goal.x*10, y:_goal.y*10}; // convert cm to mm
	var velocity = {x:0, y:0};
	var spd = {x:0, y:0};
	var distThresh = {
  	x:(abs(dist_left.x) > 0) ? GoalThresh : CalibThresh,
  	y:(abs(dist_left.y) > 0) ? GoalThresh : CalibThresh
  };
  var calib_mode = {x:false, y:false};

	var intervalID = setInterval(function(){
		console.log("in interval");
		if (!navdata.getvar('flying')) {
  		// console.log("Drone is not flying, exiting process...");
  		clearInterval(intervalID);
  		process.exit(1);
  	}
  	if(abs(dist_left.x) > distThresh.x || abs(dist_left.y) > distThresh.y
  	   || abs(velocity.x) > velStopThresh || abs(velocity.y) > velStopThresh)
  	{
  		if (!navdata.isAvail()) {
  			ardrone.stop();
  			ardrone.land();
  			// console.log("navdata not available, exiting process...");
				sleep.sleep(1);
				process.exit(1);
  		}

  		if (abs(dist_left.x) < distThresh.x && abs(dist_left.y) < distThresh.y
  		    && (abs(velocity.x) > velStopThresh || abs(velocity.y) > velStopThresh))
  		{
  			// console.log("Reached goal but speed is not < threshold.");
  		}

  		velocity.x = navdata.getvar('velocity').x;
  		velocity.y = navdata.getvar('velocity').y + 50;
  		dist_left.x -= navdata.getvar('ref_time')*velocity.x;
  		dist_left.y -= navdata.getvar('ref_time')*velocity.y;
  		spd.x = parseFloat(speed(velocity.x, dist_left.x));
  		spd.y = parseFloat(speed(velocity.y, dist_left.y));
  		// console.log("--------------");
		  // console.log("+ velocity: ", velocity);
		  // console.log("@ frontBackDegrees: ", navdata.getvar('frontBackDegrees'));
		  // console.log("@ leftRightDegrees: ", navdata.getvar('leftRightDegrees'));
		  // console.log("@ clockwiseDegrees: ", navdata.getvar('clockwiseDegrees'));
		  // console.log("- dist_left.x: ", dist_left.x);
		  // console.log("- dist_left.y: ", dist_left.y);
  		// console.log("==spd: ", spd);

  		if (calib_mode.x == false && abs(dist_left.x) < distThresh.x) {
  			calib_mode.x = true;
  			distThresh.x = CalibThresh;
  		}
  		if (calib_mode.y == false && abs(dist_left.y) < distThresh.y) {
  			calib_mode.y = true;
  			distThresh.y = CalibThresh;
  		}

  		if (spd.x != 0) {
  			ardrone.front(spd.x);
  		}else if (!calib_mode.x) {
  			ardrone.front(0);
  			ardrone.back(0);
  		}

  		if (spd.y != 0) {
  			ardrone.right(spd.y);
  		}else if (!calib_mode.y) {
  			ardrone.right(0);
  			ardrone.left(0);
  		}

  	}else{
			console.log("Drone reached goal.");
			ardrone.stop();
			// sleep.sleep(3);
			ardrone.after(8000, function(){
				console.log("calling callback");
				callback();
				clearInterval(intervalID);
			});
			// console.log("calling callback");
			// callback();
			// clearInterval(intervalID);
			console.log("out of after");
		}

	}, navdata.getvar('ref_time')*1000);
}


function turn(diff_deg, callback){
	var prev_deg = navdata.getvar('clockwiseDegrees');
	var cur_deg = prev_deg;
	var diff = 0;
	/**
	 * - turn right: deg+ 179->-179, -1->0
	 * - turn left: deg- -179->179, 1->0
	 */
	ardrone.stop();
	// ardrone.front(0);
	// ardrone.left(0);

	if (diff_deg === 0) {
		callback();
		return 0;
	}

	var intervalID = setInterval(function(){
		cur_deg = navdata.getvar('clockwiseDegrees');

		if (sign(prev_deg) !== sign(cur_deg)
		    && isCross180deg(diff_deg, prev_deg, cur_deg)) {
				diff = 360 - abs(cur_deg - prev_deg);
		}else{
				diff = abs(cur_deg - prev_deg);
		}
		console.log("deg turned:", diff);
		if (abs(diff-abs(diff_deg)) <= DegThresh) {
			// within taget degree tolerance range
			ardrone.stop();
			setTimeout(function(){
	  		callback();
  		}, 500);
  		sleep.usleep(500*1000);
			clearInterval(intervalID);
		}else{
			if (diff_deg > 0) {
				ardrone.clockwise(MaxYaw);
				console.log("ardrone.clockwise(MaxYaw)");
			}else{
				ardrone.counterClockwise(MaxYaw);
				console.log("ardrone.counterClockwise(MaxYaw)");
			}
		}

	}, navdata.getvar('ref_time')*1000);
}


function isCross180deg(diff_deg, prev_deg, cur_deg){
	if (diff_deg > 0) {
		return (prev_deg > 0 && cur_deg < 0) ? true : false;
	}else{ //diff_deg < 0
		return (prev_deg < 0 && cur_deg > 0) ? true : false;
	}
}



// ardrone
// 	.after(5000, function(){
// 		this.front(0.1);
// 	})
// 	.after(2000, function(){
// 		this.back(0.1);
// 	})
// 	.after(2000, function(){
// 		this.land();
// 	});

/**
 * Notes:
 * 		frontbackDegrees: back > 0, front < 0
 * 		leftrightDegrees: right > 0, left < 0
 */

// function calibrate(){
// 	ardrone.stop();
// 	ardrone
// 	.after(3000, function(){
// 		this.calibrate(0);
// 	})
// 	.after(3000, function(){
// 		this.stop();
// 		this.land();
// 	});
// }
//
//

function move(_goal, callback){
  var dist_left = {x:_goal.x*10, y:_goal.y*10}; // cm to mm
  var x_acc = MaxAcc, y_acc = MaxAcc;
  var velocity = {x:0, y:0};
  var velThresh = 80;
  var distThresh = {
  	x:(abs(dist_left.x) > 0) ? GoalThresh : CalibThresh,
  	y:(abs(dist_left.y) > 0) ? GoalThresh : CalibThresh
  };

  var intervalID = setInterval(function(){
  	if (!navdata.getvar('flying')) {
  		console.log("Drone is not flying, exiting process...");
  		clearInterval(intervalID);
  		process.exit(1);
  	}
  	if(abs(dist_left.x) > distThresh.x || abs(dist_left.y) > distThresh.y){
				if (!navdata.isAvail()) {
					ardrone.stop();
					ardrone.land();
					console.log("navdata not available, exiting process...");
					setTimeout(function(){
						process.exit(1);
					}, 1500);
					sleep.usleep(1500 * 1000);
				}
			  velocity.x = navdata.getvar('velocity').x;
			  velocity.y = navdata.getvar('velocity').y + 50;
			  dist_left.x -= navdata.getvar('ref_time')*velocity.x;
			  dist_left.y -= navdata.getvar('ref_time')*velocity.y;
			  x_acc = MaxAcc;
			  y_acc = MaxAcc;

			  console.log("--------------");
			  console.log("+ velocity: ", velocity);
			  console.log("@ frontBackDegrees: ", navdata.getvar('frontBackDegrees'));
			  console.log("@ leftRightDegrees: ", navdata.getvar('leftRightDegrees'));
			  console.log("@ clockwiseDegrees: ", navdata.getvar('clockwiseDegrees'));
			  console.log("- dist_left.x: ", dist_left.x);
			  console.log("- dist_left.y: ", dist_left.y);

			  if (abs(dist_left.y) > distThresh.y) {
			    // if not reach goal yet
		    	if (dist_left.y > -CloseRange && dist_left < CloseRange) {
			    	if (dist_left.y > 0) {
			    	  ardrone.right(y_acc/4);
			    	  console.log("> set ardrone.right(y_acc/4)");
			    	}else{ // dist_left.y < 0
			    	  ardrone.left(y_acc/4);
			    	  console.log("> set ardrone.left(y_acc/4)");
			    	}
			    }else{
				    if (dist_left.y > 0) {
				      ardrone.right(y_acc);
				      console.log("> set ardrone.right(y_acc)");
				    }else{ // dist_left.y < 0
				      ardrone.left(y_acc);
				      console.log("> set ardrone.left(y_acc)");
				    }
			    }

			    // check if exceeds max velocity
			    if (velocity.y > MaxVelocity) {
			      ardrone.left(y_acc);
			      console.log("> set ardrone.right(0)");
			    }else if (velocity.y < -MaxVelocity) {
			      ardrone.right(y_acc);
			      console.log("> set ardrone.left(0)");
			    }

			  }else{
			  	if (distThresh.y === GoalThresh) {
			  		distThresh.y = CalibThresh;
			  		console.log("set distThresh.y to CalibThresh");
			  	}
			  	if (velocity.y < -velThresh) {
				  	ardrone.right(y_acc);
			  	}else if(velocity.y > velThresh){
			  		ardrone.left(y_acc);
			  	}else{
			  		ardrone.left(0);
			  		ardrone.right(0);
				  	console.log("reset velocity.y");
			  	}
			  }

			  if (abs(dist_left.x) > distThresh.x) {
			    // if not reach goal yet
			    if (dist_left.x > -CloseRange && dist_left < CloseRange) {
			    	if (dist_left.x > 0) {
			    	  ardrone.front(x_acc/4);
			    	  console.log("> set ardrone.front(x_acc/4)");
			    	}else{ // dist_left.x < 0
			    	  ardrone.back(x_acc/4);
			    	  console.log("> set ardrone.back(x_acc/4)");
			    	}
			    }else{
				    if (dist_left.x > 0) {
				      ardrone.front(x_acc);
				      console.log("> set ardrone.front(x_acc)");
				    }else{ // dist_left.x < 0
				      ardrone.back(x_acc);
				      console.log("> set ardrone.back(x_acc)");
				    }
			    }

			    // check if exceeds max velocity
			    if (velocity.x > MaxVelocity) {
			      ardrone.back(x_acc);
			      console.log("> set ardrone.back(x_acc)");
			    }else if (velocity.x < -MaxVelocity) {
			      ardrone.front(x_acc);
			      console.log("> set ardrone.front(x_acc)");
			    }
			  }else{
			  	if (distThresh.x === GoalThresh) {
			  		distThresh.x = CalibThresh;
			  		console.log("set distThresh.x to CalibThresh");
			  	}
			  	if (velocity.x < -velThresh) {
				  	ardrone.front(x_acc);
			  	}else if(velocity.x > velThresh){
			  		ardrone.back(x_acc);
			  	}else{
			  		ardrone.back(0);
			  		ardrone.front(0);
				  	console.log("reset velocity.x");
			  	}
			  }

  	}else{
  		console.log("Drone reached goal.");
  		ardrone.stop();
  		sleep.sleep(2);
			callback();
  		clearInterval(intervalID);
  	}
  }, navdata.getvar('ref_time')*1000); // wait for 0.06s

	// setTimeout(function(){
	// }, 1000);
	// sleep.usleep(1000*1000);
}