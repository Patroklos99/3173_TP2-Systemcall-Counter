#include<unistd.h>

int main(int argc, char **argv) {
	if (argc >= 2) {
		char * env[] = { NULL };
		execve(argv[1], &argv[1], env);
	}
	return 0;
}

