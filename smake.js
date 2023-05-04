const { LLVM } = require('@smake/llvm');
const { vscode } = require('@smake/llvm-vscode');
const { LibSocket } = require('./lib');

const test = new LLVM('test', 'aarch64-apple-darwin');
test.files = ['src/ex/socket.cc'];
LibSocket.config(test);
test.stdcxx = 'c++17';
vscode(test);

module.exports = [test];
