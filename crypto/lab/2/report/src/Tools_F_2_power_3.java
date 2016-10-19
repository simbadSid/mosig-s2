



public class Tools_F_2_power_3
{
// ------------------------------------
// Attributes
// ------------------------------------
	public final static int		kernelPoly= 11;							// Polynomial	X^3 + X + 1
	public final static int[]	inversIn2 = {0, 1, 5, 6, 7, 2, 3, 4};	// Tab[i] = Inverse of i in F(2^3)
																		// F(2^3) = (X1 X2 X3) where Xi in [0, 1]

// ------------------------------------
//Local methods
// ------------------------------------
	/**
	 * matrix[i] is the i line (polynomial)
	 */
	public static int multiplyMatrixByPoly(int[] matrix, int poly, int addPoly, int matrixXSize)
	{
		int res		= 0;
		int maskI	= 1;

		for (int i=0; i<matrix.length; i++)				// For each line of the matrix A
		{
			int li		= inverseNFirstBit(matrix[i], matrixXSize);
			int xi		= (li & poly);					//		xi = A * y  (line i)
			int bi		= addPoly;						//		bi will store the bit result[i]
			int maskJ	= 1;
			for (int j=0; j<matrixXSize; j++)			//		Count the number of 1 in xi
			{
				if ((xi & maskJ) != 0)					//			If the j th bit of xi is 1 then inverse the result bit
				{
					bi = (~bi);
				}
				maskJ = maskJ << 1;
			}
			res = res | (bi & maskI);
			maskI = maskI << 1;
		}

		return res;
	}

	public static int inverse(int a)
	{
		int res = (int) Math.pow(a, 6);		// 6 = 2^3 - 2

		return moduloPolynomial(res, kernelPoly);
	}

	/**
	 * Return the rest of the Euclidian division of the input polynom p by the divisor polynom
	 */
	public static int moduloPolynomial(int p, int divisor)
	{
		int p0 = p;
		int p1 = divisor;
		int d0 = degreeOfPolynom(p0);
		int d1 = degreeOfPolynom(p1);

		while (d0 >= d1)
		{
/*System.out.println("p0 = " + bitRepresentation(p0, 8));
System.out.println("p1 = " + bitRepresentation(p1, 8));
System.out.println("d0 = " + d0);
System.out.println("d1 = " + d1);
System.out.println("-----------------------");
*/			int dQotient	= d0 - d1;
			int mult		= p1 << dQotient;
			p0				= p0 ^ mult;
			d0				= degreeOfPolynom(p0);
		}

		return p0;
	}

	public static int leftCyclicShift(int a, int shift, int nbrInputBit)
	{
		if ((nbrInputBit < 0) || (nbrInputBit > Integer.SIZE))
			throw new RuntimeException("This function can only shift on integer");
		if (a > Math.pow(2, nbrInputBit)-1)
			throw new RuntimeException("The input int contains more significant bit than the expected number of bits");

		int realShift	= shift % Integer.SIZE;
		
		int nbrBitRight	= nbrInputBit - realShift;
		int maskRight	= (int) (Math.pow(2, nbrBitRight) - 1);
		int maskLeft	= (~maskRight) & ((int)Math.pow(2, nbrInputBit)-1);
		
		int resRight	= (a & maskRight)	<< realShift;
		int resLeft		= (a & maskLeft)	>> (nbrInputBit - realShift);

		return (resLeft | resRight);
	}

	/**
	 * a   = ....|an a(n-1)... a2     a1 <nl>
	 * res = ....|a1 a2    ... a(n-1) an
	 */
	public static int inverseNFirstBit(int a, int n)
	{
		int res;
		int maskI		= 1;
		int maskI_inv	= (int) Math.pow(2, (n-1));

		res = (int) (Math.pow(2, n) - 1);
		res = (~res) & a;

		for (int i=0; i<n; i++)
		{
			if ((a & maskI) != 0)
			{
				res = res | maskI_inv;
			}

			if ((a & maskI_inv) != 0)
			{
				res = res | maskI;
			}
			maskI		= maskI		<< 1;
			maskI_inv	= maskI_inv	>> 1;
		}

		return res;
	}

	public static int degreeOfPolynom(int polynom)
	{
		if (polynom <  0)	throw new RuntimeException("Wrong representation of a polynom: " + polynom);
		if (polynom == 0)	return 0;

		int a	= polynom;
		int deg	= -1;

		while(a > 0)
		{
			deg	++;
			a	= a >> 1;
		}

		return deg;
	}

	public static String bitRepresentation(int a, int nbrBit)
	{
		String res = "";
		int toPrint = a;
		int limit = Math.min(nbrBit, Integer.SIZE);

		for (int i=0; i<limit; i++)
		{
			res 	= (toPrint % 2) + " " + res;
			toPrint	= toPrint >> 1;
		}

		return res;
	}
}