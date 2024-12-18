#ifndef DISK_H_INCLUDED
#define DISK_H_INCLUDED
#define MAX_FILES 10        // Nombre maximum de fichiers gérés
#define BLOCK_SIZE 128      // Taille de chaque bloc en octets
#define MAX_BLOCKS 100 //Nombre maximal de blocs dans le disque
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

Bloc disk[MAX_BLOCKS];    // Disque virtuel avec des blocs

void initializeDiskChainee();
void initializeDiskContigue();
void compactDisk();
void clearDisk();
bool checkBlock(int blockID);
void fillFile(int fileID, bool isSorted, fichier *F);
void loadFile(int fileID);
void insertRecord(int fileID, Enregistrement record, bool isSorted);
void deleteRecordLogical(int fileID, int recordID);
void deleteRecordPhysical(int fileID, int recordID);
void defragmentFile(int fileID);
void deleteFile(int fileID);


#endif // DISK_H_INCLUDED
