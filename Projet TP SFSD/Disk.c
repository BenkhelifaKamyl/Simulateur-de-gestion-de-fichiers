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
    Bloc buffer;              // Temporary buffer for moving blocks
    int lastFreeBlock = -1;   // Track the last free block
    fichier F;                // Metadata structure for file

    // Iterate through all blocks in the disk
    for (int i = 0; i < MAX_BLOCKS; i++) {
        // Check if the current block is free
        if (disk[i].chainee.free) {
            // If this is the first free block found, record its index
            if (lastFreeBlock == -1) {
                lastFreeBlock = i;
            }
        } else if (lastFreeBlock != -1) {  // A free block exists earlier in the disk
            // Load the metadata of the associated file
            chargerFichierMetadonnees(i, &F);

            // Move the current block to the last free block
            memcpy(&buffer, &disk[i], sizeof(Bloc));
            memcpy(&disk[lastFreeBlock], &buffer, sizeof(Bloc));

            // Update the file metadata if necessary
            if (F->MDfile != NULL) {
                // Update the first block reference in metadata
                if (lireEntete(F, 4) == i) {
                    MajEntetenum(&F, 4, lastFreeBlock);
                }

                // Update references to the moved block in the chain
                for (int j = 0; j < MAX_BLOCKS; j++) {
                    if (disk[j].chainee.next == i) {
                        disk[j].chainee.next = lastFreeBlock;
                        break;
                    }
                }

                // Reload metadata after updates
                chargerMetadonnees(F);
            }

            // Mark the old block as free and reset its pointers
            disk[i].chainee.free = true;
            disk[i].chainee.next = -1;

            // Find the next free block starting from the current free block index
            for (int j = lastFreeBlock + 1; j < MAX_BLOCKS; j++) {
                if (disk[j].chainee.free) {
                    lastFreeBlock = j;
                    break;
                }
            }
        }
    }

    // Print completion message
    printf("Disk compacted in chained mode.\n");
}

// Compactage du Disque avec Buffer (contigue)
void compactDiskContigue() {
    Bloc buffer;              // Buffer temporaire pour le déplacement
    int lastFreeBlock = -1;   // Suivi du dernier bloc libre
    fichier F;                // Structure pour les métadonnées du fichier

    // Parcourir tous les blocs du disque
    for (int i = 0; i < MAX_BLOCKS; i++) {
        // Vérifier si le bloc actuel est libre
        if (disk[i].contigue.free) {
            if (lastFreeBlock == -1) {
                lastFreeBlock = i;  // Enregistrer le premier bloc libre
            }
        } else if (lastFreeBlock != -1) { // Un espace libre existe avant ce bloc
            // Charger les métadonnées associées au fichier
            chargerFichierMetadonnees(i, &F);

            // Déplacer le bloc courant dans le premier espace libre
            memcpy(&buffer, &disk[i], sizeof(Bloc));
            memcpy(&disk[lastFreeBlock], &buffer, sizeof(Bloc));

            // Mettre à jour les métadonnées
            if (F->MDfile != NULL) {
                // Vérifier si le bloc déplacé est le premier bloc du fichier
                if (lireEntete(F, 4) == i) {
                    MajEntetenum(&F, 4, lastFreeBlock);  // Mise à jour de l'adresse du premier bloc
                }

                // Synchroniser les métadonnées après mise à jour
                chargerMetadonnees(F);
            }

            // Marquer l'ancien bloc comme libre
            disk[i].contigue.free = true;

            // Trouver le prochain bloc libre à partir de la position actuelle
            for (int j = lastFreeBlock + 1; j < MAX_BLOCKS; j++) {
                if (disk[j].contigue.free) {
                    lastFreeBlock = j;
                    break;
                }
            }
        }
    }

    // Afficher un message une fois la compaction terminée
    printf("Disque compacté en mode contigu.\n");
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

    //Creation table Index
    Index tableIndex[];
    if(liretypeTri(F)==true){ //Cas trie
        creationTableIndexNonDenseChainee(*F,tableIndex);
        sauvegardeTableIndex(F,tableIndex);
    }
    else{
        creationTableIndexDenseChainee(*F,tableIndex);
        sauvegardeTableIndex(F,tableIndex);
    }

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

    //Creation table Index
    Index tableIndex[];
    if(liretypeTri(F)==true){ //Cas trie
        creationTableIndexNonDenseContigue(*F,tableIndex);
        sauvegardeTableIndex(F,tableIndex);
    }
    else{
        creationTableIndexDenseContigue(*F,tableIndex);
        sauvegardeTableIndex(F,tableIndex);
    }
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
        }
    }
    else {
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

    int nbBlocs = lireEntete(F, 2);  // Get the number of blocks required for the file
    int startBlock = -1;  // Address of the first contiguous block
    int choix;

    // Step 1: Search for a contiguous free space
    for (int i = 0; i < MAX_BLOCKS; i++) {
        int count = 0;
        // Check if there is a sequence of free blocks
        while (i + count < MAX_BLOCKS && disk[i + count].free && count < nbBlocs) {
            count++;
        }
        // If a sufficient contiguous sequence is found
        if (count == nbBlocs) {
            startBlock = i;
            break;
        }
    }

    // Step 2: If no contiguous space is found, ask the user for disk compaction
    if (startBlock == -1) {
        printf("Error: Not enough contiguous blocks for file %d.\n", fileID);
        do {
            printf("Do you want to compact the disk? 1) Yes 2) No\n");
            scanf("%d", &choix);
        } while (choix < 1 || choix > 2);
        if (choix == 1) {
            compactDiskContigue();  // Compact the disk to free up contiguous space
            ChargerFichierContigue(fileID, F);  // Retry after compaction
        }
    }
    else{
       // Update the file's metadata
        MajEntetenum(*F, 4, startBlock);  // Update with the address of the first block
        chargerMetadonnees(*F);  // Load the file's metadata
        printf("File loaded with %d contiguous blocks starting from block %d.\n", nbBlocs, startBlock);
    }
    // Step 4: Check if the disk is full
    bool diskFull = true;
    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (disk[i].free) {
            diskFull = false;
            break;
        }
    }
    if (diskFull) {
        printf("The storage system is full.\n");
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
        insertRecordchainee(F,record);
    else
        insertRecordcontigue(F,record);
}

// 8. Insert a Record (Sorted and Unsorted)(chainee)
void insertRecordChainee(fichier *F, Enregistrement record, bool estTrie) {
    int nbBlocks = lireEntete(*F, 2);
    int nbEnregistrements = lireEntete(*F, 3);
    int tailleBloc = sizeof(disk[0].chainee.enregistrement) / sizeof(Enregistrement);
    int numBloc = -1, deplacement = -1;

    if (estTrie) {
        // Cas trié : rechercher la position d'insertion
        rechercheEnregistrementNonDense(F, record.ID, &numBloc, &deplacement);
    } else {
        // Cas non trié : rechercher un espace libre
        rechercheEnregistrementDense(F, record.ID, &numBloc, &deplacement);
    }

    // Vérifier si un espace est disponible
    if (numBloc == -1) {
        printf("\nEspace insuffisant pour l'insertion.\n");
        return;
    }

    // Décalage pour faire de la place à l'enregistrement
    while (deplacement >= tailleBloc) {
        // Si le bloc est plein, passer au suivant
        if (disk[numBloc].chainee.next == -1) {
            // Créer un nouveau bloc si nécessaire
            if (nbBlocks >= MAX_BLOCKS) {
                printf("\nEspace insuffisant pour ajouter un nouveau bloc.\n");
                return;
            }
            int newBlock = nbBlocks++;
            disk[newBlock].chainee.free = false;
            disk[newBlock].chainee.next = -1; // Fin de la chaîne
            disk[numBloc].chainee.next = newBlock; // Lier au précédent
            numBloc = newBlock;
            deplacement = 0; // Commencer au début du nouveau bloc
        } else {
            numBloc = disk[numBloc].chainee.next;
            deplacement -= tailleBloc; // Continuer au prochain bloc
        }
    }

    // Décaler les enregistrements existants pour libérer l'espace
    int currentBlock = numBloc, currentPosition = deplacement;
    while (currentBlock != -1) {
        for (int i = tailleBloc - 1; i > currentPosition; i--) {
            disk[currentBlock].chainee.enregistrement[i] = disk[currentBlock].chainee.enregistrement[i - 1];
        }

        // Passer au bloc suivant pour continuer le décalage, si nécessaire
        if (disk[currentBlock].chainee.next != -1) {
            disk[disk[currentBlock].chainee.next].chainee.enregistrement[0] = disk[currentBlock].chainee.enregistrement[tailleBloc - 1];
        }
        currentPosition = 0;
        currentBlock = disk[currentBlock].chainee.next;
    }

    // Insérer l'enregistrement dans la position libérée
    disk[numBloc].chainee.enregistrement[deplacement] = record;

    // Mise à jour des métadonnées
    nbEnregistrements++;
    MajEntetenum(F, 2, nbBlocks);
    MajEntetenum(F, 3, nbEnregistrements);

    // Mettre à jour la table d'index
    updateIndexTable(F, record);

    chargerMetadonnees(*F);
    printf("\nEnregistrement inséré avec succès.\n");
}

// 8. Insert a Record (Sorted and Unsorted)(contiguous)

void insertRecordContigu(fichier *F, Enregistrement record, bool estTrie) {
    int nbBlocks = lireEntete(*F, 2);
    int nbEnregistrements = lireEntete(*F, 3);
    int tailleBloc = sizeof(disk[0].contigu.enregistrement) / sizeof(Enregistrement);
    int dernierBloc = nbBlocks - 1;

    if (nbBlocks >= MAX_BLOCKS && (nbEnregistrements % tailleBloc == 0)) {
        printf("\nEspace insuffisant pour l'insertion.");
        return;
    }

    if (estTrie) {
        // Recherche de la position d'insertion pour le cas trié
        int numBloc = -1, deplacement = -1;
        rechercheEnregistrementNonDense(F, record.clé, &numBloc, &deplacement);

        if (numBloc == -1) {
            printf("\nEspace insuffisant pour l'insertion (trié).");
            return;
        }

        // Décalage pour faire de la place
        for (int i = nbEnregistrements; i > (numBloc * tailleBloc + deplacement); i--) {
            int srcBloc = (i - 1) / tailleBloc;
            int srcPos = (i - 1) % tailleBloc;
            int destBloc = i / tailleBloc;
            int destPos = i % tailleBloc;

            if (destBloc >= MAX_BLOCKS) {
                printf("\nEspace insuffisant pendant le décalage.");
                return;
            }

            // Ajouter un nouveau bloc si nécessaire
            if (destBloc >= nbBlocks) {
                nbBlocks++;
                memset(&disk[destBloc], 0, sizeof(Bloc));
            }

            disk[destBloc].contigu.enregistrement[destPos] = disk[srcBloc].contigu.enregistrement[srcPos];
        }

        // Insérer le nouvel enregistrement
        disk[numBloc].contigu.enregistrement[deplacement] = record;

    } else {
        // Cas non trié (dense)
        int numBloc = -1, deplacement = -1;
        rechercheEnregistrementDense(F, record.clé, &numBloc, &deplacement);

        if (numBloc == -1) {
            printf("\nEspace insuffisant pour l'insertion (non trié).");
            return;
        }

        if (deplacement < tailleBloc) {
            // Insérer dans l'espace libre du bloc courant
            disk[numBloc].contigu.enregistrement[deplacement] = record;
        } else {
            // Ajouter un nouveau bloc si nécessaire
            dernierBloc++;
            nbBlocks++;
            memset(&disk[dernierBloc], 0, sizeof(Bloc));
            disk[dernierBloc].contigu.enregistrement[0] = record;
        }
    }

    // Mise à jour des métadonnées
    nbEnregistrements++;
    MajEntetenum(F, 2, nbBlocks);
    MajEntetenum(F, 3, nbEnregistrements);

    // Mise à jour de la table d'index
    updateIndexTable(F, record);

    // Charger les métadonnées mises à jour
    chargerMetadonnees(*F);

    printf("\nEnregistrement inséré avec succès.");
}

// 9. Logical Deletion of a Record(chainee)
void deleteRecordLogicalchainee(int fileID, int recordID) {
    if (fileID < 0 || fileID >= MAX_BLOCKS) { // Check if the file ID is valid
        printf("Error: Invalid file ID %d.\n", fileID);
        return;
    }
    int currentBlockID = fileID; // Assume fileID is the starting block ID
    while (currentBlockID != -1) { // Traverse linked blocks
        if (!disk[currentBlockID].chainee.free) { // Check if the block is in use
            for (int j = 0; j < BLOCK_SIZE; j++) { // Traverse records in the block
                if (disk[currentBlockID].chainee.enregistrement[j].ID == recordID) {
                    if (disk[currentBlockID].chainee.enregistrement[j].Supprime) {
                        printf("Record %d in file %d is already logically deleted.\n", recordID, fileID);
                        return;
                    }
                    disk[currentBlockID].chainee.enregistrement[j].Supprime = true; // Mark as deleted
                    printf("Record %d in file %d marked as logically deleted.\n", recordID, fileID);
                    return;
                }
            }
        }
        currentBlockID = disk[currentBlockID].chainee.next; // Move to the next block
    }

    printf("Error: Record %d not found in file %d.\n", recordID, fileID);
}



// 9. Logical Deletion of a Record(contiguous)
void deleteRecordLogicalcontigue(int fileID, int recordID) {
    if (fileID < 0 || fileID >= MAX_FILES) { // Validate file ID
        printf("Error: Invalid file ID %d.\n", fileID);
        return;
    }

    fichier F;
    rechercheFichierMeta(fileID, &F); // Load file metadata based on fileID

    int startBlock = lireEntete(F, 4); // Get the starting block of the file
    int recordCount = lireEntete(F, 3); // Get the total number of records in the file

    if (startBlock == -1) { // Check if the file is initialized
        printf("Error: File %d not initialized.\n", fileID);
        return;
    }

    // Traverse the records in the contiguous blocks
    for (int i = 0; i < recordCount; i++) {
        int blockIndex = startBlock + (i / BLOCK_SIZE);
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
    if (fileID < 0 || fileID >= MAX_BLOCKS) { // Validate file ID
        printf("Error: Invalid file ID %d.\n", fileID);
        return;
    }

    fichier F;
    rechercheFichierMeta(fileID, &F); // Load metadata for the file
    MetaDonnee meta;
    rewind(F.MDfile);
    fread(&meta, sizeof(MetaDonnee), 1, F.MDfile);

    int currentBlockID = meta.premiereAdresse; // Get the starting block address

    if (currentBlockID == -1) { // Check if the file is initialized
        printf("Error: File %d not initialized.\n", fileID);
        return;
    }

    while (currentBlockID != -1) { // Traverse linked blocks
        if (!disk[currentBlockID].chainee.free) { // Check if the block is in use
            for (int j = 0; j < BLOCK_SIZE; j++) { // Traverse records in the block
                if (disk[currentBlockID].chainee.enregistrement[j].ID == recordID) { // If the record matches the given ID
                    // Physically delete the record by resetting its fields
                    disk[currentBlockID].chainee.enregistrement[j].ID = 0; // Reset record ID to indicate it's removed
                    disk[currentBlockID].chainee.enregistrement[j].Supprime = false; // Reset "deleted" flag
                    memset(disk[currentBlockID].chainee.enregistrement[j].Data, 0, sizeof(disk[currentBlockID].chainee.enregistrement[j].Data)); // Clear data
                    printf("Record %d in file %d physically deleted.\n", recordID, fileID);

                    // Update metadata
                    meta.nbEnregistrements--; // Decrement the record count
                    rewind(F.MDfile);
                    fwrite(&meta, sizeof(MetaDonnee), 1, F.MDfile);

                    return; // Exit after deletion
                }
            }
        }
        currentBlockID = disk[currentBlockID].chainee.next; // Move to the next block
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
    int firstAdress = lireEntete(*F,4);
    int currentBlockID = firstAdress;  // Counter for free blocks

    // Traverse and free all blocks of the file using the linked list
    while (currentBlockID!=-1) {
        int nextBlockID = disk[currentBlockID].chainee.next;  // Store next block ID

        // Clear current block
        initializeBlockChainee(currentBlockID); //Reinitialise le bloc

        currentBlockID = nextBlockID;  // Move to the next block
    }

    // Remove file from the index table
    removeFromIndexTable(*F); // SUpprime la table d'index

    supprimeFichierMetadonnees(F); //Supprime le fichier de metadonnees

    printf("File %d deleted successfully.\n", fileID);
}

// Delete a file with contiguous allocation
void deleteFileContigue(fichier *F) {
    int firstAdress = lireEntete(*F,4);
    int nbBlocs = lireEntete(*F,2);

    // Traverse and free all blocks of the file using the linked list
    for(int i = firstAdress; i<nbBlocs+firstAdress; i++) {
        int nextBlockID = disk[currentBlockID].chainee.next;  // Store next block ID

        // Clear current block
        initializeBlockContigue(currentBlockID); //Reinitialise le bloc

        currentBlockID = nextBlockID;  // Move to the next block
    }

    // Remove file from the index table
    removeFromIndexTable(*F); // SUpprime la table d'index

    supprimeFichierMetadonnees(F); //Supprime le fichier de metadonnees

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
