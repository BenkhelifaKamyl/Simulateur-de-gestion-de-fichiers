#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "TextColor.h"
#include "Metadonnees.h"
#include "TableIndex.h"
#include "Disk.h"

Bloc disk[MAX_BLOCKS];      // Disque virtuel avec des blocs
MetaDonnee files[MAX_FILES];   // Tableau des fichiers et leurs métadonnées
int fileCount = 0;           // Nombre de fichiers actuellement créés
Index TableAllocation[MAX_BLOCKS];



int main()
{

    return 0;
}
