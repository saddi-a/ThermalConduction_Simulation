#include "Initialisation.h"

    #include "Initialisation.h"
    #include "listeSC.h"
    #define TXT ".txt"
    #define DEBUT_ADRESSE "data/Resultat/"
    #define DECIMALE 10

/** Cr�ation de Issue_Condition
	  * @param Nx : �chantillonage spatial sur x (abcisses)
	  * @param Ny : �chantillonage spatial sur y (ordonn�es)
	  * @param domain : donn�es du environment d'�tude du probl�me
	  * @return init : Issue_Condition g�n�r�
	  */

Issue_Condition Create_Issue_Condition(int Nx , int Ny,Environment_Definition domain){

    Issue_Condition init;
    init.Domaine_Init=domain;
    init.Temp_Init= Allocate_Table_double_2D(Nx,Ny);

    return init;
}


/** Suppressions des deux matrixs Sources de chaleur et Temp_Init(temp�ratures initiales) dans la structure Issue_Condition
	  * @param init : Issue_Condition � supprimer
	  */

void Free_Issue_Condition(Issue_Condition *init){
    int y=(init->Domaine_Init.Ny); //r�cup�re la taille des matrixs
    Free_Table((void **)init->Domaine_Init.HeatSources,y); //appelle la fonction Free_Table pour supprimer source de chaleur
    Free_Table((void **)init->Temp_Init,y); //appelle la fonction Free_Table pour supprimer Temp_Init
}


/** Lecture et �criture des conditions du probl�me
	  * @param adress : adress d'un file qui contient les donn�es de la structure Issue_Condition
		* @param heatSourcesAdress : adress d'un file qui contient l'emplacement et les variations des sources de chaleur
	  * @param materialsTypesAdress : adress qd'un file ui contient les nombres qui correspondent � diff�rents types de mat�riaux
	  * @param materials : liste contenant les mat�riaux et leurs caract�ristiques
	  * @return init : Issue_Condition g�n�r�
	  */

Issue_Condition Read_Issue_Condition(char* adress , char* heatSourcesAdress,char*materialsTypesAdress,Materials_List materials){
    Issue_Condition init;
    init.Temp_Init=NULL; //s�curit�
    init.Domaine_Init.HeatSources=NULL; //s�curit�

    Environment_Definition domain=Read_Environment_Definition(heatSourcesAdress,materialsTypesAdress, materials); //on cr�e un environment
    double x,y,t,T;
    int Nx,Ny,Nt,i,j;


    FILE* fTxt=fopen(adress,"r"); //ouverture du file
    if(fTxt != NULL){
        if(fscanf(fTxt,"%lf %d %lf %d %lf %d",&x,&Nx,&y,&Ny,&t,&Nt)==6 ){ //Lecture et stockage des conditions initiales
            init = Create_Issue_Condition(Nx,Ny,domain); //cr�ation de la structure des conditions initiales
            //�criture des param�tres d'initialisation dans init
            init.Domaine_Init.x=x;    init.Domaine_Init.dx=x/Nx;   init.Domaine_Init.Nx=Nx;
            init.Domaine_Init.y=y;    init.Domaine_Init.dy=y/Ny;   init.Domaine_Init.Ny=Ny;
            init.Domaine_Init.t=t;    init.Domaine_Init.dt=t/Nt;   init.Domaine_Init.Nt=Nt;

            for(i=0; i<Ny; i++){
                for(j=0;j<Nx;j++){
                    if( fscanf(fTxt,"%lf ",&T) ) //�criture des temp�ratures initiales dans Temp_Init
                        init.Temp_Init[i][j]=T;
                    else
                        printf("Erreur, les valeurs d'initialisation n'ont pas ete lue\n");

                    }//fin for Nx
                }//fin for Ny
            }//fin if scanf
        else
            printf("Erreur, les param�tres d'initialisation n'ont pas �t� lue\n");

        fclose(fTxt); //fermeture du file
        }
    else
        printf("Erreur, le file d'initialisation n'a pas �t� ouvert\n") ;

    return init;
}


/** Lecture et �criture des donn�es du environment d'�tude
		* @param heatSourcesAdress : adress d'un file qui contient l'emplacement et les variations des sources de chaleur
	  * @param materialsTypesAdress : adress qd'un file ui contient les nombres qui correspondent � diff�rents types de mat�riaux
	  * @param materials : liste contenant les mat�riaux et leurs caract�ristiques
	  * @return environment : Environment_Definition g�n�r�
	  */

Environment_Definition Read_Environment_Definition(char* heatSourcesAdress,char* materialsTypesAdress,Materials_List materials){

    Environment_Definition environment;
    environment.HeatSources=Read_Source_De_Chaleur(heatSourcesAdress ); //cr�er la matrix Sources de Chaleur qui indique si il y a une source de chaleur en chaque pts de l'espace

    environment.LocalAlpha=Compute_alpha(materialsTypesAdress,materials); //cr�er la matrix HeatSources qui contientle alpha en chacun des points de l'espace �tudi�

    return environment;
}


/** Lecture et �criture d'une matrix qui indique la position des sources de Chaleur
		* @param adress : adress d'un file qui contient l'emplacement et les variations des sources de chaleur
	  * @return environment : sourceDeChall g�n�r�
	  */

int** Read_Source_De_Chaleur(char *adress){

    int **sourceDeChall=NULL;//s�curit�
    int x,y,i,j;
    FILE* Ftxt=fopen(adress,"r");//ouverture du file
    if(Ftxt != NULL){//test d'ouverture
        fscanf(Ftxt,"%d %d",&x,&y);//on r�cup�re les dimensions de la matrix � cr�er = dimensions du probl�me

        sourceDeChall = Allocate_Table_int_2D(x,y); //cr�ation de la matrix sourceDeChall vide

        for(i=0; i<y; i++){

            for(j=0;j<x;j++){
                if ( fscanf(Ftxt,"%d ",&(sourceDeChall[i][j])) )  {//remplit de donn�es sur les sources la matrix
                }
                else
                    printf("Erreur, les valeurs de sources de chaleur n'ont pas ete lue\n");
            }
        }

        fclose(Ftxt); //fermeture du file
    }
    else
        printf("Erreur, le file des sources de chaleur n'a pas ete ouvert\n") ;

    return sourceDeChall;
}


/** Lecture et cr�ation d'une liste de mat�riaux
	  * @param adress : adress d'un file txt contenant les caract�ristiques et noms des mat�riaux
	  * @return mat : liste chain�e contenant les nom et donn�es des mat�riaux disponibles
	  */

Materials_List Read_Materiaux(char* adress){
    Materiau element;

    Materials_List mat=Create_Materiaux();

    FILE* fTxt=fopen(adress,"r"); //ouverture
    if(fTxt != NULL){ //test d'ouverture
        while(fscanf(fTxt,"%lf %lf %lf",&(element.K),&(element.C),&(element.rho))==3 ){ //Lecture des caract�ristiques des materials

            fscanf(fTxt,"%s",&(element.Nom)); //Lecture des noms des materials

            element.alpha = (element.K)/(element.rho * element.C); //calcul de alpha

            ajout(mat.Elements, &element, 2); //Ajout a la liste mat
        }
        fclose(fTxt);//fermeture du file
    } else {
    printf("Erreur, le file de Materiaux n'a pas �t� ouvert \n") ;
    }

    return mat;
}


/** Cr�ation d'une liste de mat�riaux vide
	  * @return mat : liste g�n�r�e
	  */

Materials_List Create_Materiaux(){
    Materials_List mat;

    mat.Elements = creerListe(sizeof(Materiau));

    return mat;
}


/** Suppression d'une liste de mat�riaux
		* @param mat : liste � supprimer
	  */

void Free_Materials_List(Materials_List *mat){
    freeListe(mat->Elements);
    mat->Elements=NULL;//s�curit�
}


/** Cr�ation d'une matrix d'entier vide
	  * @param x : nombre de colonne
	  * @param y : nombre de ligne
	  * @return matrix g�n�r�
	  */

int** Allocate_Table_int_2D(int x,int y){

    int **matrix=NULL;//s�curit�
    matrix=calloc(y, sizeof( int* )); //cr�ation du vecteur de pointeur
    for(int i=0;i<y;i++)
        matrix[i]=calloc(x, sizeof( int )); //cr�ation des vecteurs d'entier
    return matrix;
}


/** Cr�ation d'une matrix de type double vide
	  * @param x : nombre de colonne
	  * @param y : nombre de ligne
	  * @return matrix g�n�r�
	  */

double** Allocate_Table_double_2D(int x,int y){
    double **matrix=NULL;//s�curit�
    matrix=calloc(y, sizeof( double* )); //cr�ation du vecteur de pointeur
    for(int i=0;i<y;i++)
        matrix[i]=calloc(x, sizeof( double )); //cr�ationdes des vecteurs d'entier
    return matrix;
}


/** Suppression d'un tableau
	  * @param matrix : matrix � supprimer
	  * @param y : taille de la matrix � supprimer
	  */

void Free_Table(void **matrix,int y){
    for(int i=0;i<y;i++){
        free(matrix[i]);
    }
    free(matrix);
    matrix=NULL;//s�curit�
}


/** Lecture et �criture d'une matrix contenant les types de mat�riaux
	  * @param materialsTypesAdress : adress d'un file ui contient les nombres qui correspondent � diff�rents types de mat�riaux
	  * @param length : nombre de mat�riau disponible
	  * @param x : nombre de colonne
	  * @param y : nombre de ligne
	  * @return
	  */

int** Read_TypeMat(char* materialsTypesAdress,int length,int *x,int *y){
    int** Type_Mat=NULL; //s�curit�
    int i=0,j=0;
    int A;

    FILE* ftxt=fopen(materialsTypesAdress,"r");//ouverture du file
    if(ftxt!=NULL){ //test d'ouverture

        fscanf(ftxt,"%d %d",&i,&j); //r�cup�re les dimensions du probl�me
        *x=i;*y=j;
        Type_Mat=Allocate_Table_int_2D(i,j); //cr�ation de la matrix vide Type_Mat de taille i, j
        for(i=0;i<*y;i++){
            for(j=0;j<*x;j++){ //pour chaque point de l'espace

                fscanf(ftxt,"%d",&A);
                if(A<length){ //on v�rifie que le mat�riau existe
                    Type_Mat[i][j]=A; //on l'ajoute � Type_Mat
                }
                else{
                    printf("Materiaux non d�finie dans le file materials \n");
                }
            }
        }
        fclose(ftxt); //fermeture du file
    }
    else
        printf("erreur a l'ouverture de file TypesDeMateriaux \n");


    return Type_Mat;
}


/** Cr�er une matrix contenant tous les alphas de l'espace �tudi�
	  * @param materialsTypesAdress : adress d'un file ui contient les nombres qui correspondent � diff�rents types de mat�riaux
	  * @param materials : liste contenant les mat�riaux et leurs caract�ristiques
	  * @return localAlpha : matrix r�ponse
	  */

double** Compute_alpha(char *materialsTypesAdress,Materials_List materials){

    int x,y;
    int k= materials.Elements->length;
    int** typeMat =Read_TypeMat(materialsTypesAdress,k, &x, &y);//�criture de typeMat

    double** localAlpha=Allocate_Table_double_2D(x,y); //cr�ation de la matrix localAlpha
    int i=0,j=0;
    double* alphaMat=calloc(k,sizeof(double));
    Materiau elements;
    ListeSC* chaine=materials.Elements;

    for( chaine->current =  chaine->root; hasNext( chaine); getNext( chaine) ){//pour chaque �l�ment de la liste de mat�riaux
        elements= * ((Materiau*) (chaine->current->data)) ;
        alphaMat[j]=elements.alpha; //on remplit alphaMat des alphas en fonction des mat�riaux correspondant
        j++;
    }

    for(i=0;i<y;i++){
        for(j=0;j<x;j++) //pour chaque point de l'espace
            localAlpha[i][j]=alphaMat[typeMat[i][j]];//on remplit localAlpha
    }

    free(alphaMat); //lib�ration de la m�moire
    Free_Table((void**) typeMat,y); //lib�ration de la m�moire

    return localAlpha;
}


/** Ajout ou supression de chaleur
	  * @param adress : adress contenant les valeurs des temp�ratures des sources
	  * @param chaleur : matrix � compl�ter
	  */

void Read_VariableHeat(char *adress,double** chaleur){
    int x,y,i,j;
    double A;
    FILE*ftxt=fopen(adress,"r");//ouverture du file
    if(ftxt!=NULL){//test d'ouverture
        fscanf(ftxt,"%d %d",&x,&y); //on r�cup�re les dimensions du probl�me
        for(i=0;i<y;i++){
            for(j=0;j<x;j++){ //pour chaque point de l'espace �tudi�
                fscanf(ftxt,"%lf",&A);
                chaleur[i][j]=A; //on modifie la temp�rature de la source au point i, j
            }

        }
    fclose(ftxt);//fermeture du file
    }
    else
        printf("erreur a l'ouverture du file de Chaleur Variable %s\n",adress);
}
