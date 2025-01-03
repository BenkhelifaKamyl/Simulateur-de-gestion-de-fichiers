#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "TextColor.h"
#include "Metadonnees.h"
#include "TableIndex.h"
#include "Disk.h"

bool isDiskContigu(){
    if(currentMode==1){
        return false;
    }
    return true;
}
void AfficherDisqueContigue(){
    int i,j,k,nbBlocs,nbEnregistrements;
    fichier F;
    char filename[30];
    for(i=0; i<MAX_BLOCKS; i++){
        if(checkBlockContigue(i)==false)
                printf("\nBloc libre.");
        else{
            rechercheFichierMeta(i, &F); //Recuperer les metadonnees du bloc
            nbBlocs=lireEntete(F,2);
            nbEnregistrements=lireEntete(F,3);
            lireNomFichier(F,filename);
            k=0;
            while(i<MAX_BLOCKS && k<nbBlocs){
                if(k==nbBlocs-1){
                    j=nbEnregistrements%BLOCK_SIZE;
                    if(j==0){
                        j=BLOCK_SIZE;
                    }
                }
                else{
                    j=BLOCK_SIZE;
                }
                 printf("\nNom du fichier:  %s et nombre d'enregistrements: %d",filename, j);
                k++;
            }
            i+= nbBlocs+1;
            fclose(F.MDfile);
        }
    }
}
void AfficherDisqueChainee(){
    int i,j,k,nbBlocs,nbEnregistrements;
    fichier F;
    char filename[30];
    for(i=0; i<MAX_BLOCKS; i++){
        if(checkBlock(i)==false)
                printf("\nBloc libre.");
        else{
            rechercheFichierMeta(i, &F); //Recuperer les metadonnees du bloc
            nbBlocs=lireEntete(F,2);
            nbEnregistrements=lireEntete(F,3);
            lireNomFichier(F,filename);
            k=0;
            while(i<MAX_BLOCKS && k<nbBlocs){
                if(k==nbBlocs-1){
                    j=nbEnregistrements%BLOCK_SIZE;
                    if(j==0){
                        j=BLOCK_SIZE;
                    }
                }
                else{
                    j=BLOCK_SIZE;
                }
                 printf("\nNom du fichier:  %s et nombre d'enregistrements: %d",filename, j);
                k++;
            }
            i+= nbBlocs+1;
            fclose(F.MDfile);
        }
    }
}

// Initialisation d'un bloc pour l'organisation chaînée
void initializeBlockChainee(int i) {
    disk[i].chainee.free = true;
    disk[i].chainee.next = -1;
   memset(disk[i].chainee.enregistrement, 0, BLOCK_SIZE * sizeof(disk[i].chainee.enregistrement[0])); //boucle for et c tout!!!
}

// Initialisation d'un bloc pour l'organisation contiguë
void initializeBlockContigue(int i) {
    disk[i].contigue.free = true;
    memset(disk[i].contigue.enregistrement, 0, BLOCK_SIZE * sizeof(disk[i].contigue.enregistrement[0]));
}

// Initialisation du disque en mode chaîné
void initializeDiskChainee() {
    currentMode = MODE_CHAINE;
    if (MAX_BLOCKS <= 0 || BLOCK_SIZE <= 0) {
        printf("Erreur : configuration de disque invalide.\n");
        return;
    }
    for (int i = 0; i < MAX_BLOCKS; i++) {
        initializeBlockChainee(i);
    }
    printf("Disque initialise avec %d blocs en mode chainé.\n", MAX_BLOCKS);
}

// Initialisation du disque en mode contiguë
void initializeDiskContigue() {
    currentMode = MODE_CONTIGUE;
    if (MAX_BLOCKS <= 0 || BLOCK_SIZE <= 0) {
        printf("Erreur : configuration de disque invalide.\n");
        return;
    }
    for (int i = 0; i < MAX_BLOCKS; i++) {
        initializeBlockContigue(i);
    }
    printf("Disque initialise avec %d blocs en mode contigue.\n", MAX_BLOCKS);
}

void compactDiskChainee() {
    Bloc buffer; // Temporary buffer for moving blocks
    int lastFreeBlock = -1; // Track the last free block
    fichier F;

    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (disk[i].chainee.free) {
            if (lastFreeBlock == -1) {
                lastFreeBlock = i; // Find the first free block
            }
        } else if (lastFreeBlock != -1) {
            // Load the metadata of the associated file
            chargerFichierMetadonnees(i, &F);

            // Use the temporary buffer to store the current block
            memcpy(&buffer, &disk[i], sizeof(Bloc));

            // Move the block to the last free space
            memcpy(&disk[lastFreeBlock], &buffer, sizeof(Bloc));

            // Update the file metadata and pointers
            if (F.MDfile != NULL) {
                // Update the block's metadata if it's the first block
                if (lireEntete(F, 4) == i) {
                    MajEntetenum(&F, 4, lastFreeBlock); // Update the first address
                }
                // Update any references to the moved block in the chain
                for (int j = 0; j < MAX_BLOCKS; j++) {
                    if (disk[j].chainee.next == i) {
                        disk[j].chainee.next = lastFreeBlock;
                        break;
                    }
                }
                chargerMetadonnees(F); // Reload metadata
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
            if(F.MDfile!=NULL){
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
        chargerFichierMetadonnees(i,&F);
        initializeBlockChainee(i);
        if(F.MDfile!=NULL){
            supprimeFichierMetadonnees(&F);
            removeIndexTable(&F);
        }
    }
    printf("Disk cleared.\n");
}

 // Vider la Mémoire Secondaire (contigue)
void clearDiskContigue() {
    fichier F;
    // Clear all blocks by setting them as free
    for (int i = 0; i < MAX_BLOCKS; i++) {
        chargerFichierMetadonnees(i,&F);
        initializeBlockContigue(i);
        if(F.MDfile!=NULL){
            supprimeFichierMetadonnees(&F);
            removeIndexTable(&F);
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

//6. Fill a File (Sorted or Unsorted) Using a Buffer (Chainee)
void fillFileChainee(bool isSorted, fichier *F) {
    const int BUFFER_SIZE = BLOCK_SIZE;
    Enregistrement buffer[BUFFER_SIZE];
    int bufferIndex = 0;
    int recordsFilled = 0;

    // Retrieve metadata
    int nbBlocs = lireEntete(*F, 2); // Number of blocks
    int nbEnregistrements = lireEntete(*F, 3); // Number of records
    int premiereAdresse = lireEntete(*F, 4); // First block address

    if (nbBlocs <= 0 || nbEnregistrements <= 0) {
        printf("Error: Invalid metadata.\n");
        return;
    }

    // Fill buffer with records
    for (int i = 0; i < BUFFER_SIZE && i < nbEnregistrements; i++) {
        buffer[i].ID = i + 1; // Assign IDs sequentially
        snprintf(buffer[i].Data, sizeof(buffer[i].Data), "Record_%d", buffer[i].ID);
        buffer[i].Supprime = false;
    }

    // Sort the buffer if required
    if (isSorted) {
        for (int i = 0; i < BUFFER_SIZE - 1 && i < nbEnregistrements - 1; i++) {
            for (int j = i + 1; j < BUFFER_SIZE && j < nbEnregistrements; j++) {
                if (buffer[j].ID < buffer[i].ID) {
                    Enregistrement temp = buffer[i];
                    buffer[i] = buffer[j];
                    buffer[j] = temp;
                }
            }
        }
    }

    // Allocate the first block if premiereAdresse is invalid
    int currentBlockAddress = premiereAdresse < 0 ? AllouerBlocChainee(0) : premiereAdresse;
    int previousBlockAddress = -1;

    if (currentBlockAddress == -1) {
        printf("Error: No free block available to start filling file.\n");
        return;
    }

    // Allocate and fill the required number of blocks
    for (int i = 0; i < nbBlocs && recordsFilled < nbEnregistrements; i++) {
        if (currentBlockAddress == -1) {
            currentBlockAddress = AllouerBlocChainee(0);
            if (currentBlockAddress == -1) {
                printf("Error: Unable to allocate block.\n");
                break;
            }
        }

        // Mark block as used
        disk[currentBlockAddress].chainee.free = false;

        // Link previous block to the current block
        if (previousBlockAddress != -1) {
            disk[previousBlockAddress].chainee.next = currentBlockAddress;
        }

        // Fill the current block with records
        for (int j = 0; j < BLOCK_SIZE && recordsFilled < nbEnregistrements; j++) {
            disk[currentBlockAddress].chainee.enregistrement[j] = buffer[bufferIndex++];
            recordsFilled++;
        }

        // Update previous block and allocate the next block
        previousBlockAddress = currentBlockAddress;
        currentBlockAddress = AllouerBlocChainee(0);
    }

    // Mark the end of the file's block chain
    if (previousBlockAddress != -1) {
        disk[previousBlockAddress].chainee.next = -1;
    }

    // Create and save the index table
    Index tableIndex[100];
    if (liretypeTri(*F)) { // Sorted case
        creationTableIndexNonDenseChainee(*F, tableIndex);
    } else { // Unsorted case
        creationTableIndexDenseChainee(*F, tableIndex);
    }
    sauvegardeTableIndex(F, tableIndex);
    printf("File filled with %d records. Sorted: %s\n", recordsFilled, isSorted ? "Yes" : "No");
}

//6. Fill a File (Sorted or Unsorted) Using a Buffer (Contigue)
void fillFileContigue(bool isSorted, fichier *F) {
    const int BUFFER_SIZE = BLOCK_SIZE;
    Enregistrement buffer[BUFFER_SIZE]; // Declare buffer
    int recordsFilled = 0;
    int blocksUsed = 0;
    int firstBlockAddress = -1;
    int lastBlockAddress = -1;
    int bufferIndex = 0;

    // Retrieve metadata
    int nbBlocs = lireEntete(*F, 2); // Number of blocks
    int nbEnregistrements = lireEntete(*F, 3); // Number of records
    int premiereAdresse = lireEntete(*F, 4); // First block address

    if (nbBlocs <= 0 || nbEnregistrements <= 0) {
        printf("Error: Invalid metadata.\n");
        return;
    }

    // Fill buffer with records
    for (int i = 0; i < BUFFER_SIZE && i < nbEnregistrements; i++) {
        buffer[i].ID = i + 1; // Assign IDs sequentially
        snprintf(buffer[i].Data, sizeof(buffer[i].Data), "Record_%d", buffer[i].ID);
        buffer[i].Supprime = false;
    }

    // Sort the buffer if required
    if (isSorted) {
        for (int i = 0; i < BUFFER_SIZE - 1 && i < nbEnregistrements - 1; i++) {
            for (int j = i + 1; j < BUFFER_SIZE && j < nbEnregistrements; j++) {
                if (buffer[j].ID < buffer[i].ID) {
                    Enregistrement temp = buffer[i];
                    buffer[i] = buffer[j];
                    buffer[j] = temp;
                }
            }
        }
    }

    // Insert records from buffer into the disk in contiguous blocks
    for (int i = 0; i < MAX_BLOCKS && bufferIndex < nbEnregistrements; i++) {
        if (disk[i].contigue.free) {
            disk[i].contigue.free = false; // Mark the block as occupied
            blocksUsed++;

            // Update the first block address if not set
            if (firstBlockAddress == -1)
                firstBlockAddress = i;

            // Copy buffer records to the current disk block
            for (int j = 0; j < BLOCK_SIZE && bufferIndex < nbEnregistrements; j++) {
                disk[i].contigue.enregistrement[j] = buffer[bufferIndex++];
                recordsFilled++;
            }

            lastBlockAddress = i; // Update last block address
        }
    }
    // Create and save the index table
    Index tableIndex[200];
    if (liretypeTri(*F)) { // Sorted case
        creationTableIndexNonDenseContigue(*F, tableIndex);
    } else { // Unsorted case
        creationTableIndexDenseContigue(*F, tableIndex);
    }
    sauvegardeTableIndex(F, tableIndex);
    printf("File filled in contiguous mode with %d records. Sorted: %s\n", recordsFilled, isSorted ? "Yes" : "No");
}

// Function: ChargerFichierChainee
void ChargerFichierChainee(fichier *F) {
    int nbBlocs = lireEntete(*F, 2);  // Get the number of blocks required for the file
    int allocatedBlocks = 0;
    int firstBlockAddress = -1;
    int lastBlockAddress = -1;
    int choix;

    // Step 1: Allocate free blocks and link them in a chain
    while (allocatedBlocks < nbBlocs) {
        if (lastBlockAddress == -1) {
            lastBlockAddress = AllouerBlocChainee(0);  // Get the first free block
        } else {
            int nextBlock = AllouerBlocChainee(lastBlockAddress);  // Allocate the next block
            if (nextBlock != -1) {
                disk[lastBlockAddress].chainee.next = nextBlock;  // Link the blocks
                lastBlockAddress = nextBlock;
            }
        }
        if (lastBlockAddress != -1) {
            if (firstBlockAddress == -1) {
                firstBlockAddress = lastBlockAddress;  // Set the first block
            }
            allocatedBlocks++;
        } else {
            break;  // Stop if no more blocks are available
        }
    }

    // Step 2: Handle insufficient blocks
    if (allocatedBlocks < nbBlocs) {
        printf("Error: Not enough free blocks for the file.\n");
        do {
            printf("Do you want to compact the disk? 1) Yes 2) No\n");
            scanf("%d", &choix);
        } while (choix < 1 || choix > 2);

        if (choix == 1) {
            compactDiskChainee();            // Compact the disk
            Defragmentationchainee(F);      // Defragment the file
            ChargerFichierChainee(F);       // Retry loading the file
        }
    } else {
        if (disk[firstBlockAddress].chainee.free == true) {
            MajEntetenum(F, 4, firstBlockAddress);  // Update the header with the first block address
            chargerMetadonnees(*F);                // Load file metadata
        }
        printf("File loaded with %d blocks allocated.\n", allocatedBlocks);
    }

    // Step 3: Check if the disk is full
    bool diskFull = true;
    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (disk[i].chainee.free) {
            diskFull = false;
            break;
        }
    }
    if (diskFull) {
        printf("The disk is full.\n");
    }
}

// Function: ChargerFichierContigue
void ChargerFichierContigue(fichier *F) {
    int nbBlocs = lireEntete(*F, 2);  // Get the number of blocks required for the file
    int startBlock = -1;  // Address of the first contiguous block
    int choix;

    // Step 1: Search for a contiguous free space
    for (int i = 0; i < MAX_BLOCKS; i++) {
        int count = 0;
        // Check if there is a sequence of free blocks
        while (i + count < MAX_BLOCKS && disk[i + count].contigue.free && count < nbBlocs) {
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
        printf("Error: Not enough contiguous blocks for the file.\n");
        do {
            printf("Do you want to compact the disk? 1) Yes 2) No\n");
            scanf("%d", &choix);
        } while (choix < 1 || choix > 2);

        if (choix == 1) {
            compactDiskContigue();  // Compact the disk to free up contiguous space
            ChargerFichierContigue(F);  // Retry after compaction
        }
    } else {
        // Mark the blocks as allocated
        for (int i = startBlock; i < startBlock + nbBlocs; i++) {
            disk[i].contigue.free = false;
        }
        // Update the file's metadata
        MajEntetenum(F, 4, startBlock);  // Update with the address of the first block
        MajEntetenum(F, 2, nbBlocs);    // Update the number of blocks
        chargerMetadonnees(*F);         // Load the file's metadata
        printf("\nFile loaded with %d contiguous blocks starting from block %d.\n", nbBlocs, startBlock);
    }

    // Step 3: Check if the disk is full
    bool diskFull = true;
    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (disk[i].contigue.free) {
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
    scanf("%d",&E.ID);
    return E;
}
void insertRecord(fichier *F, Enregistrement record){
    if(lireEnteteGlobal(*F)==Chainee)
        insertRecordChainee(F,record,liretypeTri(*F));
    else
        insertRecordContigue(F,record, liretypeTri(*F));
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
            memcpy(&disk[disk[currentBlock].chainee.next].chainee.enregistrement[0],&disk[currentBlock].chainee.enregistrement[tailleBloc - 1], sizeof(Enregistrement));
        }
        currentPosition = 0;
        currentBlock = disk[currentBlock].chainee.next;
    }

    // Insérer l'enregistrement dans la position libérée
    memcpy(&disk[numBloc].chainee.enregistrement[deplacement],&record, sizeof(Enregistrement));

    // Mise à jour des métadonnées
    nbEnregistrements++;
    MajEntetenum(F, 2, nbBlocks);
    MajEntetenum(F, 3, nbEnregistrements);

    // Mettre à jour la table d'index
    if(liretypeTri(*F))
        MajTableIndexNonDense(F,2,record.ID,numBloc);
    else
        MajTableIndexDense(F,2,record.ID,numBloc);

    chargerMetadonnees(*F);
    printf("\nEnregistrement inséré avec succès.\n");
}

// 8. Insert a Record (Sorted and Unsorted)(contiguous)
void insertRecordContigue(fichier *F, Enregistrement record, bool estTrie) {
    int nbBlocks = lireEntete(*F, 2);
    int nbEnregistrements = lireEntete(*F, 3);
    int tailleBloc = sizeof(disk[0].contigue.enregistrement) / sizeof(Enregistrement);
    int dernierBloc = nbBlocks - 1;
    int numBloc=-1;

    if (nbBlocks >= MAX_BLOCKS && (nbEnregistrements % tailleBloc == 0)) {
        printf("\nEspace insuffisant pour l'insertion.");
        return;
    }

    if (estTrie) {
        // Recherche de la position d'insertion pour le cas trié
        int deplacement = -1;
        rechercheEnregistrementNonDense(F, record.ID, &numBloc, &deplacement);

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
            memcpy(&disk[destBloc].contigue.enregistrement[destPos], &disk[srcBloc].contigue.enregistrement[srcPos], sizeof(Enregistrement));
        }

        // Insérer le nouvel enregistrement
        memcpy(&disk[numBloc].contigue.enregistrement[deplacement], &record,sizeof(Enregistrement));

    } else {
        // Cas non trié (dense)
        int deplacement = -1;
        rechercheEnregistrementDense(F, record.ID, &numBloc, &deplacement);

        if (numBloc == -1) {
            printf("\nEspace insuffisant pour l'insertion (non trié).");
            return;
        }

        if (deplacement < tailleBloc) {
            // Insérer dans l'espace libre du bloc courant
            memcpy(&disk[numBloc].contigue.enregistrement[deplacement], &record,sizeof(Enregistrement));
        } else {
            // Ajouter un nouveau bloc si nécessaire
            dernierBloc++;
            nbBlocks++;
            memset(&disk[dernierBloc], 0, sizeof(Bloc));
            memcpy(&disk[numBloc].contigue.enregistrement[deplacement], &record,sizeof(Enregistrement));
        }
    }

    // Mise à jour des métadonnées
    nbEnregistrements++;
    MajEntetenum(F, 2, nbBlocks);
    MajEntetenum(F, 3, nbEnregistrements);

    // Mise à jour de la table d'index
    if(liretypeTri(*F))
        MajTableIndexNonDense(F,2,record.ID,numBloc);
    else
        MajTableIndexDense(F,2,record.ID,numBloc);

    // Charger les métadonnées mises à jour
    chargerMetadonnees(*F);

    printf("\nEnregistrement inséré avec succès.");
}

// 9. Logical Deletion of a Record(chainee)
void deleteRecordLogicalchainee(fichier *F, int recordID) {
    int currentBlockID = lireEntete(*F, 4); // Get the starting block of the file
    if (currentBlockID == -1) { // Check if the file is initialized
        printf("Error: File not initialized.\n");
        return;
    }

    bool isSorted = liretypeTri(*F); // Determine if the file is sorted
    while (currentBlockID != -1) { // Traverse linked blocks
        if (!disk[currentBlockID].chainee.free) { // Check if the block is in use
            for (int j = 0; j < BLOCK_SIZE; j++) { // Traverse records in the block
                if (disk[currentBlockID].chainee.enregistrement[j].ID == recordID) {
                    if (disk[currentBlockID].chainee.enregistrement[j].Supprime) {
                        printf("Record %d is already logically deleted.\n", recordID);
                        return;
                    }
                    disk[currentBlockID].chainee.enregistrement[j].Supprime = true; // Mark as deleted
                    printf("Record %d marked as logically deleted.\n", recordID);
                    return;
                }
                if (isSorted && disk[currentBlockID].chainee.enregistrement[j].ID > recordID) {
                    // Exit early if sorted and passed potential record location
                    printf("Error: Record %d not found in sorted file.\n", recordID);
                    return;
                }
            }
        }
        currentBlockID = disk[currentBlockID].chainee.next; // Move to the next block
    }

    printf("Error: Record %d not found.\n", recordID);
}

// 9. Logical Deletion of a Record(contigue)
void deleteRecordLogicalcontigue(fichier *F, int recordID) {
    int startBlock = lireEntete(*F, 4); // Get the starting block of the file
    int recordCount = lireEntete(*F, 3); // Get the total number of records
    bool isSorted = liretypeTri(*F); // Determine if the file is sorted

    if (startBlock == -1) { // Check if the file is initialized
        printf("Error: File not initialized.\n");
        return;
    }

    for (int i = 0; i < recordCount; i++) {// Traverse the records in the contiguous blocks
        int blockIndex = startBlock + (i / BLOCK_SIZE);
        int recordIndex = i % BLOCK_SIZE;

        if (disk[blockIndex].contigue.enregistrement[recordIndex].ID == recordID) {
            if (disk[blockIndex].contigue.enregistrement[recordIndex].Supprime) {
                printf("Record %d is already logically deleted.\n", recordID);
                return;
            }
            disk[blockIndex].contigue.enregistrement[recordIndex].Supprime = true; // Mark as deleted
            printf("Record %d marked as logically deleted.\n", recordID);
            return;
        }
        if (isSorted && disk[blockIndex].contigue.enregistrement[recordIndex].ID > recordID) {
            // Early exit for sorted files if passed the potential location
            printf("Error: Record %d not found in sorted file.\n", recordID);
            return;
        }
    }
    // If the record is not found
    printf("Error: Record %d not found.\n", recordID);
}


// 10. Physical Deletion of a Record(chained)
void deleteRecordPhysicalchaine(fichier *F, int recordID) {
    int currentBlockID = lireEntete(*F, 4); // Get the starting block of the file
    if (currentBlockID == -1) { // Check if the file is initialized
        printf("Error: File not initialized.\n");
        return;
    }

    while (currentBlockID != -1) { // Traverse linked blocks
        if (!disk[currentBlockID].chainee.free) { // Check if the block is in use
            for (int j = 0; j < BLOCK_SIZE; j++) { // Traverse records in the block
                if (disk[currentBlockID].chainee.enregistrement[j].ID == recordID) { // If the record matches the given ID
                    // Physically delete the record by resetting its fields
                    disk[currentBlockID].chainee.enregistrement[j].ID = 0; // Reset record ID
                    disk[currentBlockID].chainee.enregistrement[j].Supprime = false; // Reset "deleted" flag
                    memset(disk[currentBlockID].chainee.enregistrement[j].Data, 0, sizeof(disk[currentBlockID].chainee.enregistrement[j].Data)); // Clear data
                    printf("Record %d physically deleted.\n", recordID);

                    // Update metadata
                    int recordCount = lireEntete(*F, 3);
                    MajEntetenum(F, 3, recordCount - 1); // Decrement the record count
                    chargerMetadonnees(*F);
                    return; // Exit after deletion
                }
            }
        }
        currentBlockID = disk[currentBlockID].chainee.next; // Move to the next block
    }

    // If the record is not found in any block
    printf("Error: Record %d not found.\n", recordID);
}

// 10. Physical Deletion of a Record (Contiguous)
void deleteRecordPhysicalContiguous(fichier *F, int recordID) {
    int startBlock = lireEntete(*F, 4); // Get the starting block of the file
    int recordCount = lireEntete(*F, 3); // Get the total number of records

    if (startBlock == -1) { // Check if the file is initialized
        printf("Error: File not initialized.\n");
        return;
    }

    // Traverse the records in the contiguous blocks
    for (int i = 0; i < recordCount; i++) {
        int blockIndex = startBlock + (i / BLOCK_SIZE);
        int recordIndex = i % BLOCK_SIZE;

        if (disk[blockIndex].contigue.enregistrement[recordIndex].ID == recordID) { // Match found
            // Physically delete the record by resetting its fields
            disk[blockIndex].contigue.enregistrement[recordIndex].ID = 0; // Reset record ID
            disk[blockIndex].contigue.enregistrement[recordIndex].Supprime = false; // Reset "deleted" flag
             // Clear data
            memset(disk[blockIndex].contigue.enregistrement[recordIndex].Data, 0, sizeof(disk[blockIndex].contigue.enregistrement[recordIndex].Data));
            printf("Record %d physically deleted.\n", recordID);

            // Update metadata
            int recordCount = lireEntete(*F, 3);
            MajEntetenum(F, 3, recordCount - 1); // Decrement the record count
            chargerMetadonnees(*F);
            return; // Exit after deletion
        }
    }

    // If the record is not found
    printf("Error: Record %d not found.\n", recordID);
}

// Function to perform defragmentation(chained): update metadata, table index, and compact blocks
void Defragmentationchainee(fichier *F) {
    printf("Défragmentation en cours...\n");

    int nbBlocs = lireEntete(*F, 2);  // Read the number of blocks from metadata
    int nbEnregistrements = lireEntete(*F, 3);  // Read the number of records from metadata

    int currentBlockID = lireEntete(*F, 4); // Get the first block of the file
    int freeBlockIndex = -1;

    // Identify the first free block
    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (disk[i].chainee.free) {
            freeBlockIndex = i;
            break;
        }
    }

    if (freeBlockIndex == -1) {
        printf("Pas de blocs libres disponibles pour la défragmentation.\n");
        return;
    }

    while (currentBlockID != -1) {
        if (disk[currentBlockID].chainee.free) {
            // If the block is free, find the next used block
            int nextUsedBlockID = currentBlockID;
            while (nextUsedBlockID != -1 && disk[nextUsedBlockID].chainee.free) {
                nextUsedBlockID = disk[nextUsedBlockID].chainee.next;
            }

            if (nextUsedBlockID == -1) break; // No more used blocks

            // Move the used block to the free space
            memcpy(&disk[freeBlockIndex], &disk[nextUsedBlockID], sizeof(Bloc));
            initializeBlockChainee(nextUsedBlockID); // Free the old block

            // Update the linked list pointers
            for (int i = 0; i < MAX_BLOCKS; i++) {
                if (disk[i].chainee.next == nextUsedBlockID) {
                    disk[i].chainee.next = freeBlockIndex;
                    break;
                }
            }

            freeBlockIndex = nextUsedBlockID;
        }
        currentBlockID = disk[currentBlockID].chainee.next;
    }

    // Update metadata
    MajEntetenum(F, 2, nbBlocs); // Update number of blocks
    MajEntetenum(F, 3, nbEnregistrements); // Update number of records
    printf("Défragmentation terminée.\n");
}

// Function to perform defragmentation(contiguous) : update metadata, table index, and compact blocks
void DefragmentationContigue(fichier *F) {
    printf("Defragmentation in progress...\n");

    int nbBlocs = lireEntete(*F, 2);  // Read the number of blocks from metadata
    int nbEnregistrements = lireEntete(*F, 3);  // Read the number of records from metadata

    int firstBlock = lireEntete(*F, 4); // Get the first block of the file
    if (firstBlock == -1) {
        printf("No blocks allocated to the file.\n");
        return;
    }

    int freeBlockIndex = -1;

    // Identify the first free block
    for (int i = firstBlock; i < firstBlock + nbBlocs; i++) {
        if (disk[i].contigue.free) {
            freeBlockIndex = i;
            break;
        }
    }

    if (freeBlockIndex == -1) {
        printf("No free blocks available for defragmentation.\n");
        return;
    }

    for (int i = freeBlockIndex + 1; i < firstBlock + nbBlocs; i++) {
        if (!disk[i].contigue.free) {
            // Move the block content to the free space
            memcpy(&disk[freeBlockIndex], &disk[i], sizeof(Bloc));
            initializeBlockContigue(i); // Free the old block

            freeBlockIndex++;
        }
    }

    // Update metadata
    MajEntetenum(F, 2, nbBlocs); // Update the number of blocks
    MajEntetenum(F, 3, nbEnregistrements); // Update the number of records
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
    removeIndexTable(F); // SUpprime la table d'index

    supprimeFichierMetadonnees(F); //Supprime le fichier de metadonnees

    printf("File deleted successfully.\n");
}

// Delete a file with contiguous allocation
void deleteFileContigue(fichier *F) {
    int firstAdress = lireEntete(*F,4);
    int nbBlocs = lireEntete(*F,2);

    // Traverse and free all blocks of the file using the linked list
    for(int i = firstAdress; i<nbBlocs+firstAdress; i++) {
        initializeBlockContigue(i); //Reinitialise le bloc
    }

    // Remove file from the index table
    removeIndexTable(F); // SUpprime la table d'index

    supprimeFichierMetadonnees(F); //Supprime le fichier de metadonnees

    printf("File deleted successfully.\n");
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
