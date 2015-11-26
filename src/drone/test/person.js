var Person = (function() {
    var private = new WeakMap();
    function Person(name) {
        var privateProperties = {
            name: name
        };
        private.set(this, privateProperties);
    }
    Person.prototype.getName = function() {
        return private.get(this).name;
    };
    return Person;
}());
var p = new Person('John');
console.log('Person 4 name: ' + p.getName());
delete p.name;
console.log('Person 4 name: ' + p.getName() + ' — stays private.');
console.log('Person 4 properties: ' + Object.getOwnPropertyNames(p));
