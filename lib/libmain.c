// Called from entry.S to get us going.
// entry.S already took care of defining envs, pages, uvpd, and uvpt.

#include <inc/lib.h>

extern void umain(int argc, char **argv);

const volatile struct Env *thisenv;
const char *binaryname = "<unknown>";

void
_log(const char *file, int line, const char * color, const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	// cprintf("[%s:%d]: ", file, line);
	cprintf("%s", color);
	vcprintf(fmt, ap);
	cprintf("\n" AT_RESET);
	va_end(ap);
}

void
libmain(int argc, char **argv)
{
	// set thisenv to point at our Env structure in envs[].
	// LAB 3: Your code here.
	thisenv = envs + ENVX(sys_getenvid());

	// save the name of the program so that panic() can use it
	if (argc > 0)
		binaryname = argv[0];

	// call user main routine
	umain(argc, argv);

	// exit gracefully
	exit();
}

