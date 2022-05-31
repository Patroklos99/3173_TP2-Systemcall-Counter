# TP1 - `echelon`

L'objectif du TP1 est de développer l'utilitaire `echelon` qui compte le nombre d'appels système `fork`, `clone` ou `execve` faient par un processus et ses processus enfants.

## Avant de commencer

* Cloner (le bouton `fork` en haut à droite) ce dépôt sur le gitlab départemental.
* Le rendre privé : dans `Settings` → `General` → `Visibility` → `Project visibility` → `Private` (n'oubliez pas d'enregistrer).
* Ajouter l'utilisateur `@abdenbi_m` comme mainteneur (oui, j'ai besoin de ce niveau de droits) : `Project information` → `Members` → `Invite member` → `@abdenbi_m`.
* ⚠️ Mal effectuer ces étapes vous expose à des pénalités importantes sans préavis.

## Description de l'outil

```
echelon commande [argument...]
```

`echelon` va créer un processus enfant avec l'appel `fork`. Dans ce processus enfant `echelon`, avec l'appel système `execve`, va exécuter `commande` et ses arguments s'il y a lieu. 
Par la suite, `echelon` compte et affiche le nombre d'appels système `fork`, `clone` et `execve` effectués par le processus `commande` et ses sous-processus.
À chaque fois que le processus `commande` ou l'un de ses processus enfants, fait un de ces appela système, `echelon` va comptabiliser cet appel. Les totaux sont affichés à la fin de l'exécution du processus `commande` et de tous ses processus enfants dans cet ordre,
- un premier nombre qui représente la somme des nombres d'appels à `fork` et à `clone`;
- un deuxième nombre qui représente le nombre d'appels à `execve`.
- si aucun appel `fork` et `clone` ou `execve` n'est fait, la valeur 0 sera affichée. 

<p>

<details>

<summary>Exemple</summary>

<pre>
<b>iam@groot:~/$</b> ./echelon /bin/bash -c "echo 'aucun test ne passe :(' | wc -c"
23
2
2
<b>iam@groot:~/$</b> strace -c /bin/bash -c "echo 'aucun test ne passe :(' | wc -c" 2>&1 | grep -E '(calls|clone|execve)'
% time     seconds  usecs/call     calls    errors syscall
 30,97    0,000083          41         2           clone
  0,00    0,000000           0         1           execve
</pre>
On voit que `echelon` retourne un total de 2 appels à `clone` et 2 appels `execve`. Contrairement à `strace` qui retourne 2 appels à `clone` et 1 appel à `execve` (voir la colonne `calls` qui donne le nombre d'appels effectués). La raison est que `echelon` va compter le `execve` que son processus enfant va faire pour lancer la commande `/bin/bash -c "echo 'aucun test ne passe :(' | wc -c"`, ce qui fait qu'il y a 1 de plus que ce que `strace` comptabilise.
</details>

</p>

### Traitement des erreurs et valeur de retour

- Le code de retour de `echelon` est celui de `commande`.
- Si un appel système utilisé par `echelon` échoue, la valeur 1 doit être retournée.
- Si `commande` est terminée par un signal, le code de retour est 128 + le numéro du signal.
- `echelon` **n'affiche aucun message d'erreur**.

## Directives d'implémentation

Vous devez développer le programme en C.
Le fichier source doit s'appeler `echelon.c` et être à la racine du dépôt.
Vu la taille du projet, tout doit rentrer dans ce seul fichier source.

Pour la réalisation du TP, vous devez respecter les directives suivantes.

### Appels système

L'appel système principal que vous devez utiliser est `ptrace`.
C'est un appel système bas niveau offert par le noyau Linux (et la plupart des systèmes Unix) qui sert à implémenter les débogueurs (comme `gdb`) ou d'autres outils comme `strace`.

`ptrace` est un appel système très puissant, car il permet à un processus de prendre le contrôle d'un autre processus, y compris sa mémoire et le code machine qu'il exécute.

La logique principale de `ptrace` c'est que le processus surveillé (*ptracé*) s'exécute normalement jusqu'à ce qu'il se passe un événement particulier (réception d'un signal, appel système, etc.).
À ce moment-là, le processus surveillé passe dans un **état bloqué** particulier (*ptrace stopped*) qui apparait sous un `t` avec la commande `ps`.

Le processus qui surveille (le traceur) est notifié que le tracé est arrêté avec l'appel système `wait`.
Plutôt que de créer un appel système dédié, les concepteurs ont réutilisé cet appel système, ce qui peut être déroutant.

En résumé, **vous devez utiliser** les appels système suivants,
- `ptrace` pour surveiller l'exécution d'un processus.
- `execve` pour tout recouvrement. **Aucune fonction** de librairie parmi `execl`, `execlp`, `execle`, `execv`, `execvp` ou `execvpe` **n'est autorisée**.
- En plus des appels système cités plus haut, vous devrez également utiliser les appels système `fork` et `wait`.
- Noubliez pas de traiter les erreurs de chaque appel système.

### Précisions

- Dans le cadre du TP, seul le sous-ensemble suivant des requêtes `ptrace` devra être utilisé,
   - `PTRACE_TRACEME` pour tracer `commande`.
   - `PTRACE_SETOPTIONS` avec les flags `PTRACE_O_EXITKILL`, `PTRACE_O_TRACEEXEC`, `PTRACE_O_TRACEFORK` et `PTRACE_O_TRACECLONE`.
   - `PTRACE_CONT` pour faire continuer l'exécution du processus surveillé stoppé.
- Lisez la page de manuel de `ptrace`. Elle est dense et tout ne vous sera pas utile, mais il ne faut pas ignoner les parties qui le seront.
- Commencez par `PTRACE_O_EXITKILL`, `PTRACE_O_TRACEEXEC` et ajoutez `PTRACE_O_TRACEFORK` et `PTRACE_O_TRACECLONE` dans un second temps.
- Attention, `PTRACE_TRACEME` ne stoppe pas l'appelant, vous pouvez donc faire `raise(SIGSTOP);` dans le processus fils juste après le `ptrace` et avant le `execve` (RTFM pour les détails).
- Attention, remettez le signal éventuel dans `PTRACE_CONT` pour permettre à un processus de continuer son exécution s'il y a lieu (RTFM pour les détails).
- [Lisez la page de manuel de `ptrace`](https://twitter.com/jcsrb/status/1392459191353286656).
- L'appel système `wait` va bloquer jusqu'à ce qu'un évenement dans le processus enfant d'`echelon` ou l'un de ses enfants se produit, vous devez donc distinguer les PID des processus qui déclencent ces événements et les traiter en conséquence.
   - Plusieurs appels à `wait` seront nécessaires.
   - C'est à la fin du processus `commande` (terminaison par valeur retournée ou par signal) que `echelon` se terminera.

## Acceptation et remise du TP

### Remise

La remise s'effectue simplement en poussant votre code sur la branche `main` (c'est la seule branche qui sera considérée) de votre dépôt gitlab privé.
Seule la dernière version disponible avant le **dimanche 19 juin à 23h55** sera considérée pour la correction.

### Intégration continue

Vouz pouvez compiler avec `make` (le `Makefile` est fourni).

Vous pouvez vous familiariser avec le contenu du dépôt, en étudiant chacun des fichiers (`README.md`, `Makefile`, `check.bats`, `.gitlab-ci.yml`, etc.).

⚠️ À priori, il n'y a pas de raison de modifier un autre fichier du dépôt.
Si vous en avez besoin, ou si vous trouvez des bogues ou problèmes dans les autres fichiers, merci de me contacter.

Le système d'intégration continue vérifie votre TP à chaque `push`.
Vous pouvez vérifier localement avec `make check` (l'utilitaire `bats` entre autres est nécessaire).

Les tests fournis ne couvrent que les cas d'utilisation de base, en particulier ceux présentés ici.
Il est **fortement suggéré** d'ajouter vos propres tests dans [local.bats](local.bats) et de les pousser pour que l’intégration continue les prenne en compte.
Ils sont dans un job distincts pour avoir une meilleure vue de l'état du projet.

❤ En cas de problème pour exécuter les tests sur votre machine, merci de 1. lire la documentation présente ici et 2. poser vos questions en classe ou sur [Mattermost](https://mattermost.info.uqam.ca/forum/channels/inf3173).
Attention toutefois à ne pas fuiter de l’information relative à votre solution (conception, morceaux de code, etc.)

### Barème et critères de correction

Le barème utilisé est le suivant

* Seuls les tests qui passent sur l'instance `gitlab.info.uqam.ca` (avec l'intégration continue) seront considérés.
  * 60%: pour le jeu de test public fourni dans le sujet (voir section intégration).
  * 40%: pour un jeu de test privé exécuté lors de la correction. Ces tests pourront être plus gros, difficiles et/ou impliquer des cas limites d'utilisation (afin de vérifier l'exactitude et la robustesse de votre code).
* Des pénalités pour des bogues spécifiques et des défauts dans le code source du programme, ce qui inclut, mais sans s'y limiter l'exactitude, la robustesse, la lisibilité, la simplicité, la conception, les commentaires, etc.
* Note: consultez la section suivante pour des exemples de pénalités et éventuellement des conseils pour les éviter.

## Mentions supplémentaires importantes

⚠️ **Intégrité académique**
Rendre public votre dépôt personnel ou votre code ici ou ailleurs ; ou faire des MR contenant votre code vers ce dépôt principal (ou vers tout autre dépôt accessible) sera considéré comme du **plagiat**.

⚠️ Attention, vérifier **=/=** valider.
Ce n'est pas parce que les tests passent chez vous ou ailleurs ou que vous avez une pastille verte sur gitlab que votre TP est valide et vaut 100%.
Par contre, si des tests échouent quelque part, c'est généralement un bon indicateur de problèmes dans votre code.

⚠️ Si votre programme **ne compile pas** ou **ne passe aucun test public**, une note de **0 sera automatiquement attribuée**, et cela indépendamment de la qualité de code source ou de la quantité de travail mise estimée.
Il est ultimement de votre responsabilité de tester et valider votre programme.

Pour les tests, autant publics que privés, les résultats qui font foi sont ceux sur l'instance `gitlab.info.uqam.ca`. Si un test réussi presque ou de temps en temps, il est considéré comme échoué (sauf rares exceptions).


Quelques exemples de bogues fréquents dans les copies TP de INF3173 qui causent une perte de points, en plus d'être responsable de tests échoués:

* Utilisation de variables ou de mémoire non initialisés (comportement indéterminé).
* Mauvaise gestion de la mémoire dynamique (`free`, `malloc`, `calloc` et compagnie).
* Mauvaise vérification des cas d'erreur des fonctions et appels système (souvent comportement indéterminé si le programme continue comme si de rien n'était)
* Utilisation de valeurs numériques arbitraires (*magic number*) qui cause des comportements erronés si ces valeurs sont dépassées (souvent dans les tailles de tableau).
* Code inutilement compliqué, donc fragile dans des cas plus ou moins limites.


Quelques exemples de pénalités additionnelles:

* Vous faites une MR sur le dépôt public avec votre code privé : à partir de -10%
* Vous demandez à être membre du dépôt public : -5%
* Si vous critiquez à tort l'infrastructure de test : -10%
* Vous modifiez un fichier autre que le fichier source ou `local.bats` (ou en créez un) sans avoir l’autorisation : à partir de -10%
* Votre dépôt n'est pas un `fork` de celui-ci : -100%
* Votre dépôt n'est pas privé : -100%
* L'utilisateur `@abdenbi_m` n'est pas mainteneur : -100%
* Votre dépôt n'est pas hébergé sur le gitlab départemental : -100%
* Vous faites une remise par courriel : -100%
* Vous utilisez « mais chez-moi ça marche » (ou une variante) comme argument : -100%
* Si je trouve des morceaux de votre code sur le net (même si vous en êtes l'auteur) : -100%
