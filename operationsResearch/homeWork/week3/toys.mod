/*********************************************
 * OPL 12.6.0.0 Model
 * Toy Shop program
 *********************************************/


// Parameters
int nbrToyType = 4;
float c[1..nbrToyType] = [53, 56, 62, 82];

float timeWorkshop1PerToy[1..nbrToyType] = [2.4, 3.1, 0.8, 3.1];
float timeWorkshop2PerToy[1..nbrToyType] = [1.6, 1.5, 2.9, 4];

int totalTimeWorkshop1 = 1700;
int totalTimeWorkshop2 = 2600;

int nbrTotalToyToProduce = 1000;

// Decision variables
dvar int+ x[1 .. nbrToyType];

// Objective
maximize sum(i in 1 .. nbrToyType) x[i] * c[i];

// Constraint
subject to
{
	sum(i in 1 .. nbrToyType) x[i] * timeWorkshop1PerToy[i] <= totalTimeWorkshop1;
	sum(i in 1 .. nbrToyType) x[i] * timeWorkshop2PerToy[i] <= totalTimeWorkshop2;
	sum(i in 1 .. nbrToyType) x[i] == nbrTotalToyToProduce;
}

/* Affichage de la solution */
execute
{
  writeln("Post-traitement: ");
  writeln("La valeur de l'objectif est de "+cplex.getObjValue());
} 
