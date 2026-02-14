# Babel

Babel is a collection of language implementations, in the form of
several compilers and interpreters. The repository includes implementations
for Brainfuck, a CHIP-8 emulator and assembler, and a Forth interpreter and transpiler for x86.

## Components
* **Brainfuck:** simple interpreter for `.bf` programs (see [tests/brainfuck](./tests/brainfuck/)).
* **CHIP-8:** emulator for ROMs (see [tests/chip8](./tests/chip8/)), and assembler ([tests/chip8as](./tests/chip8as/)).
* **Forth:** interpreter and compiler (see [tests/forth](./tests/forth/)).

## Build
Requires a C compiler, by default GCC. Run
```
make <program>
```

## Testing & examples
Example programs live under [tests](./tests/); use them to exercise the
binaries produced by `make`.

## License
MIT. See [LICENSE](./LICENSE) for further details.