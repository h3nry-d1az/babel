; TEST 05: Memory and BCD

    LD I x200   ; Point I to safe memory
    LD V0 123   ; 123 decimal
    LD B V0     ; Store BCD of 123 at I, I+1, I+2

    ; Multi-register dump test
    LD V0 x01
    LD V1 x02
    LD I x300
    LD [I] V1   ; Dump V0 through V1 to memory at I

    ; Clear registers
    LD V0 x00
    LD V1 x00

    ; Load back
    LD I x300
    LD V1 [I]   ; Load memory back into V0 through V1

Stop:
    JP @Stop