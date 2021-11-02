data = '''
#define T_DIVIDE     0		// divide error
#define T_DEBUG      1		// debug exception
#define T_NMI        2		// non-maskable interrupt
#define T_BRKPT      3		// breakpoint
#define T_OFLOW      4		// overflow
#define T_BOUND      5		// bounds check
#define T_ILLOP      6		// illegal opcode
#define T_DEVICE     7		// device not available
#define T_DBLFLT     8		// double fault
#define T_TSS       10		// invalid task switch segment
#define T_SEGNP     11		// segment not present
#define T_STACK     12		// stack exception
#define T_GPFLT     13		// general protection fault
#define T_PGFLT     14		// page fault
#define T_FPERR     16		// floating point error
#define T_ALIGN     17		// aligment check
#define T_MCHK      18		// machine check
#define T_SIMDERR   19		// SIMD floating point error
#define T_SYSCALL   48		// system call
'''

import pyperclip
import re

msg = []

for line in data.strip().split('\n'):
    tokens = re.split('\s+', line)
    id = int(tokens[2])
    suf = '     ' if 8 <= id and id <= 14 else '_NOEC'
    msg.append(f'TRAPHANDLER{suf}(  trap_hander_{tokens[1][2:]:7s}  ,  {tokens[1]:9s}  ,  0  )')
    # msg.append(f'extern void hander_{tokens[1]:10s}();')
    # msg.append(f'SETGATE(idt[{tokens[1]:10s}], 1, GD_KT, trap_hander_DIVIDE, 0);')

pyperclip.copy('\n'.join(msg))