#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "TextColor.h"
#include "Metadonnees.h"
#include "TableIndex.h"
#include "Disk.h"

void creationFichier(MetaDonnee *MD, int choix){ //Saisie des informations du fichier
    int orgInterne;
    printf("\nDonnez le nom du fichier de metadonnees: ");
    scanf("%s",MD->name);
    printf("\nDonnez le nombre d'enregistrements du fichier: ");
    scanf("%d",&MD->nbEnregistrements);
    MD->nbBlocs = (MD->nbEnregistrements + BLOCK_SIZE - 1) / BLOCK_SIZE; //Calcule le nbre de blocs utilises
    if(choix==1)
        MD->globalOrg = Chainee;
    else
        MD->globalOrg = Contigue;
    printf("\nDonnez le type d'organisation interne: 1)Triee 2)Non triee: ");
    do{
        scanf("%d",&orgInterne);
        if (orgInterne != 1 && orgInterne != 2)
            printf("\nEntrée invalide. Veuillez entrer 1 pour Triee ou 2 pour Non triee:");
    } while(orgInterne!=2 && orgInterne!=1);
    if(orgInterne==1){
        MD->interneOrg = triee;
    }
    else{
        MD->interneOrg = nonTriee;
    }
    MD->premiereAdresse = -1;
}
void creationFichierMetadonnees(fichier *F, int choix){ //Permet de creer le fichier et de le charger en "MS"
    MetaDonnee buffer;
    creationFichier(&buffer, choix); //Recupere les infos du fichier a inserer
    printf("\nTest\n");
    F->MDfile = fopen(buffer.name, "wb+");
    if (F->MDfile == NULL) { printf("\nErreur lors de l'ouverture du fichier %s.\n", buffer.name); return; }
    fwrite(&buffer, sizeof(MetaDonnee),1,F->MDfile);
    fclose(F->MDfile);
    F->MDfile = fopen(buffer.name, "rb+");
    if(choix==1) //Verifier si on peut creer le fichier
        ChargerFichierChainee(F);
    else
        ChargerFichierContigue(F);
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
    default:
        return -1;
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
    fclose(F->MDfile);
    F->MDfile = fopen(MD.name, "wb+");
    fwrite(&MD,sizeof(MetaDonnee),1,F->MDfile);
    fclose(F->MDfile);
    F->MDfile = fopen(MD.name, "rb+");
}
void MajEntetenum(fichier *F, int nc, int val) { //Met a jour les caracteristiques
    rewind(F->MDfile);
    MetaDonnee MD;
    fread(&MD, sizeof(MetaDonnee), 1, F->MDfile);
    switch (nc) {
    case 2:
        MD.nbBlocs = val;
        break;
    case 3:
        MD.nbEnregistrements = val;
        break;
    case 4:
        MD.premiereAdresse = val;
        break;
    }
    rewind(F->MDfile);
    fwrite(&MD, sizeof(MetaDonnee), 1, F->MDfile);
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
    for(int i=0; i<MAX_FILES; i++){
        printf("\n*****Affichage des caracteristiques du %d fichier*****\n",i);
        printf("\nNom fichier: %s", Meta[i].name);
        printf("\nNombre de blocs: %d",Meta[i].nbBlocs);
        printf("\nNombre d'enregistrements: %d",Meta[i].nbEnregistrements);
        printf("\nPremiere adresse du fichier dans le disque: %d",Meta[i].premiereAdresse);
        if(Meta[i].globalOrg == Chainee)
            printf("\nOrganisation globale: Chainee");
        else
            printf("\nOrganisation globale: Contigue");
        if(Meta[i].interneOrg == triee)
            printf("\nOrganisation interne: Triee");
        else
            printf("\nOrganisation interne: Non triee");
    }
}
void OuvrirFichier(fichier *F, char mode, int choix){ //Ouvre le fichier, "w" pour ecrire et "r" pour lire
    MetaDonnee buffer;
    if(mode=='w'){ //Mode ecriture
        printf("\nFichier ouvert en mode ecriture.");
        creationFichierMetadonnees(F, choix); //Cree et charge le fichier de metadonnees en "MS"
        if(F->MDfile!=NULL && lireEntete(*F,4)!=-1){ //Remplis le fichier sur le disk selon les modes d'organisation
            if(lireEnteteGlobal(*F)==Chainee)
                fillFileChainee(liretypeTri(*F), F);
            else
                fillFileContigue(liretypeTri(*F),F);
        }
        //Chargement fichier index se fait soit dans le fillFile soit ici, en attente
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
int AllouerBlocChainee(int i){ //Retourne le premier bloc libre
    while(i<MAX_BLOCKS &&disk[i].chainee.free==false)
        i++;
    if(i<MAX_BLOCKS)
        return i;
    return -1;
}
int AllouerBlocContigue(int i){ //Retourne le premier bloc libre
    while(i<MAX_BLOCKS &&disk[i].contigue.free==false)
        i++;
    if(i<MAX_BLOCKS)
        return i;
    return -1;
}
void initializeMetaFiles(int choix){
    for(int i=0; i<MAX_FILES;i++){
        if(choix==1){
            Meta[i].globalOrg = Chainee;
        }
        else{
            Meta[i].globalOrg = Contigue;
        }
        Meta[i].interneOrg=triee;
        strcpy(Meta[i].name, " ");
        Meta[i].nbBlocs=-1;
        Meta[i].nbEnregistrements = -1;
        Meta[i].premiereAdresse = -1;
    }
}
void chargerMetadonnees(fichier F) { // Charge le fichier de metadonnees dans la "MS"
    char filename[30];
    fichier G;
    int i;
    lireNomFichier(F, filename);

    // Check if Meta file already exists

    rewind(F.MDfile);
    if (F.MDfile == NULL) {
        printf("\nLe fichier metaDonnees n'est pas ouvert dans la fct chargerMetadonnees.");
        return;
    }

    MetaDonnee buffer;
    if (fread(&buffer, sizeof(MetaDonnee), 1, F.MDfile) != 1) {
        printf("\nErreur de lecture du fichier metaDonnees dans la fct chargerMetadonnees.");
        return;
    }
    rechercheNomFichier(&G, filename, &i);
    printf("\i: %d\n",i);
    if(i>=0 && i<MAX_FILES){
        memcpy(&Meta[i],&buffer,sizeof(buffer));
    }
    else{
        i=0;
        while(i<MAX_FILES){
            if(Meta[i].nbBlocs==-1){
                memcpy(&Meta[i],&buffer,sizeof(buffer));
                break;
            }
            else{
                i++;
            }
        }
    }
}
void chargerFichierMetadonnees(int premiereAdresse, fichier *F){ //Recupere le fichier de metadonnees cherché selon la premiere adresse d'un fichier

    bool check=false;
    int i = 0;
    while(i <MAX_FILES){
        if(Meta[i].premiereAdresse==premiereAdresse){
            F->MDfile=fopen(Meta[i].name,"wb+");
            if(F->MDfile==NULL){
                printf("\nErreur d'ecriture dans le fichier MDfile dans la fct chargerFichierMetaDonnees.");
                return;
            }
            fwrite(&Meta[i], sizeof(MetaDonnee),1,F->MDfile);
            check=true;
            break;
        }
        i++;
    }


    if(check==false){
        printf("\nCe fichier n'existe pas.");
        F->MDfile=NULL;
    }
}
void rechercheFichierMeta(int nBloc, fichier *F){ //Recupere le fichier de metadonnees selon un numero de bloc
    bool trouve=false;
    int i=0;
    while(trouve==false && i<MAX_FILES){
        if(Meta[i].premiereAdresse==nBloc){
            chargerFichierMetadonnees(nBloc,F);
            trouve=true;
        }
        else if(nBloc>Meta[i].premiereAdresse && nBloc<=(Meta[i].premiereAdresse + Meta[i].nbBlocs)){
            chargerFichierMetadonnees(Meta[i].premiereAdresse,F);
            trouve=true;
        }
        i++;
    }
    if(trouve==false)
        printf("\nLe fichier de metadonnees n'a pas ete trouve dans recherchefichiermeta.");
}
void rechercheNomFichier(fichier *F, char filename[30], int *i){
    bool trouve=false;
    (*i)=0;
    while(trouve== false && (*i)<MAX_FILES){
        if(strcmp(filename,Meta[*i].name)==0){
            chargerFichierMetadonnees(Meta[*i].premiereAdresse,F);
            trouve=true;
        }
        if(trouve==false) (*i)++;
    }
    if(trouve==false){
        printf("\nLe fichier de metadonnees n'a pas ete trouve dans recherchenomficher.");
        *i=-1;
        F->MDfile=NULL;
    }
}
void supprimeFichierMetadonnees(fichier *F){
    int i=0;
    char filename[30];
    lireNomFichier(*F,filename);
    rechercheNomFichier(F,filename,&i);
    fclose(F->MDfile);
    remove(filename);

        if(strcmp(filename,Meta[i].name)==0){
            strcpy(Meta[i].name, " ");
            Meta[i].nbBlocs=-1;
            Meta[i].nbEnregistrements = -1;
            Meta[i].premiereAdresse = -1;
        }
}

void renommerFichier(fichier *F){
    char filename[30],filename2[30];
    int i;
    printf("\nDonnez le nom du fichier que vous voulez renommer: ");
    scanf("%s", filename);
    rechercheNomFichier(F,filename,&i);
    if(i==-1){
        printf("\nCe fichier n'existe pas!\n");
        return;
    }
    printf("\nDonnez le nouveau nom du fichier: ");
    scanf("%s", filename2);
    MajEntetenom(F,filename2);
    strcpy(Meta[i].name,filename2);
}
