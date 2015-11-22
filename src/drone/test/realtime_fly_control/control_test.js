var ardrone = require('ar-drone').createClient();
var Navdata = require('./navdata');
var sleep = require('sleep');
var abs = Math.abs;
var sign = Math.sign;

var MaxVelocity = 500;
var GoalThresh = 30;
var CloseRange = GoalThresh + 200;
var CalibThresh = 80; // mm
var velThresh = 70; // mm/s
var velStopThresh = 100;
var MaxAcc = 0.1;
var MaxYaw = 0.2;
var DegThresh = 5;


ardrone.config('general:navdata_demo', 'TRUE');

navdata = new Navdata(ardrone);

// navdata.log('velocity');
// navdata.log('clockwiseDegrees');

ardrone.takeoff();
ardrone.stop();
ardrone
	.after(5000, function(){
		this.front(0.1);
	})
	.after(4000, function(){
		turn(180, function(){
			ardrone.stop();
			ardrone.after(2000, function(){
				this.land();
			});
		});
		console.log("hovering");
	});
	.after(3000, function(){
		ardrone.stop();
		console.log("ardrone stopped")
	})

function turn(diff_deg, callback){
	var prev_deg = navdata.getvar('clockwiseDegrees');
	var cur_deg = prev_deg;
	var diff = 0;
	/**
	 * - turn right: deg+ 179->-179, -1->0
	 * - turn left: deg- -179->179, 1->0
	 */
	// ardrone.stop();
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
			ardrone.clockwise(0);
  		// sleep.sleep(1);
  		callback();
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

5
function isCross180deg(diff_deg, prev_deg, cur_deg){
	if (diff_deg > 0) {
		return (prev_deg > 0 && cur_deg < 0) ? true : false;
	}else{ //diff_deg < 0
		return (prev_deg < 0 && cur_deg > 0) ? true : false;
	}
}