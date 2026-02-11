    jp x0230

; Args:
;  - t : vA
delay:
    ld DT vA
__delay_loop:
    ld v0 DT
    se v0 0
    jp @__delay_loop
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
print:
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

main:
    ld vA K
    ld v9 vA
    call @print
    ld vA 15
    call @delay

next:
    ld vA K
    add v9 vA
    call @print
    ld vA 15
    call @delay

    ld vA v9
    call @print
    jp @next
