// Template for the tube map type things.

import javax.microedition.lcdui.*;
import javax.microedition.midlet.*;
import java.io.*;

// Main class which inits the connection and create the screens
public class MapApp extends MIDlet implements CommandListener
{
	// These are the screens that are shown to the user.
	private ImageCanvas imageCanvas = new ImageCanvas();
	private Index index = new Index();

	// Stores all the settings in the phone.
	private Settings settings = new Settings();
	
	// Commands for use with the screens.
	private Command exitCommand = new Command("Exit", Command.EXIT, 1);
	private Command searchCommand = new Command("Search", Command.OK, 1);
	private Command backCommand = new Command("Back", Command.OK, 2);
	private Command okCommand = new Command("OK", Command.OK, 1);
	private Command clearCommand = new Command("Clear", Command.EXIT, 1);
	
	public MapApp()
	{
		// Load the map definition.
		MapDefinition def = loadMapDef();
		if (def == null)
		{
			fatalError("Invalid map definition file.");
			return;
		}
		
		imageCanvas.addCommand(exitCommand);
		imageCanvas.addCommand(searchCommand);
		imageCanvas.setCommandListener(this);
		imageCanvas.setTitle("Map"); // TODO: Read a string from the mapdef.
		
//		imageCanvas.setFullScreenMode(true); TODO: Draw the command labels ourselves and use the soft key keycodes.
		
		index.addCommand(okCommand);
		index.addCommand(clearCommand);
		index.addCommand(backCommand);
		index.setCommandListener(this);
		index.setTitle("Index");
		
		// See if there is a record of the previously viewed location.
		settings.readSettings();
		
		imageCanvas.loadMap(def, settings.getXPos(), settings.getYPos(), settings.getZoom());
		Display.getDisplay(this).setCurrent(imageCanvas);
	}
	
	private MapDefinition loadMapDef()
	{
		MapDefinition def = new MapDefinition();
		try
		{
			InputStream is = getClass().getResourceAsStream("/definition.dat");
			DataInputStream din = new DataInputStream(is);

			def.tileHeight = din.readInt();
			def.tileWidth = din.readInt();
			def.totalHeight = din.readInt();
			def.totalWidth = din.readInt();
			def.mapHeight = din.readInt();
			def.mapWidth = din.readInt();
			def.overviewMapHeight = din.readInt();
			def.overviewMapWidth = din.readInt();
			def.overviewTotalHeight = din.readInt();
			def.overviewTotalWidth = din.readInt();
			
/*			System.out.println("Got definition: " + String.valueOf(def.tileHeight)
					+ " " +  String.valueOf(def.tileWidth)
					+ " " +  String.valueOf(def.totalHeight)
					+ " " +  String.valueOf(def.totalWidth)
					+ " " +  String.valueOf(def.mapHeight)
					+ " " +  String.valueOf(def.mapWidth)
					+ " " +  String.valueOf(def.overviewMapHeight)
					+ " " +  String.valueOf(def.overviewMapWidth)
					+ " " +  String.valueOf(def.overviewTotalHeight)
					+ " " +  String.valueOf(def.overviewTotalWidth));*/

			din.close();
		}
		catch (Exception e)
		{
			return null;
		}

		return def;
	}
	
	public void startApp()
	{
		// This is called when the app starts, possible including after pausing. We don't
		// need to do anything.
	}
	
	public void pauseApp()
	{
		// This is called when the app needs to be paused, for example if they received a phone call during use.
		// We don't reaaally need to do anything. Could stop scrolling but they probably don't care that much.
	}
	
	public void destroyApp(boolean unconditional)
	{
		// This is called when the app is about to be destroyed. Call notifyDestroyed() to actually exit.
		
		// Save settings.
		settings.setXPos(imageCanvas.getXPos());
		settings.setYPos(imageCanvas.getYPos());
		settings.setZoom(imageCanvas.getZoom());
		settings.writeSettings();
		
		// It appears that any of our threads are killed automatically.
		// The java docs are pretty shit.
	}	
	
	public void commandAction(Command command, Displayable d)
	{
		if (command == exitCommand)
		{
			destroyApp(true);
			notifyDestroyed();
		}
		else if (command == searchCommand)
		{
			index.clearSearch();
			Display.getDisplay(this).setCurrent(index);
		}
		else if (command == okCommand)
		{
			if (d == index)
			{
				Index.IndexEntry loc = index.getEntry();
				if (loc != null)
				{
					imageCanvas.setXPos(imageCanvas.getCurrentMapWidth() * loc.x / 0xFFFF);
					imageCanvas.setYPos(imageCanvas.getCurrentMapHeight() * (0xFFFF - loc.y) / 0xFFFF);
					imageCanvas.repaint();
					Display.getDisplay(this).setCurrent(imageCanvas);
				}
			}
		}
		else if (command == backCommand)
		{
			Display.getDisplay(this).setCurrent(imageCanvas);
		}
		else if (command == clearCommand)
		{
			if (index.getText().length() == 0)
			{
				Display.getDisplay(this).setCurrent(imageCanvas);
			}
			else
			{
				index.deleteChar();
			}
		}
	}
	
	public void fatalError(String s)
	{
		// For now use a form. In future we will use TextDisplay when it is finished!
		Form form = new Form("Fatal Error");
		form.append(new StringItem(null, s));
		form.addCommand(exitCommand);
		form.setCommandListener(this);
		Display.getDisplay(this).setCurrent(form);
	}

}