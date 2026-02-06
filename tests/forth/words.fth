( --- FORTH WORD DEFINITION & DICTIONARY TEST --- )

CR .( Starting Dictionary and Word Execution Tests... ) CR
.( --------------------------------------- ) CR

( TEST 1: Basic Word Definition and Execution )
( Testing if the interpreter can compile and find a new word )
: PLUS-FIVE 5 + ;

10 PLUS-FIVE 15 = 
IF .( [+] Basic Word Definition: Passed ) ELSE .( [!] Basic Word Definition: FAILED ) THEN CR


( TEST 2: Word Nesting )
( Testing if a word can successfully call another word )
( This verifies the Return Stack is saving addresses correctly )
: DOUBLE-PLUS-TEN PLUS-FIVE PLUS-FIVE ;

10 DOUBLE-PLUS-TEN 20 = 
IF .( [+] Word Nesting (Call/Return): Passed ) ELSE .( [!] Word Nesting: FAILED ) THEN CR


( TEST 3: Multiple Definitions with same base )
: TRIPLE-PLUS-FIFTEEN DOUBLE-PLUS-TEN PLUS-FIVE ;

10 TRIPLE-PLUS-FIFTEEN 25 = 
IF .( [+] Multiple Nesting Levels: Passed ) ELSE .( [!] Multiple Nesting Levels: FAILED ) THEN CR


( TEST 4: Logic and Branching inside a Word )
( Testing if IF/ELSE structures work when compiled into a word )
: CHECK-LIMIT ( n -- flag )
    100 > IF 1 ELSE 0 THEN ;

150 CHECK-LIMIT 1 =
IF .( [+] Logic inside Word (True): Passed ) ELSE .( [!] Logic inside Word (True): FAILED ) THEN CR

50 CHECK-LIMIT 0 =
IF .( [+] Logic inside Word (False): Passed ) ELSE .( [!] Logic inside Word (False): FAILED ) THEN CR


( TEST 5: Loops inside a Word )
( Testing if the Return Stack handles both the loop and the word call )
: SUM-TO-THREE ( -- sum )
    0 ( Start accumulator )
    4 1 DO I + LOOP ;

SUM-TO-THREE 6 = 
IF .( [+] Loop inside Word: Passed ) ELSE .( [!] Loop inside Word: FAILED ) THEN CR


.( --------------------------------------- ) CR
.( Word System Diagnostics Complete. ) CR