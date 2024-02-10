#include "Computation.h"

    #include "Initialisation.h"
    #include "listeSC.h"

    #define TXT ".txt"
    #define DECIMALE 10
    #define INHOMOGENEOUS2D "data/Results/Inhomogeneous"
    #define INHOMOGENEOUS1D "data/Results/Inhomogeneous.txt"
    #define VARIABLEHEAT2D "data/VariableHeat/VariableHeat"
    #define VARIABLEHEAT1D "data/VariableHeat.txt"


/**Choisis le type du probleme (bi ou mono) en fonction de ses dimensuions
	  * @param materialAdress : adress qui contient les caract�ristiques des mat�riaux
	  * @param issueConditionAdress : adress d'un fichier qui contient les donn�es de la structure Issue_Condition
	  * @param heatSourcesAdress : adress d'un fichier qui contient l'emplacement et les variations des sources de heat
	  * @param materialsTypesAdress : adress qd'un fichier ui contient les nombres qui correspondent � diff�rents types de mat�riaux
	  */

void InhomogeneousEnvironments(char* materialAdress, char* issueConditionAdress, char* heatSourcesAdress,char *materialsTypesAdress){

    Materials_List materiaux = Read_Materiaux(materialAdress);//On r�cup�re les caract�ristiques de tous les mat�riaux dans une liste chain�e
    Issue_Condition init = Read_Issue_Condition(issueConditionAdress ,heatSourcesAdress, materialsTypesAdress,materiaux);  //On r�cup�re toutes les donn�es de conditions des probl�mes

    if(init.Domaine_Init.Nx>2){
        switch(init.Domaine_Init.Ny){

            case 1: //pour Ny = 1
                ComputationInhomogeneous1D(init ,INHOMOGENEOUS1D);
            break;

            case 2: //pour Ny = 2
                printf("Erreur, il ne peut y avoir 2 echantillons en y (1 ou plus de 2) \n");
            break;

            default : //pour Ny>2
                ComputationInhomogeneous2D(init ,INHOMOGENEOUS2D);
            }
    }
    else
        printf("erreur il doit y avoir plus de 2 echantillons en x \n");

    Free_Issue_Condition(&init);
    Free_Materials_List(&materiaux);//lib�ration de la m�moire
}


/** Mod�lisation de diffusion de heat dans un espace bidimensionnel ihonomog�ne
	  * @param init : Issue_Condition avec donn�es du probl�me � r�soudre
	  * @param directoryAdress : o� placer les fichiers r�ponses
	  */

void ComputationInhomogeneous2D(Issue_Condition init,char* directoryAdress) {
    // -------------- //
    // INITIALISATION //
    int n, j, m, i; //indice spatial et temporel
    double KG, KC, KD, KH, KB; //temperature aux points Gauche Centre Droite Haut Bas
    char adress[50];//vecteur d'adress de lecture et d'�criture de
    double Y,X,R;//Coefficient de l'�quation differentielle discr�tis�e
    double **computation = Allocate_Table_double_2D(init.Domaine_Init.Nx,init.Domaine_Init.Ny);
    double *tmpData=calloc(init.Domaine_Init.Ny, sizeof( double ));//permet de sauvegarder nos temp�ratures pr�c�dentes
    double** heat=Allocate_Table_double_2D(init.Domaine_Init.Nx,init.Domaine_Init.Ny);

    ConcatenationFichier2D(adress,directoryAdress,0); //Initialisation de l'adress des fichiers
    Initilisation_Computation2D(adress,init,computation); //On remplit les fichiers texte des dimensions du probl�me


    // ------------------ //
    //     COMPUTATION    //
    for (j=1; j<=init.Domaine_Init.Nt; j++){

        ConcatenationFichier2D(adress,VARIABLEHEAT2D,j); //R�initialisation de l'adress pour heat

        Read_VariableHeat(adress,heat); //On r�cup�re les nouvelles valeurs des sources de heat au temps j

        ConcatenationFichier2D(adress,directoryAdress,j); //R�initialisation de l'adress pour placer les fichiers r�ponses de diffusion

        FILE*FTXT = fopen(adress, "w"); //fichier r�ponse
        if(FTXT != NULL){//test d'ouverture

            fprintf( FTXT, "%d %d ",init.Domaine_Init.Nx,init.Domaine_Init.Ny);//Dimension de la matrice de solution [X,Y]
            fprintf(FTXT,"%lf\n",init.Domaine_Init.dt * j); //On �crit le moment tj

            for(i=0;i<init.Domaine_Init.Nx;i++){ //pour chaque �chantillon sur x
                fprintf(FTXT,"%lf\t",computation[0][i]); //on �crit la temp�rature Ta au bord
                tmpData[i]=computation[0][i]; //On la sauvegarde dans tmpData
            }
            fprintf(FTXT,"\n");

            for(m=1;m<init.Domaine_Init.Ny-1;m++){ //pour chaque �chantillon sur y

                fprintf(FTXT,"%lf\t",computation[m][0]);

                //R�initialisation des valeurs de temp�ratures aux points importants
                										    KH = tmpData[1];
                KG = computation[m][0]; KC = computation[m][1]  ;KD = computation[m][2];
               											    KB = computation[m+1][1];

                for (n=1; n<(init.Domaine_Init.Nx-1); n++){ //pour chaque �chantillon sur x

                    Y=(KH+KB-2*KC)/(init.Domaine_Init.dy * init.Domaine_Init.dy);
                    X=(KG+KD-2*KC)/(init.Domaine_Init.dx * init.Domaine_Init.dx);

                    R= init.Domaine_Init.dt * init.Domaine_Init.LocalAlpha[m][n];
                    computation[m][n] = KC + (init.Domaine_Init.HeatSources[m][n] * R *(Y+X))+ heat[m][n];

                  	//R�initialisation des points de temp�ratures importantes
                    KG = KC;
                    KC = KD;
                    KD = computation[m][n+2];
                    KB = computation[m+1][n+1];
                    KH = tmpData[n+1];

                    tmpData[n]=computation[m][n]; //sauvergarde des temp�ratures

                    fprintf( FTXT,"%lf\t", computation[m][n]); //enregistrement des donn�es

                } //fin for n

                fprintf(FTXT,"%lf\n",computation[m][init.Domaine_Init.Nx-1]); //enregistrement des donn�es

            } //fin for m

            for(i=0;i<init.Domaine_Init.Nx;i++)
                fprintf(FTXT,"%lf\t",computation[init.Domaine_Init.Ny-1][i]); //On �crit Tb, temp�rature � l'autre bord

            fclose(FTXT); //fermeture de fichier

        }else
        printf("Erreur a l'ouverture du fichier de Resultat : %s \n",adress);

    }//fin for j

    Free_Table((void**)computation,init.Domaine_Init.Ny);
    Free_Table((void**)heat,init.Domaine_Init.Ny);
    computation=NULL; //lib�ration de la m�moire
}


/** Mod�lisation de diffusion de heat dans un espace monodimensionnel ihonomog�ne
	  * @param init : Issue_Condition avec donn�es du probl�me � r�soudre
	  * @param adress : o� placer les fichiers r�ponses
	  */

void ComputationInhomogeneous1D(Issue_Condition init ,char* adress){
    // -------------- //
    // INITIALISATION //
    double** heat =Allocate_Table_double_2D(init.Domaine_Init.Nx,init.Domaine_Init.Nt); //cr�ation d'une matrice contenant les valeurs des sources de heat

    Read_VariableHeat(VARIABLEHEAT1D,heat);//on remplit cette matrice

    FILE*ftxt = fopen(adress, "w"); //fichier r�ponse
    if(ftxt != NULL){//test d'ouverture

        int n, j; //indice spatial et temporel
        double K0, K1, K2; //temperature aux points a-1, a, a+1

        double s = init.Domaine_Init.dt  / (init.Domaine_Init.dx * init.Domaine_Init.dx); //Coefficient de l'�quation differentielle discr�tis�e

        double *computation = calloc(init.Domaine_Init.Nx, sizeof( double )); //Vecteur 1D contenant les valeurs de la simulation
        //taille variable selon le nombre d'�chantillon
        InitialisationTXT1D(ftxt, init, computation);

        // ------------------ //
        //     COMPUTATION    //
        for (j=1; j<=init.Domaine_Init.Nt; j++){
            fprintf(ftxt,"%lf\t%lf\t",init.Domaine_Init.dt*j,computation[0]);//colonne 1 temps colonne 2 Ta
            K0 = computation[0]; K1 = computation[1]; K2 = computation[2];//temperature aux points a-1, a, a+1

            for (n=1; n<(init.Domaine_Init.Nx-1); n++){

                computation[n] = K1 + (init.Domaine_Init.HeatSources[0][n] *init.Domaine_Init.LocalAlpha[0][n] * s * (K0+K2-2*K1) )+ heat[j-1][n];

                K0 = K1;//temperature aux points a-1, a, a+1
                K1 = K2;
                K2 = computation[n+2];
                fprintf( ftxt,"%lf\t", computation[n]); //enregistrement des donn�es

            }//fin for j
        fprintf(ftxt,"%lf \n",computation[init.Domaine_Init.Nx-1]); //derniere colone Tb
        } //fin for n

        fclose(ftxt); //fermeture de fichier

        Free_Table((void**)heat,init.Domaine_Init.Nt-1);
        free(computation);
        computation=NULL; //lib�ration de la m�moire
    }
    else
        printf("Erreur a l'ouverture du fichier de Resultat \n");
}


/**Ecriture des dimensions et des temp�ratures initiales du probl�me monodimensionnel dans un fichier txt
	  * @param ftxt : fichier texte � initialiser
	  * @param init : Issue_Condition
	  * @param computation : vecteur 1D contenant les valeurs de la simulation
	  */

void InitialisationTXT1D(FILE*ftxt,Issue_Condition init,double *computation){
        fprintf( ftxt, "%d %d\n",init.Domaine_Init.Nx,init.Domaine_Init.Nt);//Dimension de la matrice de solution [X,Y]
        int n;

        for (n=0; n<=init.Domaine_Init.Nx; n++)
            fprintf( ftxt, "%lf\t", n*init.Domaine_Init.dx); //�criture des coordonn�es en X en premiere ligne
        fprintf(ftxt,"\n");


        //ecriture des conditions initales en premiere ligne
        fprintf(ftxt,"%lf\t",0.0);//t0

        for (n=0; n<init.Domaine_Init.Nx; n++){
            computation[n]=init.Temp_Init[0][n];//On r�cup�re les temp�ratures initiales
            fprintf( ftxt,"%lf\t", computation[n]);//On les �crit dans le fichier ftxt
        }

        fprintf(ftxt,"\n");
}


/**Initilisation de l'adress d'un fichier
	  * @param adress : adress du fichier � concat�ner
	  * @param racine : d�but adress o� placer les fichiers � concat�ner
	  * @param  i : num�ro du fichier
	  */

void ConcatenationFichier2D(char *adress,const char* racine,int i){
    char nombre[50];
    strcpy(adress, racine);//copie racine � la suite d'adress
    itoa(i,nombre,DECIMALE);
    strcat(adress,"_");
    strcat(adress,nombre);
    strcat(adress,TXT); //On obtient : adress = data/Resultat/Inhomogeneous_i.txt
}


/**Ecriture des dimensions et temp�ratures initiales du probl�me bidimensionnel dans un fichier txt
	  * @param adress : fichier texte � initialiser
	  * @param init : Issue_Condition
	  * @param computation : matrice contenant les valeurs de la simulation
	  */

void Initilisation_Computation2D(char* adress,Issue_Condition init,double **computation){
    int n,m;

    FILE*ftxt = fopen(adress, "w"); //fichier r�ponse
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
                computation[m][n]=init.Temp_Init[m][n]; //On r�cup�re les temp�ratures initiales
                fprintf( ftxt,"%lf\t", computation[m][n]); //On les �crit dans le fichier ftxt
            }
        fprintf(ftxt,"\n");
        }
    fclose(ftxt);//fermeture du fichier
    }
    else
        printf("Erreur a l'ouverture du fichier de Resultat : %s \n",adress);

}

