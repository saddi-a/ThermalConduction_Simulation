#include "Initialisation.h"

    #include "Initialisation.h"
    #include "listeSC.h"
    #define TXT ".txt"
    #define DEBUT_ADRESSE "data/Resultat/"
    #define DECIMALE 10

/** Création de Condition_Probleme
	  * @param Nx : échantillonage spatial sur x (abcisses)
	  * @param Ny : échantillonage spatial sur y (ordonnées)
	  * @param domain : données du domaine d'étude du problème
	  * @return init : Condition_Probleme généré
	  */

Condition_Probleme Creer_Condition_Probleme(int Nx , int Ny,Def_Domaine domain){

    Condition_Probleme init;
    init.Domaine_Init=domain;
    init.Temp_Init= Allocation_Tableau_double_2D(Nx,Ny);

    return init;
}


/** Suppressions des deux matrices Sources de chaleur et Temp_Init(températures initiales) dans la structure Condition_Probleme
	  * @param init : Condition_Probleme à supprimer
	  */

void Liberer_Condition_Probleme(Condition_Probleme *init){
    int y=(init->Domaine_Init.Ny); //récupère la taille des matrices
    Liberer_Tableau((void **)init->Domaine_Init.SourcesDeChaleur,y); //appelle la fonction Liberer_Tableau pour supprimer source de chaleur
    Liberer_Tableau((void **)init->Temp_Init,y); //appelle la fonction Liberer_Tableau pour supprimer Temp_Init
}


/** Lecture et écriture des conditions du problème
	  * @param adresse : adresse d'un fichier qui contient les données de la structure Condition_Probleme
		* @param adresseSourcesDeChaleur : adresse d'un fichier qui contient l'emplacement et les variations des sources de chaleur
	  * @param adresseTypeMat : adresse qd'un fichier ui contient les nombres qui correspondent à différents types de matériaux
	  * @param materiaux : liste contenant les matériaux et leurs caractéristiques
	  * @return init : Condition_Probleme généré
	  */

Condition_Probleme Lire_Condition_Probleme(char* adresse , char* adresseSourcesDeChaleur,char*adresseTypeMat,ListeMateriaux materiaux){
    Condition_Probleme init;
    init.Temp_Init=NULL; //sécurité
    init.Domaine_Init.SourcesDeChaleur=NULL; //sécurité

    Def_Domaine domain=Lire_Def_Domaine(adresseSourcesDeChaleur,adresseTypeMat, materiaux); //on crée un domaine
    double x,y,t,T;
    int Nx,Ny,Nt,i,j;


    FILE* fTxt=fopen(adresse,"r"); //ouverture du fichier
    if(fTxt != NULL){
        if(fscanf(fTxt,"%lf %d %lf %d %lf %d",&x,&Nx,&y,&Ny,&t,&Nt)==6 ){ //Lecture et stockage des conditions initiales
            init = Creer_Condition_Probleme(Nx,Ny,domain); //création de la structure des conditions initiales
            //écriture des paramètres d'initialisation dans init
            init.Domaine_Init.x=x;    init.Domaine_Init.dx=x/Nx;   init.Domaine_Init.Nx=Nx;
            init.Domaine_Init.y=y;    init.Domaine_Init.dy=y/Ny;   init.Domaine_Init.Ny=Ny;
            init.Domaine_Init.t=t;    init.Domaine_Init.dt=t/Nt;   init.Domaine_Init.Nt=Nt;

            for(i=0; i<Ny; i++){
                for(j=0;j<Nx;j++){
                    if( fscanf(fTxt,"%lf ",&T) ) //écriture des températures initiales dans Temp_Init
                        init.Temp_Init[i][j]=T;
                    else
                        printf("Erreur, les valeurs d'initialisation n'ont pas ete lue\n");

                    }//fin for Nx
                }//fin for Ny
            }//fin if scanf
        else
            printf("Erreur, les paramètres d'initialisation n'ont pas été lue\n");

        fclose(fTxt); //fermeture du fichier
        }
    else
        printf("Erreur, le fichier d'initialisation n'a pas été ouvert\n") ;

    return init;
}


/** Lecture et écriture des données du domaine d'étude
		* @param adresseSourcesDeChaleur : adresse d'un fichier qui contient l'emplacement et les variations des sources de chaleur
	  * @param adresseTypeMat : adresse qd'un fichier ui contient les nombres qui correspondent à différents types de matériaux
	  * @param materiaux : liste contenant les matériaux et leurs caractéristiques
	  * @return domaine : Def_Domaine généré
	  */

Def_Domaine Lire_Def_Domaine(char* adresseSourceDeChaleur,char* adresseTypeMat,ListeMateriaux materiaux){

    Def_Domaine domaine;
    domaine.SourcesDeChaleur=Lire_Source_De_Chaleur(adresseSourceDeChaleur ); //créer la matrice Sources de Chaleur qui indique si il y a une source de chaleur en chaque pts de l'espace

    domaine.AlphaLocal=Calcul_alpha(adresseTypeMat,materiaux); //créer la matrice AlphaLocal qui contientle alpha en chacun des points de l'espace étudié

    return domaine;
}


/** Lecture et écriture d'une matrice qui indique la position des sources de Chaleur
		* @param adresse : adresse d'un fichier qui contient l'emplacement et les variations des sources de chaleur
	  * @return domaine : sourceDeChall généré
	  */

int** Lire_Source_De_Chaleur(char *adresse){

    int **sourceDeChall=NULL;//sécurité
    int x,y,i,j;
    FILE* Ftxt=fopen(adresse,"r");//ouverture du fichier
    if(Ftxt != NULL){//test d'ouverture
        fscanf(Ftxt,"%d %d",&x,&y);//on récupère les dimensions de la matrice à créer = dimensions du problème

        sourceDeChall = Allocation_Tableau_int_2D(x,y); //création de la matrice sourceDeChall vide

        for(i=0; i<y; i++){

            for(j=0;j<x;j++){
                if ( fscanf(Ftxt,"%d ",&(sourceDeChall[i][j])) )  {//remplit de données sur les sources la matrice
                }
                else
                    printf("Erreur, les valeurs de sources de chaleur n'ont pas ete lue\n");
            }
        }

        fclose(Ftxt); //fermeture du fichier
    }
    else
        printf("Erreur, le fichier des sources de chaleur n'a pas ete ouvert\n") ;

    return sourceDeChall;
}


/** Lecture et création d'une liste de matériaux
	  * @param adresse : adresse d'un fichier txt contenant les caractéristiques et noms des matériaux
	  * @return mat : liste chainée contenant les nom et données des matériaux disponibles
	  */

ListeMateriaux Lire_Materiaux(char* adresse){
    Materiau element;

    ListeMateriaux mat=Creer_Materiaux();

    FILE* fTxt=fopen(adresse,"r"); //ouverture
    if(fTxt != NULL){ //test d'ouverture
        while(fscanf(fTxt,"%lf %lf %lf",&(element.K),&(element.C),&(element.rho))==3 ){ //Lecture des caractéristiques des materiaux

            fscanf(fTxt,"%s",&(element.Nom)); //Lecture des noms des materiaux

            element.alpha = (element.K)/(element.rho * element.C); //calcul de alpha

            ajout(mat.Elements, &element, 2); //Ajout a la liste mat
        }
        fclose(fTxt);//fermeture du fichier
    } else {
    printf("Erreur, le fichier de Materiaux n'a pas été ouvert \n") ;
    }

    return mat;
}


/** Création d'une liste de matériaux vide
	  * @return mat : liste générée
	  */

ListeMateriaux Creer_Materiaux(){
    ListeMateriaux mat;

    mat.Elements = creerListe(sizeof(Materiau));

    return mat;
}


/** Suppression d'une liste de matériaux
		* @param mat : liste à supprimer
	  */

void Liberer_ListeMateriaux(ListeMateriaux *mat){
    freeListe(mat->Elements);
    mat->Elements=NULL;//sécurité
}


/** Création d'une matrice d'entier vide
	  * @param x : nombre de colonne
	  * @param y : nombre de ligne
	  * @return matrice généré
	  */

int** Allocation_Tableau_int_2D(int x,int y){

    int **matrice=NULL;//sécurité
    matrice=calloc(y, sizeof( int* )); //création du vecteur de pointeur
    for(int i=0;i<y;i++)
        matrice[i]=calloc(x, sizeof( int )); //création des vecteurs d'entier
    return matrice;
}


/** Création d'une matrice de type double vide
	  * @param x : nombre de colonne
	  * @param y : nombre de ligne
	  * @return matrice généré
	  */

double** Allocation_Tableau_double_2D(int x,int y){
    double **matrice=NULL;//sécurité
    matrice=calloc(y, sizeof( double* )); //création du vecteur de pointeur
    for(int i=0;i<y;i++)
        matrice[i]=calloc(x, sizeof( double )); //créationdes des vecteurs d'entier
    return matrice;
}


/** Suppression d'un tableau
	  * @param matrice : matrice à supprimer
	  * @param y : taille de la matrice à supprimer
	  */

void Liberer_Tableau(void **matrice,int y){
    for(int i=0;i<y;i++){
        free(matrice[i]);
    }
    free(matrice);
    matrice=NULL;//sécurité
}


/** Lecture et écriture d'une matrice contenant les types de matériaux
	  * @param adresse_Typ_Mat : adresse d'un fichier ui contient les nombres qui correspondent à différents types de matériaux
	  * @param length : nombre de matériau disponible
	  * @param x : nombre de colonne
	  * @param y : nombre de ligne
	  * @return
	  */

int** Lire_TypeMat(char* adresse_Typ_Mat,int length,int *x,int *y){
    int** Type_Mat=NULL; //sécurité
    int i=0,j=0;
    int A;

    FILE* ftxt=fopen(adresse_Typ_Mat,"r");//ouverture du fichier
    if(ftxt!=NULL){ //test d'ouverture

        fscanf(ftxt,"%d %d",&i,&j); //récupère les dimensions du problème
        *x=i;*y=j;
        Type_Mat=Allocation_Tableau_int_2D(i,j); //création de la matrice vide Type_Mat de taille i, j
        for(i=0;i<*y;i++){
            for(j=0;j<*x;j++){ //pour chaque point de l'espace

                fscanf(ftxt,"%d",&A);
                if(A<length){ //on vérifie que le matériau existe
                    Type_Mat[i][j]=A; //on l'ajoute à Type_Mat
                }
                else{
                    printf("Materiaux non définie dans le fichier materiaux \n");
                }
            }
        }
        fclose(ftxt); //fermeture du fichier
    }
    else
        printf("erreur a l'ouverture de fichier TypesDeMateriaux \n");


    return Type_Mat;
}


/** Créer une matrice contenant tous les alphas de l'espace étudié
	  * @param adresse_Typ_Mat : adresse d'un fichier ui contient les nombres qui correspondent à différents types de matériaux
	  * @param materiaux : liste contenant les matériaux et leurs caractéristiques
	  * @return alphaLocal : matrice réponse
	  */

double** Calcul_alpha(char *adresse_Typ_Mat,ListeMateriaux materiaux){

    int x,y;
    int k= materiaux.Elements->length;
    int** typeMat =Lire_TypeMat(adresse_Typ_Mat,k, &x, &y);//écriture de typeMat

    double** alphaLocal=Allocation_Tableau_double_2D(x,y); //création de la matrice alphaLocal
    int i=0,j=0;
    double* alphaMat=calloc(k,sizeof(double));
    Materiau elements;
    ListeSC* chaine=materiaux.Elements;

    for( chaine->current =  chaine->root; hasNext( chaine); getNext( chaine) ){//pour chaque élément de la liste de matériaux
        elements= * ((Materiau*) (chaine->current->data)) ;
        alphaMat[j]=elements.alpha; //on remplit alphaMat des alphas en fonction des matériaux correspondant
        j++;
    }

    for(i=0;i<y;i++){
        for(j=0;j<x;j++) //pour chaque point de l'espace
            alphaLocal[i][j]=alphaMat[typeMat[i][j]];//on remplit alphaLocal
    }

    free(alphaMat); //libération de la mémoire
    Liberer_Tableau((void**) typeMat,y); //libération de la mémoire

    return alphaLocal;
}


/** Ajout ou supression de chaleur
	  * @param adresse : adresse contenant les valeurs des températures des sources
	  * @param chaleur : matrice à compléter
	  */

void Lire_ChaleurVariable(char *adresse,double** chaleur){
    int x,y,i,j;
    double A;
    FILE*ftxt=fopen(adresse,"r");//ouverture du fichier
    if(ftxt!=NULL){//test d'ouverture
        fscanf(ftxt,"%d %d",&x,&y); //on récupère les dimensions du problème
        for(i=0;i<y;i++){
            for(j=0;j<x;j++){ //pour chaque point de l'espace étudié
                fscanf(ftxt,"%lf",&A);
                chaleur[i][j]=A; //on modifie la température de la source au point i, j
            }

        }
    fclose(ftxt);//fermeture du fichier
    }
    else
        printf("erreur a l'ouverture du fichier de Chaleur Variable %s\n",adresse);
}
