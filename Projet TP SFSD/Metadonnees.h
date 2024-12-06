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

int lireEntete (File *F, int nc);
void MajEntetenom(FILE *F, char nom[30]);
void MajEntetenum(FILE *F, int nc, int val);
void MajeEnteteOrga(FILE *F, int nc);
void MajeEntetetri(FILE *F, int nc);

#endif // METADONNEES_H_INCLUDED
