TARGETS = 4

all: ${TARGETS}

${TARGETS}: %: %.c
	gcc -O3 -o $@ $< -lssl -lcrypto
