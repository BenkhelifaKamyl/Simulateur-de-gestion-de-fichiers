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
        rewind(F->MDfile);
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
            memcpy(&buffer,disk[i],sizeof(Bloc)); //Copie du bloc
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

void chargementFichierIndexDense(FILE  tableIndex, Index tableIndexDense []){
         fichier *g ; // pour lire les fichiers d'index 
     int j;
rewind(tableIndex);

       for(int i=0;i<MAX_FILES;i++){
        fread(&g,sizeof(fichier),1,tableIndex); //lire un fichier  d'aprés le fichier tablesindex
        if(liretypeTri(g) == true){ // voir si le cas dense ou nondense
            chargementFichierIndexNonDense(g,  tableIndexNonDense[]);   // le fichier est nondense donc il exécute l'autre procédure
             }
             else{
             int nbrEngis = lireEntet(g,3);
              Index buffer[nbrEngis];
            fread(&buffer,sizeof(Index),1,g);
             
             for(j=0;j<nbrEngis;j++){
                 tableIndexDense[j].ID=buffer[j].ID;
                 tableIndexDense[j].numbloc=buffer[j].numbloc;

                       }
                }

       } 
}

void chargementFichierIndexNonDense(FILE tableIndex, Index tableIndexNonDense[]){
fichier *g; // pour lire les fichiers index 
rewind(tableIndex);
for(int i=0;i<MAX_FILES;i++){
fread(&g,sizeof(fichier),1,tableIndex); //lire un fichier  d'aprés le fichier tablesindex
    if(liretypeTri(g) == false){ // voir si le cas dense ou nondense
        chargementFichierIndexDense(g, tableIndexDense[]); // le fichier est dense donc il exécute l'autre procédure
    }
    else{
        int nbrBlocs = lireEntete(g,2);
        Index buffer[nbrBlocs];
        fread(&buffer,sizeof(Index),1,g);
        for(int j=0;j<nbrBlocs;j++){
            tableIndexNonDense[j].ID=buffer[j].ID;
            tableIndexNonDense[j].numbloc=buffer[j].numbloc;
        }
    }
}

}


