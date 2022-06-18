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

#include <sys/ptrace.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>

/*
 * Affiche les données correspondante lorsque le programme s'est terminé à cause d'un signal.
 *
 * @param nb_execve nb d'execve executés.
 * @param nb_fork_clone nb de clones et fork executés.
 * @param status valeur necessaire pour les validations.
 *
 * return le numero du signal correspondant à la cause de la sortie + 128.
 */
int valider_sortie_signal(const int *nb_execve, const int *nb_fork_clone, const int status) {
    printf("%d\n", *nb_fork_clone);
    printf("%d", *nb_execve);
    return 128 + WTERMSIG(status);
}

/*
 * Affiche les données correspondante lorsque le programme s'est terminé correctement.
 *
 * @param nb_execve nb d'execve executés.
 * @param nb_fork_clone nb de clones et fork executés.
 * @param status valeur necessaire pour les validations.
 *
 * return le numero du signal correspondant à la cause de la sortie.
 */
int valider_sortie_correcte(const int *nb_execve, const int *nb_fork_clone, const int status) {
    if (*nb_execve + *nb_fork_clone == 0) {
        printf("%d", *nb_fork_clone);
        return WEXITSTATUS (status);
    } else {
        printf("%d\n", *nb_fork_clone);
        printf("%d", *nb_execve);
        return WEXITSTATUS (status);
    }
}

/*
 * Valide les sigtrap pour l'incrementation correspondante des compteurs
 *
 * @param nb_execve nb d'execve executés.
 * @param nb_fork_clone nb de clones et fork executés.
 * @param status valeur necessaire pour les validations.
 */
void incrementer_compteurs(int *nb_execve, int *nb_fork_clone, int status) {
    if (status >> 8 == (SIGTRAP | (PTRACE_EVENT_EXEC << 8)))
        (*nb_execve)++;
    if (status >> 8 == (SIGTRAP | (PTRACE_EVENT_FORK << 8)))
        (*nb_fork_clone)++;
    if (status >> 8 == (SIGTRAP | (PTRACE_EVENT_CLONE << 8)))
        (*nb_fork_clone)++;
}

/*
 * Valide si le signal est un sigtrap pour faire la modification correspondante.
 *
 * @param numero_signal valeur à modifier.
 * @param status valeur necessaire pour les validations.
 *
 * return numero_signal numero correspondant si cest un sigtrap ou une autre valeur s'il ne l'est pas.
 */
int trouver_numero_signal(int *numero_signal, int status) {
    if (WIFSTOPPED(status)) {
        *numero_signal = WSTOPSIG(status);
        if (*numero_signal == SIGTRAP)
            *numero_signal = 0;
    }
    return *numero_signal;
}

/*
 * Suit le processus enfant pour le valider et le traiter.
 *
 * @param nb_execve premiere variable à augmenter
 * @param nb_fork_clone deuxieme variable à augmenter.
 * @param pid_t pid_enfant valeur du pid du precessus enfant suivi.
 *
 * return le numero de sortie adéquat.
 */
int executer_processus_parent(int status, int *nb_execve, int *nb_fork_clone, pid_t pid_enfant) {
    int numero_signal = 0;
    while (1) {
        pid_t pid_valide = wait(&status);
        if (pid_valide == -1)
            return pid_valide;
        ptrace(PTRACE_SETOPTIONS, pid_enfant, 0, PTRACE_O_EXITKILL | PTRACE_O_TRACEEXEC | PTRACE_O_TRACEFORK | PTRACE_O_TRACECLONE);
        incrementer_compteurs(nb_execve, nb_fork_clone, status);
        numero_signal = trouver_numero_signal(&numero_signal, status);
        if (WIFEXITED(status) && pid_valide == pid_enfant)
            return valider_sortie_correcte(nb_execve, nb_fork_clone, status);
        else if (WIFSIGNALED(status))
            return valider_sortie_signal(nb_execve, nb_fork_clone, status);

        ptrace(PTRACE_CONT, pid_valide, 0, numero_signal);
    }
}

int main(int argc, char *argv[]) {
    int status = 0;
    int nb_execve = 0;
    int nb_fork_clone = 0;
    int err = 0;
    pid_t pid_enfant = fork();
    if (pid_enfant < 0)
        exit(1);
    else if (pid_enfant == 0) {
        err = (int) ptrace(PTRACE_TRACEME, pid_enfant, NULL, NULL);
        if (err == 1)
            return err;
        raise(SIGSTOP);
        execve(argv[1], &argv[1], NULL);
        return 1;
    } else
        return executer_processus_parent(status, &nb_execve, &nb_fork_clone, pid_enfant);
}
