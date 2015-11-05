var zed = require('bindings')('zed');

var args = process.argv.slice(2); //get args from the third argument (0,1,2,...)
var arg_str = args.join(' '); // join args to a string for initZed

zed.initZed(arg_str);