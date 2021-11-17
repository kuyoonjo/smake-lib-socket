const { LLVM } = require('smake');
const { LibSocket } = require('./lib');

const test = new LLVM('test', 'x86_64-linux-gnu');
test.files = ['src/ex/socket.cc'];
LibSocket.config(test);
test.stdcxx = 'c++17';

module.exports = [test];
