; Minimal Chip-8 "Standard Library"

; Args:
;  - t : vA
sleep:
    ld DT vA
__sleep_loop:
    ld v0 DT
    se v0 0
    jp @__sleep_loop
    ret

; Args:
;  - c : VA
;  - x : VB
;  - y : VC
putchar:
    ld F vA
    drw vB vC 5
    ret

; Args:
;  - n : VA
putint:
    ld vB 0
    ld vC 0

    ld I xFFD
    ld B vA

    cls

    ld v2 [I]
    ld vA v0
    call @putchar

    add vB 5
    ld vA v1
    call @putchar

    add vB 5
    ld vA v2
    call @putchar

    ret