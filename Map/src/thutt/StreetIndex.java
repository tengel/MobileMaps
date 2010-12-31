/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package thutt;

import javax.microedition.io.*;
import javax.microedition.io.file.*;
import java.io.*;
import javax.microedition.lcdui.*;
import java.util.Vector;

public class StreetIndex extends Canvas
{
	// An entry in the index. Represents one street, streets cannot have
	// the same name, but should be named, e.g. "High Street (Kensignton)"
	public class IndexEntry
	{
		public String name; // Name of the street, + location if disambiguation is necessary.
		public int x; // Coordinates of the street centre in pixels from the bottom left of the map image.
		public int y; // Range is 0 to 0xFFFF spanning the full range of the map.
	}
	
	// The loader class that reads the index file.
	private class Loader implements Runnable
	{
		private boolean quit = false; // If we should quit or not.
		private String term = null; // The term to search for, e.g. "01203"
		private Object termSync = new Object(); // Used for synchronisation of term.
		private FileStore fileStore = null; // The file store to read from.
		private int fileNum = 0; // The file in the file store to use.

		public Loader(FileStore fs, int fn)
		{
			fileStore = fs;
			fileNum = fn;
			new Thread(this).start();
		}
		
		private String readString(DataInputStream din) throws IOException
		{
			Vector v = new Vector();

			for (byte b = din.readByte(); b != 0; b = din.readByte())
			{
				v.addElement(new Byte(b));
			}
			
			byte bytes[] = new byte[v.size()];
			
			for (int i = 0; i < v.size(); ++i)
				bytes[i] = ((Byte)v.elementAt(i)).byteValue();
			
			return new String(bytes, "UTF-8");
		}
		
		// Useful for storing strings only containing digits.
		private String readBCDString(DataInputStream din) throws IOException
		{
			StringBuffer buff = new StringBuffer();
			while (true)
			{
				byte b = din.readByte();
				if ((b & 0x0F) != 0x0F)
				{
					buff.append(b & 0x0F);
				}
				if ((b & 0xF0) != 0xF0)
				{
					buff.append((b & 0xF0) >> 4);
				}
				else
				{
					break;
				}
			}
			return buff.toString();
		}

		public void run()
		{
			String s; // The current search term.
			
			while(!quit)
			{
				s = null;

				synchronized (termSync)
				{
					if (term != null)
					{
						s = term;
						term = null;
					}
					else
					{
						try
						{
							termSync.wait();
					    }
						catch (InterruptedException e)
						{
						}
					}
				}

				if (s == null)
					continue;
				
				if (s.length() == 0)
					continue;
				
				System.err.println("Finding entries for: " + s);

				try
				{
					int numSent = 0;
					DataInputStream din = fileStore.getInputStream(fileNum);
					din.readInt(); // Skip past the length int.
					int atDigit = 0;
					
					try
					{
						while (numSent < 10) // This code smells.
						{
							int numRecords = din.readUnsignedByte();
							int length = din.readInt();
							if (numRecords == 0)
							{ // It's a digititem
								char digit = (char)din.readByte();
								if (atDigit < s.length() && s.charAt(atDigit) > digit)
								{
									din.skip(length - 1 - 4 - 1);
									continue;
								}
								else
								{
									++atDigit;
								}
							}
							else if (numRecords == 0xFF)
							{
								// End of file.
								din.close();
								throw new Exception();
							}
							else
							{
								// This is a set of records.
								String numTitle = readBCDString(din);
								for (int i = 0; i < numRecords; ++i)
								{
									IndexEntry ent = new IndexEntry();
									ent.name = readString(din);
									ent.x = din.readInt();
									ent.y = din.readInt();
									if (numTitle.startsWith(s) && numTitle.length() >= s.length())
									{
										synchronized (termSync)
										{
											if (term == null)
											{
												addEntry(ent);
											}
											else
											{
												din.close();
												throw new Exception();
											}
										}
									}
									++numSent;
								}
							}
							
							// Check to make sure we are still supposed to be
							// looking for this string.
							
							synchronized (termSync)
							{
								if (term != null)
								{
									din.close();
									throw new Exception();
								}
							}

						}
					}
					catch (Exception e)
					{
					}

					din.close();
				}
				catch (IOException e)
				{
					System.err.println("IO exception reading index file: " + e);
				}
				catch (Exception e)
				{
					System.err.println("Exception reading index file: " + e);
				}
			}
		}

		public void setTerm(String newTerm)
		{
			synchronized (termSync)
			{
				if(!quit)
				{
					term = newTerm;
					termSync.notify();
				}
			}
		}

		// TODO: Quit is not called!
		public void quit()
		{
			synchronized (termSync)
			{
				quit = true;
				termSync.notify();
			}
		}
	}
	
			
	private void addEntry(IndexEntry e)
	{
		entries.addElement(e);
		repaint();
	}
	
	private Vector entries;
	private int curSel = 0;
	private StringBuffer curText;
	
	private Loader loader;
	
	public StreetIndex()
	{
		entries = new Vector();
		curText = new StringBuffer();
	}
	
	private Command clearCommand = null;
	
	public void startIndex(FileStore fileStore, int fileNum, Command clear)
	{
		loader = new Loader(fileStore, fileNum);
		clearCommand = clear;
	}
	
	protected void paint(Graphics g)
	{
		int width = getWidth();
		int height = getHeight();
		
		Font font = Font.getFont(Font.FACE_PROPORTIONAL, Font.STYLE_PLAIN, Font.SIZE_SMALL);
		
		g.setColor(0xFFFFFF);
		g.fillRect(0, 0, width, height);
		g.setFont(font);

		for (int i = 0; i < entries.size() && (i + 2) * font.getHeight() < height; ++i)
		{
			if (i == curSel)
			{
				g.setColor(0xCCCCFF);
				g.fillRect(0, i * font.getHeight(), width, font.getHeight());
			}
			
			g.setColor(0x000000);
			
			g.drawString(((IndexEntry)entries.elementAt(i)).name, 3, i * font.getHeight(), Graphics.TOP | Graphics.LEFT);
		}

		g.setColor(0x777777);
		g.drawLine(0, height - font.getHeight(), width, height - font.getHeight());
		g.setColor(0x000000);
		g.drawString(curText.toString(), 5, height, Graphics.BOTTOM | Graphics.LEFT);
	}
	
	public void deleteChar()
	{
		int l = curText.length();
		if (l != 0)
			curText.deleteCharAt(l - 1);

		entries.removeAllElements();
		curSel = 0;
		if (loader != null)
			loader.setTerm(curText.toString());
		
		updateClearCommand();
		
		repaint();
	}
	
	public void clearSearch()
	{
		curText.setLength(0);
		entries.removeAllElements();
		curSel = 0;
		if (loader != null)
			loader.setTerm(curText.toString());
		
		updateClearCommand();

		repaint();
	}
	
	protected void keyPressed(int keyCode)
	{
		switch (keyCode)
		{
			case KEY_NUM0:
				curText.append('0');
				break;
			case KEY_NUM1:
				curText.append('1');
				break;
			case KEY_NUM2:
				curText.append('2');
				break;
			case KEY_NUM3:
				curText.append('3');
				break;
			case KEY_NUM4:
				curText.append('4');
				break;
			case KEY_NUM5:
				curText.append('5');
				break;
			case KEY_NUM6:
				curText.append('6');
				break;
			case KEY_NUM7:
				curText.append('7');
				break;
			case KEY_NUM8:
				curText.append('8');
				break;
			case KEY_NUM9:
				curText.append('9');
				break;
			case KEY_STAR:
				clearSearch();
				break;
//			case -7: // Nokia clear key?
//				deleteChar();
//				break;
			default:
				switch (getGameAction(keyCode))
				{
					case UP:
						if (curSel > 0)
						{
							--curSel;
							repaint();
						}
						return;

					case DOWN:
						if (curSel < entries.size() - 1)
						{
							++curSel;
							repaint();
						}
						return;
				}
				break;
		}
		
		entries.removeAllElements();
		curSel = 0;
		if (loader != null)
			loader.setTerm(curText.toString());
		
		updateClearCommand();

		repaint();
	}
	protected void repeatedKey(int keyCode)
	{
		keyPressed(keyCode);
		repaint();
	}
	
	public IndexEntry getEntry()
	{
		if (entries == null)
			return null;
		if (curSel >= 0 && curSel < entries.size())
			return (IndexEntry)entries.elementAt(curSel);
		return null;
	}
	public String getText()
	{
		return curText.toString();
	}
	
	private void updateClearCommand()
	{
		if (clearCommand == null)
			return;
//		if (curText.length() == 0)
//			clearCommand.
		// ARGH STUPID JAVA DOESN'T LET YOU DO ANYTHING! I JUST WANT TO CHANGE THE LABEL.
	}
}
