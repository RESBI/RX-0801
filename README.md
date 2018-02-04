# About:
 * 2 / 2 / 2018   12:00 Begin.
 * 2 / 2 / 2018   22:00 RX-0001
 * 2 / 3 / 2018   17:03	RX-0801

# RX-0801:
## Registers:
+-------+
| 8 Bit |
+-------+------------------------+
| From: 0000(0) To: 1111(F)      |
+--------------------------------+
| 0000        : Always zero      |
+--------------------------------+
| 0001 ~ 1101 : User's registers |
+--------------------------------+
| 1110        : Keyboard input   |
+--------------------------------+
| 1111        : Memory Address   |
+--------------------------------+

## Command:
+--------+
| 16 Bit |
+--------+------------------------+
| 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 |
||---------------|-------|-------||
|       |||          ||      ||   |
|     Control       Reg1    Reg2  |
+---------------------------------+


+---------+----------+
| NULL    | 00000000 |
+---------+----------+--+
| Module  | 0000: A & L |
+---+-----+-------------+
| A |
+---+----------------+---------------------+
| 00000001 reg1 reg2 | cache = reg1 + reg2 |
+--------------------+---------------------+
| 00000010 reg1 reg2 | cache = reg1 - reg2 |
+--------------------+---------------------+---------------+
| 00000011 reg1 reg2 | cache = reg1 * reg2 | (4Bit * 4Bit) |
+--------------------+---------------------+---------------+
| 00000100 reg1 reg2 | cache = reg1 / reg2 | (Int output)  |
+---+----------------+---------------------+---------------+
| L |
+---+----------------+----------------------+
| 00000101 reg1 reg2 | cache = reg1 << reg2 |
+--------------------+----------------------+
| 00000110 reg1 reg2 | cache = reg1 >> reg2 |
+--------------------+----------------------+
| 00000111 reg1 reg2 | cache = reg1 or reg2 |
+--------------------+----------------------+
| 00001000 reg1      | cache = not reg2     |
+--------------------+----------------------+

+--------+-----------------------------+
| Module | 0001: RAM & ROM & Registers |
+--------+-----------+-----------------++
| 00010000 reg1 reg2 | reg1 = RAM[reg2] |
+--------------------+------------------|
| 00010001 reg1 reg2 | RAM[reg1] = reg2 |
+--------------------+------------------|
| 00010010 reg1 reg2 | reg1 = ROM[reg2] |
+--------------------+------------------|
| 00010011 reg1 reg2 | ROM[reg1] = reg2 |
+--------------------+------------------|
| 00010100 reg1      | reg1 = cache     |
+--------------------+------------------+---------------------------------+
| 00010101 num       | cache = num      | (num at here is a 8Bit number.) |
+--------------------+------------------+---------------------------------+

+--------+------------+
| Module | 0010: THEN |
+--------+-----------++---------------------------------------------+-------------------------------------------------------------------+
| 00100000 reg1 reg2 | Compare registers[reg1] with registers[reg2] | If registers[reg1] == registers[reg2] then continue else TIMER++. |
+---------------------------------------------------------------------------------------------------------------------------------------+

## ASM LIST:
+-----+
| ASM |
+-----++----------+------+
| CMD  | BIN      | HEX  |
+------+----------+------+
| ADD  | 00000001 | 0x01 |
| MIN  | 00000010 | 0x02 |
| MUL  | 00000011 | 0x03 |
| DIV  | 00000100 | 0x04 |
| LFT  | 00000101 | 0x05 |
| RGT  | 00000110 | 0x06 |
| OR   | 00000111 | 0x07 |
| NOT  | 00001000 | 0x08 |
| MAR  | 00010000 | 0x10 |
| MRA  | 00010001 | 0x11 |
| MOR  | 00010010 | 0x12 |
| MRO  | 00010011 | 0x13 |
| MCR  | 00010100 | 0x14 |
| MIC  | 00010101 | 0x15 |
| CMP  | 00100000 | 0x20 |
+------+----------+------+

## PS:
+---------+        +--------------------+
| MIC XXX | Means: | JUMP TO ROM[ XXX ] |
| MCR F   |        +--------------------+
+---------+

+---------+
| MIC XXX |
| MCR A   |        +-------------------------------+
| MIC YYY |        | IF REGISTER[2] == REGISTER[3] |
| MCR B   | Means: | JUMP TO ROM[ XXX ]            |
| ADD A 0 |        | ELSE                          |
| CMP 2 3 |        | JUMP TO ROM[ YYY ]            |
| MCR F   |        +-------------------------------+
| ADD B 0 |
| MCR F   |
+---------+
