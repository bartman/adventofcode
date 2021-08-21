#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <alloca.h>
#include <assert.h>
#include <err.h>
#include <limits.h>
#include <openssl/md5.h>

#define INPUT_SIZE 8
const char input[INPUT_SIZE+1] = "ckczppom";

static inline int count_zeros(const unsigned char out[MD5_DIGEST_LENGTH])
{
	int i;
	for (i=0; i<MD5_DIGEST_LENGTH; i++) {
		if (!out[i])
			continue;
		if (!(out[i] & 0xF0))
			return (2*i) + 1;
		return 2*i;
	}
	return 2*MD5_DIGEST_LENGTH;
}

int main(int argc, char *argv[])
{
	MD5_CTX c;
	int i, rc, want, found;
	char in[4096];
	unsigned char out[MD5_DIGEST_LENGTH];

	if (argc != 2)
		errx(1, "need a <prefix> size");
	want = atoi(argv[1]);
	assert(want);

	for (i=0; i<INT_MAX; i++) {

		rc = snprintf(in, sizeof(in), "%s%u", input, i);
		assert(rc>INPUT_SIZE);

		MD5_Init(&c);
		MD5_Update(&c, in, rc);
		MD5_Final(out, &c);

		found = count_zeros(out);
		if (found >= want) {
			printf("[%u]  \"%s\"  ", i, in);
			for (i=0; i<MD5_DIGEST_LENGTH; i++) {
				printf("%02x", out[i]);
			}
			printf("\n");
			return 0;
		}
	}

	errx(1, "not found");

	return 1;
}


