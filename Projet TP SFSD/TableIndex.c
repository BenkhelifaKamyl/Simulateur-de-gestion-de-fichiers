#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "TextColor.h"
#include "Metadonnees.h"
#include "TableIndex.h"
#include "Disk.h"

void creationTableIndexDense(Index densetableIndex []){
    Bloc buffer;
    MetaDonnee MD;
    int m,k=0;
    Index X;
    fichier F;
    for(int i=0;i<MAX_BLOCKS;i++){
        if(checkBlock(i) == true ){
            memcpy(&buffer, &disk[i],sizeof(Bloc)); //Copie d'enregistrement
            rechercheFichierMeta(i,&F);
            fread(&MD, sizeof(MetaDonnee),1,F.MDfile);
            for(int j=0;j<BLOCK_SIZE;j++){
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
 /*void creeTableIndexNonDense (FILE *disk, FILE *f, Index tableIndex []){
    rewind(f);
    Bloc buffer;
    for(int i=0;i<MAX_BLOCKS;i++){
        if(checkblock(i)){
            fread(&buffer,sizeof(Bloc),1,disk);
            tableIndex[i]=buffer.enregistrement[0].ID;
            tableIndex[i].numbloc=buffer.adresse;
        }
    }
}
*/
 /*void sauvegardeTableIndex(FILE *disk, FILE *fmeta, FILE *findex, Index tableindex[]){
    findex=fopen("TableIndex.bin","wb+");
    int nbentrees = lireEntete(*fmeta,2);
    if(findex == NULL){
        printf("Impossible d'ouvrir le fichier");
        return -1;
    }
    Index buffer [FacteurBlocage];
    int j,k=0;
    for(int i=0;i<nbentrees;i++){
        j=0;
        while(k<nbentrees && j<BLOCK_SIZE){
            buffer[j].ID = tableindex[k].ID;
            buffer[j].numbloc=tableindex[k].numbloc;
            j++; k++;
        }
        fwrite(&buffer,sizeof(Index),1,findex);
    }
    fclose(findex);
 }
*/
