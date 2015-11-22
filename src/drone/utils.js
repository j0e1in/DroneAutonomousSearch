var sleep = require('sleep');

module.exports = {

	waitForZed: function (zed){
		console.log("\n@ Waiting for zed...\n");
		while(true){
			if (zed.is_initZed_ready()) {
				console.log("\n@ Zed is ready.\n");
				return true;
			}
			sleep.usleep(100000); // sleep for 0.1s
		}
	},
}


