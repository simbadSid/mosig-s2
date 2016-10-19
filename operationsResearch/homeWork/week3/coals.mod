/*********************************************
 * OPL 12.6.0.0 Model
 * Wipers program
 *********************************************/

// Parameters
int nbrCoalType = 8;
int nbrCoalInMix= 4;

float c[1..nbrCoalType] = [12, 14, 17, 10, 13, 9, 15, 11];
float s[1..nbrCoalType] = [0.02, 0.025, 0.01, 0.05, 0.01, 0.05, 0.02, 0.015];

float thresholdCoalMin = 0.05;
float thresholdCoalMax = 1.0;
float thresholdSili = 0.018;





// Decision variable
dvar float+ coalProportion[1..nbrCoalType];
dvar boolean coalIsInMix[1..nbrCoalType];




// Objective
minimize sum(i in 1..nbrCoalType) (coalProportion[i] * c[i]);





// Constraint
subject to
{
    // Constraint on the minimum proportion of coal
    forall (i in 1..nbrCoalType)
    {
        coalProportion[i] >= coalIsInMix[i] * thresholdCoalMin;
coalProportion[i] <= coalIsInMix[i];
    }

/// A relation is missing between coalProportion and coalIsInMix
/// Thanks for the hint ;)

    // Constraint on the maximum proportion of coal
    forall (i in 1..nbrCoalType)
        coalProportion[i] <= thresholdCoalMax;

    // Constraint on the sum of the proportions
    sum(i in 1..nbrCoalType)(coalProportion[i]) == thresholdCoalMax;

    // Constraint on the silicon proportions
    sum(i in 1..nbrCoalType)(coalProportion[i] * s[i]) <= thresholdSili;

    //Constraint on the total number of coal type in the mix
    sum(i in 1..nbrCoalType)(coalIsInMix[i]) == nbrCoalInMix;

    //Constraint on c1 and c3
    coalIsInMix[3] >= coalIsInMix[1];

    //Constraint on c4 and c6
    coalIsInMix[6] >= coalIsInMix[4];
}




/* Affichage de la solution */
execute {
  writeln("Post-traitement: ");
  writeln("La valeur de l'objectif est de "+cplex.getObjValue());
} 
