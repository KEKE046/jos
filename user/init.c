#include <inc/lib.h>

struct {
	char msg1[5000];
	char msg2[1000];
} data = {
	"this is initialized data",
	"so is this"
};

char bss[6000];

int
sum(const char *s, int n)
{
	int i, tot = 0;
	for (i = 0; i < n; i++)
		tot ^= i * s[i];
	return tot;
}

void
umain(int argc, char **argv)
{
	int i, r, x, want;
	char args[256];

	clogp("init: running");

	want = 0xf989e;
	if ((x = sum((char*)&data, sizeof data)) != want)
		clogw("init: data is not initialized: got sum %08x wanted %08x", x, want);
	else
		clogi("init: data seems okay");
	if ((x = sum(bss, sizeof bss)) != 0)
		clogw("bss is not initialized: wanted sum 0 got %08x", x);
	else
		clogi("init: bss seems okay");

	// output in one syscall per line to avoid output interleaving 
	strcat(args, "init: args:");
	for (i = 0; i < argc; i++) {
		strcat(args, " '");
		strcat(args, argv[i]);
		strcat(args, "'");
	}
	cprintf("%s\n", args);

	cprintf("init: running sh\n");

	// being run directly from kernel, so no file descriptors open yet
	close(0);
	if ((r = opencons()) < 0)
		panic("opencons: %e", r);
	if (r != 0)
		panic("first opencons used fd %d", r);
	if ((r = dup(0, 1)) < 0)
		panic("dup: %e", r);
	while (1) {
		clogp("init: starting sh");
		r = spawnl("/sh", "sh", (char*)0);
		if (r < 0) {
			cloge("init: spawn sh: %e", r);
			continue;
		}
		wait(r);
	}
}
