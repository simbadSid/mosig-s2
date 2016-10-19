package crypto;

import java.io.BufferedReader;
import java.io.IOException;
import java.util.LinkedList;








public class CryptoTools
{
// ------------------------------------------------------
// Attributes:
// ------------------------------------------------------
	public final static int		nbrAlphabeticChar					= 26;
	public final static double	probaDraw2SameChar_MonoAlphabetic	= 0.038;	// Doesn't depend from the language
//	public final static double	probaDraw2SameChar_PolyAlphabetic	= 0.065;	// Computed for English
	public final static double	probaDraw2SameChar_PolyAlphabetic	= 0.0746;	// Computed for French

// ------------------------------------------------------
// Local methodes
// ------------------------------------------------------
	/**=============================================================
	 * Transform an alphabetic char to an integer between 0 and 25.
	 * If the char is not an alphabetic char, -1 is returned
	 ===============================================================*/
	public static int alphabeticCharToNumerical(char c)
	{
		if		((c >= 'a') && (c <= 'z'))	return (c - 'a');
		else if	((c >= 'A') && (c <= 'Z'))	return (c - 'A');
		else								return -1;
	}

	/**=============================================================
	 * Transform an alphabetic char to an integer between 0 and 25.
	 * If the char is not an alphabetic char, -1 is returned
	 ===============================================================*/
	public static char mumericalCharToAlphabetic(int n)
	{
		if		((n >= 0) && (n <= nbrAlphabeticChar))	return (char) ('a' + n);
		else											return (char) -1;
	}

	/**=============================================================
	 * Transform an alphabetic char to an integer between 0 and 25.
	 * If the char is not an alphabetic char, -1 is returned
	 ===============================================================*/
	public static int shiftNumericalChar(int n, int shift)
	{
		if ((n < 0)	||
			(n > nbrAlphabeticChar))		throw new RuntimeException("Unhandeled char to shift: " + n);
		if ((shift < -nbrAlphabeticChar) ||
			(shift > nbrAlphabeticChar))	throw new RuntimeException("Unhandeled shift: " + shift);

		if		(shift >= 0)	return ((n + shift) % nbrAlphabeticChar);	// Case positive shift
		else if (-shift <= n)	return (n + shift);							// Case neg shift without roll back
		else					return (nbrAlphabeticChar + (n + shift));	// Case neg shift with roll back
	}

	/**=======================================================================
	 * Transform the hole content of the input file into an list of integer.
	 * The non alphabetic char are replaced by the input parameter "nonAlphabeticReplace", or
	 * ignored if the parameter is null.
	 ==========================================================================*/
	public static LinkedList<Integer> numerizeText(BufferedReader inputClearText, Integer nonAlphabeticReplace)
	{
		try
		{
			LinkedList<Integer> res = new LinkedList<Integer>();
			int textChar;
			textChar = inputClearText.read();

			while(textChar != -1)
			{
				int i = alphabeticCharToNumerical((char) textChar);
				if 		(i != -1)						res.add(i);
				else if (nonAlphabeticReplace != null)	res.add(new Integer(nonAlphabeticReplace));
				textChar = inputClearText.read();
			}
			return res;
		}
		catch (IOException e)
		{
			e.printStackTrace();
			System.exit(0);
		}
		return null;
	}

	/**=======================================================================
	 * Transform the hole content of the input text into an list of integer.
	 * The non alphabetic char are replaced by the input nonAlphabeticReplace, or
	 * ignored if the parameter is null
	 ==========================================================================*/
	public static LinkedList<Integer> numerizeText(String clearText, Integer nonAlphabeticReplace)
	{
		LinkedList<Integer> res = new LinkedList<Integer>();

		for (int i=0; i<clearText.length(); i++)
		{
			int clearChar = alphabeticCharToNumerical((char) clearText.charAt(i));
			if 		(clearChar != -1)				res.add(clearChar);
			else if (nonAlphabeticReplace != null)	res.add(new Integer(nonAlphabeticReplace));
		}
		return res;
	}

	/**=========================================================================
	 * Return the number of occurrences of each character from the input message
	 ===========================================================================*/
	public static void getNbrOccurence(LinkedList<Integer> message, double[] occurrence)
	{

		for (int i=0; i<nbrAlphabeticChar; i++)
		{
			occurrence[i] = 0;
		}

		for (int i: message)
		{
			if ((i < 0) || (i >= nbrAlphabeticChar)) throw new RuntimeException("Unknown numerical char: " + i);
			occurrence[i] ++;
		}
	}

	/**==========================================================================
	 * Return the coincidence index of the given char in the given text.
	 * The coincidence index  provides a measure of how likely it is to draw
	 * two matching letters by randomly selecting two letters from a given text.
	 ============================================================================*/
	public static double getCoincidenceIndex(LinkedList<Integer> message)
	{
		double occurrence[] = new double[nbrAlphabeticChar];
		double res = 0;
		double messageSize = message.size();

		getNbrOccurence(message, occurrence);

		for(int c=0; c<nbrAlphabeticChar; c++)
		{
			res += occurrence[c] * (occurrence[c]-1);
		}
		return res / (messageSize * (messageSize-1));
	}
}
