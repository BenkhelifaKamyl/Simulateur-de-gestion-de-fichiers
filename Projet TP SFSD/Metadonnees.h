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
    FILE *file;
} fichier;

MetaDonnee creationFichier(char filename[30]);
int lireEntete (fichier F, int nc);
typeOrganisation lireEnteteGlobal(FILE F);
typeTri lireEnteteInterne(FILE F);
void MajEntetenom(fichier *F, char nom[30]);
void MajEntetenum(fichier *F, int nc, int val);
void MajeEnteteOrga(fichier *F, int nc); //1 pour chainee 2 pour contigue
void MajeEntetetri(fichier *F, int nc); //1 pour triee et 2 pour non triee
void OuvrirFichier(fichier *F, char nomFichier[30], char mode);
void fermerFichier(fichier *F);
void LireBloc(fichier *F, int i, Bloc *Buffer);
void EcrireBloc(fichier *F, int i, BLOC Buffer);
#endif // METADONNEES_H_INCLUDED
