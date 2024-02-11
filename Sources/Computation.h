#ifndef Computation_h
#define Computation_h

    #include "listeSC.h"
    #include "Util.h"

    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>

typedef struct{

    double x; //distance suivant x
    double dx; //�chantillonage spatial suivant x
    int Nx; //Nombre d'�chantillion spatial suivant x

    double y; //distance suivant y
    double dy; //�chantillonage spatial suivant y
    int Ny; //Nombre d'�chantillion spatial suivant y

    double t; //temps de simulation
    double dt; //�chantillonage temporel
    int Nt; //Nombre d'�chantillion temporel

    int** HeatSources; //Sources de challeur fixe (0) ou variable(1)
    double** LocalAlpha; //Valleur de alpha en chaque points

}Environment_Definition; //Structure contenant les informations du environments du probleme


typedef struct {

    double** Temp_Init; //matrice pour initialiser la temp�rautre
    Environment_Definition Domaine_Init; //pour initialiser notre environment �tude

} Issue_Condition; //Structure contenant les informations du problemme

typedef struct{
    double K; //conductivit� thermique
    double C; //capacit� calorifique
    double rho; //masse volumique

    double alpha; //coefficient
    char Nom[30]; //Nom du materiaux

}Materiau; //Structure contenant les informations d'un mat�riau donn�

typedef struct{

    ListeSC *Elements;//Liste chain�e de Materiaux

}Materials_List;//Structure contenant une liste chain�e de Materiaux


void InhomogeneousEnvironments(char* materialAdress, char* issueConditionAdress, char* heatSourcesAdress,char *materialsTypesAdress);//Fonction principale

void ComputationInhomogeneous2D(Issue_Condition init,char* directoryAdress);//Fonction de computation et d'�criture des r�sultat pour une simulation en 2D

void ComputationInhomogeneous1D(Issue_Condition init ,char* adress);//Fonction de computation et d'�criture des r�sultat pour une simulation en 1D

void InitialisationTXT1D(FILE*ftxt, Issue_Condition init,double *computation);//Fonction d'initialisation du file de r�ponse 1D

void ConcatenationFile2D(char* adress,const char* debut,int i);//Contatenation d'adress pour la l�cture et l'�criture de file txt

void Initilisation_Computation2D(char* adress,Issue_Condition init,double **computation);//Fonction d'initialisation des computations en 2D


#endif // Computation_h
