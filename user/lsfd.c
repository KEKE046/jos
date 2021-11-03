#include <inc/lib.h>

void
usage(void)
{
	cloge("usage: lsfd [-1]");
	exit();
}

void
umain(int argc, char **argv)
{
	int i, usefprint = 0;
	struct Stat st;
	struct Argstate args;

	argstart(&argc, argv, &args);
	while ((i = argnext(&args)) >= 0)
		if (i == '1')
			usefprint = 1;
		else
			usage();

	for (i = 0; i < 32; i++)
		if (fstat(i, &st) >= 0) {
			if (usefprint)
				fprintf(1, AT_MAG"fd %d"AT_RESET":"AT_BRI_GRN" name %s" AT_GRY" isdir %d size %d "AT_BRI_YLW"dev %s\n"AT_RESET,
					i, st.st_name, st.st_isdir,
					st.st_size, st.st_dev->dev_name);
			else
				cprintf(   AT_MAG"fd %d"AT_RESET":"AT_BRI_GRN" name %s" AT_GRY" isdir %d size %d "AT_BRI_YLW"dev %s\n"AT_RESET,
					i, st.st_name, st.st_isdir,
					st.st_size, st.st_dev->dev_name);
		}
}
