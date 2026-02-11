; Compile with `chip8as adder.s --start-as-entry`
#include "std.s"

_start:
    ld vA K
    ld v9 vA
    call @putint
    ld vA 15
    call @sleep

next:
    ld vA K
    add v9 vA
    call @putint
    ld vA 15
    call @sleep

    ld vA v9
    call @putint
    jp @next
