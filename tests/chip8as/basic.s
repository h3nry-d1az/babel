; TEST 01: Basic Syntax
; Verifying immediate loads and addition without commas

    LD V0 x02   ; Load 2 into V0
    LD V1 x03   ; Load 3 into V1
    ADD V0 V1   ; V0 = V0 + V1 (Should be 5)

; Infinite loop to stop execution
Stop:
    JP @Stop