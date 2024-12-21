#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define FacteurBlocage 5
#define MAX_FILES 10        // Nombre maximum de fichiers gérés
#define BLOCK_SIZE 128      // Taille de chaque bloc en octets
#define MAX_BLOCKS 100      //Nombre maximal de blocs dans le disque

typedef struct{
    int ID;
    char Data[100];
    bool Supprime; //Pour la potentielle suppression logique
} Enregistrement;

typedef union { //permet de regrouper les deux types de blocs dans une seule structure
    BlocContigue contigue;
    BlocChainee chainee;
} Bloc;

typedef struct{
    Enregistrement enregistrement[FacteurBlocage];
    int adresse;
    bool free; //Pour savoir si le bloc est libre ou occupe
}BlocContigue;

typedef struct{
    Enregistrement enregistrement[FacteurBlocage];
    int adresse;
    bool free; //Pour savoir si le bloc est libre ou occupe
    int next; //Pour pointer vers le prochain bloc
}BlocChainee;

typedef struct{
    int table[50];
    int taille;
}tableAllocation;

typedef enum { Contigue, Chainee } typeOrganisation;
typedef enum {triee, nonTriee} typeTri;

typedef struct{
    char name[30];
    int nbBlocs;
    int nbEnregistrements;
    int premiereAdresse;
    typeOrganisation globalOrg;
    typeTri interneOrg;
} MetaDonnee;
typedef struct{

    int ID;
    int numbloc;

}Index;


void creerTableIndexDense(FILE *disque, FILE *f , Index densetableIndex []){
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
    
   


        void creeFichierIndex (Enregistrement *f, FILE *fm, Index tableIndex []){
 
            rewind(fi);
            Index buffer[7];
              int nbrEntrees = lireEntete(fm,2);












        }

     



