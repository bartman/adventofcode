#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <alloca.h>
#include <assert.h>

static inline bool isnice(const char *str)
{
	bool cond1 = false, cond2 = false;

	for (int i=0; str[i]; i++) {

		if (i && !cond1) {
			char seek[3] = { str[i-1], str[i], 0 };
			const char *ch = strstr(str+i+1, seek);
			if (ch)
				cond1 = true;
		}

		if (i>1 && !cond2) {
			if (str[i-2] == str[i])
				cond2 = true;
		}

		if (cond1 && cond2)
			return true;
	}

	return false;
}

int main(int argc, char *argv[])
{
	const char *name = "input.txt";
	int fd, rc, blen;
	struct stat st;
	char *buf, *p, *line, *lend;
	long total = 0;

	if (argc > 1)
		name = argv[1];

	fd = open(name, O_RDONLY);
	assert(fd>0);

	rc = fstat(fd, &st);
	assert(rc == 0);

	buf = (char*)alloca(st.st_size + 1);
	assert(buf != NULL);

	blen = read(fd, buf, st.st_size);
	assert(blen == st.st_size);

	buf[blen] = 0;

	for (p=buf; *p; ) {

		line = p;
		lend = strchr(line, '\n');
		if (!lend)
			lend = buf+blen;

		*lend = 0;
		if (isnice(line))
			total ++;

		p = lend+1;
	}

	printf("%ld\n", total);

	return 0;
}


