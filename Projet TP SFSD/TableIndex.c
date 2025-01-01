#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "TextColor.h"
#include "Metadonnees.h"
#include "TableIndex.h"
#include "Disk.h"

void creationTableIndexDenseContigue(fichier F, Index densetableIndex []){
    Bloc buffer;
    MetaDonnee MD;
    int m,k=0;
    Index X;
    int nbBlocs = lireEntete(F,2);
    int premiereAdresse = lireEntete(F,4);
    int nbEnregistrements = lireEntete(F,3);
    for(int i=premiereAdresse; i<=premiereAdresse+nbBlocs; i++){
        rewind(F.MDfile);
        if(checkBlockContigue(i) == true ){
            memcpy(&buffer, &disk[i],sizeof(Bloc)); //Copie du bloc
            fread(&MD, sizeof(MetaDonnee),1,F.MDfile);
            for(int j=0;j<BLOCK_SIZE && k<nbEnregistrements;j++){
                X.id=buffer.contigue.enregistrement[j].ID;
                X.numBloc= i; //Position du bloc pas de l'enregistrement
                m=0;
                while(m<k && X.id > densetableIndex[m].id){ //Recherche de la position dans laquelle inserer
                    m++;
                }
                for(int n=k; n>m; n++){ //Decalage
                    densetableIndex[n]=densetableIndex[n-1];
                }
                memcpy(&densetableIndex[m],&X, sizeof(Index)); //Copie d'enregistrement
                k++;
        }
  }
  printf("\n la table d'index à été crée avec succes.");
}
}
void creationTableIndexDenseChainee(fichier F, Index densetableIndex []){
    Bloc buffer;
    MetaDonnee MD;
    int m,k=0;
    Index X;
    int nbBlocs = lireEntete(F,2);
    int premiereAdresse = lireEntete(F,4);
    int nbEnregistrements = lireEntete(F,3);
    int i=premiereAdresse;

    while(i!=-1){
        rewind(F.MDfile);
        if(checkBlock(i) == true ){
            memcpy(&buffer, &disk[i],sizeof(Bloc)); //Copie du bloc
            fread(&MD, sizeof(MetaDonnee),1,F.MDfile);
            for(int j=0;j<BLOCK_SIZE && k<nbEnregistrements;j++){
                X.id=buffer.chainee.enregistrement[j].ID;
                X.numBloc= i; //Position du bloc pas de l'enregistrement
                i=buffer.chainee.next;
                m=0;
                while(m<k && X.id > densetableIndex[m].id){ //Recherche de la position dans laquelle inserer
                    m++;
                }
                for(int n=k; n>m; n++){ //Decalage
                    densetableIndex[n]=densetableIndex[n-1];
                }
                memcpy(&densetableIndex[m],&X, sizeof(Index)); //Copie d'enregistrement
                k++;
            }
        }
    }
    printf("\n la table d'index à été crée avec succes.");
}
void creationTableIndexNonDenseContigue (fichier F, Index tableIndex []){
    Bloc buffer;
    MetaDonnee MD;
    int k=0;
    Index X;
    //Lecture des metadonnees
    int nbBlocs = lireEntete(F,2);
    int premiereAdresse = lireEntete(F,4);
    int nbEnregistrements = lireEntete(F,3);

    for(int i=premiereAdresse;i<=premiereAdresse+nbBlocs;i++){
        if(checkBlockContigue(i)){ //Verifie si le bloc est valide
            memcpy(&buffer,&disk[i],sizeof(Bloc)); //Copie du bloc
            rewind(F.MDfile);
            fread(&MD,sizeof(MetaDonnee),1,F.MDfile);
            X.id=buffer.contigue.enregistrement[0].ID;
            X.numBloc= i; //Position du bloc pas de l'enregistrement
            memcpy(&tableIndex[k],&X,sizeof(Index)); //Ajout dans la table d'index
            k++;
        }
    }
    printf("\n la table d'index non dense à été crée avec succes.");
 }
void creationTableIndexNonDenseChainee (fichier F, Index tableIndex []){
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
            rewind(F.MDfile);
            fread(&MD,sizeof(MetaDonnee),1,F.MDfile);
            X.id=buffer.chainee.enregistrement[0].ID;
            X.numBloc= i; //Position du bloc pas de l'enregistrement
            i=buffer.chainee.next;
            memcpy(&tableIndex[k],&X,sizeof(Index)); //Ajout dans la table d'index
            k++;
        }
    }
    printf("\n la table d'index non dense à été crée avec succes.");
}
void sauvegardeTableIndex(fichier *F, Index tableindex[]){ //Mettre la table d'index en "MS"
    //Lecture des metadonnees
    char filename[30];
    lireNomFichier(*F,filename);
    int nbBlocs = lireEntete(*F,2), i;
    int nbEnregistrements = lireEntete(*F,3);
    rechercheTableIndex(F, &i);
    if(i==0){ //Si le fichier n'existe pas
        tablesIndex = fopen("tablesIndex.bin","ab+");
        fwrite(&filename,sizeof(filename),1,tablesIndex);
    }
    else{ //Cas de mise a jour
        tablesIndex = fopen("tablesIndex.bin","rb+");
        fseek(tablesIndex,i*sizeof(char),SEEK_SET);
    }
    if(liretypeTri(*F)==true){ //Cas d'un fichier non dense
        Index buffer[nbBlocs]; //La taille d'une table d'index non dense correspond au nombre de blocs utilises
        for(int i=0; i<nbBlocs; i++){
            buffer[i].id = tableindex[i].id;
            buffer[i].numBloc=tableindex[i].numBloc;
        }
        if(i==0) fwrite(&buffer, sizeof(buffer),1,F->TableIndex);
        fwrite(&buffer, sizeof(buffer),1,tablesIndex);
        }
    else{ //Cas d'un fichier dense
        Index buffer[nbEnregistrements];
        int j,k=0;
        for(int i=0;i<nbEnregistrements;i++){
            buffer[i].id = tableindex[i].id;
            buffer[i].numBloc=tableindex[i].numBloc;
        }
        if (i==0) fwrite(&buffer, sizeof(buffer),1,F->TableIndex);
        fwrite(&buffer, sizeof(buffer),1,tablesIndex);
    }
    fclose(tablesIndex);
}
void rechercheTableIndex(fichier *F, int *i){
    char filename[30], filename2[30],nomIndex[36] = "Index";
    lireNomFichier(*F,filename);
    strcat(nomIndex,filename);
    tablesIndex = fopen("tablesIndex.bin","rb+");
    F->TableIndex = fopen(nomIndex,"rb+");
    char C;
    int position;
    *i=0;
    Index buffer;
    while(fread(&C,sizeof(char),1,tablesIndex)){
        if(C==filename[0]){
            fseek(tablesIndex,-1*sizeof(char),SEEK_CUR);
            fread(&filename2,sizeof(filename2),1,tablesIndex);
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
}
void chargementFichierIndexDense(fichier  *F, Index tableIndexDense [100]){
        rewind(F->TableIndex);
        int nbBlocs = lireEntete(*F,2);
        int nbrEngis = lireEntete(*F,3);
        Index buffer;
        int k=0;
        for(int i=0;i<nbBlocs;i++){
            fread(&buffer,sizeof(Index),1,F->TableIndex);
            int j=0;
            while(j<BLOCK_SIZE && k<nbrEngis){
                tableIndexDense[k].id=buffer.id;
                tableIndexDense[k].numBloc=buffer.numBloc;
                j++;
                k++;
            }
        }
}
void chargementFichierIndexNonDense(fichier *F, Index tableIndexNonDense[]){
    rewind(F->TableIndex);
        int nbrBlocs = lireEntete(*F,2); //Nombre de blocs
        Index buffer;

        for(int j=0;j<nbrBlocs;j++){
            fread(&buffer,sizeof(Index),1,F->TableIndex);
            tableIndexNonDense[j].id=buffer.id;
            tableIndexNonDense[j].numBloc=buffer.numBloc;
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

        // Lire l'entrée de l'index au milieu

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
            printf("\nEnregistrement trouvé , numbloc:%d , déplacement:%d \n", *numBloc, *deplacement);
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
                *numBloc=AllouerBlocChainee();
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
    printf("\nEnregistrement non trouvé\n");
    }

}
void rechercheEnregistrementNonDense(fichier *F, int ID, int *numbloc, int *deplacement) {
    Bloc bloc;
    rewind(F->MDfile);
    int nbrBlocs = lireEntete(*F, 2); //nbBlocs
    int gauche = 0, droite = nbrBlocs - 1, milieu; //Dichotomie
    typeOrganisation type = lireEnteteGlobal(*F); //Chaine ou contigu
    Index tableIndex[100];
    chargementFichierIndexDense(F,tableIndex); //Charger le fichier d'index en MC

    while (gauche <= droite) {
        milieu = (gauche + droite) / 2;

        // Lire l'entrée de l'index au milieu

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
            printf("\nEnregistrement trouvé , numbloc:%d , déplacement:%d \n", *numbloc, *deplacement);
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
        printf("\nEnregistrement non trouvé\n");
    }

}

void removeIndexTable(fichier *F){

    char filename[30], filename2[30],nomIndex[36] = "Index";
    lireNomFichier(*F,filename);
    strcat(nomIndex,filename);
    char C; int i;
    bool found=false;
    Index buffer;
    rechercheTableIndex(F,&i);
    fclose(F->TableIndex);
    remove(nomIndex); //Supprimer physiquement le fichier d'index
    FILE *temp = fopen("temp.bin","wb+");
    tablesIndex = fopen("tablesIndex.bin","rb+");

    while(fread(&C,sizeof(char),1,tablesIndex)){ //Lis le fichier caractere par caractere
        if(C==filename[0]){
            fseek(tablesIndex,-1*sizeof(char),SEEK_CUR);
            fread(&filename2,sizeof(filename2),1,tablesIndex);
            if(strcmp(filename2,filename)!=0){
                fwrite(&filename2, sizeof(filename2),1,temp);
            }
            else{
                found=true;
                while(fread(&buffer,sizeof(Index),1,tablesIndex)==1){
                    //Ignore
                }
            }
        }
        else{
            fwrite(&C,sizeof(char),1,temp);
        }
    }
    fclose(temp); fclose(tablesIndex);
    if(found){
        remove("tablesIndex.bin");
        rename("temp.bin", "tablesIndex.bin");
    }
    else{
        printf("\nFichier d'index non trouve");
    }
}

void MajTableIndexDense(fichier *F, int mode,int ID, int numbloc){
    int nbEngis=lireEntete(*F,3);
    int i,deplacement,j;
    rewind(F->TableIndex);
    Index tableIndex[100];
    chargementFichierIndexDense(F,tableIndex); //Charger le fichier d'index en MC
    if(mode<1 && mode>3){
        printf("\nOperation invalide\n");
    }

    if(mode == 1){ // cas de suppression
        rechercheEnregistrementDense(F,ID,&j,&deplacement);
        i=0;
        while(j< MAX_BLOCKS && i<nbEngis){
            memcpy(&tableIndex[i],&tableIndex[i+1],sizeof(Index));
            i++;
        }
    }
   else if(mode == 2){ // cas d'insertion
            i=0;
    while(ID>tableIndex[i].id){
        i++;
      int  k=nbEngis;
        while(k>i){
            memcpy(&tableIndex[k],&tableIndex[k-1],sizeof(Index));
            k--;
        }
      //  memcpy(&tableIndex[i],&index,sizeof(Index));
    }

    }
    else if(mode == 3){  // cas de changement d'adresse
            rechercheEnregistrementDense(&F,ID,&j,&deplacement);
        tableIndex[j].numBloc=numbloc;
    }


}

void MajTableIndexNonDense(fichier *F, int mode,int ID, int numbloc) {
    int nbrBlocs = lireEntete(*F, 2); //Lire le nbre de blocs
    int i, deplacement, j;
    rewind(F->TableIndex);
    Index tableIndex[100];
    rechercheTableIndex(F, &i); //Charger le fichier
    chargementFichierIndexNonDense(F, tableIndex); // Charger le fichier d'index en MC

    if (mode < 1 || mode > 3) {
        printf("\nOperation invalide\n");
    }

    if (mode == 1) { // cas de suppression
        rechercheEnregistrementNonDense(&F, ID, &j, &deplacement); //Trouver l'adresse de l'enregistrement a supprimer
        i = 0;
        while (j < nbrBlocs) {
            memcpy(&tableIndex[j], &tableIndex[j + 1], sizeof(Index));
            i++;
        }
    }
    else if (mode == 2) { // cas d'insertion
        i = 0;
        while (ID > tableIndex[i].id) {
            i++;
            int k = nbrBlocs;
            while (k > i) {
                memcpy(&tableIndex[k], &tableIndex[k - 1], sizeof(Index));
                k--;
            }
        }
    }
    else if (mode == 3) {
        rechercheEnregistrementNonDense(&F, ID, &j, &deplacement);
        tableIndex[j].numBloc = numbloc;
    }
}
