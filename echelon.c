/*  INF3173 - TP1
 *  Session : été 2022
 *
 *  IDENTIFICATION.
 *
 *      Nom : Salcedo
 *      Prénom : Renzo A.
 *      Code permanent : SALR02089408
 *      Groupe : 40
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ptrace.h>

int main(int argc, char *argv[]) {   
    int status;
    int compteur1 = 0;
    int compteur2 = 0;
    pid_t pid_enfant = fork();
    if (pid_enfant < 0)
        exit(1);
    else if (pid_enfant == 0) {
        ptrace(PTRACE_TRACEME, pid_enfant, NULL, NULL);
        raise(SIGSTOP);
        execve(argv[1], &argv[1], NULL);
    else {
	wait(status);
	ptrace(PTRACE_SETOPTIONS, pid_enfant, 0,PTRACE_O_EXITKILL | PTRACE_O_TRACEEXEC | PTRACE_O_TRACEFORK | PTRACE_O_TRACECLONE);
	while(1) {
		break;
	}
    }
    return 0;
}
