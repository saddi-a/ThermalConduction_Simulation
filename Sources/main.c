#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Calculs.h"
#include "listeSC.h"
#include "Initialisation.h"

#define MAT "data/Materiaux.txt"
#define CONDINIT "data/ConditionsInitiales.txt"
#define SOURCEDECHAL "data/SourcesDeChaleur.txt"
#define TYPEMAT "data/TypesDeMateriaux.txt"


int main(){

    MillieuxInhomogene( MAT , CONDINIT, SOURCEDECHAL,TYPEMAT);

    return 0;
}
