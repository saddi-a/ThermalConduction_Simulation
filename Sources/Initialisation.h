#ifndef Initialisation_h
#define Initialisation_h

#include <stdlib.h>
#include <stdio.h>
#include "Calculs.h"


Condition_Probleme Creer_Condition_Probleme(int Nx , int Ny,Def_Domaine domain); //Initialiser une structure Condition_Problemme

Condition_Probleme Lire_Condition_Probleme(char* adresse , char* adresseSourcesDeChaleur,char*adresseTypeMat,ListeMateriaux materiaux);//Lecture et création de la structure des conditions du problemme

void Liberer_Condition_Probleme(Condition_Probleme *init); //Libération de la mémoire allouée a la structure des conditions du problemme

ListeMateriaux Creer_Materiaux(); //Crée une liste chainée vide de materiaux

ListeMateriaux Lire_Materiaux(char* adresse);//Fonction de lecture du fichier de materiaux pour les enregistrer dans une liste chainée

void Liberer_ListeMateriaux(ListeMateriaux *mat);//liberation la mémoire alloué dynamiquement

int** Allocation_Tableau_int_2D(int x,int y);//Alocation dynamique d'une matrice d'entier

double** Allocation_Tableau_double_2D(int x,int y);//Alocation dynamique d'une matrice de réelle

void Liberer_Tableau(void **matrice,int y);//Libération de la mémoire allouée pour une matrice

Def_Domaine Lire_Def_Domaine(char* adresseSourceDeChaleur,char* adresseTypeMat,ListeMateriaux materiaux);//lecture des fichier de la structure de définition de domaine

int** Lire_Source_De_Chaleur(char *adresse);//Lecture du fichier de source de chaleur

int** Lire_TypeMat(char* adresse_Typ_Mat,int length,int *x,int *y);//Lecture du fichier de type de materiaux

double** Calcul_alpha(char *adresse_Typ_Mat,ListeMateriaux materiaux);//Détérmination de la valleur local du coefficient alpha

void Lire_ChaleurVariable(char *adresse,double** chaleur);//Lecture du fichier challeur variable

#endif
