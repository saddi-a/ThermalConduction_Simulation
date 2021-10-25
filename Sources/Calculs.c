#include "Calculs.h"

    #include "Initialisation.h"
    #include "listeSC.h"

    #define TXT ".txt"
    #define DECIMALE 10
    #define INHOMOGENE2D "data/Resultat/Inhomogene/Inhomogene"
    #define INHOMOGENE1D "data/Resultat/Inhomogene.txt"
    #define VARIABLE2D "data/ChaleurVariable/ChaleurVariable"
    #define VARIABLE1D "data/ChaleurVariable.txt"


/**Choisis le type du probl�me (bi ou mono) en fonction de ses dimensuions
	  * @param adresseMat : adresse qui contient les caract�ristiques des mat�riaux
	  * @param adresseCondPb : adresse d'un fichier qui contient les donn�es de la structure Condition_Probleme
	  * @param adresseSourcesDeChaleur : adresse d'un fichier qui contient l'emplacement et les variations des sources de chaleur
	  * @param adresseTypeMat : adresse qd'un fichier ui contient les nombres qui correspondent � diff�rents types de mat�riaux
	  */

void MillieuxInhomogene(char* adresseMat, char* adresseCondPb, char* adresseSourcesDeChaleur,char *adresseTypeMat){

    ListeMateriaux materiaux = Lire_Materiaux(adresseMat);//On r�cup�re les caract�ristiques de tous les mat�riaux dans une liste chain�e
    Condition_Probleme init = Lire_Condition_Probleme(adresseCondPb , adresseSourcesDeChaleur,adresseTypeMat,materiaux);  //On r�cup�re toutes les donn�es de conditions des probl�mes

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
    Liberer_ListeMateriaux(&materiaux);//lib�ration de la m�moire
}


/** Mod�lisation de diffusion de chaleur dans un espace bidimensionnel ihonomog�ne
	  * @param init : Condition_Probleme avec donn�es du probl�me � r�soudre
	  * @param RacineAdresse : o� placer les fichiers r�ponses
	  */

void CalculsInhomogene2D(Condition_Probleme init,char* RacineAdresse) {
    // -------------- //
    // INITIALISATION //
    int n, j, m, i; //indice spatial et temporel
    double KG, KC, KD, KH, KB; //temperature aux points Gauche Centre Droite Haut Bas
    char adresse[50];//vecteur d'adresse de lecture et d'�criture de
    double Y,X,R;//Coefficient de l'�quation differentielle discr�tis�e
    double **calcule = Allocation_Tableau_double_2D(init.Domaine_Init.Nx,init.Domaine_Init.Ny);
    double *tmpData=calloc(init.Domaine_Init.Ny, sizeof( double ));//permet de sauvegarder nos temp�ratures pr�c�dentes
    double** chaleur=Allocation_Tableau_double_2D(init.Domaine_Init.Nx,init.Domaine_Init.Ny);

    ConcatenationFichier2D(adresse,RacineAdresse,0); //Initialisation de l'adresse des fichiers
    Initilisation_Calculs2D(adresse,init,calcule); //On remplit les fichiers texte des dimensions du probl�me


    // -------------- //
    //     CALCULE    //
    for (j=1; j<=init.Domaine_Init.Nt; j++){

        ConcatenationFichier2D(adresse,VARIABLE2D,j); //R�initialisation de l'adresse pour chaleur

        Lire_ChaleurVariable(adresse,chaleur); //On r�cup�re les nouvelles valeurs des sources de chaleur au temps j

        ConcatenationFichier2D(adresse,RacineAdresse,j); //R�initialisation de l'adresse pour placer les fichiers r�ponses de diffusion

        FILE*FTXT = fopen(adresse, "w"); //fichier r�ponse
        if(FTXT != NULL){//test d'ouverture

            fprintf( FTXT, "%d %d ",init.Domaine_Init.Nx,init.Domaine_Init.Ny);//Dimension de la matrice de solution [X,Y]
            fprintf(FTXT,"%lf\n",init.Domaine_Init.dt * j); //On �crit le moment tj

            for(i=0;i<init.Domaine_Init.Nx;i++){ //pour chaque �chantillon sur x
                fprintf(FTXT,"%lf\t",calcule[0][i]); //on �crit la temp�rature Ta au bord
                tmpData[i]=calcule[0][i]; //On la sauvegarde dans tmpData
            }
            fprintf(FTXT,"\n");

            for(m=1;m<init.Domaine_Init.Ny-1;m++){ //pour chaque �chantillon sur y

                fprintf(FTXT,"%lf\t",calcule[m][0]);

                //R�initialisation des valeurs de temp�ratures aux points importants
                										KH = tmpData[1];
                KG = calcule[m][0]; KC = calcule[m][1]  ;KD = calcule[m][2];
               											KB = calcule[m+1][1];

                for (n=1; n<(init.Domaine_Init.Nx-1); n++){ //pour chaque �chantillon sur x

                    Y=(KH+KB-2*KC)/(init.Domaine_Init.dy * init.Domaine_Init.dy);
                    X=(KG+KD-2*KC)/(init.Domaine_Init.dx * init.Domaine_Init.dx);

                    R= init.Domaine_Init.dt * init.Domaine_Init.AlphaLocal[m][n];
                    calcule[m][n] = KC + (init.Domaine_Init.SourcesDeChaleur[m][n] * R *(Y+X))+ chaleur[m][n];

                  	//R�initialisation des points de temp�ratures importantes
                    KG = KC;
                    KC = KD;
                    KD = calcule[m][n+2];
                    KB = calcule[m+1][n+1];
                    KH = tmpData[n+1];

                    tmpData[n]=calcule[m][n]; //sauvergarde des temp�ratures

                    fprintf( FTXT,"%lf\t", calcule[m][n]); //enregistrement des donn�es

                } //fin for n

                fprintf(FTXT,"%lf\n",calcule[m][init.Domaine_Init.Nx-1]); //enregistrement des donn�es

            } //fin for m

            for(i=0;i<init.Domaine_Init.Nx;i++)
                fprintf(FTXT,"%lf\t",calcule[init.Domaine_Init.Ny-1][i]); //On �crit Tb, temp�rature � l'autre bord

            fclose(FTXT); //fermeture de fichier

        }else
        printf("Erreur a l'ouverture du fichier de Resultat : %s \n",adresse);

    }//fin for j

    Liberer_Tableau((void**)calcule,init.Domaine_Init.Ny);
    Liberer_Tableau((void**)chaleur,init.Domaine_Init.Ny);
    calcule=NULL; //lib�ration de la m�moire
}


/** Mod�lisation de diffusion de chaleur dans un espace monodimensionnel ihonomog�ne
	  * @param init : Condition_Probleme avec donn�es du probl�me � r�soudre
	  * @param adresse : o� placer les fichiers r�ponses
	  */

void CalculsInhomogene1D(Condition_Probleme init ,char* adresse){
    // -------------- //
    // INITIALISATION //
    double** chaleur =Allocation_Tableau_double_2D(init.Domaine_Init.Nx,init.Domaine_Init.Nt); //cr�ation d'une matrice contenant les valeurs des sources de chaleur

    Lire_ChaleurVariable(VARIABLE1D,chaleur);//on remplit cette matrice

    FILE*ftxt = fopen(adresse, "w"); //fichier r�ponse
    if(ftxt != NULL){//test d'ouverture

        int n, j; //indice spatial et temporel
        double K0, K1, K2; //temperature aux points a-1, a, a+1

        double s = init.Domaine_Init.dt  / (init.Domaine_Init.dx * init.Domaine_Init.dx); //Coefficient de l'�quation differentielle discr�tis�e

        double *calcule = calloc(init.Domaine_Init.Nx, sizeof( double )); //Vecteur 1D contenant les valeurs de la simulation
        //taille variable selon le nombre d'�chantillon
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
                fprintf( ftxt,"%lf\t", calcule[n]); //enregistrement des donn�es

            }//fin for j
        fprintf(ftxt,"%lf \n",calcule[init.Domaine_Init.Nx-1]); //derniere colone Tb
        } //fin for n

        fclose(ftxt); //fermeture de fichier

        Liberer_Tableau((void**)chaleur,init.Domaine_Init.Nt-1);
        free(calcule);
        calcule=NULL; //lib�ration de la m�moire
    }
    else
        printf("Erreur a l'ouverture du fichier de Resultat \n");
}


/**Ecriture des dimensions et des temp�ratures initiales du probl�me monodimensionnel dans un fichier txt
	  * @param ftxt : fichier texte � initialiser
	  * @param init : Condition_Probleme
	  * @param calcule : vecteur 1D contenant les valeurs de la simulation
	  */

void InitialisationTXT1D(FILE*ftxt,Condition_Probleme init,double *calcule){
        fprintf( ftxt, "%d %d\n",init.Domaine_Init.Nx,init.Domaine_Init.Nt);//Dimension de la matrice de solution [X,Y]
        int n;

        for (n=0; n<=init.Domaine_Init.Nx; n++)
            fprintf( ftxt, "%lf\t", n*init.Domaine_Init.dx); //�criture des coordonn�es en X en premiere ligne
        fprintf(ftxt,"\n");


        //ecriture des conditions initales en premiere ligne
        fprintf(ftxt,"%lf\t",0.0);//t0

        for (n=0; n<init.Domaine_Init.Nx; n++){
            calcule[n]=init.Temp_Init[0][n];//On r�cup�re les temp�ratures initiales
            fprintf( ftxt,"%lf\t", calcule[n]);//On les �crit dans le fichier ftxt
        }

        fprintf(ftxt,"\n");
}


/**Initilisation de l'adresse d'un fichier
	  * @param adresse : adresse du fichier � concat�ner
	  * @param racine : d�but adresse o� placer les fichiers � concat�ner
	  * @param  i : num�ro du fichier
	  */

void ConcatenationFichier2D(char *adresse,const char* racine,int i){
    char nombre[50];
    strcpy(adresse, racine);//copie racine � la suite d'adresse
    itoa(i,nombre,DECIMALE);
    strcat(adresse,"_");
    strcat(adresse,nombre);
    strcat(adresse,TXT); //On obtient : adresse = data/Resultat/Inhomogene/Inhomogene_i.txt
}


/**Ecriture des dimensions et temp�ratures initiales du probl�me bidimensionnel dans un fichier txt
	  * @param adresse : fichier texte � initialiser
	  * @param init : Condition_Probleme
	  * @param calcule : matrice contenant les valeurs de la simulation
	  */

void Initilisation_Calculs2D(char* adresse,Condition_Probleme init,double **calcule){
    int n,m;

    FILE*ftxt = fopen(adresse, "w"); //fichier r�ponse
    if(ftxt != NULL){//test d'ouverture
        fprintf( ftxt, "%d\t%d\t%d\n",init.Domaine_Init.Nx,init.Domaine_Init.Ny,init.Domaine_Init.Nt);//Dimension de la matrice de solution [X,Y]

        for(n=0; n<init.Domaine_Init.Nx; n++){//pour chaque �chantillon sur x
            fprintf( ftxt,"%lf\t", init.Domaine_Init.dx*n); //On �crit chaque valeur que prend x
        }
        fprintf( ftxt,"\n");

        for(m=0; m<init.Domaine_Init.Ny; m++){ //pour chaque �chantillon sur y
            fprintf( ftxt,"%lf\t", init.Domaine_Init.dy*m);//On �crit chaque valeur que prend y
        }
        fprintf( ftxt,"\n");

        for(m=0; m<init.Domaine_Init.Ny; m++){
            for (n=0; n<init.Domaine_Init.Nx; n++){
                calcule[m][n]=init.Temp_Init[m][n]; //On r�cup�re les temp�ratures initiales
                fprintf( ftxt,"%lf\t", calcule[m][n]); //On les �crit dans le fichier ftxt
            }
        fprintf(ftxt,"\n");
        }
    fclose(ftxt);//fermeture du fichier
    }
    else
        printf("Erreur a l'ouverture du fichier de Resultat : %s \n",adresse);

}

