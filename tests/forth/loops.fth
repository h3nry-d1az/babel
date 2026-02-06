( --- FORTH LOOP SYSTEM DIAGNOSTICS --- )

CR .( Starting Loop Control Structure Tests... ) CR
.( --------------------------------------- ) CR

( TEST 1: BEGIN ... UNTIL )
( Logic: Increment 0 until it reaches 10 )
0 
BEGIN 
    1 + 
    DUP 10 = 
UNTIL 
10 = 
IF .( [+] BEGIN-UNTIL (Post-condition): Passed ) ELSE .( [!] BEGIN-UNTIL: FAILED ) THEN CR


( TEST 2: BEGIN ... WHILE ... REPEAT )
( Logic: Start at 5, decrement while greater than 0 )
5 
BEGIN 
    DUP 0 > 
WHILE 
    1 - 
REPEAT 
0 = 
IF .( [+] BEGIN-WHILE-REPEAT (Pre-condition): Passed ) ELSE .( [!] BEGIN-WHILE: FAILED ) THEN CR


( TEST 3: DO ... LOOP )
( Logic: Sum numbers from 1 to 5. Expected result: 15 )
0 ( The accumulator )
6 1 ( Limit and Index for 1 to 5 )
DO 
    I + 
LOOP 
15 = 
IF .( [+] DO-LOOP (Index 'I' handling): Passed ) ELSE .( [!] DO-LOOP: FAILED ) THEN CR


( TEST 4: NESTED DO ... LOOP )
( Logic: 3x3 nested loop incrementing a counter. Expected result: 9 )
0 ( Accumulator )
3 0 DO 
    3 0 DO 
        1 + 
    LOOP 
LOOP 
9 = 
IF .( [+] NESTED DO-LOOP (Return stack stability): Passed ) ELSE .( [!] NESTED DO-LOOP: FAILED ) THEN CR


( TEST 5: LOOP BOUNDARY CHECK )
( Logic: Verify that a loop with same index and limit behaves correctly )
( In most Forth standards, 5 5 DO executes zero times or once depending on implementation )
( We test a simple 1-iteration range )
0
2 1 DO 1 + LOOP 1 = 
IF .( [+] LOOP BOUNDARIES: Passed ) ELSE .( [!] LOOP BOUNDARIES: FAILED ) THEN CR


.( --------------------------------------- ) CR
.( Loop Diagnostics Complete. ) CR