const { LLVM } = require('@smake/llvm');
const { vscode } = require('@smake/llvm-vscode');
const { LibSocket } = require('./lib');

// const test = new LLVM('test', 'aarch64-apple-darwin');
const test = new LLVM('test', 'aarch64-linux-gnu');
// const test = new LLVM('test', 'x86_64-pc-windows-msvc');
test.files = ['socket.cxx'];
LibSocket.config(test);
test.stdcxx = 'c++17';
test.ldflags = [
    ...test.ldflags,
    '-lpthread',
];
// test.cxxflags = [
//     ...test.cxxflags,
//     '-EHsc',
// ];

vscode(test);

module.exports = [test];
