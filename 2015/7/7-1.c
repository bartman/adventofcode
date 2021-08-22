#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <alloca.h>
#include <assert.h>
#include <err.h>

enum op {
	UNINITIALIZED,
	PASS,
	NOT,
	AND,
	OR,
	LSHIFT,
	RSHIFT,
	OP_MAX
};

struct arg {
	union {
		uint32_t raw;
		struct {
			uint16_t is_gate:1;
			uint16_t value;
		};
	};
};

struct gate {
	uint8_t op;
	uint8_t evaluated:1;
	uint8_t computed:1;
	uint16_t cache_value;
	struct arg in1, in2;
};

static const char *op_names[OP_MAX] = {
	"UNINITIALIZED",
	"PASS",
	"NOT",
	"AND",
	"OR",
	"LSHIFT",
	"RSHIFT",
};

static enum op op_from_string(const char *txt)
{
	for (int i=0; i<OP_MAX; i++) {
		if (!strcmp(txt, op_names[i]))
			return (enum op)i;
	}
	errx(1, "bad op name: %s", txt);
}

#define GATE_MAX (100*100)
static struct gate gates[GATE_MAX] = {0,};
static unsigned unique_gates = 0;
static unsigned evaluated_gates = 0;

typedef uint16_t (*op_func)(uint16_t in1, uint16_t in2);

static uint16_t func_uninitialized(uint16_t in1, uint16_t in2);
static uint16_t func_value(uint16_t in1, uint16_t in2);
static uint16_t func_pass(uint16_t in1, uint16_t in2);
static uint16_t func_not(uint16_t in1, uint16_t in2);
static uint16_t func_and(uint16_t in1, uint16_t in2);
static uint16_t func_or(uint16_t in1, uint16_t in2);
static uint16_t func_lshift(uint16_t in1, uint16_t in2);
static uint16_t func_rshift(uint16_t in1, uint16_t in2);

static op_func funcs[OP_MAX] = {
	func_uninitialized,
	func_pass,
	func_not,
	func_and,
	func_or,
	func_lshift,
	func_rshift,
};

static uint16_t gate_id_from_string(const char *txt)
{
	unsigned a, b;
	uint16_t id;

	assert(isalpha(txt[0]));
	if (!txt[1]) {
		a = 0;
		b = tolower(txt[0]) - 'a' + 1;
	} else {
		assert(isalpha(txt[1]));
		assert(!txt[2]);

		a = tolower(txt[0]) - 'a' + 1;
		b = tolower(txt[1]) - 'a' + 1;

	}

	assert(a<32);
	assert(b<32);

	id = (a*100) + b;

	assert(id < GATE_MAX);

	//printf("%s: %s %u\n", __func__, txt, id);

	return id;
}

static const char *gate_name_from_id(uint16_t id)
{
	unsigned a, b;
	static char buf[3] = {0,};

	a = id / 100;
	b = id % 100;

	if (a) {
		buf[0] = 'a' + a - 1;
		buf[1] = 'a' + b - 1;
	} else {
		assert(b);
		buf[0] = 'a' + b - 1;
		buf[1] = 0;
	}

	//printf("%s: %u %s\n", __func__, id, buf);

	assert(id == gate_id_from_string(buf));

	return buf;
}

static struct arg arg_from_string(const char *txt)
{
	struct arg arg;
	char *end = NULL;

	long tmp = strtol(txt, &end, 10);
	if (tmp != LONG_MAX && !(end && *end)) {
		assert(tmp < UINT16_MAX);
		arg.is_gate = 0;
		arg.value = tmp;
		return arg;
	}

	arg.is_gate = 1;
	arg.value = gate_id_from_string(txt);
	return arg;
}

static uint16_t func_uninitialized(uint16_t in1, uint16_t in2)
{
	(void)in1;
	(void)in2;
	errx(1, "unintialized gate");
	return 0;
}
static uint16_t func_value(uint16_t in1, uint16_t in2)
{
	(void)in2;
	return in1;
}
static uint16_t func_pass(uint16_t in1, uint16_t in2)
{
	(void)in2;
	return in1;
}
static uint16_t func_not(uint16_t in1, uint16_t in2)
{
	(void)in2;
	return ~in1;
}
static uint16_t func_and(uint16_t in1, uint16_t in2)
{
	return in1 & in2;
}
static uint16_t func_or(uint16_t in1, uint16_t in2)
{
	return in1 | in2;
}
static uint16_t func_lshift(uint16_t in1, uint16_t in2)
{
	return in1 << in2;
}
static uint16_t func_rshift(uint16_t in1, uint16_t in2)
{
	return in1 >> in2;
}


static uint16_t gate_value(uint16_t id);

static uint16_t arg_value(struct arg arg)
{
	if (!arg.is_gate)
		return arg.value;

	return gate_value(arg.value);
}

static const char * dump_arg(char *buf, size_t bsize, const struct arg *arg)
{
	if (arg->is_gate) {
		snprintf(buf, bsize, "G/%s", gate_name_from_id(arg->value));
	} else {
		snprintf(buf, bsize, "C/%u", arg->value);
	}
	return buf;
}

static const char * dump_gate(char *buf, size_t bsize, const struct gate *g)
{
	char str1[128];
	char str2[128];

	switch (g->op) {
	default:
	case UNINITIALIZED:
		snprintf(buf, bsize, "%s", op_names[g->op]);
		break;
	case PASS:
	case NOT:
		snprintf(buf, bsize, "%s { %s }", op_names[g->op],
			 dump_arg(str1, sizeof(str1), &g->in1));
		break;
	case AND:
	case OR:
	case LSHIFT:
	case RSHIFT:
		snprintf(buf, bsize, "%s { %s } { %s }", op_names[g->op],
			 dump_arg(str1, sizeof(str1), &g->in1),
			 dump_arg(str2, sizeof(str2), &g->in2));
		break;
	}

	return buf;
}

static uint16_t gate_value(uint16_t id)
{
	struct gate *g;
	op_func f;
	uint16_t in1, in2, out;
	char tmp[256];

	assert(id < GATE_MAX);
	g = gates + id;

	if (g->computed)
		return g->cache_value;

	if (!g->evaluated) {
		g->evaluated = 1;
		evaluated_gates ++;

#if 0
		printf("[%u/%u] <%s> %s\n",
		       evaluated_gates, unique_gates,
		       gate_name_from_id(id),
		       dump_gate(tmp, sizeof(tmp), g));
#endif
	}

	f = funcs[g->op];
	in1 = arg_value(g->in1);
	in2 = arg_value(g->in2);

	out = f(in1, in2);

	g->computed = 1;
	g->cache_value = out;

	return out;
}


int main(int argc, char *argv[])
{
	const char *name = "input.txt";
	int fd, rc;
	struct stat st;
	char *buf, *p;
	unsigned int n, num, idx, out;
	char tmp[256];

	if (argc > 1) {
		name = argv[1];
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
		struct gate g = {PASS,0,0};
		char left[16]="", what[16]="", right[16]="", target[16]="";

		//printf("%.*s\n", (unsigned)(strchr(p, '\n')-p), p);

		rc = sscanf(p, "%15s -> %15s%n", left, target, &n);
		if (rc == 2) {
			char *end = NULL;
			unsigned long tmp;
			assert(n>=6);
			p+=n;
			g.op = PASS;
			g.in1 = arg_from_string(left);
			goto assign_and_advance;
		}

		rc = sscanf(p, "NOT %15s -> %s%n", left, target, &n);
		if (rc == 2) {
			assert(n>=9);
			assert(target[0]);
			p+=n;
			g.op = NOT;
			g.in1 = arg_from_string(left);
			goto assign_and_advance;
		}

		rc = sscanf(p, "%15s %15s %15s -> %s%n", left, what, right, target, &n);
		if (rc == 4) {
			assert(left[0]);
			assert(what[0]);
			assert(right[0]);
			assert(target[0]);
			assert(n>=10);
			p+=n;

			g.in1 = arg_from_string(left);
			g.op = op_from_string(what);
			g.in2 = arg_from_string(right);
			goto assign_and_advance;
		} else {
			char *end = strchr(p, '\n');
			errx(1, "failed parsing: %.*s", (unsigned)(end-p), p);
		}

assign_and_advance:
		idx = gate_id_from_string(target);

		//printf("[%s] %s\n", gate_name_from_id(idx), dump_gate(tmp, sizeof(tmp), &g));
		if (g.in1.is_gate)
			assert(idx != g.in1.value);
		if (g.in2.is_gate)
			assert(idx != g.in2.value);

		if (gates[idx].op != UNINITIALIZED) {
			warnx("gate %u already set", idx);
			printf("[%s] %s\n", gate_name_from_id(idx), dump_gate(tmp, sizeof(tmp), &gates[idx]));
			exit(1);
		}

		assert(idx<GATE_MAX);
		gates[idx] = g;
		unique_gates ++;

		while (*p && isspace(*p)) p++;

		//printf("%s %u,%u through %u,%u\n", ops[op].name, p1.x, p1.y, p2.x, p2.y);
	}

	//printf("unique_gates=%u\n", unique_gates);
	//printf("---\n");

	idx = gate_id_from_string("a");
	out = gate_value(idx);

	printf("%d\n", out);

	return 0;
}


