#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "TextColor.h"
#include "Metadonnees.h"
#include "TableIndex.h"
#include "Disk.h"

MetaDonnee creationFichier(char filename[30]){
    int orgGlobale;
    int orgInterne;
    MetaDonnee MD;
    strcpy(MD.name, filename);
    printf("\nDonnez le nombre d'enregistrements du fichier: ");
    scanf("%d",&MD.nbEnregistrements);
    printf("\nDonnez le type d'organisation globale: 1) Chainee 2) Contigue");
    do{
        scanf("%d",&orgGlobale);
        if (orgGlobale != 1 && orgGlobale != 2)
            printf("Entrée invalide. Veuillez entrer 1 pour Chainee ou 2 pour Contigue.\n");
    } while(orgGlobale!=2 && orgGlobale!=1);
    if(orgGlobale==1)
        MD.globalOrg = Chainee;
    else
        MD.globalOrg = Contigue;
    printf("\nDonnez le type d'organisation interne: 1)Triee 2)Non triee");
    do{
        scanf("%d",&orgInterne);
        if (orgInterne != 1 && orgInterne != 2)
            printf("Entrée invalide. Veuillez entrer 1 pour Triee ou 2 pour Non triee.\n");
    } while(orgInterne!=2 && orgInterne!=1);
    if(orgInterne==1)
        MD.interneOrg = triee;
    else
        MD.interneOrg = nonTriee;
    MD.premiereAdresse = ?
    return MD;
}

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
typeOrganisation lireEnteteGlobal(FILE *F){
    rewind(F);
    MetaDonnee MD;
    fread(&MD, sizeof(MetaDonnee),1,F);
    return MD.globalOrg;
}

typeTri lireEnteteInterne(FILE *F){
    rewind(F);
    MetaDonnee MD;
    fread(&MD, sizeof(MetaDonnee),1,F);
    return MD.interneOrg;
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
    fseek(F, 30*sizeof(char)+3*sizeof(int), SEEK_SET);
    switch(nc){
    case 1:
        fwrite(&Chainee,sizeof(typeOrganisation),1,F);
        break;
    case 2:
        fwrite(&Contigue,sizeof(typeOrganisation),1,F);
        break;
    }
}
void MajeEntetetri(FILE *F, int nc){
    rewind(F);
    fseek(F, 30*sizeof(char)+3*sizeof(int) + sizeof(typeOrganisation), SEEK_SET);
    switch(nc){
    case 1:
        fwrite(&triee,sizeof(typeTri),1,F);
        break;
    case 2:
        fwrite(&nonTriee,sizeof(typeTri),1,F);
        break;
    }
}
