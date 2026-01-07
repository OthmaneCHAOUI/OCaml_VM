# Machine Virtuelle d'OCaml

# Contexte

Le but de ce projet est d'implémenter (une partie de) la machine virtuelle pour le bytecode d'OCaml.

Le langage de programmation OCaml, est compilé vers du bytecode, un langage bas niveau qui est ensuite interprété dans une machine virtuelle pour exécuter le programme. L'avantage principal est qu'il suffit d'avoir une machine virtuelle pour exécuter le code, quelque soit l'architecture sur laquelle il a été compilé.

# Description de la machine virtuelle

On considère deux types de données :
* des codes, qui représentent les instructions du bytecode, et qui sont implémentés sur 32 bits ; on supposera qu'on peut utiliser le type int;
* des valeurs, qui représentent des données (qui seront potentiellement des adresses en mémoire), et qui sont implémentés sur 64 bits ; on supposera qu'on peut utiliser le type long int.

Pour représenter des entiers comme des valeurs et pouvoir les distinguer, ils seront encodés par des entiers impairs : l'entier n sera représenté par la valeur 2n+1. De même, les booléens true et false seront représentés comme les entiers 1 et 0, et ils seront donc encodés respectivement par 3 et 1.

La machine virtuelle OCaml est constituée des parties suivantes :

* Un tableau de codes, qui contient les instructions du programme (mais aussi des données).
* Un indice qui indique dans quelle case du tableau de code on se trouve. Initialement, cet indice vaudra 0.
* Un accumulateur qui contient une valeur. Initialement, cette valeur vaudra 1.
* Une pile de valeurs, qui sera initialement vide. On sera amené à consulter les valeurs située à une profondeur donnée quelconque dans la pile. L'élément au sommet sera considéré à la profondeur 0, celui en dessous à la profondeur 1, etc.
* Un tableau de valeurs, qui contiendra des valeurs globales du programme.

Pour exécuter le programme, on regarde le code situé à l'indice courant. On effectue l'action associée à ce code (cf. infra), qui peut lire ou modifier l'indice, l'accumulateur, la pile et/ou les valeurs globales. Puis on passe à l'indice suivant (sauf indication contraire).

On peut regrouper les instructions en plusieurs ensembles. On n'implémentera qu'un sous-ensemble des instructions de la machine virtuelle, uniquement celles présentées ici. On pourra également se référer à [ce document][caml_instructions] qui date un peu mais reste pertinent pour décrire les instructions.

# Approche et méthodologie

**processus de compilation d'OCaml** comprendre les etapes de compilation, et laquelle on est sense de faire (machine virtuelle) + comprendre les entrees et les sorties

**structure des fichiers**: isoler le main, les entetes, les implementations vient d'ameliorer la qualiter de code, et diminue la complexite de comprendre et detecter les differents fonctionalites du projet.

**verifier structure des fichiers .sobf**: deux possibilites ont etait posees. La premier c'est de faire une fonction qui lit le fichier .sobf tout entiere et retourn les donnees du fichier comme une chaine de caractere et apres verifier si le fichier respecte la structure en accedant à la chaine apartir des indices. La deuxieme c'est de faire une fonction pour lire le fichier et verifier la structure au fur et a mesure en precisons le nombre des octects a lire. La conclusion etait de suivre la deuxieme methode car sera moins couteuse à cause que la premier emethode necessite la conversion des valeurs apartir la chaine de caractere...

**sauvegarder les donnees du fichier .sobf**: j'avais penser en deux possibilites. Soit, pour chaque donnees des fichiers de test .sobf je fais une fonctions qui lit et retourn cette donnee. Soit je fait une structure de donnee et je cree une seul fonction qui lit et sauvegarde les donnees du fichier dans les elements de la structure. J'avais me concacre sur la deuxieme car je lit le fichier une seul fois, au contraire de la methode 1 qui dois lit le fichier pour chaque donnee.

**`rb` vs `r`** rb pour binaire

**fonction is_sobf_file** additionel

**lire les tailles apartir les fichiers**

# Outils utilisees

* SublimeText
* GCC
* Git
* Debian dans WSL
* Debian dans VirtualBox

# Resources

* **manpages** pour consulter les manuels des fonctions
* **stackoverflow** pour chercher des solutions des problèmes trouvées





[caml_instructions]: ./documentation/caml-instructions.pdf
