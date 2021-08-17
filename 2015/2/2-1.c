#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <alloca.h>
#include <assert.h>

static inline long area(int l, int w, int h)
{
	long area, extra;

	area = 2*l*w + 2*w*h + 2*h*l;

	if (l<=h && w<=h)
		extra = l * w;
	else if (l<=w && h<=w)
		extra = l * h;
	else
		extra = w * h;

	return area + extra;
}

int main(int argc, char *argv[])
{
	const char *name = "input.txt";
	int fd, rc;
	struct stat st;
	char *buf, *p;
	int l, w, h, n;
	long total = 0;

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

	for (p=buf; *p; ) {
		l=w=h=n=0;
		rc = sscanf(p, "%dx%dx%d%n", &l, &w, &h, &n);
		assert(rc==3);
		assert(n>=5);

		total += area(l, w, h);

		p += n;
		while (*p && isspace(*p)) p++;
	}

	printf("%d\n", total);

	return 0;
}


