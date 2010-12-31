import javax.microedition.lcdui.*;
import javax.microedition.midlet.*;

// Main class which inits the connection and create the screens
public class MapApp extends MIDlet implements CommandListener
{
	private ImageCanvas imageCanvas;
	private Settings settings = new Settings();
	private Command exitCommand = new Command("Exit", Command.EXIT, 1);
	private Command helpCommand = new Command("Help", Command.HELP, 1);	
	private MapDefinition def;

	public MapApp()
	{
		def = new MapDefinition();
		imageCanvas = new ImageCanvas();
	}
	
	
	public void startApp()
	{
		// See if there is a record of where they were.
		settings.readSettings();
		
		imageCanvas.displayMap(def, settings.getXPos(def.xCentre), settings.getYPos(def.yCentre));
		
		imageCanvas.addCommand(helpCommand);
		imageCanvas.addCommand(exitCommand);
		imageCanvas.setCommandListener(this);
		
		Display.getDisplay(this).setCurrent(imageCanvas);
	}
	
	public void pauseApp()
	{
		// Should this do anything?
	}
	
	public void destroyApp(boolean unconditional)
	{
		settings.setXPos(imageCanvas.getXPos());
		settings.setYPos(imageCanvas.getYPos());
		settings.writeSettings();
	}
	
	public void commandAction(Command command, Displayable d)
	{
		if (command == exitCommand)
		{
			// TODO: kill all threads?
			destroyApp(true);
			notifyDestroyed();
		}
		else if (command == helpCommand)
		{
			// Show help screen.			
			Alert help = new Alert("Help", "Tube Map\nwritten by Tim Hutt.\n\ntdhutt.googlepages.com\n\nUse the number or direction keys to scroll and 5 to recentre.", null, AlertType.INFO);
			help.setTimeout(Alert.FOREVER);
			Display.getDisplay(this).setCurrent(help, imageCanvas);
		}
	}
}

