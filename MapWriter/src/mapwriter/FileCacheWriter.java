/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package mapwriter;

import java.util.*;
import java.io.*;

/**
 *
 * @author t
 */
public class FileCacheWriter
{
	public void AppendFile(byte[] file)
	{
		files.add(file);
	}
	
	public void WriteToFile(String filename) throws FileNotFoundException, IOException
	{
		// Write the stuff.
		RandomAccessFile ra = new RandomAccessFile(filename, "rw");
		
		ra.writeInt(files.size());
		int offset = files.size() * 4 + 4;
		for (int i = 0; i < files.size(); ++i)
		{
			ra.writeInt(offset);
			offset += ((byte[])files.elementAt(i)).length + 4; // 4 extra bytes for the length.
		}
		for (int i = 0; i < files.size(); ++i)
		{
			ra.writeInt(((byte[])files.elementAt(i)).length);
			ra.write((byte[])files.elementAt(i));
		}
		ra.close();
	}
	
	private Vector files = new Vector();
}