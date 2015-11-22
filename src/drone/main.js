var zed = require('bindings')('zed');
var Drone = require('./drone');
var utils = require('./utils');

var args = process.argv.slice(2); //get args from the third argument ([0],[1],[2],...)
var arg_str = args.join(' '); // join args to a string for initZed

zed.initZed(arg_str);
utils.waitForZed(zed);

var drone = new Drone(zed, useNavdata='false');

//==========================================================//

var refreshTime = 1/29; // 1/fps


setTimeout(function(){

	// drone.disableEmergency();
	console.log("Battery:", drone.navdata.getvar('battery'));

	drone.takeoff();

	drone.after(4000, function(){
		drone.stop();
	});

	drone.after(4000, function(){
		roam(15000);
	});

}, 1500);




function roam(timer){

	// set timer for stop roamming
	var stopRoamming = false;
	setTimeout(function(){
		stopRoamming = true;
		console.log("time out");
	}, timer);


	var evtStk = { // recording an event happens squentially
		stk: 0,
		thresh: 15,
		event: '',
	};

	var curMove = '';

	var intervalID = setInterval(function(){

		if (stopRoamming){
			console.log('Stop roamming');
			drone.land();
			clearInterval(intervalID)
		}

		zed.setIsMovingForward((curMove == 'f'));

		if (zed.has_danger_area()){

			if (evtStk['event'] == 'danger' && evtStk['stk'] > evtStk['thresh']){

				// disable rotation
				if (curMove == 'c'){
					drone.stop();
					curMove = 's';
				}

				if (curMove != 'b'){
					drone.back(0.2);
					curMove = 'b';
				}

			}else{

				if (evtStk['event'] != 'danger'){
					evtStk['event'] = 'danger';
					evtStk['stk'] = 1;
				}else{ // add up stk
					evtStk['stk'] += 1;
				}
			}

		}else if (zed.has_invalid_area()){

			if (evtStk['event'] == 'invalid' && evtStk['stk'] > evtStk['thresh']){

				// stop before rotate
				if (curMove != 's' && curMove != 'c'){
					drone.stop();
					curMove = 's';
				}
				if (curMove != 'c'){
					drone.clockwise(0.2); // rotate if cannot move forward
					curMove = 'c';
				}

			}else{

				if (evtStk['event'] != 'invalid'){
					evtStk['event'] = 'invalid';
					evtStk['stk'] = 1;
				}else{ // add up stk
					evtStk['stk'] += 1;
				}
			}

		}else{ //all areas are valid

			if (evtStk['event'] == 'valid' && evtStk['stk'] > evtStk['thresh']){

				// disable rotation
				if (curMove == 'c'){
					drone.stop();
					curMove = 's';
				}

				if (curMove != 'f'){
					drone.front(0.1);
					curMove = 'f';
				}

			}else{

				if (evtStk['event'] != 'valid'){
					evtStk['event'] = 'valid';
					evtStk['stk'] = 1;
				}else{ // add up stk
					evtStk['stk'] += 1;
				}
			}
		}

	}, refreshTime);

}//function roam



function moveBackTest(timer){

	// set timer for stop roamming
	var stopTest = false;
	setTimeout(function(){
		stopTest = true;z
	}, timer);

	var curMove = '';
	var evtStk = { // recording an event happens squentially
		stk: 0,
		thresh: 15,
		event: '',
	};

	while (!stopTest){

		if (zed.has_danger_area()){
			if (curMove != 'b' && evtStk['event'] == 'danger' && evtStk['stk'] > evtStk['thresh']){
				drone.back(0.2);
				curMove = 'b';
			}else{
				if (evtStk['event'] != 'danger'){
					evtStk['event'] = 'danger';
					evtStk['stk'] = 1;
				}else{
					evtStk['stk'] += 1;
				}
			}
		}else{
			if (curMove != 's' && evtStk['event'] == 'safe' && evtStk['stk'] > evtStk['thresh']){
				drone.stop();
				curMove = 's';
			}else{
				if (evtStk['event'] != 'safe'){
					evtStk['event'] = 'safe';
					evtStk['stk'] = 1;
				}else{
					evtStk['stk'] += 1;
				}
			}
			// console.log("event: " + evtStk['event'] + ", stk: " + evtStk['stk']);
		}

	}//while !stopTest
	drone.land();

}//function moveBackTest

