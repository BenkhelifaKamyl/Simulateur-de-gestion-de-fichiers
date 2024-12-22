#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "TextColor.h"
#include "Metadonnees.h"
#include "TableIndex.h"
#include "Disk.h"

MetaDonnee creationFichier(){
    int orgGlobale;
    int orgInterne;
    MetaDonnee MD;
    printf("\nDonnez le nom du fichier de metadonnees: ");
    scanf("%s",MD.name);
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
void creationFichierMetadonnees(fichier *F){
    MetaDonnee buffer;
    buffer = creationFichier();
    F->MDfile = fopen(buffer.name, "wb+");
    fwrite(&buffer, sizeof(MetaDonnee),1,F->MDfile);
    chargerMetadonnees(*F);
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

bool liretypeTri(fichier F){
    rewind(F.MDfile);
    MetaDonnee buffer;
    fread(&buffer, sizeof(MetaDonnee),1,F.MDfile);
    if(buffer.interneOrg == triee){
        return true;
    }
    else{
        return false;
    }
}
void MajEntetenom(fichier *F, char nom[30]){
    rewind(F->MDfile);
    MetaDonnee MD;
    fread(&MD, sizeof(MetaDonnee),1,F->MDfile);
    strcpy(MD.name,nom);
    rewind(F->MDfile);
    fwrite(&MD,sizeof(MetaDonnee),1,F->MDfile);
    fclose(F->MDfile);
    F->MDfile = fopen(MD.name, "rb+");
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
void AfficherEntete(){
    rewind(Meta);
    MetaDonnee Buffer;
    int i=1;
    while(fread(&Buffer,sizeof(MetaDonnee),1,Meta)==1){
        printf("\n*****Affichage des caracteristiques du %d fichier*****\n",i);
        printf("\nNom fichier: %s", Buffer.name);
        printf("\nNombre de blocs: %d",Buffer.nbBlocs);
        printf("\nNombre d'enregistrements: %d",Buffer.nbEnregistrements);
        printf("\nPremiere adresse du fichier dans le disque: %d",Buffer.premiereAdresse);
        if(Buffer.globalOrg == Chainee)
            printf("\nOrganisation globale: Chainee");
        else
            printf("\nOrganisation globale: Contigue");
        if(Buffer.interneOrg == triee)
            printf("\nOrganisation interne: Triee");
        else
            printf("\nOrganisation interne: Non triee");
        i++;
    }
}
void OuvrirFichier(fichier *F, char mode){
    MetaDonnee buffer;
    if(mode=='w'){
        printf("\nFichier ouvert en mode ecriture.");
        creationFichierMetadonnees(F);
        if(F->MDfile!=NULL){
            if(lireEnteteGlobal(*F)==Chainee)
                fillFileChainee(-1, liretypeTri(*F), F);
            else
                fillFileContigue(-1, liretypeTri(*F),F);
        }
    }
    else if (mode=='r'){
        printf("\nFichier ouvert en mode lecture.");
        if(F->MDfile!=NULL){
            fread(&buffer, sizeof(MetaDonnee),1,F->MDfile);
        }
    }
    else{
        printf("\nCe mode d'ouverture n'existe pas.");
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
    rewind(F.MDfile);
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
    while(fread(&buffer, sizeof(MetaDonnee),1,Meta)==1){
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
void rechercheFichierMeta(int nBloc, fichier *F){
    Meta = fopen("Meta.bin","rb+");
    MetaDonnee buffer;
    bool trouve=false;
    while(trouve==false && fread(&buffer, sizeof(MetaDonnee),1,Meta)==1){
        if(buffer.premiereAdresse==nBloc){
            chargerFichierMetadonnees(nBloc,F);
            trouve=true;
        }
        else if(nBloc>buffer.premiereAdresse && nBloc<=(buffer.premiereAdresse + buffer.nbBlocs)){
            chargerFichierMetadonnees(buffer.premiereAdresse,F);
            trouve=true;
        }
    }
    if(trouve==false)
        printf("\nLe fichier de metadonnees n'a pas ete trouve.");
    fclose(Meta);
}
