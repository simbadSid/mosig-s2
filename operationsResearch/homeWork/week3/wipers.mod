/*********************************************
 * OPL 12.6.0.0 Model
 * Wipers program (QUESTION 1)
 *********************************************/

//Make sure you use c[i] to access the i-th cost 
//and do not remove/change the following line
float c[1..6] = [400, 600, 200, 250, 150, 230];
// Not used. Replace by costPerHour and fixedCost




// Parameters
int nbrMachine = 4;
int nbrProduces= 2;

float costPerHour[1..nbrMachine] = [c[1], c[2], c[3], c[4]];//[400, 600, 200, 250];
float fixedCost[1..nbrMachine] = [0, 0, c[5], c[6]];//[0, 0, 150, 230];

float totalTimeOfWork = 8;
float totalProduction[1..nbrProduces] = [1800, 1600];

int productionPerMachinePerProduce[1..nbrMachine][1..nbrProduces] = [[300, 200], [300, 100], [400, 0], [0, 300]];





// Decision variables
dvar float+ timeOfWork[1..nbrMachine];
dvar boolean hasWorked[1..nbrMachine];





// Objective
minimize sum(m in 1..nbrMachine) (timeOfWork[m] * costPerHour[m] + hasWorked[m] * fixedCost[m]);





// Constraint
subject to
{
  // Constraint on the working time
  forall(m in 1..nbrMachine)
  {
    timeOfWork[m] <= totalTimeOfWork;
    timeOfWork[m] >= 0;
    hasWorked[m] <= timeOfWork[m];
  }

  // Constraint on the production
  forall(p in 1..nbrProduces)
    sum (m in 1..nbrMachine) (timeOfWork[m] * productionPerMachinePerProduce[m][p]) >= totalProduction[p];

  // Constraint on the fixed cost
  forall(m in 1..nbrMachine)
  {
    timeOfWork[m] <= totalTimeOfWork * hasWorked[m];
//        hasWorked[m] == (timeOfWork[m] > 0);
  }
}

/* Affichage de la solution */
execute {
  writeln("Post-traitement: ");
  writeln("La valeur de l'objectif est de "+cplex.getObjValue());
} 
