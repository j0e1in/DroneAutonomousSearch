module.exports = Navdata;

function Navdata(drone){

	this._drone = drone;
	this._var = {};
	this._var['ref_time'] = 5/100; // sec
	this._var['demo'] = undefined;
	this._var['battery'] = undefined;
	this._var['velocity'] = undefined;
	this._var['altitude'] = undefined;
	this._var['frontBackDegrees'] = undefined;
	this._var['leftRightDegrees'] = undefined;
	this._var['clockwiseDegrees'] = undefined;
	this._var['flying'] = undefined;
	this._logList = [];

	function startNavdata(self){
		self._drone.on('navdata', function(nd){
			// demo navdata
		  if (nd.demo && typeof nd.demo !== "undefined")
		  	self._var['demo'] = nd.demo;
			if (nd.demo && typeof nd.demo.batteryPercentage !== "undefined")
	      self._var['battery'] = nd.demo.batteryPercentage;
		  if (nd.demo && typeof nd.demo.velocity !== "undefined"){
		  	self._var['velocity'] = nd.demo.velocity;
		  }
		  if (nd.demo && typeof nd.demo.altitude !== "undefined")
		  	self._var['altitude'] = nd.demo.altitude;
		  if (nd.demo && typeof nd.demo.frontBackDegrees !== "undefined")
		  	self._var['frontBackDegrees'] = nd.demo.frontBackDegrees;
		  if (nd.demo && typeof nd.demo.leftRightDegrees !== "undefined")
		  	self._var['leftRightDegrees'] = nd.demo.leftRightDegrees;
		  if (nd.demo && typeof nd.demo.clockwiseDegrees !== "undefined")
		  	self._var['clockwiseDegrees'] = nd.demo.clockwiseDegrees;

		  // droneState navdata
		  if (nd.droneState && typeof nd.droneState.flying !== "undefined")
		  	self._var['flying'] = nd.droneState.flying;

		  // log required data constantly
		  for (var i = 0; i < self._logList.length; ++i) {
		  	console.log(self._logList[i], ">>", self._var[self._logList[i]]);
		  };

		});
	}
	startNavdata(this);
}

Navdata.prototype.isAvail = function(){
	return (typeof this._var['demo'] !== "undefined");
}

Navdata.prototype.getvar = function(var_type){
	return this._var[var_type];
}

Navdata.prototype.log = function(var_type){
	this._logList.push(var_type);
}