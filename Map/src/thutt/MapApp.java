/* FINAL FEATURE SET:
 * 
 * Overview Map: More than one level?
 * Pub locations: Generic layers support - just positions.
 * */


package thutt;


import javax.microedition.lcdui.*;
import javax.microedition.midlet.*;
import java.io.*;

// Main class which inits the connection and create the screens
public class MapApp extends MIDlet implements CommandListener, FileBrowser.FileBrowserListener
{
	// These are the screens that are shown to the user.
	private ImageCanvas imageCanvas = new ImageCanvas();
	private StreetIndex streetIndex = new StreetIndex();
	private TextDisplay helpDisplay = new TextDisplay("# Introduction ~ This is a street map of a large area of London. The images come (illegally unfortunately) from streetmap.co.uk. The street index is from openstreetmap.org. Not every street is indexed (but most are) and some are indexed that you can't see in the map. # Scrolling ~ To scroll around simply use the 1,2,3,4,6,7,8,9 keys or the direction pad/joystick on your phone. You can also press the hash key to return to the centre of the map. # Zooming ~ To zoom in or out press the 5 key. # Searching ~ To search for a street, press the 'Search' menu item and type in the name of the street as if you were using predictive text. Use 1 for symbols and accented characters. Press * to clear the search text and up/down to select a different entry. Press OK to go to that street. To leave the search screen without going to a street, clear all the characters and then press clear once more. # Help! ~ If you need more help see tdhutt.googlepages.com or email tdhutt@gmail.com.");
	
	// Stores all the settings in the phone.
	private Settings settings = new Settings();
	
	// File store, which contains the index data and map tiles.
	private FileStore fileStore = new FileStore();
	
	// Commands for use with the screens.
	private Command exitCommand = new Command("Exit", Command.EXIT, 1);
	private Command helpCommand = new Command("Help", Command.HELP, 2);
	private Command searchCommand = new Command("Search", Command.OK, 1);
	private Command backCommand = new Command("Back", Command.OK, 2);
	private Command okCommand = new Command("OK", Command.OK, 1);
	private Command clearCommand = new Command("Clear", Command.EXIT, 1);
	private Command selectMapCommand = new Command("Select Map", Command.SCREEN, 3);
	
	public MapApp()
	{
		imageCanvas.addCommand(exitCommand);
		imageCanvas.addCommand(searchCommand);
		imageCanvas.addCommand(helpCommand);
		imageCanvas.addCommand(selectMapCommand);
		imageCanvas.setCommandListener(this);
		imageCanvas.setTitle("Maps");
//		imageCanvas.setFullScreenMode(true); TODO: Draw the command labels ourselves and use the soft key keycodes.
		
		streetIndex.addCommand(okCommand);
		streetIndex.addCommand(clearCommand);
		streetIndex.setCommandListener(this);
		streetIndex.setTitle("Street Index");
		
		helpDisplay.addCommand(okCommand);
		helpDisplay.setCommandListener(this);
		helpDisplay.setTitle("Help");
		
		// See if there is a record of the previously viewed location.
		settings.readSettings();
		
		if (settings.getMapLoc() == null || settings.getMapLoc().length() == 0)
		{
			// Ask them where the map is.
			FileBrowser b = new FileBrowser();
			b.setListener(this);
			Display.getDisplay(this).setCurrent(b);
			return;			
		}
		
		loadMap();
	}
	private void loadMap()
	{
		// Load the map.
		try
		{
			fileStore.open(settings.getMapLoc());
			streetIndex.startIndex(fileStore, fileStore.size()-1, clearCommand);
			imageCanvas.loadMap(fileStore, settings.getXPos(), settings.getYPos(), settings.getLayer());
			// If this is the first run, show help first, otherwise show the map.
			Display.getDisplay(this).setCurrent(imageCanvas);
			return;
		}
		catch (IOException E)
		{
			fatalError("IOException reading map file: " + E);
		}
		catch (SecurityException E)
		{
			fatalError("Insufficient permissions to read map file: " + E);
		}
		catch (Exception E)
		{
			fatalError("Unexpected exception: " + E);
		}
		
		// Loading failed. Set the loc to "" so they can chose a new loc
		// next time they run the app.
		settings.setMapLoc("");
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
		settings.setLayer(imageCanvas.getLayer());
		settings.setZoom(imageCanvas.getZoom());
		settings.writeSettings();
		
		// It appears that any of our threads are killed automatically.
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
			Display.getDisplay(this).setCurrent(streetIndex);
		}
		else if (command == helpCommand)
		{
			Display.getDisplay(this).setCurrent(helpDisplay);
		}
		else if (command == okCommand)
		{
			if (d == streetIndex)
			{
				StreetIndex.IndexEntry street = streetIndex.getEntry();
				if (street != null)
				{
					streetIndex.clearSearch();
					imageCanvas.setXPos(imageCanvas.getCurrentMapWidth() * street.x / 0xFFFF);
					imageCanvas.setYPos(imageCanvas.getCurrentMapHeight() * (0xFFFF - street.y) / 0xFFFF);
					imageCanvas.repaint();
					Display.getDisplay(this).setCurrent(imageCanvas);
				}
			}
			else if (d == helpDisplay)
			{
				Display.getDisplay(this).setCurrent(imageCanvas);
			}
		}
		else if (command == backCommand)
		{
			Display.getDisplay(this).setCurrent(imageCanvas);
		}
		else if (command == clearCommand)
		{
			if (streetIndex.getText().length() == 0)
				Display.getDisplay(this).setCurrent(imageCanvas);
			else
				streetIndex.deleteChar();
		}
		else if (command == selectMapCommand)
		{
			// Ask them where the map is.
			FileBrowser b = new FileBrowser();
			b.setListener(this);
			Display.getDisplay(this).setCurrent(b);
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
	
	public void fileSelected(String fileLoc)
	{
		settings.setMapLoc(fileLoc);
		loadMap();
	}
	public void fileSelectionCancelled()
	{
		if (settings.getMapLoc() == null)
			fatalError("You must select a map file to use. You can get one from http://tdhutt.googlepages.com/mobilelondonstreetmap");
		else
			Display.getDisplay(this).setCurrent(imageCanvas);
	}

}