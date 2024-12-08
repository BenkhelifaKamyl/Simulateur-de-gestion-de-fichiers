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

int lireEntete(fichier F, int nc){
    switch (nc){
    case 2:
        return F.Entete.nbBlocs;
        break;
    case 3:
        return F.Entete.nbEnregistrements;
        break;
    case 4:
        return F.Entete.premiereAdresse;
        break;
    }
}
typeOrganisation lireEnteteGlobal(fichier F){
    return F.Entete.globalOrg;
}

typeTri lireEnteteInterne(fichier F){
    return F.Entete.interneOrg;
}
void MajEntetenom(fichier *F, char nom[30]){
    strcpy(F->Entete.name, nom);
}
void MajEntetenum(fichier *F, int nc, int val){
    switch(nc){
    case 2: //nombre blocs
        F->Entete.nbBlocs = val;
        break;
    case 3: //nombres enregistrements
        F->Entete.nbEnregistrements = val;
        break;
    case 4: //premiereAdress
        F->Entete.premiereAdresse = val;
        break;
    }
}
void MajeEnteteOrga(fichier *F, int nc){
    switch(nc){
    case 1:
        F->Entete.globalOrg = typeOrganisation.Chainee;
        break;
    case 2:
        F->Entete.globalOrg = typeOrganisation.Contigue;
        break;
    }
}
void MajeEntetetri(fichier *F, int nc){
    switch(nc){
    case 1:
        F->Entete.interneOrg= typeTri.triee;
        break;
    case 2:
        F->Entete.interneOrg = typeTri.nonTriee;
        break;
    }
}
void OuvrirFichier(Fichier *F,char nomFichier[30], char mode){
    BLOC Buffer;
    if(mode == 'w'){
        F->file = fopen(nomFichier,"wb+");
        MajEntetenom()
    }

}
void LireBloc(fichier *F, int i, Bloc *Buffer){
    fseek(F->file,sizeof(MetaDonnee)+ (i-1)*sizeof(Bloc), SEEK_SET); //Lire depuis le bloc i-1
    fread(Buffer, sizeof(Bloc),1,F->file);
}
void EcrireBloc(fichier *F, int i, BLOC Buffer){
    fseek(F->file,sizeof(MetaDonnee)+ (i-1)*sizeof(Bloc), SEEK_SET); //Ecrire dans le bloc i-1
    fwrite(&Buffer, sizeof(Bloc),1,F->file);
}


