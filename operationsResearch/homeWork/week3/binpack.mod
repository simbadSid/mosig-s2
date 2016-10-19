/*********************************************
 * OPL 12.6.0.0 Model
 * Bin packing program
 *********************************************/


//Make sure you use c[i] to access the i-th size 
//and do not remove/change the four following lines:
int NbObj = 15;
int W = 20;
range N =1..NbObj;
float c[N] = [6,6,6,7,7,7,8,8,8,9,9,9,10,10,10];

// Parameters
int maxNbrBin = NbObj;



// Decision variables
dvar boolean objectIsInBin[1..maxNbrBin][1..NbObj];
dvar boolean isBinNonEmpty[1..maxNbrBin];




// Objective
minimize (sum (bin in 1..maxNbrBin)(isBinNonEmpty[bin]));




subject to
{
    // Constraint on the presence of all the objects in one only bin
    forall (object in 1..NbObj)
        sum (bin in 1..maxNbrBin) (objectIsInBin[bin][object]) == 1;

    // Constraint on the max payload of a bin
    forall (bin in 1..maxNbrBin)
        sum (object in 1..NbObj) (objectIsInBin[bin][object] * c[object]) <= W;

    forall (bin in 1..maxNbrBin)
    {
//        int xb = sum (object in 1..NbObj)(objectIsInBin[bin][object]);
        sum (object in 1..NbObj)(objectIsInBin[bin][object]) <= isBinNonEmpty[bin] * NbObj;
        sum (object in 1..NbObj)(objectIsInBin[bin][object]) >= isBinNonEmpty[bin];
    }
}



/* show solution */
execute {
   writeln("Value of the objective is: "+cplex.getObjValue());
}

