#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "TextColor.h"
#include "Metadonnees.h"
#include "TableIndex.h"
#include "Disk.h"
//Determine si un bloc est contigu ou chaine
bool isDiskContigu(){
    for(int i=0; i<MAX_BLOCKS;i++){
        if(disk[i].chainee.next<0)
            return false;
    }
    return true;
}
//Affichage du disque
void AfficherDisqueContigue(){
    Bloc buffer;
    int j;
    fichier F;
    char filename[30];
    for(int i=0; i<MAX_BLOCKS; i++){
        if(checkBlockContigue(i)==false)
                printf("\nBloc libre.");
        else{
            memcpy(&buffer, &disk[i],sizeof(Bloc));
            j=0;
            while(j<BLOCK_SIZE && strlen(disk[i].contigue.enregistrement[j].Data)!=0)
                j++;
            rechercheFichierMeta(i, &F);
            lireNomFichier(F,filename);
            printf("\nNom du fichier:  %s et nombre d'enregistrements: %d",filename, j);
        }
    }
}
void AfficherDisqueChainee(){
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

// Compactage du Disque avec Buffer (chainee)
void compactDiskChainee() {
    Bloc buffer; // Temporary buffer for moving blocks
    int lastFreeBlock = -1;
    fichier F;

    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (disk[i].chainee.free) {
            if (lastFreeBlock == -1) lastFreeBlock = i;  // Find the first free block
        } else if (lastFreeBlock != -1) {
            chargerFichierMetadonnees(i, &F);  // Load the metadata of the associated file

            // Use the temporary buffer
            memcpy(&buffer, &disk[i], sizeof(Bloc));

            // Move the block to the free space
            memcpy(&disk[lastFreeBlock], &buffer, sizeof(Bloc));

            // Update the file metadata if necessary
            if (F->MDfile != NULL) {
                MajEntetenum(&F, 4, lastFreeBlock); // Update the first address
                chargerMetadonnees(F);             // Reload metadata
            }

            // Free the old block
            disk[i].chainee.free = true;
            disk[i].chainee.next = -1;

            // Find the next free block
            for (int j = lastFreeBlock + 1; j < MAX_BLOCKS; j++) {
                if (disk[j].chainee.free) {
                    lastFreeBlock = j;
                    break;
                }
            }
        }
    }
    printf("Disk compacted in chained mode.\n");
}

// Compactage du Disque avec Buffer (contigue)
void compactDiskContigue() {
    Bloc buffer; // Buffer temporaire pour le déplacement
    int lastFreeBlock = -1;
    fichier F;

    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (disk[i].contigue.free) {
            if (lastFreeBlock == -1) lastFreeBlock = i;  // Trouve le premier bloc libre
        }
        else if (lastFreeBlock != -1) {
            chargerFichierMetadonnees(i, &F); //Si il s'agit de la premiere
            // Utilise le buffer temporaire
            memcpy(&buffer, &disk[i], sizeof(Bloc));

            // Déplace le bloc dans l'espace libre
            memcpy(&disk[lastFreeBlock], &buffer, sizeof(Bloc));
            if(F->MDfile!=NULL){
                MajEntetenum(&F,4,lastFreeBlock);
                chargerMetadonnees(F);
            }
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

    printf("Disque compacte en mode contigu.\n");
}

// 4 Vider la Mémoire Secondaire (chainee)
void clearDiskchainee() {
    fichier F;
    // Clear all blocks by setting them as free
    for (int i = 0; i < MAX_BLOCKS; i++) {
        disk[i].chainee.free = true;
        disk[i].chainee.next = -1;  // Reset the next pointer for chained blocks
        memset(disk[i].chainee.enregistrement, 0, BLOCK_SIZE);  // Clear the block content
        chargerFichierMetadonnees(i,&F);
        if(F.MDfile!=NULL){
            supprimeFichierMetadonnees(&F);
            clearIndexTable(&F);
        }
    }
    printf("Disk cleared.\n");
}


 // Vider la Mémoire Secondaire (contigue)
void clearDiskContigue() {
    fichier F;
    // Clear all blocks by setting them as free
    for (int i = 0; i < MAX_BLOCKS; i++) {
        disk[i].contigue.free = true;
        disk[i].contigue.next = -1;  // Reset the next pointer for chained blocks
        memset(disk[i].contigue.enregistrement, 0, BLOCK_SIZE);  // Clear the block content
        chargerFichierMetadonnees(i,&F);
        if(F.MDfile!=NULL){
            supprimeFichierMetadonnees(&F);
            clearIndexTable(&F);
        }
    }
    printf("Disk cleared.\n");
}

//5Vérification d'un Bloc(chainee)
bool checkBlock(int blockID) { // retourne vrai si il est rempli et faux si il est libre ou n'existe pas
    if (blockID >= 0 && blockID < MAX_BLOCKS) {
        return !disk[blockID].chainee.free;
    }
    return false;
}
bool checkBlockContigue(int blockID) {
    if (blockID >= 0 && blockID < MAX_BLOCKS) {
        return !disk[blockID].contigue.free;
    }
    return false;
}
// Fill a File (Sorted or Unsorted) Using a Buffer (chainee)
void fillFileChainee(int fileID, bool isSorted, fichier *F) {
    const int BUFFER_SIZE = BLOCK_SIZE;
    Enregistrement buffer[BUFFER_SIZE];
    int bufferIndex = 0;
    int recordsFilled = 0;
    int nbBlocs = lireEntete(*F, 2);
    int nbEnregistrements = lireEntete(*F, 3);
    int premiereAdresse = lireEntete(*F, 4);
    if (nbBlocs <= 0 || nbEnregistrements <= 0) {
        printf("Error: Invalid metadata for file %d.\n", fileID);
        return;
    }

    // Fill buffer
    for (int i = 0; i < BUFFER_SIZE && i < nbEnregistrements; i++) {
        buffer[i].ID = fileID * BUFFER_SIZE + i + 1;
        snprintf(buffer[i].Data, sizeof(buffer[i].Data), "Record_%d", buffer[i].ID);
        buffer[i].Supprime = false;
    }

    // Sort the buffer if required
    if (isSorted) {
        for (int i = 0; i < BUFFER_SIZE - 1 && i < nbEnregistrements - 1; i++) {
            for (int j = i + 1; j < BUFFER_SIZE && j < nbEnregistrements ; j++) {
                if (buffer[j].ID < buffer[i].ID) {
                    Enregistrement temp = buffer[i];
                    buffer[i] = buffer[j];
                    buffer[j] = temp;
                }
            }
        }
    }

    // Allocate the first block if premiereAdresse is invalid
    int currentBlockAddress = premiereAdresse < 0 ? AllouerBlocChainee() : premiereAdresse;
    int previousBlockAddress = -1;

    if (currentBlockAddress == -1) {
        printf("Error: No free block available to start filling file %d.\n", fileID);
        return;
    }

    // Allocate and fill the required number of blocks
    for (int i = 0; i < nbBlocs && recordsFilled < nbEnregistrements; i++) {
        if (currentBlockAddress == -1) {
            currentBlockAddress = AllouerBlocChainee();
            if (currentBlockAddress == -1) {
                printf("Error: Unable to allocate block for file %d.\n", fileID);
                break;
            }
        }

        // Mark block as used
        disk[currentBlockAddress].chainee.free = false;

        // Link previous block to the current block
        if (previousBlockAddress != -1) {
            disk[previousBlockAddress].chainee.nextBlock = currentBlockAddress;
        }

        // Fill the current block with records
        for (int j = 0; j < BLOCK_SIZE && recordsFilled < nbEnregistrements; j++) {
            disk[currentBlockAddress].chainee.enregistrement[j] = buffer[bufferIndex++];
            recordsFilled++;
        }

        // Update previous block and allocate next block
        previousBlockAddress = currentBlockAddress;
        currentBlockAddress = AllouerBlocChainee();
    }

    // Mark the end of the file's block chain
    if (previousBlockAddress != -1) {
        disk[previousBlockAddress].chainee.nextBlock = -1;
    }

    // Update metadata with the actual number of records filled
    MajEntetenum(F, 3, recordsFilled); // Update number of records
    MajEntetenum(F, 2, nbBlocs);       // Update number of blocks
    MajEntetenum(F, 4, premiereAdresse < 0 ? previousBlockAddress : premiereAdresse); // Update first block address

    printf("File %d filled with %d records. Sorted: %s\n", fileID, recordsFilled, isSorted ? "Yes" : "No");
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
void ChargerFichierChaine(int fileID, fichier *F) {
    if (fileID < 0 || fileID >= MAX_BLOCKS) {
        printf("Error: Invalid file ID %d.\n", fileID);
        return;
    }

    int nbBlocs = lireEntete(F, 2);  // Get the block count for the file
    int allocatedBlocks = 0;
    int firstBlockAddress = -1;
    int lastBlockAddress = -1;
    int choix,i=0;

    // Step 1: Find free blocks and link them in a chain
    while(allocatedBlocks < nbBlocs){
        if(lastBlockAddress==-1)
            lastBlockAddress= AllouerBlocChainee(0);
        else
            lastBlockAddress = AllouerBlocChainee(lastBlockAddress);
        if(lastBlockAddress!=-1){
            if (firstBlockAddress == -1) {
                firstBlockAddress = i;  // Set the first block
            }
            allocatedBlocks++;
        }
    }

    // Step 2: If not enough blocks, ask user for compaction
    if (allocatedBlocks < nbBlocs) {
        printf("Error: Not enough free blocks for file %d.\n", fileID);
        do {
            printf("Do you want to compact the disk? 1) Yes 2) No\n");
            scanf("%d", &choix);
        } while (choix < 1 || choix > 2);
        if (choix == 1) {
            compactDiskChaine();  // Compact the disk
            DefragmentationChaine(*F);  // Defragment the file
        } else {
            return;
        }
    } else {
        if(disk[firstBlockAddress].chainee.free==true){
            MajEntetenum(*F, 4, firstBlockAddress);  // Update the header with the first block address
            chargerMetadonnees(*F);  // Load file metadata
        }
        printf("File loaded with %d blocks allocated.\n", allocatedBlocks);
    }

    // Step 3: Check if the disk is full
    bool diskFull = true;
    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (disk[i].chaine.free) {
            diskFull = false;
            break;
        }
    }
    if (diskFull) {
        printf("The MS is full.\n");
    }
}

void ChargerFichierContigue(int fileID, fichier *F) {
    if (fileID < 0 || fileID >= MAX_BLOCKS) {
        printf("Error: Invalid file ID %d.\n", fileID);
        return;
    }

    int nbBlocs = lireEntete(F, 2);  // Get the block count for the file
    int allocatedBlocks = 0;
    int firstBlockAddress = -1;
    int lastBlockAddress = -1;
    int choix;


    // Step 1: Find contiguous free blocks
    for (int i = AllouerBlocContigue(); i < MAX_BLOCKS && i <= nbBlocs; i++) {
        if (disk[i].contigue.free) {
            if (firstBlockAddress == -1) firstBlockAddress = i;
            allocatedBlocks++;
            lastBlockAddress = i;
            if (allocatedBlocks == nbBlocs) break;
        }
    }

    // Step 2: If not enough blocks, ask user for compaction
    if (i>=MAX_BLOCKS && allocatedBlocks < nbBlocs) {
        printf("Error: Not enough free blocks for file %d.\n", fileID);
        do {
            printf("Do you want to compact the disk? 1) Yes 2) No\n");
            scanf("%d", &choix);
        } while (choix < 1 || choix > 2);

        if (choix == 1) {
            compactDiskContigue();
            DefragmentationContigue(*F);
        } else {
            return;
        }
    } else {
        MajEntetenum(*F,4,firstBlockAddress);
        chargerMetadonnees(*F);
        // Step 5: Update index table with allocated blocks
        if (liretypeTri(*F)) {
            // Creation of non-dense index table + load into "MS"
        } else {
            // Creation of dense index table + load into "MS"
        }
        printf("File loaded with %d blocks allocated.\n", allocatedBlocks);
    }

    // Step 3: Check if the disk is full
    bool diskFull = true;
    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (disk[i].contigue.free) {
            diskFull = false;
            break;
        }
    }
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
void insertRecordChainee(fichier *F, Enregistrement record, bool isSorted, int fileID) {
    if (fileID < 0 || fileID >= MAX_BLOCKS) {
        printf("Error: Invalid file ID %d.\n", fileID);
        return;
    }

    int blocksUsed = 0;
    int lastBlockAddress = -1;

    if (isSorted) { // Insertion triée
        for (int i = 0; i < MAX_BLOCKS; i++) {
            if (disk[i].chainee.free) continue; // Sauter les blocs libres
            for (int j = 0; j < BLOCK_SIZE; j++) { // Vérifier la position d'insertion
                if (disk[i].chainee.enregistrement[j].ID == 0 || record.ID < disk[i].chainee.enregistrement[j].ID) {
                    // Décaler les enregistrements pour faire de la place
                    for (int k = BLOCK_SIZE - 1; k > j; k--) {
                        disk[i].chainee.enregistrement[k] = disk[i].chainee.enregistrement[k - 1];
                    }
                    disk[i].chainee.enregistrement[j] = record; // Insérer l'enregistrement
                    printf("Record inserted in sorted order in file %d at block %d, position %d.\n", fileID, i, j);
                    // Mettre à jour la table d'index et les métadonnées
                    F->indexTable[i] = fileID;
                    blocksUsed++;
                    lastBlockAddress = i;
                    MajEntetenum(F, 2, blocksUsed); // Mise à jour du nombre de blocs
                    MajEntetenum(F, 3, blocksUsed * BLOCK_SIZE); // Mise à jour du nombre d'enregistrements
                    return;
                }
            }
            // Si le bloc est plein et qu'il n'y a pas de bloc suivant
            if (disk[i].chainee.next == -1) {
                printf("Error: No space left to insert the record in sorted order for file %d.\n", fileID);
                return;
            }
        }
    } else { // Insertion non triée
        for (int i = 0; i < MAX_BLOCKS; i++) {
            if (disk[i].chainee.free) continue; // Sauter les blocs libres
            for (int j = 0; j < BLOCK_SIZE; j++) {
                if (disk[i].chainee.enregistrement[j].ID == 0) { // Trouver un emplacement vide
                    disk[i].chainee.enregistrement[j] = record;
                    printf("Record inserted in unsorted order in file %d at block %d, position %d.\n", fileID, i, j);
                    // Mettre à jour la table d'index et les métadonnées
                    F->indexTable[i] = fileID;
                    blocksUsed++;
                    lastBlockAddress = i;
                    MajEntetenum(F, 2, blocksUsed);
                    MajEntetenum(F, 3, blocksUsed * BLOCK_SIZE);
                    return;
                }
            }
            // Vérifier et ajouter un nouveau bloc si nécessaire
            if (disk[i].chainee.next == -1) {
                for (int j = 0; j < MAX_BLOCKS; j++) {
                    if (disk[j].chainee.free) {
                        disk[j].chainee.free = false;
                        disk[i].chainee.next = j;
                        F->indexTable[j] = fileID;
                        blocksUsed++;
                        lastBlockAddress = j;
                        printf("New block %d allocated and linked to block %d for file %d.\n", j, i, fileID);
                        return;
                    }
                }
                printf("Error: No space left to allocate a new block for file %d.\n", fileID);
                return;
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
