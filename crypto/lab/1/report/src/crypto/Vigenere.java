package crypto;

import java.io.BufferedReader;
import java.util.LinkedList;








public class Vigenere
{
	/**=======================================================================
	 * Cipher the hole content of the input file using the Vigenere algorithm.
	 ==========================================================================*/
	public static LinkedList<Integer> cipher(BufferedReader inputClearText, String key)
	{
		LinkedList<Integer> clearText		= CryptoTools.numerizeText(inputClearText, null);
		LinkedList<Integer> keyNumerical	= CryptoTools.numerizeText(key, null);
		LinkedList<Integer> res				= new LinkedList<Integer>();

		int keyIndex = 0;
		int clearChar, cipheredChar, shift;
		for (int clearTextIndex=0; clearTextIndex<clearText.size(); clearTextIndex++)
		{
			shift			= keyNumerical.get(keyIndex);
			clearChar		= clearText.get(clearTextIndex);
			cipheredChar	= CryptoTools.shiftNumericalChar(clearChar, shift);
			keyIndex		= (keyIndex + 1) % key.length();
			res.add(cipheredChar);
		}
		return res;
	}
	/**=======================================================================
	 * Decipher the hole content of the input file using the Vigenere algorithm.
	 * (this algo can be done by calling VigenerCipher with the key (26 - key))
	 ==========================================================================*/
	public static LinkedList<Integer> decipher(BufferedReader inputCipheredText, String key)
	{
		LinkedList<Integer> cipheredText	= CryptoTools.numerizeText(inputCipheredText, null);
		LinkedList<Integer> keyNumerical	= CryptoTools.numerizeText(key, null);
		LinkedList<Integer> res				= new LinkedList<Integer>();

		int keyIndex = 0;
		int clearChar, cipheredChar, shift;
		for (int cipheredTextIndex=0; cipheredTextIndex<cipheredText.size(); cipheredTextIndex++)
		{
			shift			= keyNumerical.get(keyIndex);
			cipheredChar	= cipheredText.get(cipheredTextIndex);
			clearChar		= CryptoTools.shiftNumericalChar(cipheredChar, -shift);
			keyIndex		= (keyIndex + 1) % key.length();
			res.add(clearChar);
		}
		return res;
	}

	/**===================================================================
	 * Deduce an approximation of the size of the key using the Friedman's algorithm.
	 * Supposes that the input ciphered text has been ciphered using the Vigenere's algorithm.
	 =====================================================================*/
	public static int keySizeApproximation(LinkedList<Integer> message, boolean printInfo)
	{
		double	kapa= CryptoTools.getCoincidenceIndex(message);
		double	Pm	= CryptoTools.probaDraw2SameChar_MonoAlphabetic;
		double	Pp	= CryptoTools.probaDraw2SameChar_PolyAlphabetic;
		int		n	= message.size();

		double res = n*(Pp - Pm) / (kapa*(n-1) + Pp - Pm*n);

		if (printInfo)
		{
			System.out.println("-----------------------------------");
			System.out.println("\t- The size of the text is                               : " + n);
			System.out.println("\t- The coincidence index is                              : " + kapa);
			System.out.println("\t- The proba to draw 2 same cards (monoalphabetic cipher): " + Pm);
			System.out.println("\t- The proba to draw 2 same cards (polyalphabetic cipher): " + Pp);
			System.out.println("\t- The computed key size is                              : " + res);
		}
		return (int) res;
	}
}