/*********************************************
* OPL 12.6.0.0 Model
* Wines
*********************************************/

//Make sure you use c[i] to access the i-th cost 
//and do not remove/change the following line
							// Problem parameters
float	c		[1..2]	= [7, 5];		//		Price of wine (per liter)	
float	blendNeedWA	[1..3]	= [0.3, 0.2, 0.5];	//		Need of blend for wine A
float	blendNeedWB	[1..3]	= [0.5, 0.1, 0.4];	//		Need of blend for wine B
int	materialStock	[1..3]	= [1800, 400, 1500];	//		Quantity of blends in stock
							// Problem variables
dvar int+	VWA;					//		Volume of produced wine A in liters
dvar int+	VWB;					//		Volume of produced wine B in liters

maximize c[1]*VWA + c[2]*VWB;

subject to
{
	VWA * blendNeedWA[1] + VWB * blendNeedWB[1] <= materialStock[1];
	VWA * blendNeedWA[2] + VWB * blendNeedWB[2] <= materialStock[2];
	VWA * blendNeedWA[3] + VWB * blendNeedWB[3] <= materialStock[3];
}

// Run and print the solution
execute
{
	writeln("Post-traitement: ");
	writeln("The optimal quantity of wine A is: " + VWA);
	writeln("The optimal quantity of wine B is: " + VWB);
	writeln("La valeur de l'objectif est de "+cplex.getObjValue());
}
