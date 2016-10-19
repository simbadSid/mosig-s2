/*********************************************
 * OPL 12.6.0.0 Model
 * Dual program
 *********************************************/

//Formulate the dual of
//min z = 5x_1 + 6x_2 + 10x_3


//        2x_1 + 2x_2 +   x_3 >= 30
//        2x_1 +  x_2 -  2x_3 <= 10
//         x_1,   x_2,    x_3 >= 0
//WARNING: recall that you CAN NOT change the array c
 
//Make sure you use c[i] to access the i-th cost 
//and do not remove/change the following line
float c[1..2] = [30, 10];

dvar int+ w1;
dvar int w2;


//You are not allowed to change the objective to "minimize"
maximize (w1 * c[1] + w2 * c[2]);

subject to
{
	2 * w1 + 2 * w2 <= 5;
	2 * w1 +     w2 <= 6;
	    w1 + 2 * w2 <= 10;
}

/* Affichage de la solution */
execute {
  writeln("Post-traitement: ");
  writeln("La valeur de l'objectif est de "+cplex.getObjValue());
} 
