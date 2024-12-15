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
     // Mise à jour de la table d'index
    MajTableIndex(F, i, lastFreeBlock);
    // Trouver le prochain bloc libre
            for (int j = lastFreeBlock + 1; j < MAX_BLOCKS; j++) {
                if (disk[j].chainee.free) {
                    lastFreeBlock = j;
                    break;
                }
            }    
        }
    }
    // Mettre à jour les métadonnées
    int premiereAdresse = -1;
    int nbBlocsUtilises = 0;

    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (!disk[i].chainee.free) {
            if (premiereAdresse == -1) premiereAdresse = i;
            nbBlocsUtilises++;
        }
    }

    // Mettre à jour les entêtes dans le fichier de métadonnées
    MajEntetenum(F, 4, premiereAdresse);  // Met à jour premiereAdresse
    MajEntetenum(F, 2, nbBlocsUtilises); // Met à jour nbBlocs
    printf("Disk compacted.\n");
}
//4 Vider la Mémoire Secondaire
void clearDisk() {
    for (int i = 0; i < MAX_BLOCKS; i++) {
        disk[i].chainee.free = true;
        disk[i].chainee.next = -1;
        memset(disk[i].chainee.enregistrement, 0, BLOCK_SIZE);
    }
    // Update the metadata
    MajEntetenum(F, 2, 0);  // Set the number of blocks to 0
    MajEntetenum(F, 3, 0);  // Set the number of records to 0
    MajEntetenum(F, 4, 0);  // Set the first address to 0 
    // Update the index table: clear the entries, as all blocks are free
    clearIndexTable(F); 
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
 void fillFile(int fileID, bool isSorted, fichier *F) {
    int recordsFilled = 0;  // Total number of records filled in the file
    int blocksUsed = 0;
    int firstBlockAddress = -1;  // Address of the first block used

    for (int i = 0; i < MAX_BLOCKS; i++) {
        // Find the first free block
        if (disk[i].chainee.free) {
            disk[i].chainee.free = false;  // Mark the block as used
            
            // Set the first block address if it's the first block being used
            if (firstBlockAddress == -1) {
                firstBlockAddress = i;
            }

            // Fill the block with records
            if (isSorted) {
                // Sorted insertion logic
                for (int j = 0; j < BLOCK_SIZE; j++) {
                    disk[i].chainee.enregistrement[j].ID = fileID * BLOCK_SIZE + j + 1;
                    snprintf(disk[i].chainee.enregistrement[j].Data, 100, "Record_%d", fileID * BLOCK_SIZE + j + 1);
                    disk[i].chainee.enregistrement[j].Supprime = false;

                    // Sort records in the block
                    for (int k = j; k > 0 && disk[i].chainee.enregistrement[k].ID < disk[i].chainee.enregistrement[k - 1].ID; k--) {
                        Enregistrement temp = disk[i].chainee.enregistrement[k];
                        disk[i].chainee.enregistrement[k] = disk[i].chainee.enregistrement[k - 1];
                        disk[i].chainee.enregistrement[k - 1] = temp;
                    }
                }
            } else {
                // Unsorted insertion logic
                for (int j = 0; j < BLOCK_SIZE; j++) {
                    disk[i].chainee.enregistrement[j].ID = fileID * BLOCK_SIZE + j + 1;
                    snprintf(disk[i].chainee.enregistrement[j].Data, 100, "Record_%d", fileID * BLOCK_SIZE + j + 1);
                    disk[i].chainee.enregistrement[j].Supprime = false;
                }
            }

            // Update the number of records and blocks used
            recordsFilled += BLOCK_SIZE;
            blocksUsed++;

            // Update the index table to reflect that this block is occupied by the file
            F->indexTable[i] = fileID;

            // Stop filling when the total number of records is reached
            if (recordsFilled >= fileID * BLOCK_SIZE) {
                break;
            }
        }
    }

    // Update metadata based on the filled file
    MajEntetenum(F, 3, recordsFilled); // Update the number of records filled
    MajEntetenum(F, 2, blocksUsed);     // Update the number of blocks used
    MajEntetenum(F, 4, firstBlockAddress); // Update the first block address (if needed)

    printf("File %d filled. Sorted: %s\n", fileID, isSorted ? "Yes" : "No");
}


//7 Charger un Fichier
void ChargerFichier(int fileID, fichier *F) {
    // Check if the file ID is valid
    if (fileID < 0 || fileID >= MAX_BLOCKS) {
        printf("Error: Invalid file ID %d.\n", fileID);
        return;
    }

    int allocatedBlocks = 0;
    int firstBlockAddress = -1;
    bool needsDefrag = false;
    bool needsCompaction = false;
    int lastFileBlock = -1;
    
    // Step 1: Traverse the disk and allocate blocks based on the allocation table (index table)
    for (int i = 0; i < MAX_BLOCKS; i++) {
        // Check if the current block is already allocated to the file in the index table
        if (F->indexTable[i] == fileID) {
            if (disk[i].contigue.free) { // If the block is free, allocate it
                disk[i].contigue.free = false;

                // Initialize the records in this block
                for (int j = 0; j < BLOCK_SIZE; j++) {
                    disk[i].contigue.enregistrement[j].ID = fileID * BLOCK_SIZE + j + 1; // Set the record ID
                    disk[i].contigue.enregistrement[j].Supprime = false; // Logical deletion set to false
                    memset(disk[i].contigue.enregistrement[j].Data, 0, sizeof(disk[i].contigue.enregistrement[j].Data)); // Clear data
                }

                // If this is the first block, update the first block address in the metadata
                if (firstBlockAddress == -1) {
                    firstBlockAddress = i;
                }

                allocatedBlocks++;

                printf("Block %d allocated for file %d.\n", i, fileID);
            }
        }
    }

    // Step 2: Check if any blocks were allocated
    if (allocatedBlocks == 0) {
        printf("Error: No blocks available for file %d.\n", fileID);
    } else {
        printf("File %d loaded with %d blocks allocated.\n", fileID, allocatedBlocks);

        // Step 3: Update the metadata after loading the file
        MajEntetenum(F, 2, allocatedBlocks); // Update metadata for block count
        MajEntetenum(F, 3, allocatedBlocks * BLOCK_SIZE); // Update metadata for record count
        MajEntetenum(F, 4, firstBlockAddress); // Update metadata for first block address

        // Step 4: Check for fragmentation (block-level and record-level)
        bool fragmented = false;
        lastFileBlock = -1;

        // Check if there are gaps between allocated blocks for the file (block-level fragmentation)
        for (int i = 0; i < MAX_BLOCKS; i++) {
            if (F->indexTable[i] == fileID) { // Block is allocated
                if (lastFileBlock != -1 && i != lastFileBlock + 1) {
                    fragmented = true;
                    break;
                }
                lastFileBlock = i;
            }
        }

        // If fragmented, propose defragmentation
        if (fragmented) {
            printf("File %d is fragmented. Defragmentation suggested.\n", fileID);
            needsDefrag = true;
        }

        // Step 5: Check for compaction opportunities (free blocks)
        needsCompaction = false;
        for (int i = 0; i < MAX_BLOCKS; i++) {
            if (disk[i].contigue.free && F->indexTable[i] != fileID) { // Free block not allocated to file
                needsCompaction = true;
                break;
            }
        }

        if (needsCompaction) {
            printf("File %d has free blocks. Compaction suggested.\n", fileID);
        }

        // Step 6: Update the index table to reflect which blocks belong to the file
        for (int i = 0; i < MAX_BLOCKS; i++) {
            if (!disk[i].contigue.free) { // If the block is no longer free, it's allocated to this file
                F->indexTable[i] = fileID;
            }
        }

        // Step 7: Update the metadata (e.g., number of free blocks, file status)
        // You can update additional metadata fields here based on your needs, such as free space on disk
        int freeBlockCount = 0;
        for (int i = 0; i < MAX_BLOCKS; i++) {
            if (disk[i].contigue.free) {
                freeBlockCount++;
            }
        }
        int freeBlockCount = 0;
        for (int i = 0; i < MAX_BLOCKS; i++) {
            if (disk[i].contigue.free) {
                freeBlockCount++;
            }
        }
        MajEntetenum(F, 5, freeBlockCount); // Update metadata for the number of free blocks
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



