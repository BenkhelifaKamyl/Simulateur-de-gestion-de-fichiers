#ifndef TABLEINDEX_H_INCLUDED
#define TABLEINDEX_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "TextColor.h"
#include "Metadonnees.h"
#include "Disk.h"
#define MAX_INDEX_ENTRIES 100
typedef struct{
    int id;
    int numBloc;
} Index; //Verifier si cette structure est correcte pour les 2 cas dense et non dense, faire si necessaire une autre structure


 //Fichier contenant tous les fichiers d'index
//Les parametres des fonctions n'ont pas encore été attribuées
Index tablesIndex[MAX_FILES][MAX_INDEX_ENTRIES];

//Creation de la table d'index
void initializeIndexFiles();
void creationTableIndexDenseContigue(fichier F, Index densetableIndex []);
void creationTableIndexDenseChainee(fichier F, Index densetableIndex []);
void creationTableIndexNonDenseContigue (fichier F, Index tableIndex []);
void creationTableIndexNonDenseChainee(fichier F, Index tableIndex[]);
int getIndexSize(Index tableIndex[]);
//Sauvegarder la table d'index en memoire secondaire
void sauvegardeTableIndex(fichier *F, Index tableindex[]);
void rechercheTableIndex(fichier *F, int *i); //Recherche une table d'index en MS
//Mettre le fichier d'index dans une table d'index en Memoire centrale
void chargementFichierIndexDense(fichier *F, Index tableIndex[100]);
void chargementFichierIndexNonDense(fichier *F, Index tableIndexNonDense[100]);
void rechercheEnregistrementDense(fichier *F, int ID, int *numBloc, int *deplacement);
void rechercheEnregistrementNonDense(fichier *F, int ID, int *numBloc, int *deplacement);
//Cas de mise à jour, suppression, insertion et changement d'adresse  (Peut etre separe en plusieurs fonctions mais est preferable de le faire en une seule)
void MajTableIndexDense(fichier *F, int mode,int ID, int numbloc); //mode 1: insertion, mode 2: suppression
void MajTableIndexNonDense(fichier *F, int mode,int ID, int numbloc); //Ne pas oublier de charger le fichier en MS

void removeIndexTable(fichier *F);

#endif // TABLEINDEX_H_INCLUDED
