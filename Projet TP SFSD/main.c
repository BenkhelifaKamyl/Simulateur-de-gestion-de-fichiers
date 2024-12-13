#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "TextColor.h"
#include "Metadonnees.h"
#include "TableIndex.h"
#include "Disk.h"

Bloc disk[MAX_BLOCKS];    // Disque virtuel avec des blocs
MetaDonnee files[MAX_FILES];   // Tableau des fichiers et leurs métadonnées
int fileCount = 0;           // Nombre de fichiers actuellement créés




int main()
{
    fichier F;
    F.MDfile = fopen("MetaDat.bin","rb+");
    MajEntetenom(&F,"MetaDat.bin");
    MajEntetenum(&F,2,4);
    MajEntetenum(&F,3,10);
    MajEntetenum(&F,4,0);
    MajeEnteteOrga(&F,1);
    MajeEntetetri(&F,2);
    AfficherEntete(F);
    fclose(F.MDfile);
    return 0;
}
