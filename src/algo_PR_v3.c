#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
NORMAL :
for i de 0 à nbrLigne
	for j  de 0 à degreEntrant de i
		pin[i] += pio[j]*G[j, i]

NORMAL + GAUSS-SIEDEL :
for i de nbreLigne à 0
	for j de 0 à i
		pin[i] += pio[j]*G[j,i]
	for j de i à nbreLigne
		pin[i] += pin[j]*G[j,i]

MA VERSION :
for i de 0 à nbrLigne
	for j de 0 à degreSortant
		pin[ref[j] = pio[i] * G[i,ref[j]]

MA VERSION + GAUSS+SIEDEL : impossible.
*/

/*********
CONSTANTES
*********/

#define fichier_resultat "resultats/resultats_v3.txt"


/*****************
VARIABLES GLOBALES
*****************/

int quantite_memoire_allouee = 0;
int quantite_memoire_liberee = 0;
int nbr_iteration_convergence = 0;


/************************
DEFINITION DES STRUCTURES
************************/

// Liste chaîné représentant un élement dans une matrice et contenant l'adresse du prochain élément.
struct element {
	int prov;		// Contient la provenance du lien sur l'élément.
	double proba;	// Avec qu'elle proba le lien est donné.
	struct element *suiv;
};typedef struct element ELEMENT;

// Représente une ligne dans une matrice.
// Celle ci contiendra tous les éléments ayant un lien vers l'élément représenté par la ligne.
struct ligne {
	int num;
	ELEMENT *elem;
};typedef struct ligne LIGNE;

// Représente une matrice.
struct matrice {
	int nbr_elem_non_nul;
	int nbr_lignes;
	LIGNE *ligne;			// Tableau de ligne de taille nbr_lignes.
};


/****************************
OPERATIONS SUR LISTE CHAINEES
****************************/

// Initialisation de la liste composant chaque ligne à NULL
struct matrice init_matrice(struct matrice matrice) {
	for(int i = 0; i<matrice.nbr_lignes; i++)
		matrice.ligne[i].elem = NULL;
	return matrice;
}


// Cette fonction rajoute un nouvel élément au début de la liste.
ELEMENT *nouvel_element(ELEMENT *elem, int prov, double proba) {
	ELEMENT *nouvel_elem = malloc(sizeof(ELEMENT));
	quantite_memoire_allouee += sizeof(ELEMENT);
	nouvel_elem->prov = prov;
	nouvel_elem->proba = proba;
	nouvel_elem ->suiv = elem;
	return nouvel_elem;
}


/*****************
LIBERATION MEMOIRE
*****************/

ELEMENT *liberation_liste(ELEMENT *elem) {
	if(elem != NULL) {
		elem->suiv = liberation_liste(elem->suiv);
		free(elem);
		quantite_memoire_liberee += sizeof(ELEMENT);
	}
	return NULL;
}

void liberation_matrice(struct matrice matrice) {
	for(int i=0; i<matrice.nbr_lignes; i++) {
		matrice.ligne[i].elem = liberation_liste(matrice.ligne[i].elem);
	}
	quantite_memoire_liberee += sizeof(LIGNE)*matrice.nbr_lignes;
	free(matrice.ligne);
}


/********************************
FONCTIONS DE LECTURE ET D'ECRITURE
********************************/

struct matrice lecture(char const *nom_fichier, int stanford) {
	struct matrice matrice;

	int degre = 0;			// Contiendra le degre d'une ligne pour la parcourir.
	int dest = 0;			// Contiendra la destination d'un lien lors de la lecture.
	double proba = 0.0;		// Contiendra la probabilité d'un lien lors de la lecture.

	// Ouverture du fichier.
	FILE *fichier = fopen(nom_fichier, 	"r");
	if(fichier == NULL)
	{
		printf("Erreur : Le fichier renseigné est introuvable\n");
		exit(0);
	}
	printf("\nLecture du fichier...\n");

	// Lecture des deux premières lignes
	fscanf(fichier, "%d\n", &matrice.nbr_elem_non_nul);
	fscanf(fichier, "%d\n", &matrice.nbr_lignes);

	// Allocation mémoire.
	matrice.ligne = malloc(sizeof(LIGNE)*matrice.nbr_lignes);
	quantite_memoire_allouee += sizeof(LIGNE)*matrice.nbr_lignes;

	matrice = init_matrice(matrice);

	// Lecture de chaques lignes (l'ordre et la numérotation diffèrent
	// en fonction de si stanford est à 1 ou pas).
	for(int i=0; i<matrice.nbr_lignes; i++)
	{
		// Lecture des deux premiers champs.
		fscanf(fichier, "%d %d ", &matrice.ligne[i].num, &degre);
		if(stanford) matrice.ligne[i].num--;

		// Lecture de chaques éléments d'une ligne.
		for (int j=0; j<degre; j++)
		{
			// On crée un élément à la ligne destination.
			if(stanford) {
				fscanf(fichier, "%d %lf ", &dest, &proba);
				matrice.ligne[dest-1].elem = nouvel_element(matrice.ligne[dest-1].elem, i, proba);
			}
			else {
				fscanf(fichier, "%lf %d ", &proba, &dest);
				matrice.ligne[dest].elem = nouvel_element(matrice.ligne[dest].elem, i, proba);
			}
		}
		printf("\r");
		printf("%d %%", i*100/matrice.nbr_lignes+1);
	}
	printf("\n\n");

	fclose(fichier);
	return matrice;
}


void ecriture_resultat(double *pin, int taille) {
	FILE *fichier = fopen(fichier_resultat, "w+");
	if(fichier == NULL) {
		printf("Erreur : Il est impossible d'ecrire dans le fichier %s\n", fichier_resultat);
	}
	else {
		fprintf(fichier, "(%lf", pin[0]);
		for(int i = 1; i<taille; i++) {
			fprintf(fichier, ", %lf", pin[i]);
		}
		fprintf(fichier, ")\n");
		fclose(fichier);
	}
}


/*****************
CALCUL DU PAGERANK
*****************/

double *pagerank(struct matrice matrice)
{
	double eps = 0.000001;	// Variable fixant l'écart à atteindre de abs entre deux itération
	double abs = 1.0; 		// Variable stockant la valeur absolue de pin-pio
	double tmp;				// Variable temporaire permattant d'inverser abs (si négative)
	double alpha = 0.85;	// Variable stockant la probabilité de rester sur une page pour le surfeur aléatoire
	double *pio, *pin;		// Pin stockera le ranking d'une itération et pio celui de l'itération précédente.
	ELEMENT *elem_parcours;	// Cette variable nous permettera de parcourir la liste d'élément pour chaque ligne.

	// Allocation mémoire de pio et pin
	pio = malloc(sizeof(double)*matrice.nbr_lignes);
	quantite_memoire_allouee += sizeof(double)*matrice.nbr_lignes;

	pin = malloc(sizeof(double)*matrice.nbr_lignes);
	quantite_memoire_allouee += sizeof(double)*matrice.nbr_lignes;

	// Initialisation de pio
	for (int i = 0; i < matrice.nbr_lignes; ++i)
		pio[i] = 1.0/matrice.nbr_lignes;

	// Boucle cherchant la convergeance
	printf("Calcul du pagerank :\n\n");
	while(eps < abs) {
		nbr_iteration_convergence++;

		// Initialisation de abs
		abs = 0.0;

		// Calcul du pin
		for (int i = 0; i < matrice.nbr_lignes; ++i) {
			// Initialisation de pin.
			pin[i] = 0.0;

			elem_parcours = matrice.ligne[i].elem;
			while(elem_parcours != NULL) {
				pin[i] += pio[elem_parcours->prov]*elem_parcours->proba;
				elem_parcours = elem_parcours->suiv;
			}
			// Ajout du surfeur aléatoire.
			pin[i] = (1-alpha)/matrice.nbr_lignes + alpha*pin[i];

			// Calcul de la valeur absolue
			tmp = pin[i] - pio[i];
			if(tmp<0) tmp = -tmp;
			abs += tmp;
		}
		// On donne la valeur de pin à pio.
		for (int i = 0; i < matrice.nbr_lignes; ++i) {
			pio[i] = pin[i];
		}
		printf("Difference entre 2 itérations : %lf\n", abs);
	}

	// Libération mémoire de pio et pin
	quantite_memoire_liberee += sizeof(double)*matrice.nbr_lignes;
	free(pio);

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
	// Ces variables permetteront de calculer le temps de calcul et de lecture.
	clock_t debut_lecture_t, fin_lecture_t, debut_calcul_t, fin_calcul_t;
	// Cette variable va stocker la matrice du graphe du web donnée.
	struct matrice matrice;


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

	// Lecture
	debut_lecture_t = clock();
	matrice = lecture(argv[1], stanford);
	fin_lecture_t = clock();

	// Calcul du pagerank
	debut_calcul_t = clock();
	double *pin = pagerank(matrice);
	fin_calcul_t = clock();

	// Ecriture de pin dans le fichier dont le nom est definit en constante et liberation memoire de pin.
	ecriture_resultat(pin, matrice.nbr_lignes);
	quantite_memoire_liberee += sizeof(double)*matrice.nbr_lignes;
	free(pin);

	// On libère la mémoire allouée pour la matrice.
	liberation_matrice(matrice);

	printf("\n*******************************************\n");
	printf("\nNombre d'iterations pour la convergeance : %d\n", nbr_iteration_convergence);
	printf("\nTemps de lecture du fichier en CPU ticks : %lu\n", fin_lecture_t - debut_lecture_t);
	printf("Temps de lecture du fichier en ms : %lu\n", (fin_lecture_t - debut_lecture_t)*1000/CLOCKS_PER_SEC);
	printf("Temps de calcul du pagerank en CPU ticks : %lu\n", fin_calcul_t - debut_calcul_t);
	printf("Temps de calcul du pagerank en ms : %lu\n", (fin_calcul_t - debut_calcul_t)*1000/CLOCKS_PER_SEC);
	printf("\nTemps total de calcul du pagerank en CPU ticks : %lu\n", fin_calcul_t - debut_lecture_t);
	printf("Temps total de calcul du pagerank en ms : %lu\n", (fin_calcul_t - debut_lecture_t)*1000/CLOCKS_PER_SEC);
	printf("\nQuantité de memoire allouée dynamiquement en octets : %d\n", quantite_memoire_allouee);
	printf("Quantité de memoire libèrée dynamiquement en octets : %d\n", quantite_memoire_liberee);

	return 0;
}