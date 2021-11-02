/* See COPYRIGHT for copyright information. */

#include <inc/mmu.h>
#include <inc/memlayout.h>
#include <inc/trap.h>

#include <kern/picirq.h>


###################################################################
# exceptions/interrupts
###################################################################

/* TRAPHANDLER defines a globally-visible function for handling a trap.
 * It pushes a trap number onto the stack, then jumps to _alltraps.
 * Use TRAPHANDLER for traps where the CPU automatically pushes an error code.
 *
 * You shouldn't call a TRAPHANDLER function from C, but you may
 * need to _declare_ one in C (for instance, to get a function pointer
 * during IDT setup).  You can declare the function with
 *   void NAME();
 * where NAME is the argument passed to TRAPHANDLER.
 */
#define TRAPHANDLER(name, num, dpl)						\
	.globl name;		/* define global symbol for 'name' */	\
	.type name, @function;	/* symbol type is function */		\
	.align 2;		/* align function definition */		\
	.text; \
	name:			/* function starts here */		\
	pushl $(num);							\
	jmp _alltraps; \
	.data; .int name; .int num; .int dpl;

/* Use TRAPHANDLER_NOEC for traps where the CPU doesn't push an error code.
 * It pushes a 0 in place of the error code, so the trap frame has the same
 * format in either case.
 */
#define TRAPHANDLER_NOEC(name, num, dpl)					\
	.globl name;							\
	.type name, @function;						\
	.align 2;							\
	.text; \
	name:								\
	pushl $0;							\
	pushl $(num);							\
	jmp _alltraps; \
	.data; .int name; .int num; .int dpl;

.data
.global trapentry_table
trapentry_table:

.text

/*
 * Lab 3: Your code here for generating entry points for the different traps.
 */

	TRAPHANDLER_NOEC(  trap_handler_DIVIDE   ,  T_DIVIDE   ,  PL_KERNEL  )
	TRAPHANDLER_NOEC(  trap_handler_DEBUG    ,  T_DEBUG    ,  PL_KERNEL  )
	TRAPHANDLER_NOEC(  trap_handler_NMI      ,  T_NMI      ,  PL_KERNEL  )
	TRAPHANDLER_NOEC(  trap_handler_BRKPT    ,  T_BRKPT    ,  PL_USER    )
	TRAPHANDLER_NOEC(  trap_handler_OFLOW    ,  T_OFLOW    ,  PL_KERNEL  )
	TRAPHANDLER_NOEC(  trap_handler_BOUND    ,  T_BOUND    ,  PL_KERNEL  )
	TRAPHANDLER_NOEC(  trap_handler_ILLOP    ,  T_ILLOP    ,  PL_KERNEL  )
	TRAPHANDLER_NOEC(  trap_handler_DEVICE   ,  T_DEVICE   ,  PL_KERNEL  )
	TRAPHANDLER     (  trap_handler_DBLFLT   ,  T_DBLFLT   ,  PL_KERNEL  )
	TRAPHANDLER     (  trap_handler_TSS      ,  T_TSS      ,  PL_KERNEL  )
	TRAPHANDLER     (  trap_handler_SEGNP    ,  T_SEGNP    ,  PL_KERNEL  )
	TRAPHANDLER     (  trap_handler_STACK    ,  T_STACK    ,  PL_KERNEL  )
	TRAPHANDLER     (  trap_handler_GPFLT    ,  T_GPFLT    ,  PL_KERNEL  )
	TRAPHANDLER     (  trap_handler_PGFLT    ,  T_PGFLT    ,  PL_KERNEL  )
	TRAPHANDLER_NOEC(  trap_handler_FPERR    ,  T_FPERR    ,  PL_KERNEL  )
	TRAPHANDLER_NOEC(  trap_handler_ALIGN    ,  T_ALIGN    ,  PL_KERNEL  )
	TRAPHANDLER_NOEC(  trap_handler_MCHK     ,  T_MCHK     ,  PL_KERNEL  )
	TRAPHANDLER_NOEC(  trap_handler_SIMDERR  ,  T_SIMDERR  ,  PL_KERNEL  )

	TRAPHANDLER_NOEC(  trap_handler_IRQ0     ,  IRQ_OFFSET +  0 ,  PL_KERNEL  )
	TRAPHANDLER_NOEC(  trap_handler_IRQ1     ,  IRQ_OFFSET +  1 ,  PL_KERNEL  )
	TRAPHANDLER_NOEC(  trap_handler_IRQ2     ,  IRQ_OFFSET +  2 ,  PL_KERNEL  )
	TRAPHANDLER_NOEC(  trap_handler_IRQ3     ,  IRQ_OFFSET +  3 ,  PL_KERNEL  )
	TRAPHANDLER_NOEC(  trap_handler_IRQ4     ,  IRQ_OFFSET +  4 ,  PL_KERNEL  )
	TRAPHANDLER_NOEC(  trap_handler_IRQ5     ,  IRQ_OFFSET +  5 ,  PL_KERNEL  )
	TRAPHANDLER_NOEC(  trap_handler_IRQ6     ,  IRQ_OFFSET +  6 ,  PL_KERNEL  )
	TRAPHANDLER_NOEC(  trap_handler_IRQ7     ,  IRQ_OFFSET +  7 ,  PL_KERNEL  )
	TRAPHANDLER_NOEC(  trap_handler_IRQ8     ,  IRQ_OFFSET +  8 ,  PL_KERNEL  )
	TRAPHANDLER_NOEC(  trap_handler_IRQ9     ,  IRQ_OFFSET +  9 ,  PL_KERNEL  )
	TRAPHANDLER_NOEC(  trap_handler_IRQ10    ,  IRQ_OFFSET + 10 ,  PL_KERNEL  )
	TRAPHANDLER_NOEC(  trap_handler_IRQ11    ,  IRQ_OFFSET + 11 ,  PL_KERNEL  )
	TRAPHANDLER_NOEC(  trap_handler_IRQ12    ,  IRQ_OFFSET + 12 ,  PL_KERNEL  )
	TRAPHANDLER_NOEC(  trap_handler_IRQ13    ,  IRQ_OFFSET + 13 ,  PL_KERNEL  )
	TRAPHANDLER_NOEC(  trap_handler_IRQ14    ,  IRQ_OFFSET + 14 ,  PL_KERNEL  )
	TRAPHANDLER_NOEC(  trap_handler_IRQ15    ,  IRQ_OFFSET + 15 ,  PL_KERNEL  )

	TRAPHANDLER_NOEC(  trap_handler_SYSCALL  ,  T_SYSCALL  ,  PL_USER    )

.data
	.int 0; .int 0;

/*
 * Lab 3: Your code here for _alltraps
 */

.text
_alltraps:
	pushl %ds
	pushl %es
	pushal
	movw $(GD_KD), %ax
	movw %ax, %ds
	movw %ax, %es
	pushl %esp // the parameter of trap is a pointer, which just points %esp
	call trap