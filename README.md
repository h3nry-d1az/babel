# Babel

Babel is a collection of language implementations, in the form of several
compilers and interpreters. The repository includes tools for Brainfuck, a
CHIP-8 emulator and (dis)assembler, and a Forth interpreter and transpiler for x86.

## Components
* **Brainfuck:** simple interpreter for `.bf` programs ([`brainfuck.c`](./brainfuck.c)).
* **CHIP-8:** emulator ([`chip8.c`](./chip8.c)), assembler ([`chip8as.c`](./chip8as.c)),
  and disassembler ([`chip8disas.c`](./chip8disas.c)).
* **Forth:** interpreter ([`forth.c`](./forth.c)) and compiler for x86 ([`forthc.c`](./forthc.c)).

## Build
Requires a C compiler, by default GCC. Run
```
make [brainfuck|chip8|...|forthc]
```

## Testing & examples
Example programs live under [tests](./tests/); use them to exercise the binaries
produced by `make`.

## Contributing
If the proposed changes do not compile with the project [`Makefile`](./Makefile),
they will immediately rejected, same if the majority of the tests do not pass.
Make sure to format the code according to the project [`.clang-format`](./.clang-format).

## License
MIT. See [LICENSE](./LICENSE) for further details.