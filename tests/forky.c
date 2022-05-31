#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
	if (argc >= 2) {
		pid_t pid = fork();
		if (0 == pid) {
                	char * env[] = { NULL };
                	execve(argv[1], &argv[1], env);
        	}
        	waitpid(pid, 0, 0);
	}
	return 0;
}

