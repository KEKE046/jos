// Simple implementation of cprintf console output for the kernel,
// based on printfmt() and the kernel console's cputchar().

#include <inc/types.h>
#include <inc/stdio.h>
#include <inc/stdarg.h>


static void
putch(int ch, int *cnt)
{
	cputchar(ch);
	*cnt++;
}

int _cons_putc(int c);

int
vcprintf(const char *fmt, va_list ap)
{
	int cnt = 0;
	_cons_putc('v');
	_cons_putc('c');
	_cons_putc('p');
	_cons_putc('r');
	_cons_putc('i');
	_cons_putc('n');
	_cons_putc('t');
	_cons_putc('f');
	_cons_putc('\r');
	_cons_putc('\n');
	vprintfmt((void*)putch, &cnt, fmt, ap);
	return cnt;
}

int
cprintf(const char *fmt, ...)
{
	va_list ap;
	int cnt;

	va_start(ap, fmt);
	cnt = vcprintf(fmt, ap);
	va_end(ap);

	return cnt;
}

