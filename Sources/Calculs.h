#ifndef Calculs_h
#define Calculs_h

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

    int** SourcesDeChaleur; //Sources de challeur fixe (0) ou variable(1)
    double** AlphaLocal; //Valleur de alpha en chaque points

}Def_Domaine; //Structure contenant les informations du domaines du probleme


typedef struct {

    double** Temp_Init; //matrice pour initialiser la temp�rautre
    Def_Domaine Domaine_Init; //pour initialiser notre domaine �tude

} Condition_Probleme; //Structure contenant les informations du problemme

typedef struct{
    double K; //conductivit� thermique
    double C; //capacit� calorifique
    double rho; //masse volumique

    double alpha; //coefficient
    char Nom[30]; //Nom du materiaux

}Materiau; //Structure contenant les informations d'un mat�riau donn�

typedef struct{

    ListeSC *Elements;//Liste chain�e de Materiaux

}ListeMateriaux;//Structure contenant une liste chain�e de Materiaux


void MillieuxInhomogene(char* adresseMat, char* adresseCondPb, char* adresseSourcesDeChaleur,char *adresseTypeMat);//Fonction principale

void CalculsInhomogene2D(Condition_Probleme init,char* RacineAdresse);//Fonction de calcule et d'�criture des r�sultat pour une simulation en 2D

void CalculsInhomogene1D(Condition_Probleme init ,char* adresse);//Fonction de calcule et d'�criture des r�sultat pour une simulation en 1D

void InitialisationTXT1D(FILE*ftxt,Condition_Probleme init,double *calcule);//Fonction d'initialisation du fichier de r�ponse 1D

void ConcatenationFichier2D(char *adresse,const char* debut,int i);//Contatenation d'adresse pour la l�cture et l'�criture de fichier txt

void Initilisation_Calculs2D(char* adresse,Condition_Probleme init,double **calcule);//Fonction d'initialisation des calcules en 2D


#endif // Calculs_h
