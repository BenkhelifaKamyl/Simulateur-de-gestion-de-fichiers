#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "TextColor.h"
#include "Metadonnees.h"
#include "TableIndex.h"
#include "Disk.h"
//Affichage du disque
void AfficherDisque(){
    Bloc buffer;
    int j;
    for(int i=0; i<MAX_BLOCKS; i++){
        if(checkBlock(i)==false)
                printf("\nBloc libre.");
        else{
            memcpy(&buffer, &disk[i],sizeof(Bloc));
            j=0;
            while(j<BLOCK_SIZE && strlen(disk[i].chainee.enregistrement[j].Data)!=0)
                j++;
            printf("\nNom du fichier:  et nombre d'enregistrements: %d", j);
        }
    }
}

// Initialisation d'un bloc pour l'organisation chaînée
void initializeBlockChainee(int i) {
    disk[i].chainee.free = true;
    disk[i].chainee.next = -1;
    memset(disk[i].chainee.enregistrement, 0, BLOCK_SIZE);
}

// Initialisation d'un bloc pour l'organisation contiguë
void initializeBlockContigue(int i) {
    disk[i].contigue.free = true;
    memset(disk[i].contigue.enregistrement, 0, BLOCK_SIZE);
}

// Initialisation du disque en mode chaîné
void initializeDiskChainee() {
    if (MAX_BLOCKS <= 0 || BLOCK_SIZE <= 0) {
        printf("Erreur : configuration de disque invalide.\n");
        return;
    }
    for (int i = 0; i < MAX_BLOCKS; i++) {
        initializeBlockChainee(i);
    }
    printf("Disque initialisé avec %d blocs en mode chaîné.\n", MAX_BLOCKS);
}

// Initialisation du disque en mode contiguë
void initializeDiskContigue() {
    if (MAX_BLOCKS <= 0 || BLOCK_SIZE <= 0) {
        printf("Erreur : configuration de disque invalide.\n");
        return;
    }
    for (int i = 0; i < MAX_BLOCKS; i++) {
        initializeBlockContigue(i);
    }
    printf("Disque initialisé avec %d blocs en mode contiguë.\n", MAX_BLOCKS);
}
//compact disk 
void compactDisk() {
    typeOrganisation orgType = lireEnteteGlobal(*F); 
    if (orgType == Chained) {
        printf("Compacting disk with chained organization...\n");
        compactDiskChainee();
    } else if (orgType == Contiguous) {
        printf("Compacting disk with contiguous organization...\n");
        compactDiskContigue();
    } else {
        printf("Unknown disk organization. Cannot compact.\n");
    }
}


// Compactage du Disque avec Buffer (chainee)
void compactDiskChainee() {
    Bloc buffer; // Buffer temporaire pour le déplacement
    int lastFreeBlock = -1;

    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (disk[i].chainee.free) {
            if (lastFreeBlock == -1) lastFreeBlock = i;  // Trouve le premier bloc libre
        } else if (lastFreeBlock != -1) {
            // Utilise le buffer temporaire
            memcpy(&buffer, &disk[i], sizeof(Bloc));

            // Déplace le bloc dans l'espace libre
            memcpy(&disk[lastFreeBlock], &buffer, sizeof(Bloc));

            // Libère l'ancien bloc
            disk[i].chainee.free = 1;
            disk[i].chainee.next = -1;

            // Met à jour l'index du fichier
//            MajTableIndex(F, i, lastFreeBlock);

            // Trouve le prochain bloc libre
            for (int j = lastFreeBlock + 1; j < MAX_BLOCKS; j++) {
                if (disk[j].chainee.free) {
                    lastFreeBlock = j;
                    break;
                }
            }
        }
    }

    // Met à jour les métadonnées
    int premiereAdresse = -1;
    int nbBlocsUtilises = 0;

    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (!disk[i].chainee.free) {
            if (premiereAdresse == -1) premiereAdresse = i;
            nbBlocsUtilises++;
        }
    }

    MajEntetenum(F, 4, premiereAdresse);  // Met à jour premiereAdresse
    MajEntetenum(F, 2, nbBlocsUtilises); // Met à jour nbBlocs

    printf("Disk compacted.\n");
}


// Compactage du Disque avec Buffer (contigue)
void compactDiskContigue() {
    Bloc buffer; // Buffer temporaire pour le déplacement
    int lastFreeBlock = -1;

    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (disk[i].contigue.free) {
            if (lastFreeBlock == -1) lastFreeBlock = i;  // Trouve le premier bloc libre
        } else if (lastFreeBlock != -1) {
            // Utilise le buffer temporaire
            memcpy(&buffer, &disk[i], sizeof(Bloc));

            // Déplace le bloc dans l'espace libre
            memcpy(&disk[lastFreeBlock], &buffer, sizeof(Bloc));

            // Libère l'ancien bloc
            disk[i].contigue.free = true;

            // Trouve le prochain bloc libre
            for (int j = lastFreeBlock + 1; j < MAX_BLOCKS; j++) {
                if (disk[j].contigue.free) {
                    lastFreeBlock = j;
                    break;
                }
            }
        }
    }

    // Met à jour les métadonnées
    int premiereAdresse = -1;
    int nbBlocsUtilises = 0;

    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (!disk[i].contigue.free) {
            if (premiereAdresse == -1) premiereAdresse = i;
            nbBlocsUtilises++;
        }
    }

    MajEntetenum(F, 4, premiereAdresse);  // Met à jour premiereAdresse
    MajEntetenum(F, 2, nbBlocsUtilises); // Met à jour nbBlocs

    printf("Disque compacté en mode contigu.\n");
}

void clearDisk(); //Need to fill this function
// 4 Vider la Mémoire Secondaire (chainee)
void clearDiskchainee() {
    // Clear all blocks by setting them as free
    for (int i = 0; i < MAX_BLOCKS; i++) {
        disk[i].chainee.free = true;
        disk[i].chainee.next = -1;  // Reset the next pointer for chained blocks
        memset(disk[i].chainee.enregistrement, 0, BLOCK_SIZE);  // Clear the block content
    }

    // Update the metadata to reflect the cleared disk
    MajEntetenum(F, 2, 0);  // Set the number of blocks to 0
    MajEntetenum(F, 3, 0);  // Set the number of records to 0
    MajEntetenum(F, 4, 0);  // Set the first address to 0

    // Update the index table: clear the entries, as all blocks are free
  //  clearIndexTable(F);

    // Print confirmation message
    printf("Disk cleared.\n");
}


 // Vider la Mémoire Secondaire (contigue)
void clearDiskContigue() {
    // Clear all blocks by setting them as free
    for (int i = 0; i < MAX_BLOCKS; i++) {
        disk[i].contigue.free = true;  // Set the block as free
        memset(disk[i].contigue.enregistrement, 0, BLOCK_SIZE);  // Clear the block content
    }

    // Update the metadata to reflect the cleared disk
    MajEntetenum(F, 2, 0);  // Set the number of blocks to 0
    MajEntetenum(F, 3, 0);  // Set the number of records to 0
    MajEntetenum(F, 4, 0);  // Set the first address to 0

    // Update the index table: clear the entries, as all blocks are free
    // clearIndexTable(F);

    // Print confirmation message
    printf("Disk cleared in contigu mode.\n");
}

//5Vérification d'un Bloc(chainee)
bool checkBlock(int blockID) {
    if (blockID >= 0 && blockID < MAX_BLOCKS) {
        return !disk[blockID].chainee.free;
    }
    return false;
}

// Fill a File (Sorted or Unsorted) Using a Buffer (chainee)
void fillFileChainee(int fileID, bool isSorted, fichier *F) {
    const int BUFFER_SIZE = BLOCK_SIZE;
    Enregistrement buffer[BUFFER_SIZE];  // Declare buffer
    int recordsFilled = 0;
    int blocksUsed = 0;
    int firstBlockAddress = -1;
    int lastBlockAddress = -1;
    int bufferIndex = 0;

    // Fill buffer with records
    for (int i = 0; i < BUFFER_SIZE; i++) {
        buffer[i].ID = fileID * BUFFER_SIZE + i + 1;
        snprintf(buffer[i].Data, sizeof(buffer[i].Data), "Record_%d", buffer[i].ID);
        buffer[i].Supprime = false;
    }

    // Sort the buffer if required
    if (isSorted) {
        for (int i = 0; i < BUFFER_SIZE - 1; i++) {
            for (int j = i + 1; j < BUFFER_SIZE; j++) {
                if (buffer[j].ID < buffer[i].ID) {
                    Enregistrement temp = buffer[i];
                    buffer[i] = buffer[j];
                    buffer[j] = temp;
                }
            }
        }
    }

    // Insert records from buffer into the disk
    for (int i = 0; i < MAX_BLOCKS && bufferIndex < BUFFER_SIZE; i++) {
        if (disk[i].chainee.free) {
            disk[i].chainee.free = false;
            blocksUsed++;

            // Update the first and last block addresses
            if (firstBlockAddress == -1)
                firstBlockAddress = i;

            if (lastBlockAddress != -1)
                disk[lastBlockAddress].chainee.next = i;

            lastBlockAddress = i;

            // Copy buffer records to the current disk block
            for (int j = 0; j < BLOCK_SIZE && bufferIndex < BUFFER_SIZE; j++) {
                disk[i].chainee.enregistrement[j] = buffer[bufferIndex++];
                recordsFilled++;
            }
        }
    }

    // Update metadata
    MajEntetenum(F, 3, recordsFilled);     // Number of records
    MajEntetenum(F, 2, blocksUsed);        // Number of blocks used
    MajEntetenum(F, 4, firstBlockAddress); // First block address

    printf("File %d filled. Sorted: %s\n", fileID, isSorted ? "Yes" : "No");
}

// Fill a File (Sorted or Unsorted) Using a Buffer (contigue)
void fillFileContigue(int fileID, bool isSorted, fichier *F) {
    const int BUFFER_SIZE = BLOCK_SIZE;
    Enregistrement buffer[BUFFER_SIZE];  // Declare buffer
    int recordsFilled = 0;
    int blocksUsed = 0;
    int firstBlockAddress = -1;
    int lastBlockAddress = -1;
    int bufferIndex = 0;

    // Fill buffer with records
    for (int i = 0; i < BUFFER_SIZE; i++) {
        buffer[i].ID = fileID * BUFFER_SIZE + i + 1;
        snprintf(buffer[i].Data, sizeof(buffer[i].Data), "Record_%d", buffer[i].ID);
        buffer[i].Supprime = false;
    }

    // Sort the buffer if required
    if (isSorted) {
        for (int i = 0; i < BUFFER_SIZE - 1; i++) {
            for (int j = i + 1; j < BUFFER_SIZE; j++) {
                if (buffer[j].ID < buffer[i].ID) {
                    Enregistrement temp = buffer[i];
                    buffer[i] = buffer[j];
                    buffer[j] = temp;
                }
            }
        }
    }

    // Insert records from buffer into the disk in contiguous blocks
    for (int i = 0; i < MAX_BLOCKS && bufferIndex < BUFFER_SIZE; i++) {
        if (disk[i].contigue.free) {
            disk[i].contigue.free = false;  // Mark the block as occupied
            blocksUsed++;

            // Update the first and last block addresses
            if (firstBlockAddress == -1)
                firstBlockAddress = i;

            // Copy buffer records to the current disk block
            for (int j = 0; j < BLOCK_SIZE && bufferIndex < BUFFER_SIZE; j++) {
                disk[i].contigue.enregistrement[j] = buffer[bufferIndex++];
                recordsFilled++;
            }

            lastBlockAddress = i;  // Update last block address
        }
    }

    // Update metadata
    MajEntetenum(F, 3, recordsFilled);     // Number of records
    MajEntetenum(F, 2, blocksUsed);        // Number of blocks used
    MajEntetenum(F, 4, firstBlockAddress); // First block address

    printf("File %d filled in contiguous mode. Sorted: %s\n", fileID, isSorted ? "Yes" : "No");
}

void ChargerFichierchainee(int fileID, fichier *F) {
    if (fileID < 0 || fileID >= MAX_BLOCKS) { // Check if the file ID is valid
        printf("Error: Invalid file ID %d.\n", fileID);
        return;
    }

    int allocatedBlocks = 0;
    int firstBlockAddress = -1;
    bool needsDefrag = false;
    bool needsCompaction = false;
    int lastFileBlock = -1;

    // Temporary buffer to store records before writing to disk block
    Enregistrement buffer[BLOCK_SIZE];

    // Step 1: Traverse the disk and allocate blocks based on the allocation table (index table)
    for (int i = 0; i < MAX_BLOCKS; i++) {
        // Check if the current block is already allocated to the file in the index table
        if (F->indexTable[i] == fileID) {
            if (disk[i].free) { // If the block is free
                disk[i].free = false; // Mark it as allocated

                // Set the first block address if it's the first allocated block
                if (firstBlockAddress == -1) {
                    firstBlockAddress = i;
                } else {
                    // Link the previous block to the current block in the chain
                    disk[lastFileBlock].next = i;
                }

                // Prepare the buffer with records for this block
                for (int j = 0; j < BLOCK_SIZE; j++) {
                    buffer[j].ID = fileID * BLOCK_SIZE + j + 1;
                    snprintf(buffer[j].Data, sizeof(buffer[j].Data), "Record_%d", buffer[j].ID);
                    buffer[j].Supprime = false; // Mark the record as not deleted
                }

                // Now write the buffer into the current block
                for (int j = 0; j < BLOCK_SIZE; j++) {
                    disk[i].enregistrement[j] = buffer[j];
                }

                // Set the current block as the last block for linking in the next iteration
                lastFileBlock = i;
                allocatedBlocks++;
                printf("Block %d allocated for file %d.\n", i, fileID);

                // If this is the last block for the file, mark the next pointer as -1
                if (i == MAX_BLOCKS - 1 || F->indexTable[i + 1] != fileID) {
                    disk[i].next = -1; // End of chain
                }
            }
        }
    }

    // Step 2: Check if no blocks were allocated
    if (allocatedBlocks == 0) {
        printf("Error: No blocks available for file %d.\n", fileID);
        return;
    } else {
        printf("File %d loaded with %d blocks allocated.\n", fileID, allocatedBlocks);
    }

    // Step 3: Update the metadata after loading the file
    MajEntetenum(F, 2, allocatedBlocks); // Update metadata for block count
    MajEntetenum(F, 3, allocatedBlocks * BLOCK_SIZE); // Update metadata for record count
    MajEntetenum(F, 4, firstBlockAddress); // Update metadata for first block address

    // Step 4: Check for fragmentation (block-level fragmentation)
    bool fragmented = false;
    lastFileBlock = -1;
    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (F->indexTable[i] == fileID) { // Block is allocated
            if (lastFileBlock != -1 && i != lastFileBlock + 1) {
                fragmented = true; // Fragmentation detected
                break;
            }
            lastFileBlock = i;
        }
    }

    if (fragmented) {
        printf("File %d is fragmented. Defragmentation suggested.\n", fileID);
        needsDefrag = true;
    }

    // Step 5: Check for free blocks and suggest compaction
    needsCompaction = false;
    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (disk[i].free && F->indexTable[i] != fileID) { // Check if block is free and not allocated to the file
            needsCompaction = true;
            break;
        }
    }

    if (needsCompaction) {
        printf("File %d has free blocks. Compaction suggested.\n", fileID);
    }

    // Step 6: Update the index table for blocks allocated to the file
    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (!disk[i].free && F->indexTable[i] == -1) { // Block is allocated and not yet in indexTable
            F->indexTable[i] = fileID; // Set the file ID for this block in the index table
        }
    }

    // Step 7: Update metadata (e.g., free block count)
    int freeBlockCount = 0;
    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (disk[i].free) {
            freeBlockCount++;
        }
    }
    MajEntetenum(F, 5, freeBlockCount); // Update metadata for the number of free blocks
}

void ChargerFichierContigue(int fileID, fichier *F) {
    if (fileID < 0 || fileID >= MAX_BLOCKS) { // Check if the file ID is valid
        printf("Error: Invalid file ID %d.\n", fileID);
        return;
    }

    int allocatedBlocks = 0;
    int firstBlockAddress = -1;
    int lastBlockAddress = -1;
    bool needsCompaction = false;
    Enregistrement buffer[BLOCK_SIZE];  // Temporary buffer to store records before writing to disk block

    // Step 1: Find contiguous free blocks and allocate them for the file
    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (disk[i].contigue.free) { // Block is free
            if (firstBlockAddress == -1) {
                firstBlockAddress = i;  // Mark the first block
            }

            // Allocate the block
            disk[i].contigue.free = false;
            allocatedBlocks++;

            // Fill the buffer with records for this block
            for (int j = 0; j < BLOCK_SIZE; j++) {
                buffer[j].ID = fileID * BLOCK_SIZE + j + 1;
                snprintf(buffer[j].Data, sizeof(buffer[j].Data), "Record_%d", buffer[j].ID);
                buffer[j].Supprime = false;
            }

            // Write records to the disk block
            for (int j = 0; j < BLOCK_SIZE; j++) {
                disk[i].contigue.enregistrement[j] = buffer[j];
            }

            lastBlockAddress = i;  // Update last block address

            // If we have filled the required number of blocks, stop
            if (allocatedBlocks == F->indexTable[fileID]) {
                break;
            }
        }
    }

    // Step 2: Check if no blocks were allocated
    if (allocatedBlocks == 0) {
        printf("Error: No free blocks available for file %d.\n", fileID);
        return;
    } else {
        printf("File %d loaded with %d blocks allocated.\n", fileID, allocatedBlocks);
    }

    // Step 3: Update metadata after loading the file
    MajEntetenum(F, 2, allocatedBlocks);         // Update block count
    MajEntetenum(F, 3, allocatedBlocks * BLOCK_SIZE); // Update record count
    MajEntetenum(F, 4, firstBlockAddress);      // Update first block address

    // Step 4: Check for free blocks and suggest compaction
    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (disk[i].contigue.free && F->indexTable[i] != fileID) { // Block is free and not used by file
            needsCompaction = true;
            break;
        }
    }

    if (needsCompaction) {
        printf("File %d has free blocks. Compaction suggested.\n", fileID);
    }

    // Step 5: Update the index table with the allocated blocks
    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (!disk[i].contigue.free && F->indexTable[i] == -1) {
            F->indexTable[i] = fileID;  // Set the file ID for this block
        }
    }

    // Step 6: Update free block count
    int freeBlockCount = 0;
    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (disk[i].contigue.free) {
            freeBlockCount++;
        }
    }
    MajEntetenum(F, 5, freeBlockCount); // Update metadata for free block count
}

Enregistrement donneesEnregistrement(){
    Enregistrement E;
    E.Supprime=false;
    printf("\nDonnez les donnees de l'enregistrement: ");
    scanf("%s",E.Data);
    printf("\nDonnez l'ID de l'enregistrement: ");
    scanf("%d",E.ID);
    return E;
}
void insertRecord(fichier *F, Enregistrement record){
    bool isSorted = liretypeTri(*F);
    if(lireEnteteGlobal==Chainee)
        insertRecordchainee(F,record,isSorted);
    else
        insertRecordcontigue(F,record,isSorted);
}

// 8. Insert a Record (Sorted and Unsorted)(chainee)
void insertRecordchainee(fichier *F, Enregistrement record, bool isSorted) {
    if (fileID < 0 || fileID >= MAX_BLOCKS) {
        printf("Error: Invalid file ID %d.\n", fileID);
        return;
    }

    int blocksUsed = 0;
    int lastBlockAddress = -1;

    if (isSorted) { // Insert in sorted order
        for (int i = 0; i < MAX_BLOCKS; i++) {
            if (disk[i].chainee.free) {
                continue; // Skip free blocks
            }

            if (disk[i].chainee.next == -1) { // If we're at the end of the chain, check if there's space in the block
                for (int j = 0; j < BLOCK_SIZE; j++) {
                    if (disk[i].chainee.enregistrement[j].ID == 0 || record.ID < disk[i].chainee.enregistrement[j].ID) {
                        // Found the correct position
                        for (int k = BLOCK_SIZE - 1; k > j; k--) {  // Shift records to make space
                            disk[i].chainee.enregistrement[k] = disk[i].chainee.enregistrement[k - 1];
                        }
                        disk[i].chainee.enregistrement[j] = record;  // Insert the new record
                        printf("Record inserted in sorted order in file %d at block %d, position %d.\n", fileID, i, j);
                        // Update the index table
                        F->indexTable[i] = fileID; // Mark the block as allocated to the file
                        blocksUsed++;
                        lastBlockAddress = i;
                        MajEntetenum(F, 2, blocksUsed); // Update metadata for block count
                        MajEntetenum(F, 3, blocksUsed * BLOCK_SIZE); // Update metadata for record count
                        return;
                    }
                }
                if (disk[i].chainee.next == -1) { // Block is full and no next block exists
                    printf("Error: No space left to insert the record in sorted order for file %d.\n", fileID);
                    return;
                }
            }
        }
    } else { // Insert without sorting (unsorted)
        for (int i = 0; i < MAX_BLOCKS; i++) {
            if (disk[i].chainee.free) {
                continue; // Skip free blocks
            }

            // Find the first available space for the record in this block
            for (int j = 0; j < BLOCK_SIZE; j++) {
                if (disk[i].chainee.enregistrement[j].ID == 0) {  // Empty slot found
                    disk[i].chainee.enregistrement[j] = record;
                    printf("Record inserted in unsorted order in file %d at block %d, position %d.\n", fileID, i, j);
                    // Update the index table
                    F->indexTable[i] = fileID; // Mark the block as allocated to the file
                    blocksUsed++;
                    lastBlockAddress = i;
                    MajEntetenum(F, 2, blocksUsed); // Update metadata for block count
                    MajEntetenum(F, 3, blocksUsed * BLOCK_SIZE); // Update metadata for record count
                    return;
                }
            }

            if (disk[i].chainee.next == -1) { // If the block is full, move to the next block
                // Find the next available block in the chain (if any)
                int nextBlock = disk[i].chainee.next;
                if (nextBlock == -1) {
                    // Add a new block
                    for (int j = 0; j < MAX_BLOCKS; j++) {
                        if (disk[j].chainee.free) {
                            disk[j].chainee.free = false;
                            disk[i].chainee.next = j; // Link to the new block
                            F->indexTable[j] = fileID; // Update the index table for the new block
                            blocksUsed++;
                            lastBlockAddress = j;
                            printf("New block %d allocated and linked to block %d for file %d.\n", j, i, fileID);
                            return;
                        }
                    }
                    printf("Error: No space left to insert the record in unsorted order for file %d.\n", fileID);
                    return;
                }
                i = nextBlock - 1; // Set i to the next block's index (will increment on the next loop iteration)
            }
        }
    }
    printf("Error: File %d not found or no space available.\n", fileID);
}

// 8. Insert a Record (Sorted and Unsorted)(contiguous)
void insertRecordcontigue(fichier *F, Enregistrement record, bool isSorted) {
    if (fileID < 0 || fileID >= MAX_FILES) {
        printf("Error: Invalid file ID %d.\n", fileID);
        return;
    }

    // Retrieve file metadata
    FileMetadata *file = &F->files[fileID];
    if (file->startBlock == -1) {
        printf("Error: File %d not initialized.\n", fileID);
        return;
    }

    // Check if there is enough space in the contiguous allocation
    int totalBlocks = file->blockCount;
    int totalRecords = totalBlocks * BLOCK_SIZE;

    if (file->recordCount >= totalRecords) {
        printf("Error: No space left to insert the record in file %d.\n", fileID);
        return;
    }

    // Find the position to insert the record
    int insertPos = file->recordCount;
    int blockIndex = file->startBlock + (insertPos / BLOCK_SIZE);
    int recordIndex = insertPos % BLOCK_SIZE;

    if (isSorted) {
        // Insert in sorted order
        bool inserted = false;
        for (int i = 0; i < file->recordCount; i++) {
            int currentBlockIndex = file->startBlock + (i / BLOCK_SIZE);
            int currentRecordIndex = i % BLOCK_SIZE;

            if (disk[currentBlockIndex].contigue.enregistrement[currentRecordIndex].ID > record.ID) {
                // Shift records to the right to make space
                for (int j = file->recordCount; j > i; j--) {
                    int fromBlockIndex = file->startBlock + ((j - 1) / BLOCK_SIZE);
                    int fromRecordIndex = (j - 1) % BLOCK_SIZE;

                    int toBlockIndex = file->startBlock + (j / BLOCK_SIZE);
                    int toRecordIndex = j % BLOCK_SIZE;

                    disk[toBlockIndex].contigue.enregistrement[toRecordIndex] = disk[fromBlockIndex].contigue.enregistrement[fromRecordIndex];
                }

                // Insert the new record
                disk[currentBlockIndex].contigue.enregistrement[currentRecordIndex] = record;
                inserted = true;
                break;
            }
        }

        if (!inserted) {
            // Insert at the end if no larger record was found
            disk[blockIndex].contigue.enregistrement[recordIndex] = record;
        }
    } else {
        // Insert without sorting (unsorted order)
        disk[blockIndex].contigue.enregistrement[recordIndex] = record;
    }

    // Update file metadata
    file->recordCount++;
    MajEntetenum(F, 2, file->blockCount);          // Update block count metadata
    MajEntetenum(F, 3, file->recordCount);        // Update record count metadata

    printf("Record inserted in file %d at block %d, position %d.\n", fileID, blockIndex, recordIndex);
}


// 9. Logical Deletion of a Record(chainee)
void deleteRecordLogicalchainee(int fileID, int recordID) {
    if (fileID < 0 || fileID >= MAX_BLOCKS) { // Check if the file ID is valid
        printf("Error: Invalid file ID %d.\n", fileID);
        return;
    }

    for (int i = 0; i < MAX_BLOCKS; i++) { // Find the block that contains the record
        if (!disk[i].chainee.free) { // Check if the block is in use (not free)
            for (int j = 0; j < BLOCK_SIZE; j++) { // Traverse the records in the block
                if (disk[i].chainee.enregistrement[j].ID == recordID) { // If the record matches the given ID
                    disk[i].chainee.enregistrement[j].Supprime = true; // Mark the record as logically deleted
                    printf("Record %d in file %d marked as logically deleted.\n", recordID, fileID);
                    return; // Return after marking the record as deleted
                }
            }
        }
    }

    // If the record is not found in any block
    printf("Error: Record %d not found in file %d.\n", recordID, fileID);
}


// 9. Logical Deletion of a Record(contiguous)
void deleteRecordLogicalcontigue(int fileID, int recordID) {
    if (fileID < 0 || fileID >= MAX_FILES) { // Check if the file ID is valid
        printf("Error: Invalid file ID %d.\n", fileID);
        return;
    }

    // Retrieve file metadata
    FileMetadata *file = &F->files[fileID];
    if (file->startBlock == -1) {
        printf("Error: File %d not initialized.\n", fileID);
        return;
    }

    // Traverse the records in the contiguous blocks allocated to the file
    for (int i = 0; i < file->recordCount; i++) {
        int blockIndex = file->startBlock + (i / BLOCK_SIZE);
        int recordIndex = i % BLOCK_SIZE;

        if (disk[blockIndex].contigue.enregistrement[recordIndex].ID == recordID) { // If the record matches the given ID
            disk[blockIndex].contigue.enregistrement[recordIndex].Supprime = true; // Mark the record as logically deleted
            printf("Record %d in file %d marked as logically deleted.\n", recordID, fileID);
            return; // Return after marking the record as deleted
        }
    }

    // If the record is not found in the allocated blocks
    printf("Error: Record %d not found in file %d.\n", recordID, fileID);
}

// 10. Physical Deletion of a Record(chained)
void deleteRecordPhysicalchaine(int fileID, int recordID) {
    if (fileID < 0 || fileID >= MAX_BLOCKS) { // Check if the file ID is valid
        printf("Error: Invalid file ID %d.\n", fileID);
        return;
    }

    for (int i = 0; i < MAX_BLOCKS; i++) { // Find the block that contains the record
        if (!disk[i].chainee.free) { // Check if the block is in use (not free)
            for (int j = 0; j < BLOCK_SIZE; j++) { // Traverse the records in the block
                if (disk[i].chainee.enregistrement[j].ID == recordID) { // If the record matches the given ID
                    // Physically delete the record by resetting its fields
                    disk[i].chainee.enregistrement[j].ID = -1; // Reset record ID to indicate it's removed
                    disk[i].chainee.enregistrement[j].Supprime = false; // Reset "deleted" flag
                    memset(disk[i].chainee.enregistrement[j].Data, 0, sizeof(disk[i].chainee.enregistrement[j].Data)); // Clear data
                    printf("Record %d in file %d physically deleted.\n", recordID, fileID);

                    // Optionally, update the next pointer or check for block compaction if needed
                    // For now, we just return since it's a simple deletion
                    return;
                }
            }
        }
    }

    // If the record is not found in any block
    printf("Error: Record %d not found in file %d.\n", recordID, fileID);
}

// // Function to perform logical deletion (marking a record as deleted)
void SuppressionLogique(fichier *F, int recordId) {
//     // Find the record position using the search function
//     int recordPosition = rechercheEnregistrement(F, recordId);

//     if (recordPosition == -1) {
//         printf("Enregistrement introuvable.\n");
//         return;
//     }

//     // Mark the record as deleted (for example, setting a deletion flag or updating status)
//     Bloc Buffer;
//     LireBloc(F, recordPosition, &Buffer);
//     Buffer.chainee.free = true;  // Assuming the record has a 'deleted' flag
//     EcrireBloc(F, recordPosition, Buffer);

//     printf("Suppression logique effectuée pour l'enregistrement %d.\n", recordId);

//     // Perform defragmentation after logical deletion
//     Defragmentation(F);
// }

// // Function to perform physical deletion (removing the record from the disk)
// void SuppressionPhysique(fichier *F, int recordId) {
//     // Find the record position using the search function
//     int recordPosition = rechercheEnregistrement(F, recordId);

//     if (recordPosition == -1) {
//         printf("Enregistrement introuvable.\n");
//         return;
//     }

//     // Physically remove the record by clearing the block
//     Bloc Buffer;
//     LireBloc(F, recordPosition, &Buffer);
//     memset(&Buffer, 0, sizeof(Bloc));  // Clear the block content

//     // Update the block to indicate it's free (or deleted)
//     EcrireBloc(F, recordPosition, Buffer);

//     printf("Suppression physique effectuée pour l'enregistrement %d.\n", recordId);

//     // Update metadata: reduce the number of records
//     int nbEnregistrements = lireEntete(*F, 3);  // Read the number of records from metadata
//     MajEntetenum(F, 3, nbEnregistrements - 1);  // Decrease the number of records

//     // Perform defragmentation to compact the storage after physical deletion
//     Defragmentation(F);
// }

// // Function to search for a record by its ID (this should be implemented based on your file structure)
// /*int rechercheEnregistrement(fichier *F, int recordId) {
//     // Iterate over the blocks to find the record with the given ID
//     for (int i = 0; i < MAX_BLOCKS; i++) {
//         Bloc Buffer;
//         LireBloc(F, i, &Buffer);

//         if (Buffer.chainee.enregistrement[i] == recordId && !Buffer.deleted) {
//             return i;  // Return the position of the record
//         }
//     }
//     return -1;  // Record not found
 }

// 10. Physical Deletion of a Record (Contiguous Disk Structure)
void deleteRecordPhysicalContiguous(int fileID, int recordID) {
    if (fileID < 0 || fileID >= MAX_BLOCKS) { // Check if the file ID is valid
        printf("Error: Invalid file ID %d.\n", fileID);
        return;
    }

    // Iterate over blocks to find the record
    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (!disk[i].contigue.free) { // Check if the block is in use (not free)
            for (int j = 0; j < BLOCK_SIZE; j++) { // Traverse the records in the block
                if (disk[i].contigue.enregistrement[j].ID == recordID) { // If the record matches the given ID
                    // Physically delete the record by resetting its fields
                    disk[i].contigue.enregistrement[j].ID = -1; // Reset record ID to indicate it's removed
                    disk[i].contigue.enregistrement[j].Supprime = true; // Set "deleted" flag
                    memset(disk[i].contigue.enregistrement[j].Data, 0, sizeof(disk[i].contigue.enregistrement[j].Data)); // Clear data
                    printf("Record %d in file %d physically deleted.\n", recordID, fileID);
                    return; // Record found and deleted, exit function
                }
            }
        }
    }

    // If the record is not found in any block
    printf("Error: Record %d not found in file %d.\n", recordID, fileID);
}

void Defragmentation(fichier *F){
    if(lireEnteteGlobal(F)==Chainee)
        Defragmentationchainee(F);
    else
        DefragmentationContigue(F);
}
// Function to perform defragmentation(chained): update metadata, table index, and compact blocks
void Defragmentationchainee(fichier *F) {
    printf("Défragmentation en cours...\n");

    int nbBlocs = lireEntete(*F, 2);  // Read the number of blocks from metadata
    int nbEnregistrements = lireEntete(*F, 3);  // Read the number of records from metadata
    int freeBlockIndex = AllouerBloc();  // Get the next free block index

    if (freeBlockIndex == -1) {
        printf("Pas de blocs libres disponibles pour la défragmentation.\n");
        return;
    }

    // Iterate through blocks to compact data
    for (int i = 0; i < nbBlocs; i++) {
        Bloc Buffer;
        LireBloc(F, i, &Buffer);

        if (Buffer.chainee.free ) {
            // If the block is deleted or empty, move the next record to this free block
            // If there's a record to move, update the block and metadata
            if (freeBlockIndex != i) {
                LireBloc(F, freeBlockIndex, &Buffer);
                EcrireBloc(F, i, Buffer);  // Move the record to the free block
                freeBlockIndex = AllouerBloc();  // Get the next free block
            }
        }
    }

    // Update metadata after defragmentation (number of blocks, records, etc.)
    MajEntetenum(F, 2, nbBlocs);  // Update number of blocks after defragmentation
    MajEntetenum(F, 3, nbEnregistrements);  // Update number of records
    printf("Défragmentation terminée.\n");
}

// Function to perform defragmentation(contiguous) : update metadata, table index, and compact blocks
void DefragmentationContigue(fichier *F) {
    printf("Defragmentation in progress...\n");

    int nbBlocs = lireEntete(*F, 2);  // Read the number of blocks from metadata
    int nbEnregistrements = lireEntete(*F, 3);  // Read the number of records from metadata
    int freeBlockIndex = AllouerBloc();  // Get the next free block index

    if (freeBlockIndex == -1) {
        printf("No free blocks available for defragmentation.\n");
        return;
    }

    // Iterate through blocks to compact data
    for (int i = 0; i < nbBlocs; i++) {
        Bloc Buffer;
        LireBloc(F, i, &Buffer);

        if (Buffer.contigue.free) {  // Check if the block is free or marked as deleted
            // If the block is deleted or empty, move the next record to this free block
            if (freeBlockIndex != i) {
                LireBloc(F, freeBlockIndex, &Buffer);
                EcrireBloc(F, i, Buffer);  // Move the record to the free block
                freeBlockIndex = AllouerBloc();  // Get the next free block
            }
        }
    }

    // Update metadata after defragmentation (number of blocks, records, etc.)
    MajEntetenum(F, 2, nbBlocs);  // Update the number of blocks after defragmentation
    MajEntetenum(F, 3, nbEnregistrements);  // Update the number of records
    printf("Defragmentation complete.\n");
}

void deleteFile(fichier *F){
    if(lireEnteteGlobal(*F)==Chainee)
        deleteFilechainee(F);
    else
        deleteFileContigue(F);
}
// Delete a file with chained allocation
void deleteFilechainee(fichier *F) {
    if (fileID < 0 || fileID >= MAX_BLOCKS) {  // Validate file ID
        printf("Error: Invalid file ID %d.\n", fileID);
        return;
    }

    // Find the first block of the file using chained allocation
    int currentBlockID = -1;
    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (!disk[i].chainee.free && disk[i].chainee.fileID == fileID) {
            currentBlockID = i;  // Found the first block
            break;
        }
    }

    if (currentBlockID == -1) {  // If no block is found
        printf("Error: File with ID %d not found.\n", fileID);
        return;
    }

    int nbBlocsLibres = 0;  // Counter for free blocks

    // Traverse and free all blocks of the file using the linked list
    while (currentBlockID != -1) {
        int nextBlockID = disk[currentBlockID].chainee.nextBlock;  // Store next block ID

        // Clear current block
        disk[currentBlockID].chainee.free = true;
        disk[currentBlockID].chainee.fileID = -1;
        disk[currentBlockID].chainee.nextBlock = -1;

        for (int j = 0; j < BLOCK_SIZE; j++) {  // Clear records in the block
            disk[currentBlockID].chainee.enregistrement[j].ID = -1;
            disk[currentBlockID].chainee.enregistrement[j].Supprime = false;
            memset(disk[currentBlockID].chainee.enregistrement[j].Data, 0, sizeof(disk[currentBlockID].chainee.enregistrement[j].Data));
        }

        nbBlocsLibres++;  // Increase count of freed blocks
        currentBlockID = nextBlockID;  // Move to the next block
    }

    // Update metadata
    MajEntetenum(F, 2, lireEntete(*F, 2) - nbBlocsLibres);  // Update the number of blocks
    MajEntetenum(F, 3, 0);  // Reset number of records to 0
    MajEntetenum(F, 4, -1);  // Reset the first block address

    // Remove file from the index table
    removeFromIndexTable(fileID);

    printf("File %d deleted successfully.\n", fileID);
}

// Delete a file with contiguous allocation
void deleteFileContigue(fichier *F) {
    if (fileID < 0 || fileID >= MAX_BLOCKS) {  // Validate file ID
        printf("Error: Invalid file ID %d.\n", fileID);
        return;
    }

    // Find the first block of the file using contiguous allocation
    int currentBlockID = -1;
    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (!disk[i].contigue.free && disk[i].contigue.fileID == fileID) {
            currentBlockID = i;  // Found the first block
            break;
        }
    }

    if (currentBlockID == -1) {  // If no block is found
        printf("Error: File with ID %d not found.\n", fileID);
        return;
    }

    int nbBlocsLibres = 0;  // Counter for free blocks

    // Traverse and free all blocks of the file
    while (currentBlockID != -1) {
        // Clear current block
        disk[currentBlockID].contigue.free = true;
        disk[currentBlockID].contigue.fileID = -1;

        for (int j = 0; j < BLOCK_SIZE; j++) {  // Clear records in the block
            disk[currentBlockID].contigue.enregistrement[j].ID = -1;
            disk[currentBlockID].contigue.enregistrement[j].Supprime = false;
            memset(disk[currentBlockID].contigue.enregistrement[j].Data, 0, sizeof(disk[currentBlockID].contigue.enregistrement[j].Data));
        }

        nbBlocsLibres++;  // Increase count of freed blocks
        currentBlockID = -1;  // No next block in contiguous allocation
    }

    // Update metadata
    MajEntetenum(F, 2, lireEntete(*F, 2) - nbBlocsLibres);  // Update the number of blocks
    MajEntetenum(F, 3, 0);  // Reset number of records to 0
    MajEntetenum(F, 4, -1);  // Reset the first block address

    // Remove file from the index table
    removeFromIndexTable(fileID);

    printf("File %d deleted successfully.\n", fileID);
}

void LireBloc(fichier *F, int i, Bloc *Buffer){
    if(i<0 || i>=MAX_BLOCKS){
        printf("Indice de bloc invalide\n");
    }
    else
        memcpy(Buffer,&disk[i],sizeof(Bloc));
}
void EcrireBloc(fichier *F, int i, Bloc Buffer){
    if(i<0 || i>=MAX_BLOCKS)
        printf("Indice de bloc invalide\n");
    else{
        memcpy(&disk[i],&Buffer,sizeof(Bloc));
    }
}
