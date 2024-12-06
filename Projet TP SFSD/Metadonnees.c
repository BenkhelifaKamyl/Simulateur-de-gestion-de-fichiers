#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "TextColor.h"
#include "Metadonnees.h"
#include "TableIndex.h"
#include "Disk.h"

int lireEntete(FILE *F, int nc){
    rewind(F);
    MetaDonnee MD;
    fread(&MD,sizeof(MetaDonnee),1,F);
    switch (nc){
    case 2:
        return MD.nbBlocs;
        break;
    case 3:
        return MD.nbEnregistrements;
        break;
    case 4:
        return MD.premiereAdresse;
        break;
    }
}

void MajEntetenom(FILE *F, char nom[30]){
    rewind(F);
    fwrite(&nom, 30*sizeof(char),1,F);
}
void MajEntetenum(FILE *F, int nc, int val){
    rewind(F);
    switch(nc){
    case 2:
        fseek(F,30*sizeof(char),SEEK_SET);
        fwrite(&val, sizeof(int),1,F);
        break;
    case 3:
        fseek(F, 30*sizeof(char)+2*sizeof(int), SEEK_SET);
        fwrite(&val, sizeof(int),1,F);
        break;
    case 4:
        fseek(F, 30*sizeof(char)+3*sizeof(int), SEEK_SET);
        fwrite(&val, sizeof(int),1,F);
        break;
    }
}
void MajeEnteteOrga(FILE *F, int nc){
    rewind(F);
    switch(nc){
    case 1:
        fseek(F, 30*sizeof(char)+3*sizeof(int), SEEK_SET);
        fwrite(&Chainee,sizeof(typeOrganisation),1,F);
        break;
    case 2:
        fseek(F, 30*sizeof(char)+3*sizeof(int), SEEK_SET);
        fwrite(&Contigue,sizeof(typeOrganisation),1,F);
        break;
    }
}
void MajeEntetetri(FILE *F, int nc){
    rewind(F);
    switch(nc){
    case 1:
        fseek(F, 30*sizeof(char)+3*sizeof(int) + sizeof(typeOrganisation), SEEK_);
        fwrite(&triee,sizeof(typeTri),1,F);
        break;
    case 2:
        fseek(F, 30*sizeof(char)+3*sizeof(int) + sizeof(typeOrganisation), SEEK_CUR);
        fwrite(&nonTriee,sizeof(typeTri),1,F);
        break;
    }
}
