var zed = require('bindings')('zed');
var sleep = require('sleep');


module.exports.check_initZed = function check_initZed(){
	while(true){
		var is_true = zed.is_initZed_ready();
		if (is_true) {
			console.log("initZed is ready...");
			return true;
		}
		sleep.usleep(100000); // sleep for 0.1s
	}
}