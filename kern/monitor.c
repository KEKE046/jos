// Simple command-line kernel monitor useful for
// controlling the kernel and exploring the system interactively.

#include <inc/stdio.h>
#include <inc/string.h>
#include <inc/memlayout.h>
#include <inc/assert.h>
#include <inc/x86.h>

#include <kern/console.h>
#include <kern/monitor.h>
#include <kern/kdebug.h>
#include <kern/trap.h>

#include <kern/pmap.h>
#include <inc/ansiterm.h>
#include <kern/env.h>

#define CMDBUF_SIZE	80	// enough for one VGA text line


struct Command {
	const char *name;
	const char *desc;
	// return -1 to force monitor to exit
	int (*func)(int argc, char** argv, struct Trapframe* tf);
};

static struct Command commands[] = {
	{ "help", "Display this list of commands", mon_help },
	{ "kerninfo", "Display information about the kernel", mon_kerninfo },
	{ "colortest", "Test the console color", mon_color_test},
	{ "clear", "Clear the screen", mon_clear},
	{ "mem", "Memory command", mem_memcmd},
	{ "backtrace", "Backtrace the stack", mon_backtrace},
	{ "debug", "User code debug", mon_debug}
};

/***** Implementations of basic kernel monitor commands *****/

int
mon_color_test(int argc, char ** argv, struct Trapframe * tf) {
	static const int forecolor[] = {
		30, 31, 32, 33, 34, 35, 36, 37,
		90, 91, 92, 93, 94, 95, 96, 97
	};
	static const int backcolor[] = {
		40, 41, 42, 43, 44, 45, 46, 47,
		100, 101, 102, 103, 104, 105, 106, 107
	};
	cprintf("     ");
	for(int j = 0 ; j < 16; j++) {
		cprintf("%4d ", j);
	}
	cprintf("\n");
	for(int i = 0; i < 16; i++) {
		cprintf("%4d ", i);
		for(int j = 0; j < 16; j++) {
			cprintf("\033[%d;%dmTEST\033[0m ", backcolor[j], forecolor[i]);
		}
		cprintf("\n");
	}
	return 0;
}

int
mon_clear(int argc, char ** argv, struct Trapframe * tf) {
	cprintf(AT_ERASE_ALL AT_CUR_POS_HOME);
	return 0;
}

int
mon_help(int argc, char **argv, struct Trapframe *tf)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(commands); i++)
		cprintf("%s - %s\n", commands[i].name, commands[i].desc);
	return 0;
}

int
mon_kerninfo(int argc, char **argv, struct Trapframe *tf)
{
	extern char _start[], entry[], etext[], edata[], end[];

	cprintf("Special kernel symbols:\n");
	cprintf("  _start                  %08x (phys)\n", _start);
	cprintf("  entry  %08x (virt)  %08x (phys)\n", entry, entry - KERNBASE);
	cprintf("  etext  %08x (virt)  %08x (phys)\n", etext, etext - KERNBASE);
	cprintf("  edata  %08x (virt)  %08x (phys)\n", edata, edata - KERNBASE);
	cprintf("  end    %08x (virt)  %08x (phys)\n", end, end - KERNBASE);
	cprintf("Kernel executable memory footprint: %dKB\n",
		ROUNDUP(end - entry, 1024) / 1024);
	return 0;
}

int
mon_backtrace(int argc, char **argv, struct Trapframe *tf)
{
	cprintf("Stack backtrace:\n");
	struct Eipdebuginfo info;
	volatile uint32_t * ebp = (uint32_t*)read_ebp(), eip;
	uint32_t cr3 = rcr3();
	if(curenv) lcr3(PADDR(curenv->env_pgdir));
	int errno = 0;
	do {
		eip = *(ebp + 1);
		if((errno=debuginfo_eip(eip - 4, &info)) < 0) {
			loge("debuginfo_eip(%p) %e", eip - 4, errno);
			break;
		}
		cprintf("  ebp %08x eip %08x\n", ebp, eip);
		cprintf("  ebp %08x eip %08x args %08x %08x %08x %08x\n",
			ebp, eip, *(ebp + 2), *(ebp + 3), *(ebp + 4), *(ebp + 5));
		cprintf("        %s:%d: ", info.eip_file, info.eip_line);
		for(int i = 0; i < info.eip_fn_namelen; i++) {
			cputchar(info.eip_fn_name[i]);
		}
		cprintf("+%d\n", eip - info.eip_fn_addr);
		if(ebp == NULL) break;
		ebp = (uint32_t*)*ebp;
	} while(ebp);
	lcr3(cr3);
	return 0;
}

int
mon_debug(int argc, char **argv, struct Trapframe * tf) {
	if(argc > 1) {
		if(tf->tf_trapno != T_BRKPT) {
			cprintf("Trap is not a breakpoint, continuing.\n");
		}
		char * cmd = argv[1];
		if(strcmp(cmd, "si") == 0) {
			tf->tf_eflags |= FL_TF;
			env_run(curenv);
		}
		else if(strcmp(cmd, "c") == 0) {
			tf->tf_eflags &= ~FL_TF;
			env_run(curenv);
		}
	}
	cprintf("Usage: debug <si|c>\n");
	return 0;
}

/***** Kernel monitor command interpreter *****/

#define WHITESPACE "\t\r\n "
#define MAXARGS 16

static int
runcmd(char *buf, struct Trapframe *tf)
{
	int argc;
	char *argv[MAXARGS];
	int i;

	// Parse the command buffer into whitespace-separated arguments
	argc = 0;
	argv[argc] = 0;
	while (1) {
		// gobble whitespace
		while (*buf && strchr(WHITESPACE, *buf))
			*buf++ = 0;
		if (*buf == 0)
			break;


		// save and scan past next arg
		if (argc == MAXARGS-1) {
			cprintf("Too many arguments (max %d)\n", MAXARGS);
			return 0;
		}
		argv[argc++] = buf;
		while (*buf && !strchr(WHITESPACE, *buf))
			buf++;
	}
	argv[argc] = 0;

	// Lookup and invoke the command
	if (argc == 0)
		return 0;
	for (i = 0; i < ARRAY_SIZE(commands); i++) {
		if (strcmp(argv[0], commands[i].name) == 0)
			return commands[i].func(argc, argv, tf);
	}
	cprintf("Unknown command '%s'\n", argv[0]);
	return 0;
}

void
monitor(struct Trapframe *tf)
{
	char *buf;

	cprintf("Welcome to the JOS kernel monitor!\n");
	cprintf("Type 'help' for a list of commands.\n");

	if (tf != NULL)
		print_trapframe(tf);

	while (1) {
		buf = readline("K> ");
		if (buf != NULL)
			if (runcmd(buf, tf) < 0)
				break;
	}
}
