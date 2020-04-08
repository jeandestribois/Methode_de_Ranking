#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*********
CONSTANTES
*********/

#define fichier_resultat "resultats/resultats_v2.txt"

/*****************
VARIABLES GLOBALES
*****************/

int quantite_memoire_allouee = 0;
int quantite_memoire_liberee = 0;
int nbr_iterations_convergence = 0;

/************************
DEFINITION DES STRUCTURES
************************/

// Représente un élement dans une matrice.
struct element
{
	int dest;
	double proba;
};typedef struct element ELEMENT;

// Représente une ligne dans une matrice.
// On allouera autant d'élément qu'il y a de ligne
// car on ne peut pas dépasser ce nombre d'élément et ça nous
// permettra de ne pas à avoir besoin de faire des realloc à chaque itération.
struct ligne
{
	int num;
	int degre;
	ELEMENT *elem;
};typedef struct ligne LIGNE;

// Représente une matrice. Une matrice ne contient plus qu'une ligne car celle-ci sera
// réalloué à chaque itération.
struct matrice
{
	int nbr_elem_non_nul;
	int nbr_lignes;
	LIGNE ligne;
};


/*****************
LIBERATION MEMOIRE
*****************/

void liberation_matrice(struct matrice matrice) {
	quantite_memoire_liberee += sizeof(ELEMENT)*matrice.nbr_lignes;
	free(matrice.ligne.elem);
}


/*********************************
FONCTIONS DE LECTURE ET D'ECRITURE
*********************************/

FILE *premiere_lecture(struct matrice *matrice, char const *nom_fichier) {
	// Ouverture du fichier.
	FILE *fichier = fopen(nom_fichier, "r");
	if(fichier == NULL) {
		printf("Erreur : Le fichier renseigné est introuvable\n");
		exit(0);
	}

	// Lecture des deux premières lignes
	fscanf(fichier, "%d\n", &matrice->nbr_elem_non_nul);
	fscanf(fichier, "%d\n", &matrice->nbr_lignes);

	// Allocation mémoire.
	// On alloue qu'une fois un tableau d'élément de la taille du nombre de lignes
	// car c'est le nombre maximal d'éléments qui peuvent se trouvés dans une ligne.
	// Cela nous permet aussi de ne pas avoir besoin à chaque itération de réallouer la mémoire.
	matrice->ligne.elem = malloc(sizeof(ELEMENT)*matrice->nbr_lignes);
	quantite_memoire_allouee += sizeof(ELEMENT)*matrice->nbr_lignes;

	return fichier;
}

FILE *lecture_ligne(struct matrice *matrice, FILE *fichier, int stanford) {
	// Lecture d'une ligne (l'ordre et la numérotation diffèrent
	// en fonction de si stanford est à 1 ou pas).
	// Lecture des deux premiers champs :
	fscanf(fichier, "%d %d ", &matrice->ligne.num, &matrice->ligne.degre);
	if(stanford) matrice->ligne.num--;

	// Lecture de chaques éléments d'une ligne.
	for (int i=0; i<matrice->ligne.degre; i++) {
		if(stanford) {
			fscanf(fichier, "%d %lf ", &matrice->ligne.elem[i].dest, &matrice->ligne.elem[i].proba);
			matrice->ligne.elem[i].dest--;
		}
		else fscanf(fichier, "%lf %d ", &matrice->ligne.elem[i].proba, &matrice->ligne.elem[i].dest);
	}

	// Si on se trouve à la fin du fichier, on remet le curseur du fichier deux lignes apres le début.
	if(matrice->nbr_lignes == matrice->ligne.num+1) {
		rewind(fichier);
		fscanf(fichier, "%d\n%d\n", &matrice->nbr_elem_non_nul, &matrice->nbr_lignes);
	}

	return fichier;
}

void ecriture_resultat(LIGNE pin) {
	FILE *fichier = fopen(fichier_resultat, "w+");
	if(fichier == NULL) {
		printf("Erreur : Il est impossible d'ecrire dans le fichier %s\n", fichier_resultat);
	}
	else {
		fprintf(fichier, "(%lf", pin.elem[0].proba);
		for(int i = 1; i<pin.degre; i++) {
			fprintf(fichier, ", %lf", pin.elem[i].proba);
		}
		fprintf(fichier, ")\n");
		fclose(fichier);
	}
}


/*****************
CALCUL DU PAGERANK
*****************/

LIGNE pagerank(struct matrice matrice, FILE *fichier, int stanford) {
	double eps = 0.000001;	// Variable fixant l'écart à atteindre de abs entre deux itération
	double abs = 1.0; 		// Variable stockant la valeur absolue de pin-pio
	double tmp;				// Variable temporaire permattant d'inverser abs (si négative)
	double alpha = 0.85;	// Variable stockant la probabilité de rester sur une page pour le surfeur aléatoire

	LIGNE pio, pin;

	// Allocation mémoire de pio et pin
	pio.num = 0, pio.degre = matrice.nbr_lignes;
	pio.elem = malloc(sizeof(ELEMENT)*pio.degre);
	quantite_memoire_allouee += sizeof(ELEMENT)*pio.degre;

	pin.num = 0, pin.degre = matrice.nbr_lignes;
	pin.elem = malloc(sizeof(ELEMENT)*pin.degre);
	quantite_memoire_allouee += sizeof(ELEMENT)*pio.degre;

	// Initialisation de pio
	for (int i = 0; i < pio.degre; ++i)
		pio.elem[i].proba = 1.0/pio.degre;

	// Boucle cherchant la convergeance
	printf("\nCalcul du page rank :\n");
	while(eps < abs) {
		nbr_iterations_convergence++;
		// Initialisation de pin
		for (int i = 0; i < pin.degre; ++i)
			pin.elem[i].proba = 0.0;

		// Calcul du pin
		for (int i = 0; i < matrice.nbr_lignes; ++i) {
			// Lecture de la prochaine ligne
			fichier = lecture_ligne(&matrice, fichier, stanford);
			for(int j = 0; j < matrice.ligne.degre; j++) {
				// P[dest] += P[i->j] * P[i] 
				pin.elem[matrice.ligne.elem[j].dest].proba += pio.elem[i].proba * matrice.ligne.elem[j].proba;
			}
		}
		// On reparcours une fois pin pour rajouter le surfeur aleatoire.
		for(int i = 0; i<pin.degre; i++) {
			pin.elem[i].proba = (1-alpha)/pin.degre + alpha*pin.elem[i].proba;
		}

		// Calcul de la valeur absolue et on effectue pio = pin
		abs = 0.0;
		for(int i = 0; i < pin.degre; i++) {
			tmp = pin.elem[i].proba - pio.elem[i].proba;
			if(tmp<0)
				tmp = -tmp;

			abs += tmp;
			pio.elem[i].proba = pin.elem[i].proba;
		}
		printf("Difference entre 2 itérations : %lf\n", abs);
	}

	// Libération mémoire de pio et pin
	quantite_memoire_liberee += sizeof(ELEMENT)*pio.degre;
	free(pio.elem);

	return pin;
}

/*************
PROGRAMME MAIN
*************/
int main(int argc, char const *argv[])
{
	// Variable stockant 1 ou 0 en fonction de si la matric donnée
	// est du format d'une matrice de Stanford ou non.
	int stanford = 0;
	// Ces variables permetteront de calculer le temps de calcul.
	clock_t debut_calcul_t, fin_calcul_t;
	// Cette variable va stocker une ligne de la matrice du graphe du web donnée.
	struct matrice matrice;
	FILE *fichier = NULL;

	// Verification de l'entree donnée
	if(argc == 3) {
		if(!strcmp(argv[2], "--stanford"))
			stanford = 1;
		else {
			printf("Erreur : L'argument donnée n'est pas reconnu.\n");
			exit(0);
		}
	}
	else if(argc != 2) {
		printf("Erreur : Le nombre d'arguments données est incorrect.\n");
		exit(0);
	}

	// Première lecture
	fichier = premiere_lecture(&matrice, argv[1]);

	// Calcul du pagerank
	debut_calcul_t = clock();
	LIGNE pin = pagerank(matrice, fichier, stanford);
	fin_calcul_t = clock();

	// Ecriture de pin dans le fichier dont le nom est definit en constante et liberation memoire de pin.
	ecriture_resultat(pin);
	quantite_memoire_liberee += sizeof(ELEMENT)*matrice.nbr_lignes;
	free(pin.elem);

	// On libère la mémoire allouée pour la matrice.
	liberation_matrice(matrice);
	fclose(fichier);

	printf("\n*******************************************\n");
	printf("\nNombre d'iterations pour la convergeance : %d\n", nbr_iterations_convergence);
	printf("\nTemps de calcul du pagerank en CPU ticks : %lu\n", fin_calcul_t - debut_calcul_t);
	printf("Temps de calcul du pagerank en ms : %lu\n", (fin_calcul_t - debut_calcul_t)*1000/CLOCKS_PER_SEC);
	printf("\nQuantité de memoire allouée dynamiquement en octets : %d\n", quantite_memoire_allouee);
	printf("Quantité de memoire libèrée dynamiquement en octets : %d\n", quantite_memoire_liberee);

	return 0;
}