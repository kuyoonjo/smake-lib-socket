const { LLVM_Darwin, LLVM_Win32, LLVM_Linux } = require('smake');
const { addLibs } = require('@smake/libs');
const { buffer } = require('@smake/buffer');
const { socket } = require('./lib');

class darwin_arm64 extends LLVM_Darwin {
  ARCH = 'arm64';
  name = this.target;
  includedirs = [
    ...super.includedirs,
    './include',
  ];
  cxflags = [
    ...super.cxflags,
    '-O3',
    '-std=c++17',
  ];

  files = ['src/ex/socket.cc'];
}

class linux_aarch64 extends LLVM_Linux {
  target = 'aarch64-linux-gnu';
  name = this.target;
  includedirs = [
    ...super.includedirs,
    './include',
  ];
  cxflags = [
    ...super.cxflags,
    '-O3',
    '-std=c++17',
  ];

  files = ['src/ex/socket.cc'];
}

class win32_x86_64 extends LLVM_Win32 {
  ARCH = 'x86_64';
  name = this.target;
  includedirs = [
    ...super.includedirs,
    './include',
  ];
  cxflags = [
    ...super.cxflags,
    '-O3',
    '-std=c++17',
  ];

  files = ['src/ex/socket.cc'];
}

module.exports = {
  targets: [
    addLibs(darwin_arm64, buffer, socket),
    addLibs(linux_aarch64, buffer, socket),
    addLibs(win32_x86_64, buffer, socket),
  ],
};
