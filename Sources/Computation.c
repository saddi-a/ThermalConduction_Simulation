#include "Computation.h"

    #include "Initialization.h"
    #include "listeSC.h"

    #define TXT ".txt"
    #define BIN ".bin"
    #define DECIMALE 10
    #define INHOMOGENEOUS2D "data/Results/Inhomogeneous"
    #define INHOMOGENEOUS1D "data/Results/Inhomogeneous"
    #define VARIABLEHEAT2D "data/VariableHeat/VariableHeat"
    #define VARIABLEHEAT1D "data/VariableHeat.txt"
    #define BINARYFILES 0


/**Choisis le type du probleme (bi ou mono) en fonction de ses dimensuions
	  * @param materialAdress : adress qui contient les caract�ristiques des mat�riaux
	  * @param problemConditionAdress : adress d'un file qui contient les donn�es de la structure Problem_Condition
	  * @param heatSourcesAdress : adress d'un file qui contient l'emplacement et les variations des sources de heat
	  * @param materialsTypesAdress : adress qd'un file ui contient les nombres qui correspondent � diff�rents types de mat�riaux
	  */

void InhomogeneousEnvironments(char* materialAdress, char* problemConditionAdress, char* heatSourcesAdress,char *materialsTypesAdress){

    Materials_List materiaux = Read_Materiaux(materialAdress);//On r�cup�re les caract�ristiques de tous les mat�riaux dans une liste chain�e
    Problem_Condition init = Read_Problem_Condition(problemConditionAdress ,heatSourcesAdress, materialsTypesAdress,materiaux);  //On r�cup�re toutes les donn�es de conditions des probl�mes

    if(init.Domaine_Init.Nx>2){
        switch(init.Domaine_Init.Ny){

            case 1: //pour Ny = 1
                char adress[100] = INHOMOGENEOUS1D;
                ComputationInhomogeneous1D(init ,adress);
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

    Free_Problem_Condition(&init);
    Free_Materials_List(&materiaux);//lib�ration de la m�moire
}


/** Mod�lisation de diffusion de heat dans un espace bidimensionnel ihonomog�ne
	  * @param init : Problem_Condition avec donn�es du probl�me � r�soudre
	  * @param directoryAdress : o� placer les files r�ponses
	  */

void ComputationInhomogeneous2D(Problem_Condition init,char* directoryAdress) {
    // -------------- //
    // INITIALIZATION //
    int n, j, m, i; //indice spatial et temporel
    double KG, KC, KD, KH, KB; //temperature aux points Gauche Centre Droite Haut Bas
    char adress[50];//vecteur d'adress de lecture et d'�criture de
    double Y,X,R;//Coefficient de l'�quation differentielle discr�tis�e
    double **computation = Allocate_Table_double_2D(init.Domaine_Init.Nx,init.Domaine_Init.Ny);
    double *tmpData=calloc(init.Domaine_Init.Ny, sizeof( double ));//permet de sauvegarder nos temp�ratures pr�c�dentes
    double** heat=Allocate_Table_double_2D(init.Domaine_Init.Nx,init.Domaine_Init.Ny);

    ConcatenationFile2D(adress,directoryAdress,0); //Initialization de l'adress des files
    Initilisation_Computation2D(adress,init,computation); //On remplit les files texte des dimensions du probl�me


    // ------------------ //
    //     COMPUTATION    //
    for (j=1; j<=init.Domaine_Init.Nt; j++){

        ConcatenationFile2D(adress,VARIABLEHEAT2D,j); //R�initialization de l'adress pour heat

        Read_VariableHeat(adress,heat); //On r�cup�re les nouvelles valeurs des sources de heat au temps j

        ConcatenationFile2D(adress,directoryAdress,j); //R�initialization de l'adress pour placer les files r�ponses de diffusion

        FILE*FTXT = fopen(adress, "w"); //file r�ponse
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

                //R�initialization des valeurs de temp�ratures aux points importants
                										    KH = tmpData[1];
                KG = computation[m][0]; KC = computation[m][1]  ;KD = computation[m][2];
               											    KB = computation[m+1][1];

                for (n=1; n<(init.Domaine_Init.Nx-1); n++){ //pour chaque �chantillon sur x

                    Y=(KH+KB-2*KC)/(init.Domaine_Init.dy * init.Domaine_Init.dy);
                    X=(KG+KD-2*KC)/(init.Domaine_Init.dx * init.Domaine_Init.dx);

                    R= init.Domaine_Init.dt * init.Domaine_Init.LocalAlpha[m][n];
                    computation[m][n] = KC + (init.Domaine_Init.HeatSources[m][n] * R *(Y+X))+ heat[m][n];

                  	//R�initialization des points de temp�ratures importantes
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

            fclose(FTXT); //fermeture de file

        }else
        printf("Erreur a l'ouverture du file de Resultat : %s \n",adress);

    }//fin for j

    Free_Table((void**)computation,init.Domaine_Init.Ny);
    Free_Table((void**)heat,init.Domaine_Init.Ny);
    computation=NULL; //lib�ration de la m�moire
}


/** Modeling of heat diffusion in a one-dimensional inhomogeneous space.
	  * @param init : Problem_Condition containing problem data 
	  * @param adress : output file adress
	  */

void ComputationInhomogeneous1D(Problem_Condition init ,char* adress){
    // -------------- //
    // INITIALIZATION //
    double** heat =Allocate_Table_double_2D(init.Domaine_Init.Nx,init.Domaine_Init.Nt); //Creating a matrix containing heat source values

    Read_VariableHeat(VARIABLEHEAT1D,heat);//Filing the above matrix with data contained in the file

    FILE*file;

    if(BINARYFILES) {
      strcat(adress,BIN); file = fopen(adress, "wb"); //Results file 
    }
    else{
      strcat(adress,TXT); file = fopen(adress, "w"); //Results file 
    }


    if(file != NULL){//test d'ouverture

        int n, j; //indice spatial et temporel
        double K0, K1, K2; //temperature aux points a-1, a, a+1

        double s = init.Domaine_Init.dt  / (init.Domaine_Init.dx * init.Domaine_Init.dx); //Coefficient de l'�quation differentielle discr�tis�e

        double *computation = calloc(init.Domaine_Init.Nx, sizeof( double )); //Vecteur 1D contenant les valeurs de la simulation
        //taille variable selon le nombre d'�chantillon

        if(BINARYFILES) 
          InitializationBIN1D(file, init, computation);
        else
          InitializationTXT1D(file, init, computation);

        // ------------------ //
        //     COMPUTATION    //
        for (j=1; j<=init.Domaine_Init.Nt; j++){
          double time = init.Domaine_Init.dt*j;

          if(BINARYFILES) 
            fwrite(&time,sizeof(double),1,file); 
          else
            fprintf(file,"%lf\t%lf\t",time,computation[0]);//colonne 1 temps colonne 2 Ta

            K0 = computation[0]; K1 = computation[1]; K2 = computation[2];//temperature aux points a-1, a, a+1

            for (n=1; n<(init.Domaine_Init.Nx-1); n++){

                computation[n] = K1 + (init.Domaine_Init.HeatSources[0][n] *init.Domaine_Init.LocalAlpha[0][n] * s * (K0+K2-2*K1) )+ heat[j-1][n];

                K0 = K1;//temperature aux points a-1, a, a+1
                K1 = K2;
                K2 = computation[n+2];
                if(!BINARYFILES)
                fprintf( file,"%lf\t", computation[n]); //enregistrement des donn�es

            }//fin for j
            
            if(BINARYFILES)
              fwrite(&computation, sizeof(double), init.Domaine_Init.Nx, file);
            else
              fprintf(file,"%lf \n",computation[init.Domaine_Init.Nx-1]); //derniere colone Tb
        } //fin for n

        fclose(file); //fermeture de file

        Free_Table((void**)heat,init.Domaine_Init.Nt-1);
        free(computation);
        computation=NULL; //lib�ration de la m�moire
    }
    else
        printf("Erreur a l'ouverture du file de Resultat \n");
}


/**Ecriture des dimensions et des temp�ratures initiales du probl�me monodimensionnel dans un file txt
	  * @param ftxt : file texte � initialiser
	  * @param init : Problem_Condition
	  * @param computation : vecteur 1D contenant les valeurs de la simulation
	  */

void InitializationTXT1D(FILE*ftxt,Problem_Condition init,double *computation){
        fprintf( ftxt, "%d %d\n",init.Domaine_Init.Nx,init.Domaine_Init.Nt);//Dimension de la matrice de solution [X,Y]
        int n;

        for (n=0; n<=init.Domaine_Init.Nx; n++)
            fprintf( ftxt, "%lf\t", n*init.Domaine_Init.dx); //�criture des coordonn�es en X en premiere ligne
        fprintf(ftxt,"\n");


        //ecriture des conditions initales en premiere ligne
        fprintf(ftxt,"%lf\t",0.0);//t0

        for (n=0; n<init.Domaine_Init.Nx; n++){
            computation[n]=init.Temp_Init[0][n];//On r�cup�re les temp�ratures initiales
            fprintf( ftxt,"%lf\t", computation[n]);//On les �crit dans le file ftxt
        }

        fprintf(ftxt,"\n");
}

void InitializationBIN1D(FILE*fbin,Problem_Condition init,double *computation){
        printf("\n");

        //Dimension of the output [X,Y]
        fwrite(&init.Domaine_Init.Nx,sizeof(int),1,fbin);
        fwrite(&init.Domaine_Init.Nt,sizeof(int),1,fbin);
        printf("%x %x ",init.Domaine_Init.Nx,init.Domaine_Init.Nt);

        double *value = calloc(init.Domaine_Init.Nx, sizeof( double ));;
        for (int n=0; n<=init.Domaine_Init.Nx; n++){
          value[n] = init.Domaine_Init.dx * n;
        }
        
        computation = *init.Temp_Init;

        fwrite(&value ,sizeof(double),1,fbin); //X coordonate as first set of data
          
        printf("\n");

        for (int i = 0; i < init.Domaine_Init.Nx; i++)
        {
          unsigned long long int_value = *((unsigned long long*)&value[i]);
          printf("%llx %f ",int_value,value[i]);
        }
        printf("\n");

        for (int i = 0; i < init.Domaine_Init.Nx; i++)
        {
          printf("%08x",computation[i]);
        }
        

        free(value);
        //Writing inital conditions as second set of data
        fwrite(&computation, sizeof(double), init.Domaine_Init.Nx, fbin);
}

/**Initilisation de l'adress d'un file
	  * @param adress : adress du file � concat�ner
	  * @param racine : d�but adress o� placer les files � concat�ner
	  * @param  i : num�ro du file
	  */

void ConcatenationFile2D(char *adress,const char* racine,int i){
    char nombre[50];
    strcpy(adress, racine);//copie racine � la suite d'adress
    itoa(i,nombre,DECIMALE);
    strcat(adress,"_");
    strcat(adress,nombre);
    strcat(adress,TXT); //On obtient : adress = data/Resultat/Inhomogeneous_i.txt
}


/**Ecriture des dimensions et temp�ratures initiales du probl�me bidimensionnel dans un file txt
	  * @param adress : file texte � initialiser
	  * @param init : Problem_Condition
	  * @param computation : matrice contenant les valeurs de la simulation
	  */

void Initilisation_Computation2D(char* adress,Problem_Condition init,double **computation){
    int n,m;

    FILE*ftxt = fopen(adress, "w"); //file r�ponse
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
                fprintf( ftxt,"%lf\t", computation[m][n]); //On les �crit dans le file ftxt
            }
        fprintf(ftxt,"\n");
        }
    fclose(ftxt);//fermeture du file
    }
    else
        printf("Erreur a l'ouverture du file de Resultat : %s \n",adress);

}

