#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "TextColor.h"
#include "Metadonnees.h"
#include "TableIndex.h"
#include "Disk.h"



void creationTableIndexDense(FILE *disque, FILE *f , Index densetableIndex []){
   rewind(f);
   Bloc buffer;
   int pos=0;
   int m,k=0;
   Index X;
  for(int i=0;i<MAX_BLOCKS;i++){

    if(checkBlock(i) == true ){
fread(&buffer, sizeof(Bloc),1,disque);
for(int j=0;j<BLOCK_SIZE;j++){
         
         X.ID=buffer.enregistrement[j].ID;
            X.numbloc= pos*FacteurBlocage +j;
            m=0;
            while(m<k && X.ID > densetableIndex[m].ID){
                m++;
            }
            if(m==k){
                densetableIndex[k].ID=X.ID;
                densetableIndex[k].numbloc=X.numbloc;
                k++;
            }
            else{

                for(int n=m-1;n<k;n++){

                    densetableIndex[n+1].numbloc=densetableIndex[n].numbloc;
                    densetableIndex[n+1].ID=densetableIndex[n].ID;
                
                }
                densetableIndex[m-1].ID=X.ID;
                densetableIndex[m-1].numbloc=X.numbloc;
            }
            }
            pos++;
    }
  }
  printf("\n la table d'index à été crée avec succes.");

}
    
