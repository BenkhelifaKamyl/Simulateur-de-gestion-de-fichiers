#ifndef TABLEINDEX_H_INCLUDED
#define TABLEINDEX_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "TextColor.h"
#include "Metadonnees.h"
#include "Disk.h"
typedef struct{
    int id;
    int numBloc;
} Index; //Verifier si cette structure est correcte pour les 2 cas dense et non dense, faire si necessaire une autre structure


FILE *tablesIndex; //Fichier contenant tous les fichiers d'index
//Les parametres des fonctions n'ont pas encore été attribuées

//Mise a jour de l'etat des blocs apres chaque mise a jour
void TableAllocation();

//Creation de la table d'index
void creationTableIndexDense(fichier F, Index densetableIndex []);
void creationTableIndexNonDense(fichier F, Index tableIndex []);
//Sauvegarder la table d'index en memoire secondaire
void sauvegardeTableIndex(fichier *F, Index tableindex[]);
//Mettre le fichier d'index dans une table d'index en Memoire centrale
void chargementFichierIndexDense();
void chargementFichierIndexNonDense();
void rechercheEnregistrementDense();
void rechercheEnregistrementNonDense();
//Cas de mise à jour, suppression, insertion et changement d'adresse  (Peut etre separe en plusieurs fonctions mais est preferable de le faire en une seule)
void MajTableIndexDense();
void MajTableIndexNonDense();
void clearIndexTable(fichier *F); // Update the index table: clear the entries, as all blocks are free



#endif // TABLEINDEX_H_INCLUDED
