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
void AfficherDisque();
void initializeBlockChainee(int i);
void initializeBlockContigue(int i);
void initializeDiskChainee();
void initializeDiskContigue();
void compactDisk(fichier *F);
void clearDisk(fichier *F);
bool checkBlock(int blockID);
void fillFile(int fileID, bool isSorted, fichier *F);
void ChargerFichier(int fileID, fichier *F);
void insertRecord(int fileID, Enregistrement record, bool isSorted);
void deleteRecordLogical(int fileID, int recordID);
void deleteRecordPhysical(int fileID, int recordID);
void SuppressionLogique(fichier *F, int recordId);
void SuppressionPhysique(fichier *F, int recordId);
int rechercheEnregistrement(fichier *F, int recordId);
void Defragmentation(fichier *F);
void deleteFile(int fileID, fichier *F);


#endif // DISK_H_INCLUDED
