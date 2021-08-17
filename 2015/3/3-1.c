#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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
	int x, minx=0, maxx=0, sizex=0;
	int y, miny=0, maxy=0, sizey=0;
	int v;
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

	x = 0;
	y = 0;
	for (p=buf; *p; p++) {
		switch (*p) {
		case '^': y++; break;
		case 'v': y--; break;
		case '>': x++; break;
		case '<': x--; break;
		}

		if (x<minx) minx=x;
		if (x>maxx) maxx=x;
		if (y<miny) miny=y;
		if (y>maxy) maxy=y;
	}

	sizex = maxx - minx + 1;
	sizey = maxy - miny + 1;

	visited = (char*) calloc(sizex,sizey);
	assert(visited);

#define Vindex(_x,_y) ( ( (_x) - minx ) + ( ( (_y) - miny ) * sizex ) )

	x = 0;
	y = 0;
	v = Vindex(x,y);
	total = 1;
	visited[v] = 1;

	for (p=buf; *p; p++) {
		switch (*p) {
		case '^': y++; break;
		case 'v': y--; break;
		case '>': x++; break;
		case '<': x--; break;
		}

		v = Vindex(x,y);

		if (!visited[v]) {
			total ++;
			visited[v] = 1;
		}
	}

	printf("%d\n", total);
}


