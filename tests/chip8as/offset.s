; TEST 08: Jump with V0 Offset (Bnnn)

    JP x20E       ; Jump to Main

Trap:
    LD VF xEE     ; Error flag if we land here
    JP @Trap

Success:
    LD VF xFF     ; Success flag
Loop:
    JP @Loop

Target:
    JP @Trap       ; This is at Target + 0 (The trap)
    JP @Success    ; This is at Target + 2 (The goal)

Main:
    LD V0 x02     ; Offset = 2 bytes
    JP V0 @Target ; Jump to Target + V0 (should skip the trap)
