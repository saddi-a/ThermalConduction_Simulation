#include "Calculs.h"

    #include "Initialisation.h"
    #include "listeSC.h"

    #define TXT ".txt"
    #define DECIMALE 10
    #define INHOMOGENE2D "data/Resultat/Inhomogene/Inhomogene"
    #define INHOMOGENE1D "data/Resultat/Inhomogene.txt"
    #define VARIABLE2D "data/ChaleurVariable/ChaleurVariable"
    #define VARIABLE1D "data/ChaleurVariable.txt"


/**Choisis le type du problème (bi ou mono) en fonction de ses dimensuions
	  * @param adresseMat : adresse qui contient les caractéristiques des matériaux
	  * @param adresseCondPb : adresse d'un fichier qui contient les données de la structure Condition_Probleme
	  * @param adresseSourcesDeChaleur : adresse d'un fichier qui contient l'emplacement et les variations des sources de chaleur
	  * @param adresseTypeMat : adresse qd'un fichier ui contient les nombres qui correspondent à différents types de matériaux
	  */

void MillieuxInhomogene(char* adresseMat, char* adresseCondPb, char* adresseSourcesDeChaleur,char *adresseTypeMat){

    ListeMateriaux materiaux = Lire_Materiaux(adresseMat);//On récupère les caractéristiques de tous les matériaux dans une liste chainée
    Condition_Probleme init = Lire_Condition_Probleme(adresseCondPb , adresseSourcesDeChaleur,adresseTypeMat,materiaux);  //On récupère toutes les données de conditions des problèmes

    if(init.Domaine_Init.Nx>2){
        switch(init.Domaine_Init.Ny){

            case 1: //pour Ny = 1
                CalculsInhomogene1D(init ,INHOMOGENE1D);
            break;

            case 2: //pour Ny = 2
                printf("Erreur, il ne peut y avoir 2 echantillons en y (1 ou plus de 2) \n");
            break;

            default : //pour Ny>2
                CalculsInhomogene2D(init ,INHOMOGENE2D);
            }
    }
    else
        printf("erreur il doit y avoir plus de 2 echantillons en x \n");

    Liberer_Condition_Probleme(&init);
    Liberer_ListeMateriaux(&materiaux);//libération de la mémoire
}


/** Modélisation de diffusion de chaleur dans un espace bidimensionnel ihonomogène
	  * @param init : Condition_Probleme avec données du problème à résoudre
	  * @param RacineAdresse : où placer les fichiers réponses
	  */

void CalculsInhomogene2D(Condition_Probleme init,char* RacineAdresse) {
    // -------------- //
    // INITIALISATION //
    int n, j, m, i; //indice spatial et temporel
    double KG, KC, KD, KH, KB; //temperature aux points Gauche Centre Droite Haut Bas
    char adresse[50];//vecteur d'adresse de lecture et d'écriture de
    double Y,X,R;//Coefficient de l'équation differentielle discrétisée
    double **calcule = Allocation_Tableau_double_2D(init.Domaine_Init.Nx,init.Domaine_Init.Ny);
    double *tmpData=calloc(init.Domaine_Init.Ny, sizeof( double ));//permet de sauvegarder nos températures précédentes
    double** chaleur=Allocation_Tableau_double_2D(init.Domaine_Init.Nx,init.Domaine_Init.Ny);

    ConcatenationFichier2D(adresse,RacineAdresse,0); //Initialisation de l'adresse des fichiers
    Initilisation_Calculs2D(adresse,init,calcule); //On remplit les fichiers texte des dimensions du problème


    // -------------- //
    //     CALCULE    //
    for (j=1; j<=init.Domaine_Init.Nt; j++){

        ConcatenationFichier2D(adresse,VARIABLE2D,j); //Réinitialisation de l'adresse pour chaleur

        Lire_ChaleurVariable(adresse,chaleur); //On récupère les nouvelles valeurs des sources de chaleur au temps j

        ConcatenationFichier2D(adresse,RacineAdresse,j); //Réinitialisation de l'adresse pour placer les fichiers réponses de diffusion

        FILE*FTXT = fopen(adresse, "w"); //fichier réponse
        if(FTXT != NULL){//test d'ouverture

            fprintf( FTXT, "%d %d ",init.Domaine_Init.Nx,init.Domaine_Init.Ny);//Dimension de la matrice de solution [X,Y]
            fprintf(FTXT,"%lf\n",init.Domaine_Init.dt * j); //On écrit le moment tj

            for(i=0;i<init.Domaine_Init.Nx;i++){ //pour chaque échantillon sur x
                fprintf(FTXT,"%lf\t",calcule[0][i]); //on écrit la température Ta au bord
                tmpData[i]=calcule[0][i]; //On la sauvegarde dans tmpData
            }
            fprintf(FTXT,"\n");

            for(m=1;m<init.Domaine_Init.Ny-1;m++){ //pour chaque échantillon sur y

                fprintf(FTXT,"%lf\t",calcule[m][0]);

                //Réinitialisation des valeurs de températures aux points importants
                										KH = tmpData[1];
                KG = calcule[m][0]; KC = calcule[m][1]  ;KD = calcule[m][2];
               											KB = calcule[m+1][1];

                for (n=1; n<(init.Domaine_Init.Nx-1); n++){ //pour chaque échantillon sur x

                    Y=(KH+KB-2*KC)/(init.Domaine_Init.dy * init.Domaine_Init.dy);
                    X=(KG+KD-2*KC)/(init.Domaine_Init.dx * init.Domaine_Init.dx);

                    R= init.Domaine_Init.dt * init.Domaine_Init.AlphaLocal[m][n];
                    calcule[m][n] = KC + (init.Domaine_Init.SourcesDeChaleur[m][n] * R *(Y+X))+ chaleur[m][n];

                  	//Réinitialisation des points de températures importantes
                    KG = KC;
                    KC = KD;
                    KD = calcule[m][n+2];
                    KB = calcule[m+1][n+1];
                    KH = tmpData[n+1];

                    tmpData[n]=calcule[m][n]; //sauvergarde des températures

                    fprintf( FTXT,"%lf\t", calcule[m][n]); //enregistrement des données

                } //fin for n

                fprintf(FTXT,"%lf\n",calcule[m][init.Domaine_Init.Nx-1]); //enregistrement des données

            } //fin for m

            for(i=0;i<init.Domaine_Init.Nx;i++)
                fprintf(FTXT,"%lf\t",calcule[init.Domaine_Init.Ny-1][i]); //On écrit Tb, température à l'autre bord

            fclose(FTXT); //fermeture de fichier

        }else
        printf("Erreur a l'ouverture du fichier de Resultat : %s \n",adresse);

    }//fin for j

    Liberer_Tableau((void**)calcule,init.Domaine_Init.Ny);
    Liberer_Tableau((void**)chaleur,init.Domaine_Init.Ny);
    calcule=NULL; //libération de la mémoire
}


/** Modélisation de diffusion de chaleur dans un espace monodimensionnel ihonomogène
	  * @param init : Condition_Probleme avec données du problème à résoudre
	  * @param adresse : où placer les fichiers réponses
	  */

void CalculsInhomogene1D(Condition_Probleme init ,char* adresse){
    // -------------- //
    // INITIALISATION //
    double** chaleur =Allocation_Tableau_double_2D(init.Domaine_Init.Nx,init.Domaine_Init.Nt); //création d'une matrice contenant les valeurs des sources de chaleur

    Lire_ChaleurVariable(VARIABLE1D,chaleur);//on remplit cette matrice

    FILE*ftxt = fopen(adresse, "w"); //fichier réponse
    if(ftxt != NULL){//test d'ouverture

        int n, j; //indice spatial et temporel
        double K0, K1, K2; //temperature aux points a-1, a, a+1

        double s = init.Domaine_Init.dt  / (init.Domaine_Init.dx * init.Domaine_Init.dx); //Coefficient de l'équation differentielle discrétisée

        double *calcule = calloc(init.Domaine_Init.Nx, sizeof( double )); //Vecteur 1D contenant les valeurs de la simulation
        //taille variable selon le nombre d'échantillon
        InitialisationTXT1D(ftxt, init, calcule);

        // -------------- //
        //     CALCULE    //
        for (j=1; j<=init.Domaine_Init.Nt; j++){
            fprintf(ftxt,"%lf\t%lf\t",init.Domaine_Init.dt*j,calcule[0]);//colonne 1 temps colonne 2 Ta
            K0 = calcule[0]; K1 = calcule[1]; K2 = calcule[2];//temperature aux points a-1, a, a+1

            for (n=1; n<(init.Domaine_Init.Nx-1); n++){

                calcule[n] = K1 + (init.Domaine_Init.SourcesDeChaleur[0][n] *init.Domaine_Init.AlphaLocal[0][n] * s * (K0+K2-2*K1) )+ chaleur[j-1][n];

                K0 = K1;//temperature aux points a-1, a, a+1
                K1 = K2;
                K2 = calcule[n+2];
                fprintf( ftxt,"%lf\t", calcule[n]); //enregistrement des données

            }//fin for j
        fprintf(ftxt,"%lf \n",calcule[init.Domaine_Init.Nx-1]); //derniere colone Tb
        } //fin for n

        fclose(ftxt); //fermeture de fichier

        Liberer_Tableau((void**)chaleur,init.Domaine_Init.Nt-1);
        free(calcule);
        calcule=NULL; //libération de la mémoire
    }
    else
        printf("Erreur a l'ouverture du fichier de Resultat \n");
}


/**Ecriture des dimensions et des températures initiales du problème monodimensionnel dans un fichier txt
	  * @param ftxt : fichier texte à initialiser
	  * @param init : Condition_Probleme
	  * @param calcule : vecteur 1D contenant les valeurs de la simulation
	  */

void InitialisationTXT1D(FILE*ftxt,Condition_Probleme init,double *calcule){
        fprintf( ftxt, "%d %d\n",init.Domaine_Init.Nx,init.Domaine_Init.Nt);//Dimension de la matrice de solution [X,Y]
        int n;

        for (n=0; n<=init.Domaine_Init.Nx; n++)
            fprintf( ftxt, "%lf\t", n*init.Domaine_Init.dx); //écriture des coordonnées en X en premiere ligne
        fprintf(ftxt,"\n");


        //ecriture des conditions initales en premiere ligne
        fprintf(ftxt,"%lf\t",0.0);//t0

        for (n=0; n<init.Domaine_Init.Nx; n++){
            calcule[n]=init.Temp_Init[0][n];//On récupère les températures initiales
            fprintf( ftxt,"%lf\t", calcule[n]);//On les écrit dans le fichier ftxt
        }

        fprintf(ftxt,"\n");
}


/**Initilisation de l'adresse d'un fichier
	  * @param adresse : adresse du fichier à concaténer
	  * @param racine : début adresse où placer les fichiers à concaténer
	  * @param  i : numéro du fichier
	  */

void ConcatenationFichier2D(char *adresse,const char* racine,int i){
    char nombre[50];
    strcpy(adresse, racine);//copie racine à la suite d'adresse
    itoa(i,nombre,DECIMALE);
    strcat(adresse,"_");
    strcat(adresse,nombre);
    strcat(adresse,TXT); //On obtient : adresse = data/Resultat/Inhomogene/Inhomogene_i.txt
}


/**Ecriture des dimensions et températures initiales du problème bidimensionnel dans un fichier txt
	  * @param adresse : fichier texte à initialiser
	  * @param init : Condition_Probleme
	  * @param calcule : matrice contenant les valeurs de la simulation
	  */

void Initilisation_Calculs2D(char* adresse,Condition_Probleme init,double **calcule){
    int n,m;

    FILE*ftxt = fopen(adresse, "w"); //fichier réponse
    if(ftxt != NULL){//test d'ouverture
        fprintf( ftxt, "%d\t%d\t%d\n",init.Domaine_Init.Nx,init.Domaine_Init.Ny,init.Domaine_Init.Nt);//Dimension de la matrice de solution [X,Y]

        for(n=0; n<init.Domaine_Init.Nx; n++){//pour chaque échantillon sur x
            fprintf( ftxt,"%lf\t", init.Domaine_Init.dx*n); //On écrit chaque valeur que prend x
        }
        fprintf( ftxt,"\n");

        for(m=0; m<init.Domaine_Init.Ny; m++){ //pour chaque échantillon sur y
            fprintf( ftxt,"%lf\t", init.Domaine_Init.dy*m);//On écrit chaque valeur que prend y
        }
        fprintf( ftxt,"\n");

        for(m=0; m<init.Domaine_Init.Ny; m++){
            for (n=0; n<init.Domaine_Init.Nx; n++){
                calcule[m][n]=init.Temp_Init[m][n]; //On récupère les températures initiales
                fprintf( ftxt,"%lf\t", calcule[m][n]); //On les écrit dans le fichier ftxt
            }
        fprintf(ftxt,"\n");
        }
    fclose(ftxt);//fermeture du fichier
    }
    else
        printf("Erreur a l'ouverture du fichier de Resultat : %s \n",adresse);

}

