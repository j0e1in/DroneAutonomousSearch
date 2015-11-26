var Person = require('./person');

var p = new Person('John');
console.log('Person 1 name: ' + p.getName());
delete p.name;
console.log('Person 1 name: ' + p.getName() + ' — modified outside.');