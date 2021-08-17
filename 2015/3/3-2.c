#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
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
	int x[2], minx=0, maxx=0, sizex=0;
	int y[2], miny=0, maxy=0, sizey=0;
	int i, v;
	char *visited;
	int total = 0;

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

	i = 0;
	memset(x, 0, sizeof(x));
	memset(y, 0, sizeof(y));

#define MOVE(ch) ({ \
	switch (ch) { \
	case '^': y[i]++; break; \
	case 'v': y[i]--; break; \
	case '>': x[i]++; break; \
	case '<': x[i]--; break; \
	} \
})

	for (p=buf; *p; p++) {
		MOVE(*p);
		if (x[i]<minx) minx=x[i];
		if (x[i]>maxx) maxx=x[i];
		if (y[i]<miny) miny=y[i];
		if (y[i]>maxy) maxy=y[i];
		i = (i+1) % 2;
	}

	sizex = maxx - minx + 1;
	sizey = maxy - miny + 1;

	visited = (char*) calloc(sizex,sizey);
	assert(visited);

#define Vindex(_x,_y) ( ( (_x) - minx ) + ( ( (_y) - miny ) * sizex ) )

	i = 0;
	memset(x, 0, sizeof(x));
	memset(y, 0, sizeof(y));
	v = Vindex(0,0);
	visited[v] = 1;
	total = 1;

	for (p=buf; *p; p++) {
		MOVE(*p);

		v = Vindex(x[i],y[i]);

		if (!visited[v]) {
			total ++;
			visited[v] = 1;
		}

		i = (i+1) % 2;
	}

	printf("%d\n", total);
}


