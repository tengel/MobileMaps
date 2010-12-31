
package thutt;


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
	public MapDefinition LoadMap(FileStore store) throws IOException
	{
		fileStore = store;
	
		if (fileStore.size() < 1)
			return null;
		
		if (loaderThread != null)
			loaderThread.exit();
		
		byte[] header = fileStore.read(0);
		ByteArrayInputStream bais = new ByteArrayInputStream(header);
		DataInputStream din = new DataInputStream(bais);
		
		MapDefinition def = new MapDefinition();
		def.mapWidth = din.readInt();
		def.mapHeight = din.readInt();
		def.tileWidth = din.readInt();
		def.tileHeight = din.readInt();
		def.totalWidth = din.readInt();
		def.totalHeight = din.readInt();
		def.overviewMapWidth = din.readInt();
		def.overviewMapHeight = din.readInt();
		def.overviewTotalWidth = din.readInt();
		def.overviewTotalHeight = din.readInt();
		
		int nTiles = def.mapWidth * def.mapHeight + def.overviewMapWidth * def.overviewMapHeight;
		
		tiles = new Image[nTiles];
		loaderThread = new Loader(); // Automatically starts.
		
		return def;
	}
	
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
			
			for (int i = 0; i < desired.size(); ++i)
			{
				int nTile = ((Integer)desired.elementAt(i)).intValue();
				if (nTile < 0 || nTile >= tiles.length)
				{
					desired.setElementAt(new Integer(-1), i);
				}
				else if (tiles[nTile] != null)
				{
					// -1 means not to load the tile.
					desired.setElementAt(new Integer(-1), i);
					newTiles[nTile] = tiles[nTile];
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
	// The file cache!
	private FileStore fileStore;
	
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
					// See if there is a tile to load.
					if (desired != null && !desired.isEmpty())
					{
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
					// setDesired actually sets tiles that have already been loaded to
					// -1 to avoid loads of Vector operations.
					if (toLoad != -1)
					{
						byte[] b = fileStore.read(toLoad + 1);
						Image im = Image.createImage(b, 0, b.length);

						synchronized (sync)
						{
							// TODO: Minor, but this may result in extra tiles
							// being temporarily loaded because we don't check
							// that this tile is still wanted.
							tiles[toLoad] = im;
							if (desired != null && desired.isEmpty())
								listener.imagesLoaded();
						}
					}
				}
				catch (Exception e)
				{
					System.err.println("Exception reading index file: " + e);
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