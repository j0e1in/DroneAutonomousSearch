var zed = require('bindings')('zed');
var Drone = require('./drone');
var utils = require('./utils');

var args = process.argv.slice(2); //get args from the third argument ([0],[1],[2],...)
var arg_str = args.join(' '); // join args to a string for initZed

zed.initZed(arg_str);
utils.waitForZed(zed);

var drone = new Drone(zed);

//==========================================================//

var evtStk = { // recording an event happens squentially
	stk: 0,
	thresh: 5,
	event: '',
};

drone.takeoff();

drone.after(3000, function(){
	drone.stop();
});

roam();


function roam(timer){

	// set timer for stop roamming
	var stopRoamming = false;
	setTimeout(function(){
		stopRoamming = true;
	}, timer);

	var curMove = '';
	var prvMove = '';

	while (!stopRoamming){

		if (zed.has_danger_area()){

			if (curMove == 'c'){
				drone.stop();
			}

			if (curMove != 'b'){
				drone.back(0.1);
				curMove = 'b';
			}

		}else if (zed.has_invalid_area){

			if (curMove != 's' && curMove != 'c'){
				drone.stop();
				curMove = 's';
			}
			drone.clockwise(0.1); // rotate if cannot move forward
			curMove = 'c';

		}else{ //all areas are valid
			if (curMove != 'f'){
				drone.front(0.1);
				curMove = 'f';
			}
		}

	}//while !stopRoamming

}//function roam