#ifndef METADONNEES_H_INCLUDED
#define METADONNEES_H_INCLUDED
#define MAX_FILES 10
typedef enum { Contigue, Chainee } typeOrganisation;
typedef enum {triee, nonTriee} typeTri;

typedef struct{
    char name[30];
    int nbBlocs;
    int nbEnregistrements;
    int premiereAdresse;
    typeOrganisation globalOrg;
    typeTri interneOrg;
} MetaDonnee;
typedef struct{
    FILE *MDfile;
    FILE *TableIndex;
} fichier;

MetaDonnee Meta[MAX_FILES]; //Contient les fichiers metadonnees

void creationFichier(MetaDonnee *MD, int choix); //1 pour chainee et 2 pour contigue
void creationFichierMetadonnees(fichier *F, int choix); //Cree un fichier de metadonnees
void lireNomFichier(fichier F, char nomFichier[30]); //Afin de lire le nom du fichier
int lireEntete (fichier F, int nc);  // 2 pour le nombre de blocs, 3 pour le nombre d'enregistrements et 4 pour la premiere adresse
typeOrganisation lireEnteteGlobal(fichier F); //Afin de lire le type d'organisation (chainee ou contigue)
bool liretypeTri(fichier F); // Afin de le type de tri
void MajEntetenom(fichier *F, char nom[30]); //Met a jour le nom du fichier
void MajEntetenum(fichier *F, int nc, int val); //Met a jour au choix avec:  2 pour le nombre de blocs, 3 pour le nombre d'enregistrements et 4 pour la premiere adresse
void MajeEnteteOrga(fichier *F, int nc); //1 pour chainee 2 pour contigue
void MajeEntetetri(fichier *F, int nc); //1 pour triee et 2 pour non triee
void AfficherEntete(); //Affiche les fichiers de metadonnees
void OuvrirFichier(fichier *F, char mode, int choix); //Cas 2 contigu ou 1 chainee
void fermerFichier(fichier F);
int AllouerBlocChainee(int i);
int AllouerBlocContigue(int i); //Alloue un bloc a partir de l'indice i
void initializeMetaFiles(int choix); //Initialise la MS des fichiers Meta
void chargerMetadonnees(fichier F); //Charge le fichier de metadonnees en "MS"
void chargerFichierMetadonnees(int premiereAdresse, fichier *F); //Charge le fichier de metadonnees selon la premiere adresse du fichier
void rechercheFichierMeta(int nBloc, fichier *F); //Recupere le fichier de metadonnees selon un numero de bloc
void rechercheNomFichier(fichier *F, char filename[30], int *i); //Recherche un fichier selon son nom
void supprimeFichierMetadonnees(fichier *F);
void renommerFichier(fichier *F);
#endif // METADONNEES_H_INCLUDED
