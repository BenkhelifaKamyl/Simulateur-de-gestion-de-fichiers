#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "TextColor.h"
#include "Metadonnees.h"
#include "TableIndex.h"
#include "Disk.h"

MetaDonnee creationFichier(){ //Saisie des informations du fichier
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
    MD.premiereAdresse = AllouerBloc();
    return MD;
}
void creationFichierMetadonnees(fichier *F){ //Permet de creer le fichier et de le charger en "MS"
    MetaDonnee buffer;
    buffer = creationFichier(); //Recupere les infos du fichier a inserer
    F->MDfile = fopen(buffer.name, "wb+");
    fwrite(&buffer, sizeof(MetaDonnee),1,F->MDfile);
    chargerMetadonnees(*F); //Le charger en "MS"
}
void lireNomFichier(fichier F, char nomFichier[30]){ //Lire le nom du fichier
    rewind(F.MDfile);
    MetaDonnee MD;
    fread(&MD,sizeof(MetaDonnee),1,F.MDfile);
    strcpy(nomFichier, MD.name);
}
int lireEntete(fichier F, int nc){ // Retourne: 2:Nombre de blocs utilisés par le fichier, 3: Nb enregistrements, 4: Premiere Adresse en MS
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
typeOrganisation lireEnteteGlobal(fichier F){ //Retourne si le fichier suit une organisation chainee ou contigue
    rewind(F.MDfile);
    MetaDonnee MD;
    fread(&MD, sizeof(MetaDonnee),1,F.MDfile);
    return MD.globalOrg;
}
bool liretypeTri(fichier F){ //Retourne vrai si le fchier est trie, sinon faux
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
void MajEntetenom(fichier *F, char nom[30]){ //Met a jour le nom du fichier
    rewind(F->MDfile);
    MetaDonnee MD;
    fread(&MD, sizeof(MetaDonnee),1,F->MDfile);
    strcpy(MD.name,nom);
    rewind(F->MDfile);
    fwrite(&MD,sizeof(MetaDonnee),1,F->MDfile);
    fclose(F->MDfile);
    F->MDfile = fopen(MD.name, "rb+");
}
void MajEntetenum(fichier *F, int nc, int val){ //Met a jour les caracteristiques
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
void AfficherEntete(){ //Affiche les informations de tous les fichiers de metadonnees
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
void OuvrirFichier(fichier *F, char mode){ //Ouvre le fichier, "w" pour ecrire et "r" pour lire
    MetaDonnee buffer;
    if(mode=='w'){ //Mode ecriture
        printf("\nFichier ouvert en mode ecriture.");
        creationFichierMetadonnees(F); //Cree et charge le fichier de metadonnees en "MS"
        if(F->MDfile!=NULL){ //Remplis le fichier sur le disk selon les modes d'organisation
            if(lireEnteteGlobal(*F)==Chainee)
                fillFileChainee(-1, liretypeTri(*F), F);
            else
                fillFileContigue(-1, liretypeTri(*F),F);
        }
    }
    else if (mode=='r'){ //Mode lecture
        printf("\nFichier ouvert en mode lecture.");
        if(F->MDfile!=NULL){
            fread(&buffer, sizeof(MetaDonnee),1,F->MDfile); //Permet de lire un fichier
        }
    }
    else{
        printf("\nCe mode d'ouverture n'existe pas.");
    }
}
void fermerFichier(fichier F){ //Ferme les fichiers d'index et de metadonnees
    fclose(F.TableIndex);
    fclose(F.MDfile);
}
int AllouerBloc(){ //Retourne le premier bloc libre
    int i=0;
    while(i<MAX_BLOCKS &&disk[i].chainee.free==false)
        i++;
    if(i<MAX_BLOCKS)
        return i;
    return -1;
}
void chargerMetadonnees(fichier F){ //Charge le fichier de metadonnees dans la "MS"
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
void chargerFichierMetadonnees(int premiereAdresse, fichier *F){ //Recupere le fichier de metadonnees cherché selon la premiere adresse d'un fichier
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
void rechercheFichierMeta(int nBloc, fichier *F){ //Recupere le fichier de metadonnees selon un numero de bloc
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
