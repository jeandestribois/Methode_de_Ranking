#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/************VARIABLES GLOBALES*******************/

int quantite_memoire_allouee = 0;
int quantite_memoire_liberee = 0;

/************DEFINITION DES STRUCTURES************/

// Représente un élement dans une matrice.
struct element
{
	int dest;
	double proba;
};typedef struct element ELEMENT;

// Représente une ligne dans une matrice.
struct ligne
{
	int num;
	int degre;
	ELEMENT *elem;
};typedef struct ligne LIGNE;

// Représente une matrice.
struct matrice
{
	int nbr_elem_non_nul;
	int nbr_lignes;
	LIGNE *ligne;
};

/*************FONCTION DE LECTURE************/

struct matrice lecture(char const *nom_fichier, int stanford)
{
	struct matrice matrice;

	// Ouverture du fichier.
	FILE *fichier = fopen(nom_fichier, 	"r");
	if(fichier == NULL)
	{
		printf("Erreur : Le fichier renseigné est introuvable\n");
		exit(0);
	}

	// Lecture des deux premières lignes
	fscanf(fichier, "%d\n", &matrice.nbr_elem_non_nul);
	printf("%d\n", matrice.nbr_elem_non_nul);

	fscanf(fichier, "%d\n", &matrice.nbr_lignes);
	printf("%d\n", matrice.nbr_lignes);

	// Allocation mémoire.
	matrice.ligne = malloc(sizeof(LIGNE)*matrice.nbr_lignes);
	quantite_memoire_allouee += sizeof(matrice.ligne);

	// Lecture de chaques lignes (l'ordre et la numérotation diffèrent
	// en fonction de si stanford est à 1 ou pas).
	for(int i=0; i<matrice.nbr_lignes; i++)
	{
		// Lecture des deux premiers champs.
		fscanf(fichier, "%d %d ", &matrice.ligne[i].num, &matrice.ligne[i].degre);
		if(stanford) matrice.ligne[i].num--;
		printf("%d %d ", matrice.ligne[i].num, matrice.ligne[i].degre);

		// Allocation mémoire.
		matrice.ligne[i].elem = malloc(sizeof(ELEMENT)*matrice.ligne[i].degre);
		quantite_memoire_allouee += sizeof(matrice.ligne[i].elem);

		// Lecture de chaques éléments d'une ligne.
		for (int j=0; j<matrice.ligne[i].degre; j++)
		{
			if(stanford) {
				fscanf(fichier, "%d %lf ", &matrice.ligne[i].elem[j].dest, &matrice.ligne[i].elem[j].proba);
				matrice.ligne[i].elem[j].dest--;
			}
			else fscanf(fichier, "%lf %d ", &matrice.ligne[i].elem[j].proba, &matrice.ligne[i].elem[j].dest);
			printf("%lf %d ", matrice.ligne[i].elem[j].proba, matrice.ligne[i].elem[j].dest);
		}
		printf("\n");
	}

	return matrice;
}


/*************LIBERATION MEMOIRE**************/

void liberation_matrice(struct matrice matrice)
{
	for(int i=0; i<matrice.nbr_lignes; i++) {
		quantite_memoire_liberee += sizeof(matrice.ligne[i].elem);
		free(matrice.ligne[i].elem);
	}
	quantite_memoire_liberee += sizeof(matrice.ligne);
	free(matrice.ligne);
}


/*************CALCULS*************************/

void page_rank(struct matrice matrice)
{
	double eps = 0.000001;	// Variable fixant l'écart à atteindre de abs entre deux itération
	double abs = 1.0; 		// Variable stockant la valeur absolue de pin-pio
	double tmp;				// Variable temporaire permattant d'inverser abs (si négative)
	double alpha = 0.85;	// Variable stockant la probabilité de rester sur une page pour le surfeur aléatoire

	LIGNE pio, pin;

	// Allocation mémoire de pio et pin
	pio.num = 0, pio.degre = matrice.nbr_lignes;
	pio.elem = malloc(sizeof(ELEMENT)*pio.degre);
	quantite_memoire_allouee += sizeof(pio.elem);

	pin.num = 0, pin.degre = matrice.nbr_lignes;
	pin.elem = malloc(sizeof(ELEMENT)*pin.degre);
	quantite_memoire_allouee += sizeof(pin.elem);


	// Initialisation de pio
	for (int i = 0; i < pio.degre; ++i)
		pio.elem[i].proba = 1.0/pio.degre;

	// Boucle cherchant la convergeance
	while(eps < abs)
	{
		// Initialisation de pin
		for (int i = 0; i < pin.degre; ++i)
			pin.elem[i].proba = 0.0;

		// Calcul du pin
		for (int i = 0; i < matrice.nbr_lignes; ++i)
		{
			for(int j = 0; j < matrice.ligne[i].degre; j++)
			{
				// P[dest] += P[i->j] * P[i] 
				pin.elem[matrice.ligne[i].elem[j].dest].proba += matrice.ligne[i].elem[j].proba * pio.elem[i].proba;
			}
		}
		// On reparcours une fois pin pour rajouter le surfeur aleatoire.
		for(int i = 0; i<pin.degre; i++) {
			pin.elem[i].proba = (1-alpha)/pin.degre + alpha*pin.elem[i].proba;
		}

		// Calcul de la valeur absolue
		abs = 0.0;
		for(int i = 0; i < pin.degre; i++)
		{
			tmp = pin.elem[i].proba - pio.elem[i].proba;
			if(tmp<0)
				tmp = -tmp;

			abs += tmp;
			pio.elem[i].proba = pin.elem[i].proba;
		}
		printf("Abs = %lf\n", abs);
	}

	// Affichage de pin
	/*printf("pin = (");
	for(int i = 0; i<pin.degre; i++) {
		printf("%lf ", pin.elem[i].proba);
	}
	printf(")\n");*/

	// Libération mémoire de pio et pin
	quantite_memoire_liberee += sizeof(pio.elem);
	free(pio.elem);
	quantite_memoire_liberee += sizeof(pin.elem);
	free(pin.elem);
}

/*************PROGRAMME MAIN******************/
int main(int argc, char const *argv[])
{
	// Variable permettant de mesurer le temps de calcul total du programme.
	clock_t debut_t = clock();

	// Variable stockant 1 ou 0 en fonction de si la matric donnée
	// est du format d'une matrice de Stanford ou non.
	int stanford = 0;

	if(argc == 3) {
		if(!strcmp(argv[2], "-stanford"))
			stanford = 1;
		else {
			printf("Erreur : L'argument donnée n'est pas reconnu.\n");
			exit(0);
		}
	}
	else if(argc != 2) {
		printf("Erreur : Le nombre d'arguments données est incorrect.\n");
	}

	// On lit le fichier donnée et on le stock dans la structure matrice décrite en tête du fichier.
	struct matrice matrice = lecture(argv[1], stanford);

	// On applique 
	page_rank(matrice);

	// On libère la mémoire allouée pour la matrice.
	liberation_matrice(matrice);

	clock_t fin_t = clock();

	printf("\nTemps total de calcul en CPU ticks : %lu\n", fin_t - debut_t);
	printf("Temps total de calcul en ms : %lu\n", (fin_t - debut_t)*1000/CLOCKS_PER_SEC);
	printf("\nQuantité de memoire allouée manuellement en octets : %d\n", quantite_memoire_allouee);
	printf("Quantité de memoire libèrée manuellement en octets : %d\n", quantite_memoire_liberee);

	return 0;
}