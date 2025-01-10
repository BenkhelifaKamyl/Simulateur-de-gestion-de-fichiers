#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "TextColor.h"
#include "Metadonnees.h"
#include "TableIndex.h"
#include "Disk.h"


void Menu(){
    int c1,c=0,k=0, ID, i;
    char filename[30];
    fichier F;
    Enregistrement E;

        printf("Bienvenue dans le simulateur de gestion de fichiers!!!\n\n");
        do{
            printf("Voulez vous une organisation: 1)chainee ou 2)contigue: ");
            scanf("%d",&c1);
        }while(c1<1 || c1>2);
        if(c1==1)
            initializeDiskChainee();
        else
            initializeDiskContigue();
        initializeMetaFiles(c1);
        initializeIndexFiles();

    do{
        boldColor(7);
        printf("\nChoisissez ce que vous voulez faire:\n1) Creer un fichier\n2) Afficher la memoire secondaire\n3) Afficher les metadonnees des fichiers\n4) Rechercher un enregistrement\n5) Inserer un enregistrement\n6) Supprimer un enregistrement\n7) Defragmenter un fichier\n8) Supprimer un fichier\n9) Renommer un fichier\n10) Compactage de la memoire secondaire\n11) Vider la memoire secondaire \n12) Afficher les ID d'enregistrements d'un fichier \n13) Quitter le programme.\n");
        resetColor();
        scanf("%d",&c);
        switch (c){
        case 1: //Creation d'un fichier (Manque table d'index)
            OuvrirFichier(&F,'w',c1);
            break;
        case 2: //Afficher Disque
            if(c1==1)
                AfficherDisqueChainee();
            else
                AfficherDisqueContigue();
            break;
        case 3: //Afficher les metadonnees
            AfficherEntete();
            break;
        case 4: //Recherche d'enregistrement
            printf("\nDonnez le nom du fichier dont vous voulez rechercher un enregistrement: ");
            scanf("%s", filename);
            rechercheNomFichier(&F,filename,&i);
            printf("\nDonnez l'ID de l'enregistrement que vous recherchez: ");
            scanf("%d",&ID);
            int numBloc, deplacement;
            if(liretypeTri(F))
                rechercheEnregistrementNonDense(&F,ID,&numBloc,&deplacement);
            else
                rechercheEnregistrementDense(&F,ID,&numBloc,&deplacement);
            break;
        case 5: //Insertion enregistrement
            printf("\nDonnez le nom du fichier dont vous voulez inserer un enregistrement: ");
            scanf("%s", filename);
            rechercheNomFichier(&F,filename,&i);
            printf("\nDonnez les Informations de l'enregistrement a inserer: ");
            E=donneesEnregistrement();
            insertRecord(&F,E);
            break;
        case 6: //Suppression d'un enregistrement
            printf("\nDonnez le nom du fichier dont vous voulez supprimer un enregistrement: ");
            scanf("%s", filename);
            rechercheNomFichier(&F,filename,&i);
            do{
                printf("\nVoulez vous supprimer un enregistrement: 1)Logiquement 2)Physiquement  ");
                scanf("%d",&k);
            }while(k<1 || k>2);

            if(c1==1){
                AfficherFichierChainee(F);
                printf("\nDonnez l'ID de l'enregistrement a supprimer ");
                scanf("%d",&ID);
                if(k==1)
                    deleteRecordLogicalchainee(&F,ID);
                else
                    deleteRecordPhysicalchaine(&F,ID);
            }
            else{
                AfficherFichierContigue(F);
                printf("\nDonnez l'ID de l'enregistrement a supprimer ");
                scanf("%d",&ID);
                if(k==1)
                    deleteRecordLogicalcontigue(&F,ID);
                else
                    deleteRecordPhysicalContiguous(&F,ID);
            }
            break;
        case 7: //Defragmentation
            printf("\nDonnez le nom du fichier dont vous voulez faire une defragmentation: ");
            scanf("%s", filename);
            rechercheNomFichier(&F,filename,&i);
            if(c1==1)
                Defragmentationchainee(&F);
            else
                DefragmentationContigue(&F);
        break;
        case 8: //Suppression d'un fichier
            printf("\nDonnez le nom du fichier que vous voulez supprimer: ");
            scanf("%s", filename);
            rechercheNomFichier(&F,filename,&i);
            deleteFile(&F);
            break;
        case 9: //Renommer un fichier
            renommerFichier(&F);
            break;
        case 10: //Compactage
            if(c1==1)
                compactDiskChainee();
            else
                compactDiskContigue();
            break;
        case 11: //Vider le disque
            if(c1==1)
                clearDiskchainee();
            else
                clearDiskContigue();
            break;
        case 12:
            printf("\nDonnez le nom du fichier que vous voulez lire: ");
            scanf("%s", filename);
            rechercheNomFichier(&F,filename,&i);
            if(c1==1)
                AfficherFichierChainee(F);
            else
                AfficherFichierContigue(F);
            break;
        default: //Fin du programme
            if(c1==1)
                clearDiskchainee();
            else
                clearDiskContigue();
            printf("\nFin du programme!!!");
            break;
        }
    } while(c<13); //Condition d'arret pour sortir du programme


}



int main()
{
    Menu();
    return 0;
}
