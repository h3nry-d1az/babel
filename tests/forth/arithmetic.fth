( --- FORTH ARITHMETIC INTEGRITY SUITE --- )

CR .( Starting Arithmetic System Diagnostics... ) CR
.( --------------------------------------- ) CR

( TEST 1: Basic Addition )
123 456 + 579 = 
IF .( [+] Addition: Passed ) ELSE .( [!] Addition: FAILED ) THEN CR

( TEST 2: Basic Subtraction )
1000 333 - 667 = 
IF .( [+] Subtraction: Passed ) ELSE .( [!] Subtraction: FAILED ) THEN CR

( TEST 3: Basic Multiplication )
15 15 * 225 = 
IF .( [+] Multiplication: Passed ) ELSE .( [!] Multiplication: FAILED ) THEN CR

( TEST 4: Integer Division )
500 4 / 125 = 
IF .( [+] Division: Passed ) ELSE .( [!] Division: FAILED ) THEN CR

( TEST 5: Modulo Operations )
22 7 MOD 1 = 
IF .( [+] Modulo: Passed ) ELSE .( [!] Modulo: FAILED ) THEN CR

( TEST 6: Sign Inversion )
88 NEGATE -88 = 
IF .( [+] Negate: Passed ) ELSE .( [!] Negate: FAILED ) THEN CR

( TEST 7: Absolute Value )
-500 ABS 500 = 
IF .( [+] Absolute: Passed ) ELSE .( [!] Absolute: FAILED ) THEN CR

( TEST 8: Maximum Value Selection )
10 50 MAX 50 = 
IF .( [+] Max: Passed ) ELSE .( [!] Max: FAILED ) THEN CR

( TEST 9: Minimum Value Selection )
-10 -50 MIN -50 = 
IF .( [+] Min: Passed ) ELSE .( [!] Min: FAILED ) THEN CR

( TEST 10: Long Complex Chain )
( Calculation: ((100 + 50) * 2 / 3) - 10 + 5 )
( Expected: 95 )
100 50 + 2 * 3 / 10 - 5 + 95 = 
IF .( [+] Complex Chain: Passed ) ELSE .( [!] Complex Chain: FAILED ) THEN CR

( TEST 11: Negative Arithmetic Chain )
( Calculation: -10 * 5 + 20 )
( Expected: -30 )
-10 5 * 20 + -30 = 
IF .( [+] Negative Chain: Passed ) ELSE .( [!] Negative Chain: FAILED ) THEN CR

.( --------------------------------------- ) CR
.( Diagnostics Complete. ) CR