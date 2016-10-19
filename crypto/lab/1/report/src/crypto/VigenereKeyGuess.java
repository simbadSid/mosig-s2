package crypto;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.LinkedList;






public class VigenereKeyGuess
{
	private static final String	cipheredTextDir		= "resource/cipheredText/";

// -------------------------------------------------
// Main method
// -------------------------------------------------
	public static void main(String[] args) throws IOException
	{
		if (args.length < 1)
		{
			System.out.println("Usage: <cyphered text name>");
			System.exit(0);
		}
		String cipheredFileName	= cipheredTextDir + args[0];
		BufferedReader cipheredFile	= null;

		try
		{
			cipheredFile = new BufferedReader(new FileReader(cipheredFileName));
		}
		catch (Exception e)
		{
			e.printStackTrace();
			System.out.println("Usage: <cyphered text name>");
			System.exit(0);
		}
		LinkedList<Integer> cipheredText = CryptoTools.numerizeText(cipheredFile, null);
		int keySize = Vigenere.keySizeApproximation(cipheredText, true);

		cipheredFile.close();
		System.out.println("\nThe computed key size is " + keySize);
	}
}
