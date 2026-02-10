; TEST 03: Subroutines with @ syntax

    JP x0208

AddOne:
    ADD V0 x01
    RET          ; Return from subroutine

Stop:
    JP @Stop

Main:
    LD V0 x00
    CALL @AddOne ; First call
    CALL @AddOne ; Second call
    JP @Stop