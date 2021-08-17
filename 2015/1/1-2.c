#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <alloca.h>
#include <assert.h>


int main(int argc, char *argv[])
{
	const char *name = "input.txt";
	int fd, rc;
	struct stat st;
	char *buf, *p;
	int floor;

	if (argc > 1)
		name = argv[1];

	fd = open(name, O_RDONLY);
	assert(fd>0);

	rc = fstat(fd, &st);
	assert(rc == 0);

	buf = (char*)alloca(st.st_size + 1);
	assert(buf != NULL);

	rc = read(fd, buf, st.st_size);
	assert(rc == st.st_size);

	buf[rc] = 0;

	floor = 0;
	for (p=buf; *p; p++) {
		switch (*p) {
		case '(':
			floor ++;
			break;
		case ')':
			floor --;
			break;
		}
		if (floor < 0) {
			printf("%ld\n", p-buf+1);
			exit(0);
		}

	}
}


