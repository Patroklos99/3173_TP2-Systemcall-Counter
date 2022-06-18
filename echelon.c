/*  INF3173 - TP1`
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

void incrementer_compteurs(int *compteur1, int *compteur2, int status) {
    if (status >> 8 == (SIGTRAP | (PTRACE_EVENT_EXEC << 8)))
        (*compteur1)++;
    if (status >> 8 == (SIGTRAP | (PTRACE_EVENT_FORK << 8)))
        (*compteur2)++;
    if (status >> 8 == (SIGTRAP | (PTRACE_EVENT_CLONE << 8)))
        (*compteur2)++;
}

int
main(int argc, char *argv[]) {
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
    } else {
        while (1) {
            pid_t id_valide  = wait(&status);
            ptrace(PTRACE_SETOPTIONS, pid_enfant, 0,PTRACE_O_EXITKILL | PTRACE_O_TRACEEXEC | PTRACE_O_TRACEFORK | PTRACE_O_TRACECLONE);
            incrementer_compteurs(&compteur1, &compteur2, status, pid_enfant);
	    ptrace(PTRACE_CONT, id_valide, 0, 0);
        }
    }
    if (compteur1 + compteur2 == 0) {
        printf("%d", 0);
    } else if (WIFSIGNALED (status)) {
        return 128 + WTERMSIG(status);
    } else {
        printf("%d\n", compteur2);
        printf("%d", compteur1);
    }

}
