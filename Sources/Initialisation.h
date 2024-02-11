#ifndef Initialisation_h
#define Initialisation_h

#include <stdlib.h>
#include <stdio.h>
#include "Computation.h"


Issue_Condition Create_Issue_Condition(int Nx , int Ny,Environment_Definition domain); //Initialiser une structure Condition_Problemme

Issue_Condition Read_Issue_Condition(char* adresse , char* adresseSourcesDeChaleur,char*materialsTypesAdress,Materials_List materials);//Lecture et cr�ation de la structure des conditions du problemme

void Free_Issue_Condition(Issue_Condition *init); //Lib�ration de la m�moire allou�e a la structure des conditions du problemme

Materials_List Create_Materiaux(); //Cr�e une liste chain�e vide de materials

Materials_List Read_Materiaux(char* adresse);//Fonction de lecture du file de materials pour les enregistrer dans une liste chain�e

void Free_Materials_List(Materials_List *mat);//liberation la m�moire allou� dynamiquement

int** Allocate_Table_int_2D(int x,int y);//Alocation dynamique d'une matrice d'entier

double** Allocate_Table_double_2D(int x,int y);//Alocation dynamique d'une matrice de r�elle

void Free_Table(void **matrice,int y);//Lib�ration de la m�moire allou�e pour une matrice

Environment_Definition Read_Environment_Definition(char* heatSourcesAdress,char* materialsTypesAdress,Materials_List materials);//lecture des file de la structure de d�finition de environment

int** Read_Source_De_Chaleur(char *adresse);//Lecture du file de source de chaleur

int** Read_TypeMat(char* materialsTypesAdress,int length,int *x,int *y);//Lecture du file de type de materials

double** Compute_alpha(char *materialsTypesAdress,Materials_List materials);//D�t�rmination de la valleur local du coefficient alpha

void Read_VariableHeat(char *adresse,double** chaleur);//Lecture du file challeur variable

#endif
