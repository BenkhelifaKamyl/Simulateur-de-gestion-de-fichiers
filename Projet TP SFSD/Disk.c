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
void AfficherDisqueContigue() {
    int i, j, k, nbBlocs, nbEnregistrements, premiereAdresse;
    Bloc buffer;
    fichier F;
    char filename[30];

    for (i = 0; i < MAX_BLOCKS; i++) {
        if (checkBlockContigue(i) == false) {
            boldColor(2);
            printf("\n[%d] Bloc libre.",i);
            resetColor();
        } else {
            rechercheFichierMeta(i, &F); // Recuperer les metadonnees du bloc
            if(F.MDfile==NULL) continue;
            nbBlocs = lireEntete(F, 2);
            nbEnregistrements = lireEntete(F, 3);
            premiereAdresse = lireEntete(F, 4);
            lireNomFichier(F, filename);

            // Parcourir tous les blocs du fichier
            for (k = 0; k < nbBlocs; k++) {
                if (i + k >= MAX_BLOCKS) break;  // Sortir si on d�passe les blocs disponibles

                if (k == nbBlocs - 1) {
                    j = nbEnregistrements % BLOCK_SIZE;
                    if (j == 0) {
                        j = BLOCK_SIZE;
                    }
                } else {
                    j = BLOCK_SIZE;
                }
                boldColor(1);
                printf("\n[%d] Nom du fichier: %s et nombre d'enregistrements: %d",i+k, filename, j);
                resetColor();
            }
            i += nbBlocs - 1;  // Avancer `i` de `nbBlocs - 1` pour passer aux prochains blocs
            fclose(F.MDfile);  // Fermer le fichier de metadonnees apr�s traitement
        }
    }
}


void AfficherDisqueChainee(){
    int i,j,nbEnregistrements;
    Bloc buffer;
    fichier F;
    char filename[30];
    for(i=0; i<MAX_BLOCKS; i++){
        if(checkBlock(i)==false){
            boldColor(2);
            printf("\n[%d] Bloc libre.",i);
            resetColor();
        }
        else{
            memcpy(&buffer,&disk[i],sizeof(Bloc));
            rechercheFichierMeta(i, &F); //Recuperer les metadonnees du bloc
            nbEnregistrements=lireEntete(F,3);
            lireNomFichier(F,filename);
                if(buffer.chainee.next==-1){
                    j=nbEnregistrements%BLOCK_SIZE;
                    if(j==0){
                        j=BLOCK_SIZE;
                    }
                }
                else{
                    j=BLOCK_SIZE;
                }
                boldColor(1);
                printf("\n[%d] Nom du fichier:  %s et nombre d'enregistrements: %d",i,filename, j);
                resetColor();
                fclose(F.MDfile);
            }

        }
}

void AfficherBlocChainee(int i){
    int j=0;
    while(j<BLOCK_SIZE){
        if(!disk[i].chainee.enregistrement[j].Supprime)
            printf("\nID [%d]: %d",j,disk[i].chainee.enregistrement[j].ID);
        j++;
    }
}
void AfficherBlocContigue(int i){
    int j=0;
    while(j<BLOCK_SIZE){
        if(!disk[i].contigue.enregistrement[j].Supprime)
            printf("\nID [%d]: %d",j,disk[i].contigue.enregistrement[j].ID);
        j++;
    }
}
void AfficherFichierContigue(fichier F){
    int premierBloc = lireEntete(F,4);
    int nbBlocs = lireEntete(F,2);
    for (int i = premierBloc; i < nbBlocs + premierBloc; i++){
        AfficherBlocContigue(i);
    }
}
void AfficherFichierChainee(fichier F){
    int i = lireEntete(F,4);
    while(i!=-1){
        AfficherBlocChainee(i);
        i=disk[i].chainee.next;
    }
}
// Initialisation d'un bloc pour l'organisation cha�n�e
void initializeBlockChainee(int i) {
    disk[i].chainee.free = true;
    disk[i].chainee.next = -1;
   memset(disk[i].chainee.enregistrement, 0, BLOCK_SIZE * sizeof(disk[i].chainee.enregistrement[0])); //boucle for et c tout!!!
}

// Initialisation d'un bloc pour l'organisation contigu�
void initializeBlockContigue(int i) {
    disk[i].contigue.free = true;
    memset(disk[i].contigue.enregistrement, 0, BLOCK_SIZE * sizeof(disk[i].contigue.enregistrement[0]));
}

// Initialisation du disque en mode cha�n�
void initializeDiskChainee() {
    currentMode = MODE_CHAINE;
    if (MAX_BLOCKS <= 0 || BLOCK_SIZE <= 0) {
        printf("Erreur : configuration de disque invalide.\n");
        return;
    }
    for (int i = 0; i < MAX_BLOCKS; i++) {
        initializeBlockChainee(i);
    }
    printf("Disque initialise avec %d blocs en mode chain�.\n", MAX_BLOCKS);
}

// Initialisation du disque en mode contigu�
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
    Bloc buffer;  // Temporary buffer for moving blocks
    fichier F;
    int freeIndex = 0;  // Index of the first free block

    // Parcourir chaque fichier
    for (int i = 0; i < MAX_FILES; i++) {
        if (Meta[i].premiereAdresse != -1) {
            // Charger les m�tadonn�es du fichier
            chargerFichierMetadonnees(Meta[i].premiereAdresse, &F);

            int currentBlockID = lireEntete(F, 4);
            int prevBlockID = -1;

            // Parcourir les blocs associ�s � ce fichier
            while (currentBlockID != -1) {
                // Trouver le prochain bloc libre
                while (freeIndex < MAX_BLOCKS && !disk[freeIndex].chainee.free) {
                    freeIndex++;
                }

                // Si le bloc libre est avant le bloc courant, d�placer le bloc
                if (freeIndex < currentBlockID) {
                    // Utiliser le buffer temporaire pour stocker le bloc courant
                    memcpy(&buffer, &disk[currentBlockID], sizeof(Bloc));

                    // D�placer le bloc vers le dernier espace libre
                    memcpy(&disk[freeIndex], &buffer, sizeof(Bloc));

                    // Mettre � jour les m�tadonn�es et les pointeurs du fichier
                    if (prevBlockID != -1) {
                        disk[prevBlockID].chainee.next = freeIndex;
                    } else {
                        MajEntetenum(&F, 4, freeIndex);  // Mettre � jour la premi�re adresse
                    }

                    // Mettre � jour toutes les r�f�rences au bloc d�plac� dans la cha�ne
                    for (int j = 0; j < MAX_BLOCKS; j++) {
                        if (disk[j].chainee.next == currentBlockID) {
                            disk[j].chainee.next = freeIndex;
                            break;
                        }
                    }

                    // Lib�rer l'ancien bloc
                    disk[currentBlockID].chainee.free = true;
                    disk[currentBlockID].chainee.next = -1;

                    // Mettre � jour l'ID du bloc courant
                    currentBlockID = freeIndex;
                }

                // Passer au bloc suivant
                prevBlockID = currentBlockID;
                currentBlockID = disk[currentBlockID].chainee.next;
            }

            // Recharger les m�tadonn�es pour refl�ter les changements
            chargerMetadonnees(F);
        }
    }

    printf("Disk compacted in chained mode.\n");
}

// Compactage du Disque avec Buffer (contigue)
void compactDiskContigue() {
    Bloc buffer; // Buffer temporaire pour le d�placement
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

            // D�place le bloc dans l'espace libre
            memcpy(&disk[lastFreeBlock], &buffer, sizeof(Bloc));
            if(F.MDfile!=NULL){
                if(lireEntete(F,4)==i){
                    MajEntetenum(&F,4,lastFreeBlock);
                }
                chargerMetadonnees(F);
            }
            // Lib�re l'ancien bloc
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

// 4 Vider la M�moire Secondaire (chainee)
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

 // Vider la M�moire Secondaire (contigue)
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

//5V�rification d'un Bloc(chainee)
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
        buffer[i].ID = rand()%100 + 1; // Assign IDs sequentially
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

    printf("Starting block filling process...\n");

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

        printf("Filling block at address %d...\n", currentBlockAddress);
        bufferIndex=0;
        // Fill the current block with records
        for (int j = 0; j < BLOCK_SIZE && recordsFilled < nbEnregistrements; j++) {
            memcpy(&disk[currentBlockAddress].chainee.enregistrement[j], &buffer[bufferIndex++],sizeof(Enregistrement));
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
        buffer[i].ID = rand()%100 + 1; // Assign IDs sequentially
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
    for (int i = premiereAdresse; i < nbBlocs; i++) {
        if (disk[i].contigue.free) {
            disk[i].contigue.free = false; // Mark the block as occupied
            blocksUsed++;

            // Update the first block address if not set
            if (firstBlockAddress == -1)
                firstBlockAddress = i;

            bufferIndex=0;
            // Copy buffer records to the current disk block
            for (int j = 0; j < BLOCK_SIZE && recordsFilled < nbEnregistrements; j++) {
                memcpy(&disk[i].contigue.enregistrement[j], &buffer[bufferIndex], sizeof(Enregistrement));
                bufferIndex++;
                recordsFilled++;
            }

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
        // Cas tri� : rechercher la position d'insertion
        rechercheEnregistrementNonDense(F, record.ID, &numBloc, &deplacement);
    } else {
        // Cas non tri� : rechercher un espace libre
        rechercheEnregistrementDense(F, record.ID, &numBloc, &deplacement);
    }

    printf("Apr�s recherche : numBloc = %d, deplacement = %d\n", numBloc, deplacement);

    // V�rifier si un espace est disponible
    if (numBloc == -1) {
        printf("\nEspace insuffisant pour l'insertion.\n");
        return;
    }

    // D�calage pour faire de la place � l'enregistrement
    while (deplacement >= tailleBloc) {
        // Si le bloc est plein, passer au suivant
        if (disk[numBloc].chainee.next == -1) {
            // Cr�er un nouveau bloc si n�cessaire
            if (nbBlocks >= MAX_BLOCKS) {
                printf("\nEspace insuffisant pour ajouter un nouveau bloc.\n");
                return;
            }

            int newBlock = nbBlocks;
            nbBlocks++;
            disk[newBlock].chainee.free = false;
            disk[newBlock].chainee.next = -1; // Fin de la cha�ne
            disk[numBloc].chainee.next = newBlock; // Lier au pr�c�dent
            numBloc = newBlock;
            deplacement = 0; // Commencer au d�but du nouveau bloc
        } else {
            numBloc = disk[numBloc].chainee.next;
            deplacement -= tailleBloc; // Continuer au prochain bloc
        }
    }

    printf("Avant insertion : numBloc = %d, deplacement = %d\n", numBloc, deplacement);

    // D�caler les enregistrements existants pour lib�rer l'espace
    int currentBlock = numBloc, currentPosition = deplacement;
    while (currentBlock != -1) {
        for (int i = tailleBloc - 1; i > currentPosition; i--) {
            disk[currentBlock].chainee.enregistrement[i] = disk[currentBlock].chainee.enregistrement[i - 1];
        }

        // Passer au bloc suivant pour continuer le d�calage, si n�cessaire
        if (disk[currentBlock].chainee.next != -1) {
            memcpy(&disk[disk[currentBlock].chainee.next].chainee.enregistrement[0],
                   &disk[currentBlock].chainee.enregistrement[tailleBloc - 1],
                   sizeof(Enregistrement));
        }
        currentPosition = 0;
        currentBlock = disk[currentBlock].chainee.next;
    }

    // Ins�rer l'enregistrement dans la position lib�r�e
    memcpy(&disk[numBloc].chainee.enregistrement[deplacement], &record, sizeof(Enregistrement));

    printf("Apr�s insertion : numBloc = %d, deplacement = %d\n", numBloc, deplacement);

    // Mise � jour des m�tadonn�es
    nbEnregistrements++;
    MajEntetenum(F, 2, nbBlocks);
    MajEntetenum(F, 3, nbEnregistrements);

    // Mettre � jour la table d'index
    if(liretypeTri(*F)) {
        MajTableIndexNonDense(F, 2, record.ID, numBloc);
    } else {
        MajTableIndexDense(F, 2, record.ID, numBloc);
    }

    chargerMetadonnees(*F);
    printf("\nEnregistrement ins�r� avec succ�s.\n");
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
        // Recherche de la position d'insertion pour le cas tri�
        int deplacement = -1;
        rechercheEnregistrementNonDense(F, record.ID, &numBloc, &deplacement);

        if (numBloc == -1) {
            printf("\nEspace insuffisant pour l'insertion (tri�).");
            return;
        }

        // D�calage pour faire de la place
        for (int i = nbEnregistrements; i > (numBloc * tailleBloc + deplacement); i--) {
            int srcBloc = (i - 1) / tailleBloc;
            int srcPos = (i - 1) % tailleBloc;
            int destBloc = i / tailleBloc;
            int destPos = i % tailleBloc;

            if (destBloc >= MAX_BLOCKS) {
                printf("\nEspace insuffisant pendant le d�calage.");
                return;
            }

            // Ajouter un nouveau bloc si n�cessaire
            if (destBloc >= nbBlocks) {
                nbBlocks++;
                memset(&disk[destBloc], 0, sizeof(Bloc));
            }
            memcpy(&disk[destBloc].contigue.enregistrement[destPos], &disk[srcBloc].contigue.enregistrement[srcPos], sizeof(Enregistrement));
        }

        // Ins�rer le nouvel enregistrement
        memcpy(&disk[numBloc].contigue.enregistrement[deplacement], &record,sizeof(Enregistrement));

    } else {
        // Cas non tri� (dense)
        int deplacement = -1;
        rechercheEnregistrementDense(F, record.ID, &numBloc, &deplacement);

        if (numBloc == -1) {
            printf("\nEspace insuffisant pour l'insertion (non tri�).");
            return;
        }

        if (deplacement < tailleBloc) {
            // Ins�rer dans l'espace libre du bloc courant
            memcpy(&disk[numBloc].contigue.enregistrement[deplacement], &record,sizeof(Enregistrement));
        } else {
            // Ajouter un nouveau bloc si n�cessaire
            dernierBloc++;
            nbBlocks++;
            memset(&disk[dernierBloc], 0, sizeof(Bloc));
            memcpy(&disk[numBloc].contigue.enregistrement[deplacement], &record,sizeof(Enregistrement));
        }
    }

    // Mise � jour des m�tadonn�es
    nbEnregistrements++;
    MajEntetenum(F, 2, nbBlocks);
    MajEntetenum(F, 3, nbEnregistrements);

    // Mise � jour de la table d'index
    if(liretypeTri(*F))
        MajTableIndexNonDense(F,2,record.ID,numBloc);
    else
        MajTableIndexDense(F,2,record.ID,numBloc);

    // Charger les m�tadonn�es mises � jour
    chargerMetadonnees(*F);

    printf("\nEnregistrement ins�r� avec succ�s.");
}

// 9. Logical Deletion of a Record(contigue)
void deleteRecordLogicalcontigue(fichier *F, int recordID) {
    int startBlock = lireEntete(*F, 4); // Obtenir le bloc de d�part du fichier
    int recordCount = lireEntete(*F, 3); // Obtenir le nombre total d'enregistrements
    int nbBlocs = lireEntete(*F,2);
    bool isSorted = liretypeTri(*F); // D�terminer si le fichier est tri�
    Index indexTable[100];
    // V�rifier si le fichier est initialis�
    if (startBlock == -1) {
        printf("Error: File not initialized.\n");
        return;
    }

    // Parcourir les enregistrements dans les blocs contigus
    for (int i = 0; i < recordCount; i++) {
        int blockIndex = startBlock + (i / BLOCK_SIZE); // Identifier le bloc correspondant
        int recordIndex = i % BLOCK_SIZE;              // Identifier l'index dans le bloc

        // V�rifier si l'ID correspond � celui recherch�
        if (disk[blockIndex].contigue.enregistrement[recordIndex].ID == recordID) {
            // V�rifier si l'enregistrement est d�j� supprim�
            if (disk[blockIndex].contigue.enregistrement[recordIndex].Supprime) {
                printf("Record %d is already logically deleted.\n", recordID);
                return;
            }

            // Marquer l'enregistrement comme supprim�
            disk[blockIndex].contigue.enregistrement[recordIndex].Supprime = true;
            printf("Record %d marked as logically deleted.\n", recordID);

            // Mise � jour de l'index directement
            if(isSorted){
                chargementFichierIndexNonDense(F,indexTable);
                for (int j = 0; j < getIndexSize(indexTable); j++) {
                    if (indexTable[j].id == recordID) {
                        indexTable[j].id=-1;
                        sauvegardeTableIndex(F,indexTable);
                        printf("Index updated for Record %d.\n", recordID);
                        if(recordIndex==0)
                            MajEntetenum(F,2,nbBlocs-1);
                        MajEntetenum(F,3,recordCount-1);
                        chargerMetadonnees(*F);
                        return;
                    }
                }

            }
            else{
                chargementFichierIndexNonDense(F,indexTable);
                for (int j = 0; j < getIndexSize(indexTable); j++) {
                    if (indexTable[j].id == recordID) {
                        indexTable[j].id=-1;
                        printf("Index updated for Record %d.\n", recordID);
                        sauvegardeTableIndex(F,indexTable);
                        if(recordIndex==0)
                            MajEntetenum(F,2,nbBlocs-1);
                        MajEntetenum(F,3,recordCount-1);
                        chargerMetadonnees(*F);
                        return;
                    }
                }

            }
            // Si aucun index correspondant n'est trouv�
            printf("No index found for Record %d, but it was marked as deleted in the file.\n", recordID);
            return;
        }

        // Sortie anticip�e pour les fichiers tri�s si l'ID actuel est sup�rieur � l'ID recherch�
        if (isSorted && disk[blockIndex].contigue.enregistrement[recordIndex].ID > recordID) {
            printf("Error: Record %d not found in sorted file.\n", recordID);
            return;
        }
    }

    // Si l'enregistrement n'est pas trouv�
    printf("Error: Record %d not found.\n", recordID);
}
// 9. Logical Deletion of a Record(chainee)
void deleteRecordLogicalchainee(fichier *F, int recordID) {
    int currentBlockID = lireEntete(*F, 4); // Obtenir le bloc de d�part du fichier
    int recordCount = lireEntete(*F, 3); // Obtenir le nombre total d'enregistrements
    bool isSorted = liretypeTri(*F); // D�terminer si le fichier est tri�
    Index indexTable[100]; // Table d'index

    // V�rifier si le fichier est initialis�
    if (currentBlockID == -1) {
        printf("Error: File not initialized.\n");
        return;
    }

    // Parcourir les blocs cha�n�s pour trouver l'enregistrement
    while (currentBlockID != -1) { // Tant qu'il y a des blocs � parcourir
        for (int i = 0; i < BLOCK_SIZE; i++) { // Parcourir tous les enregistrements dans le bloc
            if (disk[currentBlockID].chainee.enregistrement[i].ID == recordID) {
                // V�rifier si l'enregistrement est d�j� supprim�
                if (disk[currentBlockID].chainee.enregistrement[i].Supprime) {
                    printf("Record %d is already logically deleted.\n", recordID);
                    return;
                }

                // Marquer l'enregistrement comme supprim�
                disk[currentBlockID].chainee.enregistrement[i].Supprime = true;
                printf("Record %d marked as logically deleted.\n", recordID);

                // Mise � jour de l'index directement
                chargementFichierIndexNonDense(F, indexTable);
                for (int j = 0; j < getIndexSize(indexTable); j++) {
                    if (indexTable[j].id == recordID) {
                        indexTable[j].id = -1; // Supprimer l'index pour cet enregistrement
                        sauvegardeTableIndex(F, indexTable);
                        printf("Index updated for Record %d.\n", recordID);

                        // Mettre � jour les m�tadonn�es
                        MajEntetenum(F, 3, recordCount - 1); // D�cr�menter le nombre d'enregistrements
                        chargerMetadonnees(*F); // Recharger les m�tadonn�es
                        return;
                    }
                }

                // Si aucun index correspondant n'est trouv�
                printf("No index found for Record %d, but it was marked as deleted in the file.\n", recordID);
                return;
            }
        }

        // Passer au bloc suivant dans la cha�ne
        currentBlockID = disk[currentBlockID].chainee.next;
    }

    // Si l'enregistrement n'est pas trouv�
    printf("Error: Record %d not found.\n", recordID);
}

// 10. Physical Deletion of a Record(chained)
void deleteRecordPhysicalchaine(fichier *F, int recordID) {
    int currentBlockID = lireEntete(*F, 4); // Obtenir le bloc de d�part du fichier
    int nbBlocs = lireEntete(*F, 2);
    bool isSorted = liretypeTri(*F); // D�terminer si le fichier est tri�
    int recordCount = lireEntete(*F, 3); // Lire le nombre total d'enregistrements
    int numBloc = -1, deplacement = -1;
    Index indexTable[100];

    if (currentBlockID == -1) { // V�rifier si le fichier est initialis�
        printf("Error: File not initialized.\n");
        return;
    }

    printf("Starting physical deletion for Record ID: %d\n", recordID);

    if (isSorted) {
        rechercheEnregistrementNonDense(F, recordID, &numBloc, &deplacement);
    } else {
        rechercheEnregistrementDense(F, recordID, &numBloc, &deplacement);
    }

    printf("Record located at numBloc: %d, deplacement: %d\n", numBloc, deplacement);

    if (deplacement == -1 || numBloc == -1) {
        printf("\nL'enregistrement n'existe pas.\n");
        return;
    }

    chargementFichierIndexNonDense(F, indexTable);
    int k = 0;
    bool trouve = false;
    while (k < getIndexSize(indexTable)) {
        if (indexTable[k].id == recordID) {
            trouve = true;
            break;
        }
        k++;
    }

    if (!trouve) {
        printf("No matching index found for Record %d.\n", recordID);
        return;
    }

    printf("Index located, starting deletion...\n");

    while (numBloc != -1) {
        while (deplacement < BLOCK_SIZE - 1 && disk[numBloc].chainee.enregistrement[deplacement].ID != 0) {
            if (trouve) {
                if (deplacement + 1 < BLOCK_SIZE) {
                    indexTable[k].id = disk[numBloc].chainee.enregistrement[deplacement + 1].ID;
                } else {
                    indexTable[k].id = -1; // Cas du dernier enregistrement
                    indexTable[k].numBloc = -1;
                }
                sauvegardeTableIndex(F, indexTable);
                printf("Index updated for Record %d.\n", recordID);
                trouve = false;
            }
            memcpy(&disk[numBloc].chainee.enregistrement[deplacement], &disk[numBloc].chainee.enregistrement[deplacement + 1], sizeof(Enregistrement));
            deplacement++;
        }

        if (disk[numBloc].chainee.next == -1) {
            disk[numBloc].chainee.enregistrement[deplacement].ID = 0;
            numBloc = -1;
        } else {
            memcpy(&disk[numBloc].chainee.enregistrement[deplacement], &disk[disk[numBloc].chainee.next].chainee.enregistrement[0], sizeof(Enregistrement));
            numBloc = disk[numBloc].chainee.next;
            deplacement = 0;
            if (disk[numBloc].chainee.enregistrement[deplacement + 1].ID == 0) {
                disk[numBloc].chainee.enregistrement[deplacement].ID = 0;
                MajEntetenum(F, 2, nbBlocs - 1);
            }
        }
    }

    printf("Record physically deleted, updating index and metadata...\n");

    for (int j = k; j < getIndexSize(indexTable) - 1; j++) {
        indexTable[j].id = indexTable[j + 1].id;
        indexTable[j].numBloc = indexTable[j + 1].numBloc;
    }

    indexTable[getIndexSize(indexTable) - 1].id = -1;
    indexTable[getIndexSize(indexTable) - 1].numBloc = -1;
    printf("Index updated for Record %d.\n", recordID);
    sauvegardeTableIndex(F, indexTable);
    MajEntetenum(F, 3, recordCount - 1);
    chargerMetadonnees(*F);

    printf("Deletion completed successfully.\n");
}

// 10. Physical Deletion of a Record (Contiguous)
void deleteRecordPhysicalContiguous(fichier *F, int recordID) {
    int startBlock = lireEntete(*F, 4); // Obtenir le bloc de d�part du fichier
    int nbBlocs = lireEntete(*F, 2);    // Nombre de blocs dans le fichier
    bool isSorted = liretypeTri(*F);    // D�terminer si le fichier est tri�
    int recordCount = lireEntete(*F, 3); // Lire le nombre total d'enregistrements
    Index indexTable[100]; // Table d'index

    if (startBlock == -1) { // V�rifier si le fichier est initialis�
        printf("Error: File not initialized.\n");
        return;
    }

    // Parcourir les enregistrements dans les blocs contigus
    for (int i = 0; i < recordCount; i++) {
        int blockIndex = startBlock + (i / BLOCK_SIZE); // Identifier le bloc correspondant
        int recordIndex = i % BLOCK_SIZE;              // Identifier l'index dans le bloc

        // Si l'enregistrement correspond � l'ID recherch�
        if (disk[blockIndex].contigue.enregistrement[recordIndex].ID == recordID) {
            // Supprimer physiquement l'enregistrement en r�initialisant ses champs
            disk[blockIndex].contigue.enregistrement[recordIndex].ID = 0; // R�initialiser l'ID
            disk[blockIndex].contigue.enregistrement[recordIndex].Supprime = false; // R�initialiser le flag "supprim�"
            memset(disk[blockIndex].contigue.enregistrement[recordIndex].Data, 0, sizeof(disk[blockIndex].contigue.enregistrement[recordIndex].Data)); // Effacer les donn�es
            printf("Record %d physically deleted.\n", recordID);
            int a = recordIndex; // Sauvegarder l'index de l'enregistrement supprim�

            // Mise � jour de l'index si le fichier est tri�
            if (isSorted) {
                chargementFichierIndexNonDense(F, indexTable);
                for (int j = 0; j < getIndexSize(indexTable); j++) {
                    if (indexTable[j].id == recordID) {
                        if (a < BLOCK_SIZE) {
                            indexTable[j].id = disk[blockIndex].contigue.enregistrement[a + 1].ID;
                        } else {
                            indexTable[j].id = -1; // Cas du dernier enregistrement
                            indexTable[j].numBloc = -1;
                        }
                        sauvegardeTableIndex(F, indexTable);
                        printf("Index updated for Record %d.\n", recordID);
                        if (j == 0)
                            MajEntetenum(F, 2, nbBlocs - 1);
                        MajEntetenum(F, 3, recordCount - 1);
                        chargerMetadonnees(*F);
                        return;
                    }
                }
            } else {
                chargementFichierIndexNonDense(F, indexTable);
                for (int j = 0; j < getIndexSize(indexTable); j++) {
                    if (indexTable[j].id == recordID) {
                        int k = j;
                        while (indexTable[k].numBloc != -1 && k < MAX_INDEX_ENTRIES) {
                            memcpy(&indexTable[k], &indexTable[k + 1], sizeof(Index));
                            k++;
                        }
                        indexTable[k].id = -1;
                        indexTable[k].numBloc = -1;
                        printf("Index updated for Record %d.\n", recordID);
                        sauvegardeTableIndex(F, indexTable);
                        if (j == 0)
                            MajEntetenum(F, 2, nbBlocs - 1);
                        MajEntetenum(F, 3, recordCount - 1);
                        chargerMetadonnees(*F);
                        return;
                    }
                }
            }

            // Si aucun index correspondant n'est trouv�
            printf("Warning: No index found for Record %d, but it was deleted from the file.\n", recordID);
            return;
        }
    }

    // Si l'enregistrement n'est pas trouv�
    printf("Error: Record %d not found.\n", recordID);
}

// Function to perform defragmentation(chained): update metadata, table index, and compact blocks
void Defragmentationchainee(fichier *F) {
    printf("D�fragmentation en cours...\n");

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
        printf("Pas de blocs libres disponibles pour la d�fragmentation.\n");
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
    printf("D�fragmentation termin�e.\n");
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
