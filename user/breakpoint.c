// program to cause a breakpoint trap

#include <inc/lib.h>

void
umain(int argc, char **argv)
{
	asm volatile("push %ebp");
	asm volatile("mov %esp, %ebp");
	asm volatile("int $3");
}

