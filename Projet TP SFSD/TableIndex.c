#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "TextColor.h"
#include "Metadonnees.h"
#include "TableIndex.h"
#include "Disk.h"

void creationTableIndexDenseContigue(fichier F, Index densetableIndex [100]) {
    Bloc buffer;
    int m, a,k = 0;
    Index X;
    int nbBlocs = lireEntete(F, 2);
    int premiereAdresse = lireEntete(F, 4);
    int nbEnregistrements = lireEntete(F, 3);

    for (int i = premiereAdresse; i < premiereAdresse + nbBlocs; i++) {
        if(checkBlock(i) == true ){
            memcpy(&buffer, &disk[i],sizeof(Bloc)); //Copie du bloc
            for(int j=0;j<BLOCK_SIZE && k<nbEnregistrements;j++){
                densetableIndex[a].id=buffer.contigue.enregistrement[j].ID;
                densetableIndex[a].numBloc = i;
                k++;
                a+=1;
            }
        }
    }
    for(int i=0; i<nbEnregistrements-1; i++){
        for(int j=i+1; j<nbEnregistrements; j++){
            if(densetableIndex[i].id>densetableIndex[j].id){
                X=densetableIndex[i];
                densetableIndex[i]=densetableIndex[j];
                densetableIndex[j]=X;
            }
        }
    }
    printf("\n La table d'index a ete cree avec succes.");
}

void creationTableIndexDenseChainee(fichier F, Index densetableIndex [100]){
    Bloc buffer;
    MetaDonnee MD;
    int m,a=0,k=0;
    Index X;
    int nbBlocs = lireEntete(F,2);
    int premiereAdresse = lireEntete(F,4);
    int nbEnregistrements = lireEntete(F,3);
    int i=premiereAdresse;

    while(i!=-1 && k<nbEnregistrements){
        if(checkBlock(i) == true ){
            memcpy(&buffer, &disk[i],sizeof(Bloc)); //Copie du bloc
            for(int j=0;j<BLOCK_SIZE && k<nbEnregistrements;j++){
                densetableIndex[a].id=buffer.chainee.enregistrement[j].ID;
                densetableIndex[a].numBloc = i;
                k++;
                a+=1;
            }
            i=buffer.chainee.next;
        }
    }
    for(int i=0; i<nbEnregistrements-1; i++){
        for(int j=i+1; j<nbEnregistrements; j++){
            if(densetableIndex[i].id>densetableIndex[j].id){
                X=densetableIndex[i];
                densetableIndex[i]=densetableIndex[j];
                densetableIndex[j]=X;
            }
        }
    }
    printf("\nLa table d'index a ete cree avec succes.");
}
void creationTableIndexNonDenseContigue (fichier F, Index tableIndex [100]){
    Bloc buffer;
    MetaDonnee MD;
    int k=0;
    Index X;
    //Lecture des metadonnees
    int nbBlocs = lireEntete(F,2);
    int premiereAdresse = lireEntete(F,4);
    int nbEnregistrements = lireEntete(F,3);

    for(int i=premiereAdresse;i< premiereAdresse+nbBlocs;i++){
        if(checkBlockContigue(i)){ //Verifie si le bloc est valide
            memcpy(&buffer,&disk[i],sizeof(Bloc)); //Copie du bloc
            X.id=buffer.contigue.enregistrement[0].ID;
            X.numBloc= i; //Position du bloc pas de l'enregistrement
            memcpy(&tableIndex[k],&X,sizeof(Index)); //Ajout dans la table d'index
            k++;
        }
    }
    printf("\n la table d'index non dense a ete cree avec succes.");
 }
void creationTableIndexNonDenseChainee (fichier F, Index tableIndex [100]){
    Bloc buffer;
    MetaDonnee MD;
    int k=0;
    Index X;
    //Lecture des metadonnees
    int nbBlocs = lireEntete(F,2);
    int premiereAdresse = lireEntete(F,4);
    int nbEnregistrements = lireEntete(F,3);
    int i = premiereAdresse;

    while(i!=-1){
        if(checkBlock(i)){ //Verifie si le bloc est valide
            memcpy(&buffer,&disk[i],sizeof(Bloc)); //Copie du bloc
            X.id=buffer.chainee.enregistrement[0].ID;
            X.numBloc= i; //Position du bloc pas de l'enregistrement
            memcpy(&tableIndex[k],&X,sizeof(Index)); //Ajout dans la table d'index
            k++;
            i=buffer.chainee.next;
        }
        else{
            i=-1;
        }
    }
    printf("\n la table d'index non dense a ete cree avec succes.");
}
void initializeIndexFiles(){
    for(int i=0; i<MAX_FILES; i++){
        for(int j=0;j<MAX_INDEX_ENTRIES;j++){
            tablesIndex[i][j].id=-1;
            tablesIndex[i][j].numBloc=-1;
        }
    }
}
void sauvegardeTableIndex(fichier *F, Index tableindex[100]){ //Mettre la table d'index en "MS"
    //Lecture des metadonnees
    int i;
    printf("\nTest sauvegarde\n");
    char filename[30];
    lireNomFichier(*F,filename);
    int nbBlocs = lireEntete(*F,2);
    int nbEnregistrements = lireEntete(*F,3);
    rechercheNomFichier(F,filename,&i);
    if (i == -1) { printf("Erreur : Pas d'espace disponible ou fichier inexistant pour un nouveau fichier dans la MS.\n"); return; }
    else{
        int j=0;
        while(j<nbEnregistrements){
            tablesIndex[i][j].id= tableindex[j].id;
            tablesIndex[i][j].numBloc = tableindex[j].numBloc;
            j++;
        }
    }
}
/*void rechercheTableIndex(fichier *F, int *i){
    char filename[30], filename2[30],nomIndex[36] = "Index";
    lireNomFichier(*F,filename);
    strcat(nomIndex,filename);
    tablesIndex = fopen("tablesIndex.bin","rb+");
    if (tablesIndex == NULL) { perror("Erreur lors de l'ouverture du fichier tablesIndex.bin dans la fct rechercheIndex\n"); *i = -1; F->TableIndex = NULL; return; }
    F->TableIndex = fopen(nomIndex,"rb+");
    if (F->TableIndex == NULL) { perror("Erreur lors de l'ouverture du fichier d'index dans la fct rechercheTableIndex\n"); fclose(tablesIndex); *i = -1; return; }
    char C;
    int position;
    *i=0;
    Index buffer;
    while(fread(&C,sizeof(char),1,tablesIndex)){
        if(C==filename[0]){
            fseek(tablesIndex,-1*sizeof(char),SEEK_CUR);
            if (fread(&filename2, sizeof(filename2), 1, tablesIndex) != 1) { perror("Erreur de lecture du fichier tablesIndex.bin dans la fct rechercheIndex\n"); break; }
            position = ftell(tablesIndex);
            if(strcmp(filename2,filename)==0){
                *i = (position + sizeof(filename2)) / sizeof(Index);
                while(fread(&buffer,sizeof(Index),1,tablesIndex)==1){
                    fwrite(&buffer,sizeof(Index),1,F->TableIndex);
                }
                fclose(tablesIndex);
                return;
            }
        }
    }
    fclose(tablesIndex);
    *i=-1;
}*/
void chargementFichierIndexDense(fichier  *F, Index tableIndexDense [100]){
    int i;
    char filename[30];
    lireNomFichier(*F,filename);
    int nbBlocs = lireEntete(*F,2);
    int nbEnregistrements = lireEntete(*F,3);
    rechercheNomFichier(F,filename,&i);
    if (i == -1) { printf("Erreur : Pas d'espace disponible ou fichier inexistant pour un nouveau fichier dans la MS.\n"); return; }
    else{
        int j=0;
        while(j<nbEnregistrements){
            tableIndexDense[j].id = tablesIndex[i][j].id;
            tableIndexDense[j].numBloc = tablesIndex[i][j].numBloc;
            j++;
        }
    }
}
void chargementFichierIndexNonDense(fichier *F, Index tableIndexNonDense[100]){
    int i;
    char filename[30];
    lireNomFichier(*F,filename);
    int nbBlocs = lireEntete(*F,2);
    int nbEnregistrements = lireEntete(*F,3);
    rechercheNomFichier(F,filename,&i);
    if (i == -1) { printf("Erreur : Pas d'espace disponible ou fichier inexistant pour un nouveau fichier dans la MS.\n"); return; }
    else{
        int j=0;
        while(j<nbBlocs){
            tableIndexNonDense[j].id = tablesIndex[i][j].id;
            tableIndexNonDense[j].numBloc = tablesIndex[i][j].numBloc;
            j++;
        }
    }
}
void rechercheEnregistrementDense(fichier *F, int ID, int *numBloc, int *deplacement) {
    rewind(F->MDfile);
    int nbrBlocs = lireEntete(*F,2);
    int nbEngis = lireEntete(*F, 3); //Nb enregistrements
    int gauche = 0, droite = nbEngis - 1, milieu; //Dichotomie
    Index tableIndex[100];
    chargementFichierIndexDense(F,tableIndex); //Charger le fichier d'index en MC

    while (gauche <= droite) {

        milieu = (gauche + droite) / 2;
        // Lire l'entr�e de l'index au milieu

        if (tableIndex[milieu].id == ID) {
            *numBloc = tableIndex[milieu].numBloc; //Numero du bloc
            for(int i=0; i<BLOCK_SIZE; i++){ //Numero de deplacement (position de l'enregistrement dans le bloc)
                if(lireEnteteGlobal(*F)==Chainee){
                    if(disk[*numBloc].chainee.enregistrement[i].ID==ID){
                        *deplacement=i;
                        break;
                    }
                }
                else{
                    if(disk[*numBloc].contigue.enregistrement[i].ID==ID){
                        *deplacement=i;
                        break;
                    }
                }
            }
            printf("\nEnregistrement trouve , numbloc:%d , deplacement:%d \n", *numBloc, *deplacement);
            return;
        } else if (tableIndex[milieu].id < ID) {
            gauche = milieu + 1;
        } else {
            droite = milieu - 1;
        }
    }

    if(gauche>milieu){
        *numBloc=tableIndex[gauche].numBloc; //Dans le cas ou l'enregistrement n'a pas ete trouve, on retourne l'endroit dans lequel il devrait se trouver
    int i=0; *deplacement=-1;
    if(lireEnteteGlobal(*F)==Chainee){
        do{
            while(i<BLOCK_SIZE && disk[*numBloc].chainee.enregistrement[i].ID!=0) //On l'ajoute a la fin
                i++;
            if(i<BLOCK_SIZE)
                *deplacement=i;
            else
                *numBloc=disk[*numBloc].chainee.next;
            if(*numBloc==-1){
                *numBloc=AllouerBlocChainee(0);
                *deplacement=-1;
            }
        }while(*deplacement==-1 && *numBloc!=-1); //Si numBloc= -1 alors il n'existe pas de bloc libre
    }
    else{
        do{
            while(i<BLOCK_SIZE && disk[*numBloc].contigue.enregistrement[i].ID!=0)
                i++;
            if(i<BLOCK_SIZE)
                *deplacement=i;
            else
                (*numBloc)++;
            if(*numBloc+lireEntete(*F,4)>nbrBlocs){ //Cas ou on depasse la taille du fichier
                if(checkBlockContigue((*numBloc)+1)==false){ //SI le bloc suivant est libre
                    *numBloc=AllouerBlocContigue((*numBloc));
                    *deplacement=0;
                }
                else{
                    *numBloc=-1; *deplacement=-2; //Espace insuffisant
                }
            }
        }while(*deplacement==-1);
        if(*deplacement==-2) *deplacement=-1;
    }
    printf("\nEnregistrement non trouve\n");
    }

}
void rechercheEnregistrementNonDense(fichier *F, int ID, int *numbloc, int *deplacement) {
    Bloc bloc;
    int nbrBlocs = lireEntete(*F, 2); //nbBlocs
    int gauche = 0, droite = nbrBlocs - 1, milieu; //Dichotomie
    typeOrganisation type = lireEnteteGlobal(*F); //Chaine ou contigu
    Index tableIndex[100];
    chargementFichierIndexDense(F,tableIndex); //Charger le fichier d'index en MC

    while (gauche <= droite) {
        milieu = (gauche + droite) / 2;

        // Lire l'entr�e de l'index au milieu

        if (tableIndex[milieu].id == ID) {

            *numbloc = tableIndex[milieu].numBloc;
            for(int i=0; i<BLOCK_SIZE; i++){ //Numero de deplacement (position de l'enregistrement dans le bloc)
                if(lireEnteteGlobal(*F)==Chainee){
                    if(disk[*numbloc].chainee.enregistrement[i].ID==ID){
                        *deplacement=i;
                        break;
                    }
                }
                else{
                    if(disk[*numbloc].contigue.enregistrement[i].ID==ID){
                        *deplacement=i;
                        break;
                    }
                }
            }
            printf("\nEnregistrement trouve , numbloc:%d , deplacement:%d \n", *numbloc, *deplacement);
            return;
        } else if (tableIndex[milieu].id< ID) {
            gauche = milieu + 1;
        } else {
            droite = milieu - 1;
        }
    }
    if(gauche>droite){
        *numbloc=tableIndex[gauche].numBloc; //Dans le cas ou l'enregistrement n'a pas ete trouve, on retourne l'endroit dans lequel il devrait se trouver
        int i=0;
        if(lireEnteteGlobal(*F)==Chainee){
            while(disk[*numbloc].chainee.enregistrement[i].ID<ID)
                i++;
            *deplacement=i;
        }
        else{
            while(disk[*numbloc].contigue.enregistrement[i].ID<ID)
                i++;
            *deplacement=i;
        }
        printf("\nEnregistrement non trouve\n");
    }

}
int getIndexSize(Index tableIndex[]){
    int i=0;
    while(tableIndex[i].numBloc!=-1)
        i++;
    return i;
}
void removeIndexTable(fichier *F){
     int i;
    char filename[30];
    lireNomFichier(*F,filename);
    int nbBlocs = lireEntete(*F,2);
    int nbEnregistrements = lireEntete(*F,3);
    rechercheNomFichier(F,filename,&i);
    if (i == -1) { printf("Erreur : Pas d'espace disponible ou fichier inexistant pour un nouveau fichier dans la MS.\n"); return; }
    for(int j=0;j<MAX_INDEX_ENTRIES;j++){
            tablesIndex[i][j].id=-1;
            tablesIndex[i][j].numBloc=-1;
    }
}
void MajTableIndexDense(fichier *F, int mode, int ID, int numbloc) {
    int nbEngis = lireEntete(*F, 3);
    int nbrBlocs = lireEntete(*F, 2);
    int premierAdresse = lireEntete(*F, 4);
    int i, deplacement, j, adresse;
    Index tableIndex[100];
    chargementFichierIndexDense(F, tableIndex); // Charger le fichier d'index en MC

    if (mode < 1 || mode > 3) {
        printf("\nOperation invalide\n");
    }

    if (mode == 1) { // cas de suppression
        i = 0;
        while (i < nbEngis) {
            rechercheEnregistrementDense(F, ID, &adresse, &deplacement);
            tableIndex[i + 1].numBloc = adresse;
            memcpy(&tableIndex[i], &tableIndex[i + 1], sizeof(Index));
            i++;
        }

        // v�rifications
        if (lireEnteteGlobal(*F) == Contigue) {
            int k = 0;
            for (i = premierAdresse; i < premierAdresse + nbrBlocs; i++) {
                for (j = 0; j < nbEngis; j++) {
                    if (disk[i].contigue.enregistrement[j].ID ==tableIndex[k].id) {
                        tableIndex[k].numBloc = i;
                        k++;
                    }
                    }
                }
            }
            else {
            int k = 0;
            i = numbloc;
            while (disk[i].chainee.next != -1) {
                j = 0;
                while (j < nbEngis) {
                    if (disk[i].chainee.enregistrement[j].ID == tableIndex[k].id) {
                        tableIndex[k].numBloc = i;
                    }
                    k++;
                    j++;
                }
                i = disk[i].chainee.next;
            }
        }
        sauvegardeTableIndex(F, tableIndex);
    } else if (mode == 2) { // cas d'insertion
        i = 0;
        while (ID > tableIndex[i].id) {
            i++;
        }
        int t = nbEngis;
        while (t > i) {
            memcpy(&tableIndex[t], &tableIndex[t - 1], sizeof(Index));
            t--;
        }
        Index index;
        index.id = ID;
        index.numBloc = numbloc;
        memcpy(&tableIndex[i], &index, sizeof(Index));

        if (lireEnteteGlobal(*F) == Contigue) {
            int k = 0;
            for (i = premierAdresse; i < premierAdresse + nbrBlocs; i++) {
                for (j = 0; j < nbEngis; j++) {
                    if (disk[i].contigue.enregistrement[j].ID == tableIndex[k].id) {
                        tableIndex[k].numBloc = i;
                    }
                    k++;
                }
            }
        } else {
            int k = 0;
            i = numbloc;
            while (disk[i].chainee.next != -1) {
                j = 0;
                while (j < nbEngis) {
                    if (disk[i].chainee.enregistrement[j].ID == tableIndex[k].id) {
                        tableIndex[k].numBloc = i;
                    }
                    k++;
                    j++;
                }
                i=disk[i].chainee.next;
            }
        }
        sauvegardeTableIndex(F, tableIndex);
    } else if (mode == 3) { // cas de changement d'adresse
        rechercheEnregistrementDense(F, ID, &j, &deplacement);
        tableIndex[j].numBloc = numbloc;
        sauvegardeTableIndex(F, tableIndex);
    }
}

void MajTableIndexNonDense(fichier *F, int mode, int ID, int numbloc) {
    int nbrBlocs = lireEntete(*F, 2); // Lire le nombre de blocs
    int i, deplacement, j;
    rewind(F->TableIndex);
    Index tableIndex[100];
    chargementFichierIndexNonDense(F, tableIndex); // Charger le fichier d'index en MC

    if (mode < 1 || mode > 3) {
        printf("\nOperation invalide\n");
    }

    if (mode == 1) { // cas de suppression
        rechercheEnregistrementNonDense(F, ID, &j, &deplacement); // Trouver l'adresse de l'enregistrement � supprimer
        i = 0;
        while (j < nbrBlocs) {
            memcpy(&tableIndex[j], &tableIndex[j + 1], sizeof(Index));
            i++;
        }
    } else if (mode == 2) { // cas d'insertion
        i = 0;
        while (ID > tableIndex[i].id) {
            i++;
        }
        int k = nbrBlocs;
        while (k > i) {
            memcpy(&tableIndex[k], &tableIndex[k - 1], sizeof(Index));
            k--;
        }
        Index index;
        index.id = ID;
        index.numBloc = numbloc;
        memcpy(&tableIndex[i], &index, sizeof(Index));
    } else if (mode == 3) { // cas de changement d'adresse
        rechercheEnregistrementNonDense(F, ID, &j, &deplacement);
        tableIndex[j].numBloc = numbloc;
        sauvegardeTableIndex(F, tableIndex);
    }
}
