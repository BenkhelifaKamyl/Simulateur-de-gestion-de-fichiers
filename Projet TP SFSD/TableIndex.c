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
    i = premiereAdresse;

    while(i!=-1){
        if(checkblock(i)){ //Verifie si le bloc est valide
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
    if(liretypeTri(F)==true){ //Cas d'un fichier non dense
        Index buffer[nbBlocs]; //La taille d'une table d'index non dense correspond au nombre de blocs utilises
        for(int i=0; i<nbBlocs; i++){
            buffer[i].ID = tableindex[i].ID;
            buffer[i].numBloc=tableindex[i].numbloc;
        }
        if(i==0) fwrite(&buffer, sizeof(buffer),1,F.TableIndex);
        fwrite(&buffer, sizeof(buffer),1,tablesIndex);
        }
    else{ //Cas d'un fichier dense
        Index buffer[nbEnregistrements];
        int j,k=0;
        for(int i=0;i<nbEnregistrements;i++){
            buffer[i].ID = tableindex[i].ID;
            buffer[i].numBloc=tableindex[i].numbloc;
        }
        if (i==0) fwrite(&buffer, sizeof(buffer),1,F.TableIndex);
        fwrite(&buffer, sizeof(buffer),1,tablesIndex);
    }
    fclose(tablesIndex);
}
void rechercheTableIndex(fichier *F, int *i){
    char filename[30], filename2[30],nomIndex[36] = "Index";
    lireNomFichier(*F,filename);
    strcat(nomIndex,filename);
    tablesIndex = fopen("tablesIndex.bin","rb+");
    F.TableIndex = fopen(nomIndex,"rb+");
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
void chargementFichierIndexDense(fichier  *F, Index tableIndexDense []){
        rewind(F->TableIndex);
        if(liretypeTri(F) == true){
            chargementFichierIndexNonDense(F,  tableIndexNonDense[]);   // le fichier est nondense donc il exécute l'autre procédure
        }
        else{
            int nbBlocs = lireEntete(F.MDfile,2);
            int nbrEngis = lireEntete(F.MDfile,3);
            Index buffer;
            int k=0;
            for(int i=0;i<nbBlocs;i++){
                fread(&buffer,sizeof(Index),1,F.TableIndex);
                int j=0;
                while(j<BLOCK_SIZE && k<nbEngis){
                    tableIndexDense[k].ID=buffer[j].ID;
                    tableIndexDense[k].numbloc=buffer[j].numbloc;
                    j++;
                    k++;
                }
            }
        }
}
void chargementFichierIndexNonDense(fichier *F, Index tableIndexNonDense[]){
    rewind(F->TableIndex);
    if(liretypeTri(F) == false){
        chargementFichierIndexDense(F, tableIndexDense[]); // le fichier est dense donc il exécute l'autre procédure
    }
    else{
        int nbrBlocs = lireEntete(F.MDfile,2); //Nombre de blocs
        Index buffer;

        for(int j=0;j<nbrBlocs;j++){
            fread(&buffer,sizeof(Index),1,F.TableIndex);
            tableIndexNonDense[j].ID=buffer[0].ID;
            tableIndexNonDense[j].numbloc=buffer[0].numbloc;
        }
    }
}
void rechercheEnregistrementDense(fichier *F, int ID, int *numBloc, int *deplacement) {
    rewind(F->MDfile);
    int nbEngis = lireEntete(F, 3); //Nb enregistrements
    int gauche = 0, droite = nbEngis - 1, milieu; //Dichotomie
    Index tableIndex[];
    chargementFichierIndexDense(F,tableIndex); //Charger le fichier d'index en MC

    while (gauche <= droite) {
        milieu = (gauche + droite) / 2;

        // Lire l'entrée de l'index au milieu

        if (tableIndex[milieu].ID == ID) {
            *numBloc = tableIndex[milieu].numbloc; //Numero du bloc
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
        } else if (tableIndex[milieu].ID < ID) {
            gauche = milieu + 1;
        } else {
            droite = milieu - 1;
        }
    }

    *numbloc=tableIndex[gauche].numBloc; //Dans le cas ou l'enregistrement n'a pas ete trouve, on retourne l'endroit dans lequel il devrait se trouver
    int i=0; *deplacement=-1;
    if(lireEnteteGlobal(*F)==Chainee){
        do{
            while(i<BLOCK_SIZE && disk[*numbloc].chainee.enregistrement[i].ID!=0) //On l'ajoute a la fin
                i++;
            if(i<BLOCK_SIZE)
                *deplacement=i;
            else
                *numbloc=disk[*numbloc].chainee.next;
            if(*numbloc==-1){
                *numbloc=AllouerBlocChainee();
                *deplacement=-1;
            }
        }while(*deplacement==-1 && *numBloc!=-1); //Si numBloc= -1 alors il n'existe pas de bloc libre
    }
    else{
        do{
            while(i<BLOCK_SIZE && disk[*numbloc].contigue.enregistrement[i].ID!=0)
                i++;
            if(i<BLOCK_SIZE)
                *deplacement=i;
            else
                (*numbloc)++;
            if(*numbloc+lireEntete(*F,4)>nbrBlocs){ //Cas ou on depasse la taille du fichier
                if(checkBlockContigue((*numbloc)+1)==false){ //SI le bloc suivant est libre
                    *numbloc=AllouerBlocContigue((*numBloc));
                    *deplacement=0;
                }
                else{
                    *numbloc=-1; *deplacement=-2; //Espace insuffisant
                }
            }
        }while(*deplacement==-1);
        if(*deplacement==-2) *deplacement=-1;
    }
    printf("\nEnregistrement non trouvé\n");
}
void rechercheEnregistrementNonDense(fichier *F, int ID, int *numbloc, int *deplacement) {
    Bloc bloc;
    rewind(F->MD.file);
    int nbrBlocs = lireEntete(F, 2); //nbBlocs
    int gauche = 0, droite = nbrBlocs - 1, milieu; //Dichotomie
    typeOrganisation type = lireEnteteGlobal(*F); //Chaine ou contigu
    Index tableIndex[];
    chargementFichierIndexDense(F,tableIndex); //Charger le fichier d'index en MC

    while (gauche <= droite) {
        milieu = (gauche + droite) / 2;

        // Lire l'entrée de l'index au milieu

        if (tableIndex[milieu].ID == ID) {
            *numbloc = tableIndex[milieu].numbloc;
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
            printf("\nEnregistrement trouvé , numbloc:%d , déplacement:%d \n", *numbloc, *deplacement);
            return;
        } else if (tableIndex[milieu].ID < ID) {
            gauche = milieu + 1;
        } else {
            droite = milieu - 1;
        }
    }
    *numbloc=tableIndex[gauche].numBloc; //Dans le cas ou l'enregistrement n'a pas ete trouve, on retourne l'endroit dans lequel il devrait se trouver
    int i=0; *deplacement=-1;
    if(lireEnteteGlobal(*F)==Chainee){
        do{
            while(i<BLOCK_SIZE && disk[*numbloc].chainee.enregistrement[i].ID<ID)
                i++;
            if(i<BLOCK_SIZE)
                *deplacement=i;
            else
                *numbloc=disk[*numbloc].chainee.next;
            if(*numbloc==-1){
                *numbloc=AllouerBlocChainee();
                *deplacement=0;
            }
        }while(*deplacement==-1 && *numbloc!=-1);
    }
    else{
        do{
            while(i<BLOCK_SIZE && disk[*numbloc].contigue.enregistrement[i].ID<ID)
                i++;
            if(i<BLOCK_SIZE)
                *deplacement=i;
            else
                (*numbloc)++;
            if(*numbloc+lireEntete(*F,4)>nbrBlocs){ //Cas ou on depasse la taille du fichier
                if(checkBlockContigue((*numbloc)+1)==false){ //SI le bloc suivant est libre
                    *numbloc=AllouerBlocContigue((*numbloc));
                    *deplacement=0;
                }
                else{
                    *numbloc=-1; *deplacement=-2; //Espace insuffisant
                }
            }
        }while(*deplacement==-1);
        if(*deplacement==-2) *deplacement=-1;
    }
    printf("\nEnregistrement non trouvé\n");
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


