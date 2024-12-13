#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "TextColor.h"
#include "Metadonnees.h"
#include "TableIndex.h"
#include "Disk.h"

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
    for (int i=0 ; i<MAX_BLOCKS; i++) {// Find the first free block
        if (disk[i].chainee.free) {
            disk[i].chainee.free = false;
            if (isSorted) {
                for (int j=0 ; j<BLOCK_SIZE; j++) {
                    disk[i].chainee.enregistrement[j].ID = fileID*BLOCK_SIZE+j+1;
                    snprintf(disk[i].chainee.enregistrement[j].Data, 100, "Record_%d", fileID * BLOCK_SIZE + j + 1);
                    disk[i].chainee.enregistrement[j].Supprime = false;
                    for (int k=j; k>0 && disk[i].chainee.enregistrement[k].ID<disk[i].chainee.enregistrement[k-1].ID; k--) {
                        Enregistrement temp = disk[i].chainee.enregistrement[k];
                        disk[i].chainee.enregistrement[k] = disk[i].chainee.enregistrement[k-1];
                        disk[i].chainee.enregistrement[k-1] = temp;
                    }
                }
            }
            else { // Fill the block without sorting
                for (int j=0 ;j<BLOCK_SIZE; j++) {
                    disk[i].chainee.enregistrement[j].ID = fileID*BLOCK_SIZE+j+1;
                    snprintf(disk[i].chainee.enregistrement[j].Data, 100, "Record_%d", fileID*BLOCK_SIZE+j+1);
                    disk[i].chainee.enregistrement[j].Supprime = false;
                    }
                }
            if ((fileID+1)*BLOCK_SIZE<=(i+1)*BLOCK_SIZE) // Stop filling when the file is filled completely
                break;
        }
    }
     printf("File %d filled. Sorted: %s\n", fileID, isSorted ? "Yes" : "No");
}

//7 Charger un Fichier
void loadFile(int fileID) {// Check if the file ID is valid
    if (fileID < 0 || fileID >= MAX_BLOCKS) {
        printf("Error: Invalid file ID %d.\n", fileID);
        return;
    }
    int allocatedBlocks = 0;
    for (int i = 0; i < MAX_BLOCKS; i++) {   // find and allocate free blocks
        if (disk[i].contigue.free) { // Check if the block is free
            disk[i].contigue.free = false; // Mark the block as used
            for (int j = 0; j < BLOCK_SIZE; j++) {    // Initialize the records in this block
                disk[i].contigue.enregistrement[j].ID = -1; // Default ID
                disk[i].contigue.enregistrement[j].Supprime = false; // Logical deletion set to false
                memset(disk[i].contigue.enregistrement[j].Data, 0, sizeof(disk[i].contigue.enregistrement[j].Data)); // Clear data
            }
            allocatedBlocks++;
            printf("Block %d allocated for file %d.\n", i, fileID);
            if (allocatedBlocks >= BLOCK_SIZE) { // Stop allocation if enough blocks have been allocated
                break;
            }
        }
    }

    // Check if any blocks were allocated
    if (allocatedBlocks == 0) {
        printf("Error: No blocks available for file %d.\n", fileID);
    } else {
        printf("File %d loaded with %d blocks allocated.\n", fileID, allocatedBlocks);
    }
}


//8 Insérer un Enregistrement (Trié et Non Trié)
void insertRecord(int fileID, Enregistrement record, bool isSorted) {
    if (isSorted) { // find the correct position for sorted insertion
        for (int i = 0; i < MAX_BLOCKS; i++) {
            if (!disk[i].chainee.free) { // Check if the block belongs to the file
                for (int j = 0; j < BLOCK_SIZE; j++) { // Find the correct position
                    if (disk[i].chainee.enregistrement[j].ID== 0 || record.ID<disk[i].chainee.enregistrement[j].ID) { // Correct position
                        for (int k = BLOCK_SIZE - 1; k > j; k--) {      // Shift records to make space for the new record
                            disk[i].chainee.enregistrement[k] = disk[i].chainee.enregistrement[k-1];}
                        disk[i].chainee.enregistrement[j] = record; // Insert the new record
                        printf("Record inserted in sorted order in file %d at block %d, position %d.\n", fileID, i, j);
                        return;
                    }
                }
                if (disk[i].chainee.next == -1) { // If the block is full, move to the next block
                    printf("Error: No space left to insert the record in sorted order for file %d.\n", fileID);
                    return;
                }
            }
        }
    } else {
        // Insert without sorting
        for (int i = 0; i < MAX_BLOCKS; i++) {
            if (!disk[i].chainee.free) { // Check if the block belongs to the file
                for (int j = 0; j < BLOCK_SIZE; j++) {
                    if (disk[i].chainee.enregistrement[j].ID == 0) {
                        disk[i].chainee.enregistrement[j] = record;
                        printf("Record inserted in unsorted order in file %d at block %d, position %d.\n", fileID, i, j);
                        return;
                    }
                }
                if (disk[i].chainee.next == -1) {// If the block is full, move to the next block
                    printf("Error: No space left to insert the record in unsorted order for file %d.\n", fileID);
                    return;
                }
            }
        }
    }

    printf("Error: File %d not found or no space available.\n", fileID);
}


//9Suppression Logique d'un Enregistrement
void deleteRecordLogical(int fileID, int recordID) {
    if (fileID < 0 || fileID >= MAX_BLOCKS) {    // Check if the file ID is valid
        printf("Error: Invalid file ID %d.\n", fileID);
        return;
    }
    for (int i = 0; i < MAX_BLOCKS; i++) {    // find the record
        if (!disk[i].contigue.free) { // Check if the block is in use
            for (int j = 0; j < BLOCK_SIZE; j++) {
                if (disk[i].contigue.enregistrement[j].ID == recordID) { // Check if the record matches the given record ID
                    disk[i].contigue.enregistrement[j].Supprime = true; // Mark the record as logically deleted
                    printf("Record %d in file %d marked as logically deleted.\n", recordID, fileID);
                    return;
                }
            }
        }
    }
    // If the record is not found
    printf("Error: Record %d not found in file %d.\n", recordID, fileID);
}

//10 Suppression Physique d'un Enregistrement
void deleteRecordPhysical(int fileID, int recordID) {
   if (fileID < 0 || fileID >= MAX_BLOCKS) {  // Check if the file ID is valid
        printf("Error: Invalid file ID %d.\n", fileID);
        return;
    }
    for (int i = 0; i < MAX_BLOCKS; i++) { //find the record
        if (!disk[i].contigue.free) { // Check if the block is in use
            for (int j = 0; j < BLOCK_SIZE; j++) {
                if (disk[i].contigue.enregistrement[j].ID == recordID) { // Check if the record matches the given record ID
                    // Remove the record by resetting its fields
                    disk[i].contigue.enregistrement[j].ID = -1;
                    disk[i].contigue.enregistrement[j].Supprime = false;
                    memset(disk[i].contigue.enregistrement[j].Data, 0, sizeof(disk[i].contigue.enregistrement[j].Data));
                    printf("Record %d in file %d physically deleted.\n", recordID, fileID);
                    return;
                }
            }
        }
    }
    // If the record is not found
    printf("Error: Record %d not found in file %d.\n", recordID, fileID);
}

//11 Défragmentation d'un Fichier
void defragmentFile(int fileID) {
    if (fileID < 0 || fileID >= MAX_BLOCKS) { // Check if the file ID is valid
        printf("Error: Invalid file ID %d.\n", fileID);
        return;
    }
    for (int i = 0; i < MAX_BLOCKS; i++) { // Iterate through the disk to compact the file's records
        if (!disk[i].contigue.free) { // Check if the block is in use
            int writeIndex = 0;
            for (int j = 0; j < BLOCK_SIZE; j++) {
                // Move non-deleted records to fill gaps
                if (!disk[i].contigue.enregistrement[j].Supprime) {
                    if (writeIndex != j) {
                        disk[i].contigue.enregistrement[writeIndex] = disk[i].contigue.enregistrement[j];
                    }
                    writeIndex++;
                }
            }
            for (int j = writeIndex; j < BLOCK_SIZE; j++) {// Clear remaining records in the block
                disk[i].contigue.enregistrement[j].ID = -1;
                disk[i].contigue.enregistrement[j].Supprime = false;
                memset(disk[i].contigue.enregistrement[j].Data, 0, sizeof(disk[i].contigue.enregistrement[j].Data));
            }
        }
    }

    printf("File %d defragmented.\n", fileID);
}

//12
void deleteFile(int fileID) {
    if (fileID < 0 || fileID >= MAX_BLOCKS) {// Check if the file ID is valid
        printf("Error: Invalid file ID %d.\n", fileID);
        return;
    }
    for (int i = 0; i < MAX_BLOCKS; i++) {   // Iterate through the disk to free all blocks associated with the file
        if (!disk[i].contigue.free) { // Check if the block is in use
            for (int j = 0; j < BLOCK_SIZE; j++) {  // Clear all records in the block
                disk[i].contigue.enregistrement[j].ID = -1;
                disk[i].contigue.enregistrement[j].Supprime = false;
                memset(disk[i].contigue.enregistrement[j].Data, 0, sizeof(disk[i].contigue.enregistrement[j].Data));
            }
            disk[i].contigue.free = true; // Mark the block as free
        }
    }

    printf("File %d deleted.\n", fileID);
}



