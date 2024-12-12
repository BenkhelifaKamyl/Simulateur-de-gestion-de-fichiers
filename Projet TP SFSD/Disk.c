#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "TextColor.h"
#include "Metadonnees.h"
#include "TableIndex.h"
#include "Disk.h"

void initializeDiskChainee() {
    for (int i = 0; i < MAX_BLOCKS; i++) {
        disk[i].chainee.free = true;           // Marque tous les blocs comme libres
        disk[i].chainee.next = -1;       // Pas de bloc suivant initialement
        memset(disk[i].chainee.enregistrement, 0, BLOCK_SIZE); // Vide les données du bloc
    }
    printf("Disk initialized with %d blocks.\n", MAX_BLOCKS);
}
void initializeDiskContigue() {
    for (int i = 0; i < MAX_BLOCKS; i++) {
        disk[i].contigue.free = true;           // Marque tous les blocs comme libres
        memset(disk[i].contigue.enregistrement, 0, BLOCK_SIZE); // Vide les données du bloc
    }
    printf("Disk initialized with %d blocks.\n", MAX_BLOCKS);
}
////1 Définir les Structures et les Fonctions de Base............................................................
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_BLOCKS 100
#define BLOCK_SIZE 128

// Structure d'un enregistrement
typedef struct {
    int ID;
    char Data[100];
    bool Supprime;  //  suppression logique
} Enregistrement;

// bloc (organisation contiguë)
typedef struct {
    Enregistrement enregistrement[BLOCK_SIZE];
    bool free;
} BlocContigue;

// Structure d'un bloc chaîné (organisation chaînée)
typedef struct {
    Enregistrement enregistrement[BLOCK_SIZE];
    bool free;
    int next;
} BlocChainee;

//  stocker un bloc soit contigu, soit chaîné
typedef union {
    BlocContigue contigue;
    BlocChainee chainee;
} Bloc;

// Le disque qui contient les blocs
Bloc disk[MAX_BLOCKS];

// Fonctions de gestion du disque
void initializeDiskChainee();
void initializeDiskContigue();
void compactDisk();
void clearDisk();
bool checkBlock(int blockID);
void fillFile(int fileID, bool isSorted);
void loadFile(int fileID);
void insertRecord(int fileID, Enregistrement record, bool isSorted);
void deleteRecordLogical(int fileID, int recordID);
void deleteRecordPhysical(int fileID, int recordID);
void defragmentFile(int fileID);
void deleteFile(int fileID);
//2 Initialisation du Disque
// Initialisation pour l'organisation chaînée
void initializeDiskChainee() {
    for (int i = 0; i < MAX_BLOCKS; i++) {
        disk[i].chainee.free = true;           // Marque tous les blocs comme libres
        disk[i].chainee.next = -1;             // Pas de bloc suivant initialement
        memset(disk[i].chainee.enregistrement, 0, BLOCK_SIZE); // Vide les données du bloc
    }
    printf("Disk initialized with %d blocks in chained mode.\n", MAX_BLOCKS);
}

// Initialisation pour l'organisation contiguë
void initializeDiskContigue() {
    for (int i = 0; i < MAX_BLOCKS; i++) {
        disk[i].contigue.free = true;         // Marque tous les blocs comme libres
        memset(disk[i].contigue.enregistrement, 0, BLOCK_SIZE); // Vide les données du bloc
    }
    printf("Disk initialized with %d blocks in contiguous mode.\n", MAX_BLOCKS);
}
//3 Compactage du Disque
void compactDisk() {
    int lastFreeBlock = -1;
    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (disk[i].chainee.free) {
            lastFreeBlock = i;
        } else if (lastFreeBlock != -1) {
            // Déplace le bloc non libre pour combler l'espace
            disk[lastFreeBlock] = disk[i];
            disk[i].chainee.free = true;
            disk[lastFreeBlock].chainee.next = -1;
        }
    }
    printf("Disk compacted.\n");
}
//4 Vider la Mémoire Secondaire
void clearDisk() {
    for (int i = 0; i < MAX_BLOCKS; i++) {
        disk[i].chainee.free = true;
        disk[i].chainee.next = -1;
        memset(disk[i].chainee.enregistrement, 0, BLOCK_SIZE);
    }
    printf("Disk cleared.\n");
}
//5Vérification d'un Bloc
bool checkBlock(int blockID) {
    if (blockID >= 0 && blockID < MAX_BLOCKS) {
        return !disk[blockID].chainee.free;
    }
    return false;
}
//6Remplir un Fichier (Trié et Non Trié)
void fillFile(int fileID, bool isSorted) {
    // Remplir le fichier avec des enregistrements, trié ou non
    printf("File %d filled. Sorted: %s\n", fileID, isSorted ? "Yes" : "No");
}
//7 Charger un Fichier
void loadFile(int fileID) {
    // Allouer les blocs nécessaires pour ce fichier
    printf("File %d loaded into disk.\n", fileID);
}
//8 Insérer un Enregistrement (Trié et Non Trié)
void insertRecord(int fileID, Enregistrement record, bool isSorted) {
    // Si trié, insérer à la bonne position
    if (isSorted) {
        // Code pour insérer trié
    } else {
        // Code pour insérer sans tri
    }
    printf("Record inserted in file %d.\n", fileID);
}
//9Suppression Logique d'un Enregistrement
void deleteRecordLogical(int fileID, int recordID) {
    // Chercher l'enregistrement et le marquer comme supprimé
    printf("Logical deletion of record %d in file %d.\n", recordID, fileID);
}
//10 Suppression Physique d'un Enregistrement
void deleteRecordPhysical(int fileID, int recordID) {
    // Réorganiser les blocs pour libérer l'espace
    printf("Physical deletion of record %d in file %d.\n", recordID, fileID);
}
//11Défragmentation d'un Fichier
void defragmentFile(int fileID) {
    // Défragmenter le fichier pour libérer de l'espace
    printf("File %d defragmented.\n", fileID);
}
//12void deleteFile(int fileID) {
    // Libérer les blocs du fichier
    printf("File %d deleted.\n", fileID);
}

