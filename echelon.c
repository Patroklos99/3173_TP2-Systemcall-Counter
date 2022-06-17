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
    int compteur_1 = 0;
    int counter_2 = 0;
    pid_t pid_enfant = fork();
    if (pid < 0)
        exit(1);
    else if (pid_enfant == 0) {
        ptrace(PTRACE_TRACEME, pid_enfant, NULL, NULL);
        execve(argv[1], argv[1], NULL);
    return 0;
}
