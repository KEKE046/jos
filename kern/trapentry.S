/* See COPYRIGHT for copyright information. */

#include <inc/mmu.h>
#include <inc/memlayout.h>
#include <inc/trap.h>



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
#define TRAPHANDLER(name, num)						\
	.globl name;		/* define global symbol for 'name' */	\
	.type name, @function;	/* symbol type is function */		\
	.align 2;		/* align function definition */		\
	.text; \
	name:			/* function starts here */		\
	pushl $(num);							\
	jmp _alltraps; \
	.data; .int name; .int num;

/* Use TRAPHANDLER_NOEC for traps where the CPU doesn't push an error code.
 * It pushes a 0 in place of the error code, so the trap frame has the same
 * format in either case.
 */
#define TRAPHANDLER_NOEC(name, num)					\
	.globl name;							\
	.type name, @function;						\
	.align 2;							\
	.text; \
	name:								\
	pushl $0;							\
	pushl $(num);							\
	jmp _alltraps; \
	.data; .int name; .int num;

.data
.global trapentry_table
trapentry_table:

.text

/*
 * Lab 3: Your code here for generating entry points for the different traps.
 */


	TRAPHANDLER_NOEC(  trap_hander_DIVIDE  ,0   )
	TRAPHANDLER_NOEC(  trap_hander_DEBUG   ,1   )
	TRAPHANDLER_NOEC(  trap_hander_NMI     ,2   )
	TRAPHANDLER_NOEC(  trap_hander_BRKPT   ,3   )
	TRAPHANDLER_NOEC(  trap_hander_OFLOW   ,4   )
	TRAPHANDLER_NOEC(  trap_hander_BOUND   ,5   )
	TRAPHANDLER_NOEC(  trap_hander_ILLOP   ,6   )
	TRAPHANDLER_NOEC(  trap_hander_DEVICE  ,7   )
	TRAPHANDLER     (  trap_hander_DBLFLT  ,8   )
	TRAPHANDLER     (  trap_hander_TSS     ,10  )
	TRAPHANDLER     (  trap_hander_SEGNP   ,11  )
	TRAPHANDLER     (  trap_hander_STACK   ,12  )
	TRAPHANDLER     (  trap_hander_GPFLT   ,13  )
	TRAPHANDLER     (  trap_hander_PGFLT   ,14  )
	TRAPHANDLER_NOEC(  trap_hander_FPERR   ,16  )
	TRAPHANDLER_NOEC(  trap_hander_ALIGN   ,17  )
	TRAPHANDLER_NOEC(  trap_hander_MCHK    ,18  )
	TRAPHANDLER_NOEC(  trap_hander_SIMDERR ,19  )

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
	pushl %esp
	call trap