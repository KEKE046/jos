Table 9-6. Exception Summary

Description              | Interrupt  | Return Address | Exception    | Description
:--|--:|--:|--:|:--
Divide error             |  0         | YES            | FAULT        | DIV, IDIV
Debug exceptions         |  1         | xxx            | xxx          | Any instruction
Breakpoint               |  3         | NO             | TRAP         | One-byte INT 3
Overflow                 |  4         | NO             | TRAP         | INTO
Bounds check             |  5         | YES            | FAULT        | BOUND
Invalid opcode           |  6         | YES            | FAULT        | Any illegal instruction
Coprocessor not available|  7         | YES            | FAULT        | ESC, WAIT
Double fault             |  8         | YES            | ABORT        | Any instruction that can generate an exception Coprocessor Segment
Overrun                  |  9         | NO             | ABORT        | Any operand of an ESC instruction that wraps around the end of a segment.
Invalid TSS              | 10         | YES            | FAULT        | An invalid-TSS fault is not restartable if it occurs during the processing of an external interrupt. JMP, CALL, IRET, any interrupt
Segment not present      | 11         | YES            | FAULT        | Any segment-register modifier
Stack exception          | 12         | YES            | FAULT        | Any memory reference thru SS
General Protection       | 13         | YES            | FAULT/ABORT  | All GP faults are restartable. If the fault occurs while attempting to vector to the handler for an external interrupt, the interrupted program is restartable, but the interrupt may be lost.  Any memory reference or code fetch
Page fault               | 14         | YES            | FAULT        | Any memory reference or code fetch
Coprocessor error        | 16         | YES            | FAULT        | Coprocessor errors are reported as a fault on the first ESC or WAIT instruction executed after the ESC instruction that caused the error. ESC, WAIT
Two-byte SW Interrupt    | 0-255      | NO             | TRAP         | INT n

Table 9-7. Error-Code Summary

Description                     |  Interrupt   |  Error Code Number
:--|--:|:--
Divide error                    |   0          |  No
Debug exceptions                |   1          |  No
Breakpoint                      |   3          |  No
Overflow                        |   4          |  No
Bounds check                    |   5          |  No
Invalid opcode                  |   6          |  No
Coprocessor not available       |   7          |  No
System error                    |   8          |  Yes (always 0)
Coprocessor Segment Overrun     |   9          |  No
Invalid TSS                     |  10          |  Yes
Segment not present             |  11          |  Yes
Stack exception                 |  12          |  Yes
General protection fault        |  13          |  Yes
Page fault                      |  14          |  Yes
Coprocessor error               |  16          |  No
Two-byte SW interrupt           |  0-255       |  No
