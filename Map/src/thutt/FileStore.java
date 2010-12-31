package thutt;

import javax.microedition.lcdui.*;
import java.util.*;
import javax.microedition.io.*;
import javax.microedition.io.file.*;
import java.io.*;

/** This lets you read files from a zip-like container of files. The files
 * aren't compressed but it just means the user isn't prompted each time a file
 * is read.
 * 
 * The file format is:
 * 
 * <number of files>
 * [number of files]x<file offset>
 * [number of files]x(<file length> <file>)
 *
 * All lengths/sizes are ints.
 */
public class FileStore
{
	// Open a collection file. If a file is currently open it is closed.
	public void open(String file) throws IOException
	{
		System.err.println("Opening file store: " + file);
		
		// Close previously opened files.
		try
		{
			if (fc != null)
				fc.close();
		}
		catch (IOException E)
		{
			System.err.println("IO exception closing store file: " + E);
		}
		
		fc = null;
		numFiles = 0;

		// Try to open and read the file.
		try
		{
			fc = (FileConnection)Connector.open(file, Connector.READ);

			if (!fc.canRead())
				throw new IOException("File " + file + " cannot be read.");
		}
		catch (SecurityException E)
		{
			System.err.println("Security exception reading file: " + E);
			fc = null;
			throw E;
		}
		catch (IOException E)
		{
			System.err.println("IO exception reading file: " + E);
			fc = null;
			throw E;
		}
		
		// Get the number of files within the store file.
		try
		{
			InputStream is = fc.openInputStream();
			DataInputStream din = new DataInputStream(is);
			
			numFiles = din.readInt();
			
			din.close();
		}
		catch (IOException E)
		{
			System.err.print("Error reading file: " + E);
			fc = null;
			numFiles = 0;
			throw E;
		}
		
		System.err.println("Store opened successfully.");
	}
	
	// Return the number of files in the collection.
	public int size()
	{
		return numFiles;
	}
	
	// Fully read one of the files.
	public byte[] read(int fileNum) throws IOException
	{
		if (fileNum < 0 || fileNum >= numFiles)
		{
			System.err.println("Attempted to access out-of-range file.");
			throw new IOException("Attempted to access out-of-range file: " + fileNum + " < 0 || >= " + numFiles);
		}
		
		try
		{
			InputStream is = fc.openInputStream();
			DataInputStream din = new DataInputStream(is);

			din.skip(4 + fileNum * 4);
			int offs = din.readInt();

			din.skip(offs - 4 - (fileNum+1)*4);

			int len = din.readInt();
			byte[] b = new byte[len];
			din.readFully(b, 0, len);
			din.close();
			return b;
		}
		catch (IOException E)
		{
			System.err.println("IO exception reading index file: " + E);
			throw E;
		}
	}
	
	// Return a raw DataInputStream for one of the files.
	// It has been skipped to point to the length int so
	// you must read an int before you get to the file data.
	// YOU MUST CLOSE the returned data stream otherwise errors will occur.
	public DataInputStream getInputStream(int fileNum) throws IOException
	{
		if (fileNum < 0 || fileNum >= numFiles)
		{
			System.err.println("Attempted to access out-of-range file.");
			throw new IOException("Attempted to access out-of-range file: " + fileNum + " < 0 || >= " + numFiles);
		}
		
		try
		{
			InputStream is = fc.openInputStream();
			DataInputStream din = new DataInputStream(is);

			din.skip(4 + fileNum * 4);
			int offs = din.readInt();

			din.skip(offs - 4 - (fileNum+1)*4);

			return din;
		}
		catch (IOException E)
		{
			System.err.println("IO exception reading index file: " + E);
			throw E;
		}
	}

	// The FileConnection for the file. This is stored so we don't keep getting asked to open the file.
	private FileConnection fc = null;
	// The number of files stored in the file.
	private int numFiles = 0;
}
