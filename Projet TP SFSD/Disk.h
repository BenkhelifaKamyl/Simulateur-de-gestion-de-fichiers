#ifndef DISK_H_INCLUDED
#define DISK_H_INCLUDED
#define MODE_CHAINE 1
#define MODE_CONTIGUE 2
#define BLOCK_SIZE 5      // Taille de chaque bloc (Facteur de blocage)
#define MAX_BLOCKS 100 //Nombre maximal de blocs dans le disque


int currentMode;

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
void AfficherBlocContigue(int i);
void AfficherBlocChainee(int i);
void AfficherFichierContigue(fichier F);
void AfficherFichierChainee(fichier F);
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
void fillFileChainee( bool isSorted, fichier *F);
void fillFileContigue( bool isSorted, fichier *F);
//Chargement
void ChargerFichierChainee(fichier *F);
void ChargerFichierContigue(fichier *F);
//Remplissage Enregistrement
Enregistrement donneesEnregistrement();
//Insertion
void insertRecord(fichier *F, Enregistrement record);
void insertRecordChainee(fichier *F, Enregistrement record, bool isSorted);
void insertRecordContigue(fichier *F, Enregistrement record, bool isSorted);
//Suppression
void deleteRecordPhysicalchaine(fichier *F, int recordID);
void deleteRecordLogicalcontigue(fichier *F, int recordID);
void deleteRecordLogicalchainee(fichier *F, int recordID);
void deleteRecordPhysicalContiguous(fichier *F, int recordID);
//Defragmentation
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
