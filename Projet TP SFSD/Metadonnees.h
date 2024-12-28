#ifndef METADONNEES_H_INCLUDED
#define METADONNEES_H_INCLUDED
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

FILE *Meta; //Contient les fichiers metadonnees

MetaDonnee creationFichier();
void creationFichierMetadonnees(fichier *F); //Cree un fichier de metadonnees
void lireNomFichier(fichier F, char nomFichier[30]); //Afin de lire le nom du fichier
int lireEntete (fichier F, int nc);  // 2 pour le nombre de blocs, 3 pour le nombre d'enregistrements et 4 pour la premiere adresse
typeOrganisation lireEnteteGlobal(fichier F); //Afin de lire le type d'organisation (chainee ou contigue)
bool liretypeTri(fichier F); // Afin de le type de tri
void MajEntetenom(fichier *F, char nom[30]); //Met a jour le nom du fichier
void MajEntetenum(fichier *F, int nc, int val); //Met a jour au choix avec:  2 pour le nombre de blocs, 3 pour le nombre d'enregistrements et 4 pour la premiere adresse
void MajeEnteteOrga(fichier *F, int nc); //1 pour chainee 2 pour contigue
void MajeEntetetri(fichier *F, int nc); //1 pour triee et 2 pour non triee
void AfficherEntete(); //Affiche les fichiers de metadonnees
void OuvrirFichier(fichier *F, char mode);
void fermerFichier(fichier F);
int AllouerBloc();
void chargerMetadonnees(fichier F);
void chargerFichierMetadonnees(int premiereAdresse, fichier *F);
void rechercheFichierMeta(int nBloc, fichier *F);
void rechercheNomFichier(fichier *F, char filename[30]);
#endif // METADONNEES_H_INCLUDED
