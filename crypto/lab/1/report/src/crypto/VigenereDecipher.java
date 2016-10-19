package crypto;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.Writer;
import java.util.LinkedList;









public class VigenereDecipher
{
// -------------------------------------------------
// Attributes
// -------------------------------------------------
	private static final String	clearTextDir		= "resource/clearText/";
	private static final String	cipheredTextDir		= "resource/cipheredText/";

// -------------------------------------------------
// Main method
// -------------------------------------------------
	public static void main(String[] args) throws IOException
	{
		if (args.length < 3)
		{
			System.out.println("Usage: <ciphered text name> <clear text name> <key>");
			System.exit(0);
		}
		String cipheredFileName		= cipheredTextDir + args[0];
		String clearFileName		= clearTextDir + args[1];
		String key					= args[2];
		BufferedReader cipheredFile	= null;
		Writer clearFile			= null;

		try
		{
			cipheredFile	= new BufferedReader(new FileReader(cipheredFileName));
			clearFile		= new BufferedWriter(new OutputStreamWriter(new FileOutputStream(clearFileName), "utf-8"));
		}
		catch (Exception e)
		{
			System.out.println("Usage: <ciphered text name> <clear text name> <key>");
			e.printStackTrace();
			System.exit(0);
		}
		LinkedList<Integer> clearText = Vigenere.decipher(cipheredFile, key);
		for (Integer i:clearText)
		{
			char c = CryptoTools.mumericalCharToAlphabetic(i);
			clearFile.write(c);
		}
		clearFile.close();
		cipheredFile.close();
		System.out.println("End of deciphering");
	}
}