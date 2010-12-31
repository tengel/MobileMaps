/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package thutt;

import java.io.*;
import java.util.*;
import javax.microedition.io.*;
import javax.microedition.io.file.*;
import javax.microedition.lcdui.*;
import javax.microedition.midlet.*;

// Display class that lets the user select a file.
public class FileBrowser extends List implements CommandListener
{
	// Callback interface.
	public interface FileBrowserListener
	{
		public void fileSelected(String fileLoc);
		public void fileSelectionCancelled();
	}
	
	// Set callback.
	void setListener(FileBrowserListener l)
	{
		listener = l;
	}
	
	// The listener
	FileBrowserListener listener;
	
	// The current directory
	private String currentDir;
	
	// The commands.
	private Command select = new Command("Select", Command.OK, 1);
	private Command back = new Command("Back", Command.EXIT, 1);
	
	// Our file extension.
	private static final String ext = ".mapth";

	// File and dir icons.
	private Image dirIcon = null;
	private Image fileIcon = null;
	private Image parentIcon = null;

    public FileBrowser()
	{
		super("", List.IMPLICIT);
		
		setTitle("Select Map File");
		
        currentDir = "/";

        try
		{
            dirIcon = Image.createImage("/directory.png");
            fileIcon = Image.createImage("/map.png");
			parentIcon = Image.createImage("/parent.png");
        }
		catch (IOException e)
		{
        }
		
		setSelectCommand(select);
		addCommand(back);
		setCommandListener(this);
		
		showCurrentDir();
    }

    public void commandAction(Command c, Displayable d)
	{
        if (c == select)
		{
            final String currentFile = getString(getSelectedIndex());
			
			new Thread(
					new Runnable()
					{
						public void run()
						{
							if (currentFile.endsWith("/") || currentFile.equals(".."))
							{
								traverseDirectory(currentFile);
							}
							else
							{
								if (listener != null)
									listener.fileSelected("file://localhost/" + currentDir + currentFile + ext);
							}
						}
					}
				).start();
        }
		else if (c == back)
		{
			if (listener != null)
				listener.fileSelectionCancelled();
        }
    }

	// Show files in current directory.
    void showCurrentDir()
	{
		Enumeration e;
		FileConnection fc = null;
		
		// Remove old list elements.
		deleteAll();

		try
		{
			if ("/".equals(currentDir))
			{
				e = FileSystemRegistry.listRoots();
			}
			else
			{
				fc = (FileConnection)Connector.open("file://localhost/" + currentDir, Connector.READ);
				e = fc.list();
				append("..", parentIcon);
			}

			while (e.hasMoreElements())
			{
				String fileName = (String)e.nextElement();

				if (fileName.endsWith("/"))
				{
					// This is directory
					append(fileName, dirIcon);
				}
				else
				{
					// File - check it ends with out extension.
					if (fileName.endsWith(ext))
						append(fileName.substring(0, fileName.length() - ext.length()), fileIcon);
				}
			}

			if (fc != null)
			{
				fc.close();
			}
		}
		catch (IOException E)
		{
			E.printStackTrace();
		}
		catch (SecurityException E)
		{
			listener.fileSelectionCancelled();
		}
    }

    void traverseDirectory(String dirName)
	{
		// Traverse up or down a directory.
		if (currentDir.equals("/"))
		{
			if (dirName.equals(".."))
			{
				return;
			}

			currentDir = dirName;
		}
		else if (dirName.equals(".."))
		{
			// Go up one directory
			int i = currentDir.lastIndexOf('/', currentDir.length() - 2);

			if (i != -1)
			{
				currentDir = currentDir.substring(0, i + 1);
			}
			else
			{
				currentDir = "/";
			}
		}
		else
		{
			currentDir = currentDir + dirName;
		}

		showCurrentDir();
	}
}
