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
} Index; //Structure de l'index


Index tablesIndex[MAX_FILES][MAX_INDEX_ENTRIES]; //"MS" contenant les tables d'index

//Creation de la table d'index
void initializeIndexFiles();
void creationTableIndexDenseContigue(fichier F, Index densetableIndex [100]);
void creationTableIndexDenseChainee(fichier F, Index densetableIndex [100]);
void creationTableIndexNonDenseContigue (fichier F, Index tableIndex [100]);
void creationTableIndexNonDenseChainee(fichier F, Index tableIndex[100]);
int getIndexSize(Index tableIndex[100]);
//Sauvegarder la table d'index en memoire secondaire
void sauvegardeTableIndex(fichier *F, Index tableindex[100]);
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
