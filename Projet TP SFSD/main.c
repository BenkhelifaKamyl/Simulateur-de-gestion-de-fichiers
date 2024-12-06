#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include<conio.h>
#include <windows.h>

#define FacteurBlocage 5

typedef struct{
    int ID;
    char Data[100];
    bool Supprime; //Pour la potentielle suppression logique
} Enregistrement;

typedef struct{
    Enregistrement enregistrement[FacteurBlocage];
    int adresse;
    bool free; //Pour savoir si le bloc est libre ou occupe
}BlocContigue;

typedef struct{
    Enregistrement enregistrement[FacteurBlocage];
    int adresse;
    bool free; //Pour savoir si le bloc est libre ou occupe
    int next; //Pour pointer vers le prochain bloc
}BlocChainee;

typedef union { //permet de regrouper les deux types de blocs dans une seule structure
    BlocContigue contigue;
    BlocChainee chainee;
} Bloc;

typedef struct{
    int table[50];
    int taille;
}tableAllocation;

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


//void setColor(int color) { HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); SetConsoleTextAttribute(hConsole, color); }
int main()
{

    return 0;
}
