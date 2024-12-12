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

//------compactage-------
void Compactage() {
    int freeBlockPointer = 0;

    for (int currentBlock = 0; currentBlock < memory.totalBlocks; currentBlock++) {
        if (!memory.blocks[currentBlock].isFree) {
            if (currentBlock != freeBlockPointer) {
                // Move data to the free block
                memory.blocks[freeBlockPointer] = memory.blocks[currentBlock];
                memory.blocks[currentBlock].isFree = 1;
                memset(memory.blocks[currentBlock].data, 0, BLOCK_SIZE);
                memory.blocks[freeBlockPointer].isFree = 0;
            }
            freeBlockPointer++;
        }
    }

    printf("disk compacted.\n");
}


//-----------function to clear all memory ------
void ViderMS() {
    for (int i = 0; i < memory.totalBlocks; i++) {
        memory.blocks[i].isFree = 1;
        memset(memory.blocks[i].data, 0, BLOCK_SIZE);
    }

    memory.freeBlocks = memory.totalBlocks;
    printf("All memory cleared.\n");
}

// ----------------Function to verify if a block is free-------------
int VerifBloc(int blockIndex) {
    if (blockIndex < 0 || blockIndex >= memory.totalBlocks) {
        printf("Error: Invalid block index.\n");
        return -1;
    }

    return memory.blocks[blockIndex].isFree;
}



//------------ Function to add a record to a sorted file and maintain the sorted order------
void RemplirFichierTriee(const char* fileName, const char* newRecord) {
    // Find the file by name
    for (int i = 0; i < fileCount; i++) {
        if (strcmp(files[i].metadata.name, fileName) == 0) {
            if (!files[i].metadata.isSorted) {
                printf("Error: File '%s' is not sorted.\n", fileName);
                return;
            }

            // Iterate through blocks in the file to find the correct position
            for (int j = 0; j < files[i].metadata.numBlocks; j++) {
                int blockIndex = files[i].indexTable[j].blockIndex;

                // Scan within the block to find the right position
                for (int k = 0; k < BLOCK_SIZE; k += strlen(&memory.blocks[blockIndex].data[k]) + 1) {
                    if (memory.blocks[blockIndex].data[k] == '\0' || strcmp(&memory.blocks[blockIndex].data[k], newRecord) > 0) {
                        // Shift data to make space for the new record
                        memmove(&memory.blocks[blockIndex].data[k + strlen(newRecord) + 1],
                                &memory.blocks[blockIndex].data[k],
                                BLOCK_SIZE - k - strlen(newRecord) - 1);

                        strncpy(&memory.blocks[blockIndex].data[k], newRecord, strlen(newRecord));
                        memory.blocks[blockIndex].data[k + strlen(newRecord)] = '\0';

                        printf("Record '%s' added to file '%s' in block %d at position %d.\n", newRecord, fileName, blockIndex, k);
                        return;
                    }
                }
            }

            printf("Error: No space available to add the record in file '%s'.\n", fileName);
            return;
        }
    }

    printf("Error: File '%s' not found.\n", fileName);
}




// -----------------Function to add a record to an unsorted file------------------------------
void RemplirFichierNonTriee(const char* fileName, const char* newRecord) {
    // Find the file by name
    for (int i = 0; i < fileCount; i++) {
        if (strcmp(files[i].metadata.name, fileName) == 0) {
            // Iterate through blocks in the file to find space
            for (int j = 0; j < files[i].metadata.numBlocks; j++) {
                int blockIndex = files[i].indexTable[j].blockIndex;

                // Scan within the block to find empty space
                for (int k = 0; k < BLOCK_SIZE; k += strlen(&memory.blocks[blockIndex].data[k]) + 1) {
                    if (memory.blocks[blockIndex].data[k] == '\0') {
                        strncpy(&memory.blocks[blockIndex].data[k], newRecord, strlen(newRecord));
                        memory.blocks[blockIndex].data[k + strlen(newRecord)] = '\0';

                        printf("Record '%s' added to file '%s' in block %d at position %d.\n", newRecord, fileName, blockIndex, k);
                        return;
                    }
                }
            }

            printf("Error: No space available to add the record in file '%s'.\n", fileName);
            return;
        }
    }

    printf("Error: File '%s' not found.\n", fileName);
}

