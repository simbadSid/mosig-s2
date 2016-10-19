/*********************************************
* OPL 12.6.0.0 Model
* Perfumes
*********************************************/

//Make sure you use c[i] to access the i-th cost 
//and do not remove/change the following line

// Problem parameters
float c  [1..2] = [300, 500];
float materialNeedP1 [1..3] = [1, 0, 3];
float materialNeedP2 [1..3] = [0, 2, 2];
float materialStock [1..3] = [4, 12, 18];

// Problem variables
dvar float VP1;
dvar float VP2;

maximize c[1]*VP1 + c[2]*VP2;

subject to
{
 VP1 * materialNeedP1[1] + VP2 * materialNeedP2[1] <= materialStock[1];
 VP1 * materialNeedP1[2] + VP2 * materialNeedP2[2] <= materialStock[2];
 VP1 * materialNeedP1[3] + VP2 * materialNeedP2[3] <= materialStock[3];
}

// Run and print the solution
execute
{
 writeln("Post-traitement: ");
 writeln("The optimal quantity of perfume 1 is" + VP1);
 writeln("The optimal quantity of perfume 2 is" + VP2);
 writeln("La valeur de l'objectif est de "+cplex.getObjValue());
}
