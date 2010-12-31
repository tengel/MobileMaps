

import javax.microedition.lcdui.*;
import java.util.*;
import javax.microedition.io.*;
import javax.microedition.io.file.*;
import java.io.*;

// The image cache. This records which images are available and loads then etc.
public class ImageCache
{
	public interface ImageCacheListener
	{
		// All requested images were loaded.
		public void imagesLoaded();
	}
	
	// Create an image cache, the cache starts straight away.
	// Initially all images are set as undesired. numImages sets the number of images.
	// Images are loaded from the mapFile.
	// TODO: This should probably be moved to MapApp.
	public void LoadMap(int nTiles)
	{
		if (loaderThread != null)
			loaderThread.exit();
		
		tiles = new Image[nTiles];
		base = "/tile_";
		loaderThread = new Loader(); // Automatically starts.
	}
	
	String base;
	
	// Set all the tiles in desiredTiles to be desired. All others are set as undesired.
	public void setDesired(Vector desiredTiles)
	{
		if (tiles == null)
			return;
		
		synchronized (sync)
		{
			desired = desiredTiles;
			
			Image[] newTiles = new Image[tiles.length];
			
			if (desired == null)
				return;
			
			for (int i = 0; i < desired.size();)
			{
				int nTile = ((Integer)desired.elementAt(i)).intValue();
				
				if (nTile < 0 || nTile >= tiles.length) // Invalid.
				{
					desired.removeElementAt(i);
				}
				else if (tiles[nTile] != null) // Valid but already loaded.
				{
					desired.removeElementAt(i);
					newTiles[nTile] = tiles[nTile];
				}
				else
				{
					++i;
				}
			}
			tiles = newTiles;
		}
		
		loaderThread.poke();
	}
	
	// Returns null if imNum is invalid or if the image isn't loaded yet.
	public Image getImage(int nTile)
	{
		if (tiles == null)
			return null;
		if (nTile < 0 || nTile >= tiles.length)
			return null;
		
		return tiles[nTile];
	}
	
	// When a desired image is loaded, n will be notified.
	public void setListener(ImageCacheListener l)
	{
		listener = l;
	}

	// Who to notify when tiles are loaded.
	private ImageCacheListener listener;
	// Object to synchronise access to the next three objects ----------
	private Object sync = new Object(); // Used for synchronising.
	// The images - null for no image.
	private Image[] tiles;
	// The queue of images to load. This is set by setDesired(). Tiles are removed
	// as the are loaded. Contains ints.
	private Vector desired;	
	// The loader thread that loads the images.
	private Loader loaderThread;
	
	// The loader thread.
	private class Loader implements Runnable
	{
		public Loader()
		{
			new Thread(this).start();
		}
		
		public void run()
		{		
			int toLoad;
			while(!quit)
			{
				synchronized (sync)
				{
					if (desired != null && !desired.isEmpty())
					{
						// Get the last element.
						toLoad = ((Integer)desired.lastElement()).intValue();
						desired.removeElement(desired.lastElement());
					}
					else
					{
						try
						{
							sync.wait();
					    }
						catch (InterruptedException e)
						{
						}
						// We've been awoken. Go back and check if there is
						// something to load of if we should quit.
						continue;
					}
				}
				try
				{
					Image im = Image.createImage(base + String.valueOf(toLoad) + ".png");

					synchronized (sync)
					{
						tiles[toLoad] = im;
						if (desired != null && desired.isEmpty())
							listener.imagesLoaded();
					}
				}
				catch (Exception e)
				{
					System.err.println("Exception reading image: " + e);
				}
			}
		}

		// Wake up the thread if it was sleeping.
		public void poke()
		{
			synchronized (sync)
			{
				if(!quit)
				{
					sync.notify();
				}
			}
		}

		// Tell the thread to exit. Currently this is not called.
		public void exit()
		{
			synchronized (sync)
			{
				quit = true;
				// Make sure to wake it up.
				sync.notify();
			}
		}
		
		// Whether to quit or not. Set when quit() is called.
		private boolean quit = false;
	}	
}