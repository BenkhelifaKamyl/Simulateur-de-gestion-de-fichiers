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
    //MD.premiereAdresse = ?
    return MD;
}
void lireNomFichier(fichier F, char nomFichier[30]){
    rewind(F.MDfile);
    MetaDonnee MD;
    fread(&MD,sizeof(MetaDonnee),1,F.MDfile);
    strcpy(nomFichier, MD.name);
}
int lireEntete(fichier F, int nc){
    rewind(F.MDfile);
    MetaDonnee MD;
    fread(&MD,sizeof(MetaDonnee),1,F.MDfile);
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
typeOrganisation lireEnteteGlobal(fichier F){
    rewind(F.MDfile);
    MetaDonnee MD;
    fread(&MD, sizeof(MetaDonnee),1,F.MDfile);
    return MD.globalOrg;
}

typeTri lireEnteteInterne(fichier F){
    rewind(F.MDfile);
    MetaDonnee MD;
    fread(&MD, sizeof(MetaDonnee),1,F.MDfile);
    return MD.interneOrg;
}
void MajEntetenom(fichier *F, char nom[30]){
    rewind(F->MDfile);
    MetaDonnee MD;
    fread(&MD, sizeof(MetaDonnee),1,F->MDfile);
    strcpy(MD.name,nom);
    rewind(F->MDfile);
    fwrite(&MD,sizeof(MetaDonnee),1,F->MDfile);
}
void MajEntetenum(fichier *F, int nc, int val){
     rewind(F->MDfile);
     MetaDonnee MD;
     switch(nc){
        case 2: //Nombre de blocs
        fread(&MD, sizeof(MetaDonnee),1,F->MDfile);
        MD.nbBlocs=val;
        rewind(F->MDfile);
        fwrite(&MD,sizeof(MetaDonnee),1,F->MDfile);
        break;
    case 3: // Nombre d'enregistrements
        fread(&MD, sizeof(MetaDonnee),1,F->MDfile);
        MD.nbEnregistrements=val;
        rewind(F->MDfile);
        fwrite(&MD,sizeof(MetaDonnee),1,F->MDfile);
        break;
    case 4: //Premiere adresse
        fread(&MD, sizeof(MetaDonnee),1,F->MDfile);
        MD.premiereAdresse=val;
        rewind(F->MDfile);
        fwrite(&MD,sizeof(MetaDonnee),1,F->MDfile);
        break;
    }
}
void MajeEnteteOrga(fichier *F, int nc){
    rewind(F->MDfile);
    MetaDonnee MD;
    fread(&MD, sizeof(MetaDonnee),1,F->MDfile);
    rewind(F->MDfile);
    switch(nc){
    case 1:
        MD.globalOrg = Chainee;
        fwrite(&MD,sizeof(MetaDonnee),1,F->MDfile);
        break;
    case 2:
        MD.globalOrg = Contigue;
        fwrite(&MD,sizeof(MetaDonnee),1,F->MDfile);
        break;
    }
}
void MajeEntetetri(fichier *F, int nc){
    rewind(F->MDfile);
    MetaDonnee MD;
    fread(&MD, sizeof(MetaDonnee),1,F->MDfile);
    rewind(F->MDfile);
    switch(nc){
    case 1:
        MD.interneOrg = triee;
        fwrite(&MD,sizeof(MetaDonnee),1,F->MDfile);
        break;
    case 2:
        MD.interneOrg = nonTriee;
        fwrite(&MD,sizeof(MetaDonnee),1,F->MDfile);
        break;
    }
}
void AfficherEntete(fichier F){
    rewind(F.MDfile);
    MetaDonnee MD;
    fread(&MD,sizeof(MetaDonnee),1,F.MDfile);
    printf("\nNom fichier: %s", MD.name);
    printf("\nNombre de blocs: %d",MD.nbBlocs);
    printf("\nNombre d'enregistrements: %d",MD.nbEnregistrements);
    printf("\nPremiere adresse du fichier dans le disque: %d",MD.premiereAdresse);
    if(MD.globalOrg == Chainee)
        printf("\nOrganisation globale: Chainee");
    else
        printf("\nOrganisation globale: Contigue");
    if(MD.interneOrg == triee)
        printf("\nOrganisation interne: Triee");
    else
        printf("\nOrganisation interne: Non triee");
}
void OuvrirFichier(fichier *F,char nomFichier[30], char mode){
    Bloc Buffer;
    MetaDonnee MD = creationFichier(nomFichier);
    char nomMD[46];
    strcpy(nomMD, nomFichier);
    strcat(nomMD, "Metadonnees.bin");
    strcat(nomFichier, ".bin");
    if(mode == 'w'){
        MD = creationFichier(nomFichier);
        F->MDfile = fopen(nomMD,"wb+");
        if(F->MDfile!= NULL){
        fwrite(&MD, sizeof(MetaDonnee),1,F->MDfile);
        }
        F->file = fopen(nomFichier, "wb+");
        if(F->file!=NULL){
            if(MD.interneOrg==triee)
                fillFile(-1,true,&F);
            else
                fillFile(-1,false,&F);
        }
    }
    else if(mode=='r'){
        F->MDfile = fopen(nomMD,"rb+");
        if(F->MDfile!=NULL){
        fread(&MD, sizeof(MetaDonnee),1,F->MDfile);
        }
        F->file = fopen(nomFichier, "rb+");
    }
    else{
        printf("Ce mode d'ouverture n'existe pas.\n");
    }
}
void LireBloc(fichier *F, int i, Bloc *Buffer){
    if(i<0 || i>=MAX_BLOCKS)
        printf("Indice de bloc invalide\n");
    else{
    fseek(F->file,sizeof(MetaDonnee)+ (i)*sizeof(Bloc), SEEK_SET); //Lire depuis le bloc i
    fread(Buffer, sizeof(Bloc),1,F->file);
    }
}
void EcrireBloc(fichier *F, int i, Bloc Buffer){
    if(i<0 || i>=MAX_BLOCKS)
        printf("Indice de bloc invalide\n");
    else{
    fseek(F->file,sizeof(MetaDonnee)+ (i-1)*sizeof(Bloc), SEEK_SET); //Ecrire dans le bloc i
    fwrite(&Buffer, sizeof(Bloc),1,F->file);
    memcpy(&disk[i], &Buffer, sizeof(Bloc));
}
}
void fermerFichier(fichier F){
    fclose(F.file);
    fclose(F.MDfile);
}
int AllouerBloc(){
    int i=0;
    while(disk[i].chainee.free==false)
        i++;
    if(i<MAX_BLOCKS)
        return i;
    return -1;
}
void chargerMetadonnees(fichier F){
    Meta = fopen("Meta.bin","ab+");
    if(Meta==NULL){
        printf("\nImpossible d'ouvrir le fichier.");
        return;
    }
    if(F.MDfile==NULL){
        printf("\nLe fichier metaDonnees n'est pas ouvert.");
        fclose(Meta);
        return;
    }
    MetaDonnee buffer;
    fread(&buffer, sizeof(MetaDonnee),1,F.MDfile);
    fwrite(&buffer, sizeof(MetaDonnee),1,Meta);
    fclose(Meta);
}
void chargerFichierMetadonnees(int premiereAdresse, fichier *F){
    Meta = fopen("Meta.bin","rb+");
    MetaDonnee buffer;
    rewind(F->MDfile);
    bool check=false;
    while(fread(&buffer, sizeof(MetaDonnee),1,MetaIndex)==1){
        if(buffer.premiereAdresse==premiereAdresse){
            fwrite(&buffer, sizeof(MetaDonnee),1,F->MDfile);
            check=true;
            break;
        }
    }
    if(check==false){
        printf("\nCe fichier n'existe pas.");
    }
    fclose(Meta);
}
