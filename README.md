# Projet Méthode de Ranking

Projet du module de Méthode de Ranking du M1 Informatique de Paris Saclay.

## Concepteurs

* Sofiane Atmani
* Nassime Kabache
* Jean Destribois

## Descrition

Le sujet imposé est le sujet 5 du fichier se trouvant dans le dossier ressource.
Il s'agit du calcul du pagerank par la méthode de Gauss Seidel descendant.
Trois fichiers sources écrits en langage C (chacun construisant un executable) composent se projet :
* Une premiere version implémentant l'algorithme de pagerank avec un stockage en ligne. On stock pour un élément, ses liens vers les autres éléments.
* Une deuxième version implémentant l'algorithme de pagerank en ne stockant progressivement qu'une ligne de la matrice représentant le graphe du web. Celle-ci est beaucoup plus lente à calculer le pagerank mais est bien moins coûteuse en mémoire que le précédent.
* Une troisième version implémentant l'algorithme de pagerank avec un stockage en colonne. On a donc  structure de donnée pour stocker la matrice différente des deux versions précédentes : on stock maintenant pour un élément, les éléments qui ont un lien vers celui-ci.
* Et une quatrième version implémentant l'algorithme de pagerank en utilisant la méthode de Gauss Seifel descendant. Cette version utilise un stockage en colonne de la matrice (comme la troisème version).
Chaque version affiche à la fin de l'execution, le nombre d'itération nécéssaire pour la convergence, la quantité de mémoire allouée (et libérée) dynamiquement, le temps de calcul et le temps de lecture (sauf pour la version 2 lisant ligne par ligne la matrice).

## Installation

Aucune installation n'est requise.
Cependant il est nécessaire de posèder gcc pour compiler le programme.

## Utilisation

Un fichier Makefile est présent dans le projet permettant d'automatiser l'utilisation du programme : 
* Pour la compilation entrez : "make compile".
* Pour l'execution entrez par exemple : "make run_v3_grand" pour executer la version 3 avec un grand graphe du web (il existe 3 tailles de matrice : petit, moyen, grand).
* Si vous souhaitez executer par vous même une version en donnant un graphe du web que vous souhaitez, entrez par exemple : "./bin/algo_PR_v3 le_fichier_souhaité". Il est néssaire de rajouter l'option "--stanford" à la fin de la commande si votre fichier a un format du type du graphe de Stanford. En effet cela dépend de l'ordre et la numérotation donné dans le fichier.
Si toute l'execution se passe bien, un fichier sera généré dans le dossier resultats contenant le résultat du ranking de la matrice donnée pour l'execution.

