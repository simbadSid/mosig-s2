package crypto;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.Writer;
import java.util.LinkedList;









public class VigenereCipher
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
			System.out.println("Usage: <clear text name> <ciphered text name> <key>");
			System.exit(0);
		}
		String clearFileName	= clearTextDir + args[0];
		String cipheredFileName	= cipheredTextDir + args[1];
		String key				= args[2];
		BufferedReader clearFile= null;
		Writer cipheredFile		= null;

		try
		{
			clearFile		= new BufferedReader(new FileReader(clearFileName));
			cipheredFile	= new BufferedWriter(new OutputStreamWriter(new FileOutputStream(cipheredFileName), "utf-8"));
		}
		catch (Exception e)
		{
			e.printStackTrace();
			System.out.println("Usage: <clear text name> <cyphered text name> <key>");
			System.exit(0);
		}
		LinkedList<Integer> cipheredText = Vigenere.cipher(clearFile, key);
		for (Integer i:cipheredText)
		{
			char c = CryptoTools.mumericalCharToAlphabetic(i);
			cipheredFile.write(c);
		}
		clearFile.close();
		cipheredFile.close();
		System.out.println("\nEnd ciphering the text " + clearFileName);
	}
}