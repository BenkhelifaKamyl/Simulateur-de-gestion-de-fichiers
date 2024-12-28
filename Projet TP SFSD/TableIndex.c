#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "TextColor.h"
#include "Metadonnees.h"
#include "TableIndex.h"
#include "Disk.h"

void creationTableIndexDense(fichier F, Index densetableIndex []){
    Bloc buffer;
    MetaDonnee MD;
    int m,k=0;
    Index X;
    int nbBlocs = lireEntete(F,2);
    int premiereAdresse = lireEntete(F,4);
    int nbEnregistrements = lireEntete(F,3);
    for(int i=premiereAdresse; i<=premiereAdresse+nbBlocs; i++){
        rewind(F.MDfile);
        if(checkBlock(i) == true ){
            memcpy(&buffer, &disk[i],sizeof(Bloc)); //Copie du bloc
            fread(&MD, sizeof(MetaDonnee),1,F.MDfile);
            for(int j=0;j<BLOCK_SIZE && k<nbEnregistrements;j++){
                if(MD.globalOrg==Chainee)
                    X.id=buffer.chainee.enregistrement[j].ID;
                else
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
void creeTableIndexNonDense (fichier F, Index tableIndex []){
    Bloc buffer;
    MetaDonnee MD;
    int k=0;
    Index X;
    //Lecture des metadonnees
    int nbBlocs = lireEntete(F,2);
    int premiereAdresse = lireEntete(F,4);
    int nbEnregistrements = lireEntete(F,3);

    for(int i=premiereAdresse;i<=premiereAdresse+nbBlocs;i++){
        if(checkblock(i)){ //Verifie si le bloc est valide
            memcpy(&buffer,&disk[i],sizeof(Bloc)); //Copie du bloc
            rewind(F.MDfile);
            fread(&MD,sizeof(MetaDonnee),1,F.MDfile);
            if(MD.globalOrg==Chainee)
                    X.id=buffer.chainee.enregistrement[0].ID;
            else
                    X.id=buffer.contigue.enregistrement[0].ID;
            X.numBloc= i; //Position du bloc pas de l'enregistrement
            memcpy(&tableIndex[k],&X,sizeof(Index)); //Ajout dans la table d'index
            k++;
        }
    }
    printf("\n la table d'index non dense à été crée avec succes.");
 }
void sauvegardeTableIndex(fichier *F, Index tableindex[]){
    //Lecture des metadonnees
    int nbBlocs = lireEntete(*F,2);
    int nbEnregistrements = lireEntete(*F,3);
    char nomFichier[30], nomIndex[36] = "Index";
    lireNomFichier(*F, nomFichier);
    strcat(nomIndex,nomFichier);
    tablesIndex = fopen("tablesIndex.bin","ab+");
    F.TableIndex = fopen(nomIndex,"wb+");

    if(liretypeTri(F)==true){ //Cas d'un fichier non dense
        Index buffer[nbBlocs]; //La taille d'une table d'index non dense correspond au nombre de blocs utilises
        for(int i=0; i<nbBlocs; i++){
            buffer[i].ID = tableindex[i].ID;
            buffer[i].numBloc=tableindex[i].numbloc;
        }
        fwrite(&buffer, sizeof(buffer),1,F.TableIndex);
        fwrite(&buffer, sizeof(buffer),1,tablesIndex);
    }
    else{ //Cas d'un fichier dense
        Index buffer[nbEnregistrements];
        int j,k=0;
        for(int i=0;i<nbEnregistrements;i++){
            buffer[i].ID = tableindex[i].ID;
            buffer[i].numBloc=tableindex[i].numbloc;
        }
        fwrite(&buffer, sizeof(buffer),1,F.TableIndex);
        fwrite(&buffer, sizeof(buffer),1,tablesIndex);
    }
    fclose(tablesIndex);
    fclose(F.TableIndex);
}

void chargementFichierIndexDense(fichier  *F, Index tableIndexDense []){
    
            if(liretypeTri(F) == true){
            chargementFichierIndexNonDense(g,  tableIndexNonDense[]);   // le fichier est nondense donc il exécute l'autre procédure
             }
             else{
             int nbBlocs = lireEntete(F.MDfile,2);
             int nbrEngis = lireEntete(F.MDfile,3);
              Index buffer[BLOCK_SIZE];
              for(int i=0;i<=nbBlocs-1;i++){
            fread(&buffer,sizeof(Index),1,F.TableIndex);
             int j=0; int k=0;
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


      if(liretypeTri(F) == false){
        chargementFichierIndexDense(F, tableIndexDense[]); // le fichier est dense donc il exécute l'autre procédure
    }
    else{
        int nbrBlocs = lireEntete(F.MDfile,2);
        Index buffer[nbrBlocs];
       
        for(int j=0;j<nbrBlocs;j++){
            fread(&buffer,sizeof(Index),1,F.TableIndex);
            tableIndexNonDense[j].ID=buffer[0].ID;
            tableIndexNonDense[j].numbloc=buffer[0].numbloc;
        }
    }
}

