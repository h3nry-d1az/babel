; TEST 04: Graphics and Raw Data

    JP x0206

; Two bytes explicitly exported
Sprite:
    b1000000001000000
    b0010000000010000

Main:
    LD V0 x00
    LD V1 x00
    LD I @Sprite ; Load address of Sprite label into I

; Draw at (0,0), height 4
    DRW V0 V1 x4

Stop:
    JP @Stop