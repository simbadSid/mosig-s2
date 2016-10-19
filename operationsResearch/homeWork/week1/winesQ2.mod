/*********************************************
* OPL 12.6.0.0 Model
* Wines
*********************************************/

//Make sure you use c[i] to access the i-th cost 
//and do not remove/change the following line
								// Problem parameters
float	c			[1..3]	= [1800, 400, 1500];	//		Quantity of each stock (per liter)
int	wineSellPrice		[1..2]	= [7, 5];		//		Price of wine sell per liter
int	optimalWineVolume	[1..2]	= [332, 3335];		//		Price of wine to optimize sell profits
int	maxProfits			= wineSellPrice[1] * optimalWineVolume[2] + wineSellPrice[2] * optimalWineVolume[2];
								// Problem variables
dvar float+	priceBlend1;					//		Price of the blend 1 in euros per liters
dvar float+	priceBlend2;					//		Price of the blend 2 in euros per liters
dvar float+	priceBlend3;					//		Price of the blend 3 in euros per liters

minimize c[1]*priceBlend1 + c[2]*priceBlend2 + c[3]*priceBlend3;

subject to
{
	priceBlend1 * c[1] + priceBlend2 * c[2] + priceBlend3 * c[3] >= maxProfits;
}

// Run and print the solution
execute
{
	writeln("Post-traitement: ");
	writeln("The optimal price of blend 1 is: " + priceBlend1);
	writeln("The optimal price of blend 2 is: " + priceBlend2);
	writeln("The optimal price of blend 3 is: " + priceBlend3);
	writeln("La valeur de l'objectif est de "+cplex.getObjValue());
}


