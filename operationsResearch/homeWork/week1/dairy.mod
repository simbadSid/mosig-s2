/*********************************************
* OPL 12.6.0.0 Model
* Dairy products
*********************************************/

//Make sure you use c[i] to access the i-th cost 
//and do not remove/change the following line
								// Problem parameters
float	c			[1..4]	= [20, 25, 15, 0.25];	//		selling price (per liter or per kg)
float	resourceMilk		[1..3]	= [3000, 1000, 4000];	//		Total amount of milk
float	resourceLabor			= 255*60;		//		Total amount of available labor minutes
int	milkNeededAbondance	[1..3]	= [5, 3, 3];		//		Milk (per liter) to produce 1 kg of cheese
int	milkNeededBeaufort	[1..3]	= [5, 0, 5];
int	milkNeededReblochon	[1..3]	= [2, 0, 4];
int	laborTimeNeeded		[1..3]	= [15, 30, 10];		//		Minutes needed to produce 1 kg of cheese
								// Problem variables
dvar int+	productedAbondance;				//		Weight of producted abondance(kg)
dvar int+	productedBeaufort;				//		Weight of producted beaufort(kg)
dvar int+	productedReblochon;				//		Weight of producted reblochon(kg)
dvar int+	productedRowMilk1;				//		Volume of unused milk (l)
dvar int+	productedRowMilk2;				//		Volume of unused milk (l)
dvar int+	productedRowMilk3;				//		Volume of unused milk (l)

maximize c[1]*productedAbondance + c[2]*productedBeaufort + c[3]*productedReblochon +
	 c[4]*(productedRowMilk1 + productedRowMilk2 + productedRowMilk3);

subject to
{
productedAbondance	>= 0;
productedBeaufort	>= 0;
productedReblochon	>= 0;
productedRowMilk1	>= 0;
productedRowMilk2	>= 0;
productedRowMilk3	>= 0;
	productedAbondance * milkNeededAbondance[1] +
	productedBeaufort  * milkNeededBeaufort[1]+
	productedReblochon * milkNeededReblochon[1]	<= resourceMilk[1] - productedRowMilk1;

	productedAbondance * milkNeededAbondance[2] +
	productedBeaufort  * milkNeededBeaufort[2] +
	productedReblochon * milkNeededReblochon[2]	<= resourceMilk[2] - productedRowMilk2;

	productedAbondance * milkNeededAbondance[3] +
	productedBeaufort  * milkNeededBeaufort[3] +
	productedReblochon * milkNeededReblochon[3]	<= resourceMilk[3] - productedRowMilk3;

	productedAbondance * laborTimeNeeded[1] +
	productedBeaufort  * laborTimeNeeded[2] +
	productedReblochon * laborTimeNeeded[3]		<= resourceLabor;
}

// Run and print the solution
execute
{
	writeln("Post-traitement: ");
	writeln("The optimal weight of Abondance is : "	+ productedAbondance);
	writeln("The optimal weight of Beaufort is  : "	+ productedBeaufort );
	writeln("The optimal weight of Reblochon is : "	+ productedReblochon);
	writeln("The optimal volume of row milk 1 is: "	+ productedRowMilk1);
	writeln("The optimal volume of row milk 2 is: "	+ productedRowMilk2);
	writeln("The optimal volume of row milk 3 is: "	+ productedRowMilk3);
	writeln("La valeur de l'objectif est de "+cplex.getObjValue());
}


