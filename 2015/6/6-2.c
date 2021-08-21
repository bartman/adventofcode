#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <alloca.h>
#include <assert.h>

enum {
	TURN_OFF = 0,
	TURN_ON = 1,
	TOGGLE = 2,
	OP_MAX
};

static struct op {
	const char *name;
	unsigned len;
} ops[OP_MAX] = {
	{ "turn off", 8 },
	{ "turn on",  7 },
	{ "toggle",   6 },
};

struct point { unsigned x, y; };

#define COLS 1000
#define ROWS 1000
static uint16_t array[COLS][ROWS] = {{0,},};

int main(int argc, char *argv[])
{
	const char *name = "input.txt";
	int fd, rc;
	struct stat st;
	char *buf, *p;
	struct point p1, p2, i;
	long total = 0;
	int op, n;

	if (argc > 1) {
		buf = argv[1];
		goto process;
	}

	fd = open(name, O_RDONLY);
	assert(fd>0);

	rc = fstat(fd, &st);
	assert(rc == 0);

	buf = (char*)alloca(st.st_size + 1);
	assert(buf != NULL);

	rc = read(fd, buf, st.st_size);
	assert(rc == st.st_size);

	buf[rc] = 0;

process:
	for (p=buf; *p; ) {

		//char *end = strchr(p, '\n');
		//printf("%.*s\n", end-p, p);

		for (op = 0; op < OP_MAX; op++) {
			if (strncmp(p, ops[op].name, ops[op].len))
				continue;
			if (p[ops[op].len] != ' ')
				continue;
			p += ops[op].len + 1;
			break;
		}
		assert(op<OP_MAX);

		rc = sscanf(p, "%d,%d%n", &p1.x, &p1.y, &n);
		assert(rc==2);
		assert(n>=3);
		assert(p1.x<COLS);
		assert(p1.y<ROWS);
		p += n;

		assert(!strncmp(p, " through ", 9));
		p += 9;

		rc = sscanf(p, "%d,%d%n", &p2.x, &p2.y, &n);
		assert(rc==2);
		assert(n>=3);
		assert(p2.x<COLS);
		assert(p2.y<ROWS);
		p += n;

		assert(p1.x<=p2.x);
		assert(p1.y<=p2.y);

		for (i.x = p1.x; i.x <= p2.x; i.x++) {
			for (i.y = p1.y; i.y <= p2.y; i.y++) {
				switch (op) {
				case TURN_OFF:
					if (array[i.x][i.y])
						array[i.x][i.y] -= 1;
					break;
				case TURN_ON:
					array[i.x][i.y] += 1;
					break;
				case TOGGLE:
					array[i.x][i.y] += 2;
					break;
				}
			}
		}

		while (*p && isspace(*p)) p++;

		//printf("%s %u,%u through %u,%u\n", ops[op].name, p1.x, p1.y, p2.x, p2.y);
	}

	total = 0;
	for (i.x = 0; i.x < COLS; i.x++) {
		for (i.y = 0; i.y < ROWS; i.y++) {
			total += array[i.x][i.y];
		}
	}

	printf("%d\n", total);

	return 0;
}


