; TEST 07: Timers and Randomness

    LD V0 xFF

    ; Set Delay Timer to 255 (approx 4 seconds)
    LD DT V0    

    ; Set Sound Timer to 255 (beeps for 4 seconds)
    LD ST V0    

    ; Read Delay Timer back into V1
    LD V1 DT    

    ; Generate Random Number: V2 = rand() & x0F
    RND V2 x0F  

Stop:
    JP @Stop