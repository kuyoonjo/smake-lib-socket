import { resolve } from 'path';
import { LLVM } from '@smake/llvm';
import { LibBuffer } from '@smake/buffer';

export abstract class LibSocket {
  static config(llvm: LLVM) {
    LibBuffer.config(llvm);
    llvm.includedirs = [
      ...llvm.includedirs,
      resolve(__dirname, '..', 'include').replace(/\\/g, '/'),
    ];
  }
}
