#ifndef DISK_H_INCLUDED
#define DISK_H_INCLUDED
#define MAX_FILES 10        // Nombre maximum de fichiers gérés
#define BLOCK_SIZE 5      // Taille de chaque bloc (Facteur de blocage)
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

bool isDiskContigu();
//Affiche le disque
void AfficherDisqueContigue();
void AfficherDisqueChainee();
//Initialisation
void initializeBlockChainee(int i);
void initializeBlockContigue(int i);
void initializeDiskChainee();
void initializeDiskContigue();
//Compactage
void compactDiskChainee();
void compactDiskContigue();
//Suppression disque
void clearDiskchainee();
void clearDiskContigue();
//Verfication retourne Faux si libre ou n'existe pas, vrai si rempli
bool checkBlock(int blockID); // cas chaine
bool checkBlockContigue(int blockId); //cas contigu
//Remplissage
void fillFileChainee(int fileID, bool isSorted, fichier *F);
void fillFileContigue(int fileID, bool isSorted, fichier *F);
//Chargement
void ChargerFichierChainee(int fileID, fichier *F);
bool ChargerFichierContigue(int fileID, fichier *F);
//Remplissage Enregistrement
Enregistrement donneesEnregistrement();
//Insertion
void insertRecord(fichier *F, Enregistrement record);
void insertRecordchainee(fichier *F, Enregistrement record, bool isSorted);
void insertRecordcontigue(fichier *F, Enregistrement record, bool isSorted);
//Suppression
void deleteRecordLogical(int fileID, int recordID);
void deleteRecordPhysical(int fileID, int recordID);
void SuppressionLogique(fichier *F, int recordId);
void SuppressionPhysique(fichier *F, int recordId);
//Recherche enregistrement
int rechercheEnregistrement(fichier F, int recordId);
//Defragmentation
void Defragmentation(fichier *F);
void DefragmentationContigue(fichier *F);
void Defragmentationchainee(fichier *F);
//Supprimer fichier
void deleteFile(fichier *F);
void deleteFilechainee(fichier *F);
void deleteFileContigue(fichier *F);
//Lire et ecrire bloc
void LireBloc(fichier *F, int i, Bloc *Buffer);
void EcrireBloc(fichier *F, int i, Bloc Buffer);

#endif // DISK_H_INCLUDED
