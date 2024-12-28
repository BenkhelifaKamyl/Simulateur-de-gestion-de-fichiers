#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "TextColor.h"
#include "Metadonnees.h"
#include "TableIndex.h"
#include "Disk.h"




void menu(){
    int c=0,k=0, nbE, ID;
    char filename[30];
    fichier F;
    Enregistrement E;

        printf("Bienvenue dans le simulateur de gestion de fichiers!!!\n\n");
        initializeDiskChainee();

    do{
        printf("\nChoisissez ce que vous voulez faire:\n 1)Creer un fichier, 2)Afficher la memoire secondaire, \n3) Afficher les metadonnees des fichiers, 4) Rechercher un enregistrement, \n5) Inserer un enregistrement, 6) Supprimer un enregistrement, \n7) Defragmenter un fichier, 8) Supprimer un fichier, \n9) Renommer un fichier, 10) Compactage de la memoire secondaire, \n11) Vider la memoire secondaire, 12) Quitter le programme.\n");
        scanf("%d",&c);
        switch (c){
        case 1: //Creation d'un fichier (Manque table d'index)
            OuvrirFichier(&F,'w');
            break;
        case 2: //Afficher Disque
            AfficherDisque();
            break;
        case 3: //Afficher les metadonnees
            AfficherEntete();
            break;
        case 4: //Recherche d'enregistrement
            printf("\nDonnez le nom du fichier dont vous voulez rechercher un enregistrement: ");
            scanf("%s", filename);
            rechercheNomFichier(&F,filename);
            printf("\nDonnez l'ID de l'enregistrement que vous recherchez: ");
            scanf("%d",&ID);
            rechercheEnregistrement(F,ID);
            break;
        case 5: //Insertion enregistrement
            printf("\nDonnez le nom du fichier dont vous voulez inserer un enregistrement: ");
            scanf("%s", filename);
            rechercheNomFichier(&F,filename);
            printf("\nDonnez les Informations de l'enregistrement a inserer: ");
            E=donneesEnregistrement();
            insertRecord(&F,E);
            break;
        case 6: //Suppression d'un enregistrement
            printf("\nDonnez le nom du fichier dont vous voulez supprimer un enregistrement: ");
            scanf("%s", filename);
            rechercheNomFichier(&F,filename);
            do{
                printf("\nVoulez vous supprimer un enregistrement: 1)Physiquement 2)Logiquement  ");
                scanf("%d",&k);
            }while(k<1 || k>2);
            printf("\nDonnez l'ID de l'enregistrement a supprimer ");
            scanf("%d",&ID);
            if(k==1)
                SuppressionPhysique(&F,ID);
            else
                SuppressionLogique(&F,ID);
            break;
        case 7: //Defragmentation
            printf("\nDonnez le nom du fichier dont vous voulez faire une defragmentation: ");
            scanf("%s", filename);
            rechercheNomFichier(&F,filename);
            Defragmentation(&F);
        break;
        case 8: //Suppression d'un fichier
            printf("\nDonnez le nom du fichier que vous voulez supprimer: ");
            scanf("%s", filename);
            rechercheNomFichier(&F,filename);
            deleteFile(&F);
            break;
        case 9: //Renommer un fichier
            printf("\nDonnez le nom du fichier que vous voulez renommer: ");
            scanf("%s", filename);
            rechercheNomFichier(&F,filename);
            printf("\nDonnez le nouveau nom du fichier: ");
            scanf("%s", filename);
            MajEntetenom(&F, filename);
            break;
        case 10: //Compactage
            compactDisk();
            break;
        case 11: //Vider le disque
            clearDisk();
            break;
        default: //Fin du programme
            printf("\nFin du programme!!!");
            break;
        }
    } while(c!=12); //Condition d'arret pour sortir du programme


}



int main()
{
    Menu();
    return 0;
}
