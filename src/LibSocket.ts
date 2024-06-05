import { resolve } from 'path';
import { LLVM } from '@smake/llvm';
import { LibBuffer } from '@smake/buffer';

export abstract class LibSocket {
  static config(
    llvm: LLVM,
    opts?: {
      disableSocketException?: boolean;
    }
  ) {
    LibBuffer.config(llvm);
    llvm.includedirs = [
      ...llvm.includedirs,
      resolve(__dirname, '..', 'include').replace(/\\/g, '/'),
    ];
    if (opts) {
      if (opts.disableSocketException) {
        llvm.cxxflags = [...llvm.cxxflags, '-DDISABLE_SOCKET_EXCEPTION'];
      }
    }
  }
}
