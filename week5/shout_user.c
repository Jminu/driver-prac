#include <stdio.h>

static inline void smh_putc(unsigned char c) {
        asm volatile("mov x1, %0\n mov x0, #3\n"
                        "hlt 0xf000\n"
                        :: "r" (&c): "x0", "x1", "memory");
}

int main(int argc, char *argv[]) {
	char input[1000];
	char buffer[1000];
	char *ptr;

	scanf("%s", input);

	snprintf(buffer, 1024, "%s", input);
	for (ptr = buffer; *ptr != NULL; ptr++) {
		smh_putc(*ptr);
	}

	return 0;
}	
