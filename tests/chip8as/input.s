; TEST 06: Blocking Input Wait
; Program halts until a key is pressed

    LD V0 x00

    ; Wait for key press. Value stored in V5.
    ; Execution stops here until input happens.
    LD V5 K     

    ; Set I to the location of the sprite for the character in V5
    LD F V5     

    ; Draw the character of the key pressed at (0,0)
    DRW V0 V0 x5 

Stop:
    JP @Stop