; TEST 02: Jumps and Conditionals

    JP x0208

Error:
    LD V0 xEE   ; Error marker
    JP @Error

Loop:
    JP @Loop

Main:
    LD V0 xAA
    LD V1 xAA

; Compare (Skip Equal)
    SE V0 V1    ; If V0 == V1, skip next instruction
    JP @Error   ; If this executes, SE failed

    LD V2 xFF   ; Success marker
    JP @Loop