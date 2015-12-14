var zed = require('bindings')('zed');
var path = require('path');
var fs = require('fs');

var args = process.argv.slice(2); //get args from the third argument (0,1,2,...)
// var curPath = path.dirname(process.mainModule.filename);
var casecadePath = "D:\\GraduationProject\\DroneSearch\\src\\haarcascades\\";
args.push('_cc'); // cascade classifier option
args.push(casecadePath + 'haarcascade_mcs_upperbody.xml');
args.push(casecadePath + 'haarcascade_frontalface_alt_tree.xml');
// args.push(casecadePath + 'haarcascade_frontalface_alt2.xml');
// args.push(casecadePath + 'haarcascade_frontalface_alt.xml');
// args.push(casecadePath + 'LEye18x12.xml');
var arg_str = args.join(' '); // join args to a string for initZed

zed.objdetect(arg_str);