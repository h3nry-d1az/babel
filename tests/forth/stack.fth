( --- FORTH STACK MANIPULATION DIAGNOSTICS --- )

CR .( Starting Stack Manipulation Tests... ) CR
.( --------------------------------------- ) CR

( TEST 1: DROP )
10 20 DROP 10 = 
IF .( [+] DROP: Passed ) ELSE .( [!] DROP: FAILED ) THEN CR

( TEST 2: 2DROP )
10 20 30 40 2DROP 20 = 
IF 10 = IF .( [+] 2DROP: Passed ) ELSE .( [!] 2DROP: FAILED (Underflow?) ) THEN 
ELSE .( [!] 2DROP: FAILED ) THEN CR

( TEST 3: DUP )
50 DUP 50 = 
IF 50 = IF .( [+] DUP: Passed ) ELSE .( [!] DUP: FAILED (Copy missing) ) THEN 
ELSE .( [!] DUP: FAILED ) THEN CR

( TEST 4: 2DUP )
11 22 2DUP 22 = 
IF 11 = IF 22 = IF 11 = IF .( [+] 2DUP: Passed ) 
ELSE .( [!] 2DUP: FAILED (E1 missing) ) THEN 
ELSE .( [!] 2DUP: FAILED (E2 missing) ) THEN 
ELSE .( [!] 2DUP: FAILED (C1 missing) ) THEN 
ELSE .( [!] 2DUP: FAILED (C2 missing) ) THEN CR

( TEST 5: SWAP )
1 2 SWAP 1 = 
IF 2 = IF .( [+] SWAP: Passed ) ELSE .( [!] SWAP: FAILED (E1 missing) ) THEN 
ELSE .( [!] SWAP: FAILED ) THEN CR

( TEST 6: OVER )
100 200 OVER 100 = 
IF 200 = IF 100 = IF .( [+] OVER: Passed ) 
ELSE .( [!] OVER: FAILED (Original missing) ) THEN 
ELSE .( [!] OVER: FAILED (Middle missing) ) THEN 
ELSE .( [!] OVER: FAILED ) THEN CR

( TEST 7: ROT )
1 2 3 ROT 1 = 
IF 3 = IF 2 = IF .( [+] ROT: Passed ) 
ELSE .( [!] ROT: FAILED (E1 missing) ) THEN 
ELSE .( [!] ROT: FAILED (E2 missing) ) THEN 
ELSE .( [!] ROT: FAILED ) THEN CR

( TEST 8: NIP )
( 1 2 NIP should result in 2 )
1 2 NIP 2 = 
IF .( [+] NIP: Passed ) ELSE .( [!] NIP: FAILED ) THEN CR

( TEST 9: TUCK )
( 1 2 TUCK should result in 2 1 2 )
1 2 TUCK 2 = 
IF 1 = IF 2 = IF .( [+] TUCK: Passed ) 
ELSE .( [!] TUCK: FAILED ) THEN 
ELSE .( [!] TUCK: FAILED ) THEN 
ELSE .( [!] TUCK: FAILED ) THEN CR

( TEST 10: PICK )
( 10 20 30 40 3 PICK should copy 10 to top )
10 20 30 40 3 PICK 10 = 
IF 40 = IF 30 = IF 20 = IF 10 = IF .( [+] PICK: Passed ) 
ELSE .( [!] PICK: FAILED ) THEN ELSE .( [!] PICK: FAILED ) THEN 
ELSE .( [!] PICK: FAILED ) THEN ELSE .( [!] PICK: FAILED ) THEN
ELSE .( [!] PICK: FAILED ) THEN CR

( TEST 11: ROLL )
( 10 20 30 40 3 ROLL should move 10 to top -> 20 30 40 10 )
10 20 30 40 3 ROLL 10 = 
IF 40 = IF 30 = IF 20 = IF .( [+] ROLL: Passed ) 
ELSE .( [!] ROLL: FAILED ) THEN ELSE .( [!] ROLL: FAILED ) THEN 
ELSE .( [!] ROLL: FAILED ) THEN ELSE .( [!] ROLL: FAILED ) THEN CR

.( --------------------------------------- ) CR
.( Stack Manipulation Diagnostics Complete. ) CR