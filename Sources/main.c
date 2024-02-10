#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Computation.h"
#include "listeSC.h"
#include "Initialisation.h"

#define MATERIAL "data/Materiaux.txt"
#define INITALCONDITION "data/ConditionsInitiales.txt"
#define HEATSOURCES "data/SourcesDeChaleur.txt"
#define MATERIALSTYPES "data/TypesDeMateriaux.txt"


int main(){

    InhomogeneousEnvironments( MATERIAL , INITALCONDITION, HEATSOURCES,MATERIALSTYPES);

    return 0;
}
