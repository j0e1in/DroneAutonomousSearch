var keypress = require('keypress');

module.exports = function(proc, ardrone){
	this._proc = proc;
	this._ardrone = this._ardrone;

	keypress(this._proc.stdin);
	this._proc.stdin.on('keypress', function (ch, key) {
	  console.log('got "keypress"', key);

	  // Terminate
	  if (key && key.ctrl && key.name == 'c') {
	    console.log('got SIGINT, landing...');
	    this._ardrone.stop();
	    this._ardrone.land(function() {
	      this._proc.exit(0);
	    });
	    console.log("exiting...");
	    setTimeout(function(){
	      this._proc.exit(0);
	    }, 1500);
	  }

	  // Switch to manual control
	  if (!manual && key && key.ctrl && key.name == 'q') {
	    console.log("switching to manual control");
	    manual = true;
	    this._ardrone.stop();
	  }else if(manual && key && key.ctrl && key.name == 'q'){
	    console.log("it's already in manual control")
	  }

	  // Switch to autonomous control
	  if (manual && key && key.ctrl && key.name == 'a') { // ctrl-m === enter
	    console.log("switching to autonomous control");
	    manual = false;
	  }else if(!manual && key && key.ctrl && key.name == 'a'){
	    console.log("it's already in autonomous control")
	  }

	  // Show battery
	  if (key && key.ctrl && key.name == 'b') {
	    console.log("Battery: ", batteryLevel, "%");
	  }

	  // Land
	  if (key && key.name == 'l') {
	    console.log("landing");
	    this._ardrone.land();
	  }

	  // Hover
	  if (key && key.name == 'space') {
	    console.log("hovering");
	    this._ardrone.stop();
	  }

	  // For manual control
	  if (manual) {
	    // console.log('got "keypress"', key);
	      // Takeoff
	      if (key && key.shift && key.name == 't') {
	        console.log("taking off");
	        this._ardrone.takeoff();
	      }

	      // Yaw right
	      if (key && key.name == 'right') {
	        this._ardrone.right(maxSpeed);
	      }

	      // Yaw left
	      if (key && key.name == 'left') {
	        this._ardrone.left(maxSpeed);
	      }

	      // Go foreward
	      if (key && key.name == 'up') {
	        // this._ardrone.front(maxSpeed);
	        console.log("going up");
	        this._ardrone.up(maxSpeed);
	      }

	      // Go backward
	      if (key && key.name == 'down') {
	        // this._ardrone.back(maxSpeed);
	        console.log("going down");
	        this._ardrone.down(maxSpeed);
	      }

	      // Turn clockwise
	      if (key && key.shift && key.name == 'right') {
	        this._ardrone.clockwise(maxSpeed);
	      }

	      // Turn counter clockwise
	      if (key && key.shift && key.name == 'left') {
	        this._ardrone.counterClockwise(maxSpeed);
	      }

	      // Go up
	      if (key && key.shift && key.name == 'up') {
	        console.log("going up");
	        this._ardrone.up(maxSpeed);
	      }

	      // Go down
	      if (key && key.shift && key.name == 'down') {
	        console.log("going down");
	        this._ardrone.down(maxSpeed);
	      }
	  };
	});

	this._proc.stdin.setRawMode(true); // emit on each press
	this._proc.stdin.resume();
}
