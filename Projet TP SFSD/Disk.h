#ifndef DISK_H_INCLUDED
#define DISK_H_INCLUDED
#define MAX_BLOCKS 100      // Nombre maximum de blocs dans le disque
#define MAX_FILES 10        // Nombre maximum de fichiers gérés
#define BLOCK_SIZE 128      // Taille de chaque bloc en octets

typedef struct{
    int ID;
    char Data[100];
    bool Supprime; //Pour la potentielle suppression logique
} Enregistrement;

typedef struct{
    Enregistrement enregistrement[BLOCK_SIZE];
    bool free; //Pour savoir si le bloc est libre ou occupe
}BlocContigue;

typedef struct{
    Enregistrement enregistrement[BLOCK_SIZE];
    bool free; //Pour savoir si le bloc est libre ou occupe
    int next; //Pour pointer vers le prochain bloc
}BlocChainee;

typedef union { //permet de regrouper les deux types de blocs dans une seule structure
    BlocContigue contigue;
    BlocChainee chainee;
} Bloc;

void initializeDiskChainee();
void initializeDiskContigue();
void OuvrirFichier(FILE *F, char nomFichier[30], char mode);


#endif // DISK_H_INCLUDED
