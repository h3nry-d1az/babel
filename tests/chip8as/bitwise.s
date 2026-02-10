; TEST 09: Logical Stress Test

    LD V0 xF0   ; 11110000
    LD V1 x0F   ; 00001111

    OR V0 V1    ; V0 = xFF
    AND V0 V1   ; V0 = x0F (because V1 is x0F)
    XOR V0 V1   ; V0 = x00 (x0F ^ x0F)

    LD V2 x04
    SHR V2      ; Shift Right V2 (V2 should become 2)

    LD V3 x80
    SHL V3      ; Shift Left V3 (V3 becomes 0, VF = 1 overflow)

Stop:
    JP @Stop