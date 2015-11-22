var ARDrone = require('ar-drone');
var Navdata = require('./navdata');
var sleep = require('sleep');
var abs = Math.abs;

module.exports = FlyController;

function FlyController(zed){
	this._ardrone = ARDrone.createClient();

	this._ardrone.config('general:navdata_demo', 'TRUE');
	// this._ardrone.config('control:outdoor', 'TRUE'); // enables wind estimation
	// this._ardrone.config('control:flight_without_shell', 'TRUE');

	this._navdata = new Navdata(this._ardrone);
	this._zed = zed;
	this._maxSpeed = 0.1;
	this._manual = false;
}

FlyController.prototype.start = function() {
	this._ardrone.disableEmergency();
	console.log("Battery remaining:", navdata.getvar('battery'));

	this._ardrone.takeoff();
	this._ardrone
	.after(3000, function(){
		this._ardrone.stop();
	})
	.after(3000, function(){
		console.log("start autonomous flight.");
		this._auto_fly();
	});
};

FlyController.prototype.manual = function(stat){
	this._manual = stat;
};

FlyController.prototype._auto_fly = function(){
	var t1, t2;
	var intend_pos, target_pos;
	var reached = true;

	var mainIntervalID = setInterval(function(){
		t1 = new Date();
		if (this._manual) { break; }

		// Avoid danger is top priority
		if (zed.has_danger_area()) {
			// enter avoidence mode
			target_pos = zed.getTargetPos(); // target_pos = {x: , y:, z:}
			this._move(target_pos);
		}else{
			// search for target object in a field
			if (reached) {
				intend_pos = findIntendPos();
				if (isPosEmpty(intend_pos)) {
					console.log("No place to search, pausing process...");
					this._end_fly();
					setTimeout(function(){
						clearInterval(mainIntervalID);
					}, 1000);
				}
				zed.setIntendPos(intend_pos.x, intend_pos.y, intend_pos.z);
				target_pos = zed.getTargetPos();
				reached = this._move(target_pos) ? true : false;
			}

			if (zed.has_target_obj_found()) {
			/**
				TODO:
				- If confidence is not high enough, move closer
						(should be able to be identified from 1.5-2m)
				- If confidence is high enough, print msg and hover
			**/
			}
		}

		t2 = new Date();
		console.log("mainInterval time:". t2-t1);
	}, 60);
};

FlyController.prototype._move = function(target_pos){
	/**

		TODO:
		- Case1: danger avoidance (continue to update)
		- Case2: to reach a valid pos (only one target)

	 */

};

FlyController.prototype._end_fly = function(){
	this._ardrone.stop();
	this._ardrone.land();
};

function findIntendPos () {
	var intend_pos = {x:0, y:0, z:0};
	/**
		TODO:
		- use this._map to record searched and unsearched areas
		- choose a closet area as intend_pos
	 */
	return intend_pos;
}
