#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "TextColor.h"
#include "Metadonnees.h"
#include "TableIndex.h"
#include "Disk.h"




void menu(){
    int c=0,k=0, nbE;
    char filename;
    if(k==0){
        printf("Bienvenue dans le simulateur de gestion de fichiers!!!\n\n");
        initializeDiskChainee();
        k++;
    }
    while(c!=12){
        printf("\nChoisissez ce que vous voulez faire:\n 1)Creer un fichier, 2)Afficher la memoire secondaire, \n3) Afficher les metadonnees des fichiers, 4) Rechercher un enregistrement, \n5) Inserer un enregistrement, 6) Supprimer un enregistrement, \n7) Defragmenter un fichier, 8) Supprimer un fichier, \n9) Renommer un fichier, 10) Compactage de la memoire secondaire, \n11) Vider la memoire secondaire, 12) Quitter le programme.\n");
        scanf("%d",&c);
        switch (c){
        case 1:
            printf("\nEntrez le nom de votre fichier: ");
            scanf("%s",filename);
            creationFichier(filename);


        }
    }


}



int main()
{
    /*fichier F;
    F.MDfile = fopen("MetaDat.bin","rb+");
    MajEntetenom(&F,"MetaDat.bin");
    MajEntetenum(&F,2,4);
    MajEntetenum(&F,3,10);
    MajEntetenum(&F,4,0);
    MajeEnteteOrga(&F,1);
    MajeEntetetri(&F,2);
    AfficherEntete(F);
    fclose(F.MDfile);*/
    //menu();
    //AfficherMS();
    initializeDiskChainee();
    strcpy(disk[5].chainee.enregistrement[0].Data, "Kamyl");
    strcpy(disk[5].chainee.enregistrement[1].Data, "Kamyl");
    strcpy(disk[95].chainee.enregistrement[0].Data, "Kamyl");
    strcpy(disk[95].chainee.enregistrement[1].Data, "Kamyl");
    strcpy(disk[95].chainee.enregistrement[2].Data, "Kamyl");
    disk[5].chainee.free=false;
    disk[95].chainee.free=false;
    AfficherDisque();
    return 0;
}
