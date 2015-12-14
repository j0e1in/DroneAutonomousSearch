var Navdata = require('./navdata');
var utils = require('./utils');
var ARDrone = require('ar-drone');
var sleep = require('sleep');



var Drone = (function(){
	var prv = new WeakMap(); // map for private properties

	// Constructor
	function Drone(zed, useNavdata){
		if (typeof useNavdata === 'undefined') useNavdata = true;

		/** Setting private properties **/
		var privateProperties = {
			zed: zed,
			drone: ARDrone.createClient(),
		};
		prv.set(this, privateProperties);

		/** Setup drone configs **/
		prv.get(this).drone.config('general:navdata_demo', 'TRUE');
		prv.get(this).drone.config('control:altitude_min', '800');
		prv.get(this).drone.config('control:altitude_max', '1200');
		// prv.get(this).drone.config('control:outdoor', 'TRUE'); // enables wind estimation
		// prv.get(this).drone.config('control:flight_without_shell', 'TRUE');

		/** Navdata **/
		if (useNavdata){
			this.navdata = new Navdata(prv.get(this).drone); // make navdata a public property
		}else{
			console.warn("WARN: 'useNavdata' is set to FALSE, navdata won't be available");
			this.navdata = null;
		}
	}

	Drone.prototype = {
		takeoff: function(){
			prv.get(this).drone.takeoff();
			console.log('Dronelog ## taking off');
		},
		land: function(){
			prv.get(this).drone.stop();
			prv.get(this).drone.land();
			console.log('Dronelog ## landing');
		},
		stop: function(){
			prv.get(this).drone.stop();
			console.log('Dronelog ## stoping');
		},
		front: function(speed){
			prv.get(this).drone.front(speed);
			console.log('Dronelog ## moving front');
		},
		back: function(speed){
			prv.get(this).drone.back(speed);
			console.log('Dronelog ## moving back');
		},
		left: function(speed){
			prv.get(this).drone.left(speed);
			console.log('Dronelog ## moving left');
		},
		right: function(speed){
			prv.get(this).drone.right(speed);
			console.log('Dronelog ## moving right');
		},
		clockwise: function(speed){
			prv.get(this).drone.clockwise(speed);
			console.log('Dronelog ## turning clockwise');
		},
		after: function(timer, cb){
			prv.get(this).drone.after(timer, cb);
		},
		led: function(mode, freq, duration){
			prv.get(this).drone.animateLeds(mode, freq, duration);
			/* Available modes:
				['blinkGreenRed', 'blinkGreen', 'blinkRed', 'blinkOrange', 'snakeGreenRed',
				'fire', 'standard', 'red', 'green', 'redSnake', 'blank', 'rightMissile',
				'leftMissile', 'doubleMissile', 'frontLeftGreenOthersRed',
				'frontRightGreenOthersRed', 'rearRightGreenOthersRed',
				'rearLeftGreenOthersRed', 'leftGreenRightRed', 'leftRedRightGreen',
				'blinkStandard']

				*freq & duration are in second
			 */

		},
		log: function(msg){
			console.log('Dronelog ## ' + msg);
		}
	}

	return Drone;
}());

module.exports = Drone;
