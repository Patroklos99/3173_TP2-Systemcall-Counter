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

int trouver_numero_signal(int *numero_signal, int status) {
    if (WIFSTOPPED(status)) {
        *numero_signal = WSTOPSIG(status);
        if (*numero_signal == SIGTRAP)
            *numero_signal = 0;
    }
    return *numero_signal;
}

int executer_processus_parent(int status, int *compteur1, int *compteur2, pid_t pid_enfant) {
    int numero_signal = 0;
    while (1) {
        pid_t pid_valide = wait(&status);
        ptrace(PTRACE_SETOPTIONS, pid_enfant, 0,
               PTRACE_O_EXITKILL | PTRACE_O_TRACEEXEC | PTRACE_O_TRACEFORK | PTRACE_O_TRACECLONE);
        incrementer_compteurs(compteur1, compteur2, status);
        numero_signal = trouver_numero_signal(&numero_signal, status);
        if (WIFEXITED(status) && pid_valide == pid_enfant) {
            printf("%d\n", *compteur2);
            printf("%d", *compteur1);
            return WEXITSTATUS (status);
        } else if (WIFSIGNALED(status)) {
            printf("%d\n", *compteur2);
            printf("%d", *compteur1);
            return 128 + WTERMSIG(status);
        }
        ptrace(PTRACE_CONT, pid_valide, 0, numero_signal);
    }
}


int main(int argc, char *argv[]) {
    int status = 0;
    int compteur1 = 0;
    int compteur2 = 0;
    pid_t pid_enfant = fork();
    if (pid_enfant < 0)
        exit(1);
    else if (pid_enfant == 0) {
        ptrace(PTRACE_TRACEME, pid_enfant, NULL, NULL);
        raise(SIGSTOP);
        execve(argv[1], &argv[1], NULL);
        return 1;
    } else {
        return executer_processus_parent(status, &compteur1, &compteur2, pid_enfant);
    }
}
